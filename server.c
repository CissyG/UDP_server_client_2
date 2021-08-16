/*************************************************************************
 * COEN 233 Assignment 2 - server.c
 *
 * Name: Xin Guan
 * Student ID: 1610150
 * Date: 05/29/2021
 *
 * This file defines a server that can receive access request for subscribers
 *  by customized UDP packets and send responses to the client for permitted,
 *  not paid or non-exist subscribers. It checks in database stored in the file
 *  Verification_Database.txt.
 ************************************************************************/
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include "myPacket.h"

// struct for each data entry
struct DataInfo {
  unsigned long int subNo;
  int techNo;
  int paidCondition;
};

// function prototype
int receiveAndResponse(int socketFd, uint8_t *buffer, int numEntries,
                       struct DataInfo *entries);
int readEntries(char *fileName, struct DataInfo *entries);

#define MAX_ENTRIES 1000 // maxmium numbers of entries in the database

int main() {
  struct DataInfo entries[MAX_ENTRIES];
  int numEntries = 0;
  int socketFd;
  uint8_t buffer[MAXLEN];
  struct sockaddr_in servAddr;

  // read entries from the database file
  numEntries = readEntries("Verification_Database.txt", entries);
  if (numEntries < 0) {
    perror("Failed to read database file.\n");
    exit(EXIT_FAILURE);
  }
  printf("Read %d entries from the database.\n", numEntries);

  // Create a socket
  socketFd = socket(AF_INET, SOCK_DGRAM, 0);
  if (socketFd < 0) {
    perror("Socket creation failed.\n");
    exit(EXIT_FAILURE);
  }

  // set server address and port
  memset(&servAddr, 0, sizeof(servAddr));
  const uint16_t port = 2021;
  servAddr.sin_family = AF_INET;
  servAddr.sin_addr.s_addr = INADDR_ANY;
  servAddr.sin_port = htons(port);

  // Bind server to the socket
  if (bind(socketFd, (const struct sockaddr *)&servAddr, sizeof(servAddr)) <
      0) {
    perror("Bind failed.\n");
    exit(EXIT_FAILURE);
  }

  // main loop to receive packets and respond
  while (1) {
    int ret = receiveAndResponse(socketFd, buffer, numEntries, entries);
  }
}

/* This function receives a access request packet and responds to the client
 * Parameters: socketFd - the socket identifier
 *             buffer - buffer to store the received packet
 *             numEntries - total numbers of entries in the database
 *             entries - pointer to the entries
 * Return: 0 - successfully received
 *         1 - packet contains errors
 */
int receiveAndResponse(int socketFd, uint8_t *buffer, int numEntries,
                       struct DataInfo *entries) {
  struct sockaddr_in clientAddr;
  socklen_t len = sizeof(clientAddr);

  // receive packet
  int n = recvfrom(socketFd, buffer, MAXLEN, 0, (struct sockaddr *)&clientAddr,
                   &len);

  uint8_t data[MAXLEN]; // buffer to store the data
  uint8_t packet[MAXLEN]; // buffer to store the outgoing packet
  int clientId, dataLen, segNo;
  // read the packet
  enum PACKET_ERROR ret =
      readPacket(buffer, n, &clientId, data, &dataLen, &segNo);

  if (ret == SUCCESS_ACC) {
    // successfully received the access request packet
    unsigned long int subNo;
    int techNo;
    // parse the data to get technology number and subscriber number
    parseData(data, &techNo, &subNo);

    // look for the subscriber in the database
    enum VERIFICATION_CODE result = SUB_NOT_EXIST;
    for (int i = 0; i < numEntries; i++) {
      if (entries[i].subNo == subNo && entries[i].techNo == techNo) {
        // found the subscriber number with the technology number
        if (entries[i].paidCondition == 1) {
          // paid
          result = ACC_OK;
        } else {
          // not paid
          result = NOT_PAID;
        }
        break;
      }
    }

    if (result == ACC_OK) {
      // send acknowledgment to the client that access is permitted
      printf("Subscriber %ld is permitted to use technology %dG.\n", subNo, techNo);
      int l = buildAccessOkPacket(packet, clientId, segNo, techNo, subNo);
      sendto(socketFd, packet, l, 0, (const struct sockaddr *)&clientAddr, len);
      return 0;
    } else if (result == SUB_NOT_EXIST) {
      // send subscriber does not exist on database message
      printf("Subscriber %ld with technology %dG is not found.\n", subNo, techNo);
      int l = buildNotExistPacket(packet, clientId, segNo, techNo, subNo);
      sendto(socketFd, packet, l, 0, (const struct sockaddr *)&clientAddr, len);
      return 0;
    } else if (result == NOT_PAID) {
      // send subscriber has not paid message
      printf("Subscriber %ld has not paid to use technology %dG\n", subNo, techNo);
      int l = buildNotPaidPacket(packet, clientId, segNo, techNo, subNo);
      sendto(socketFd, packet, l, 0, (const struct sockaddr *)&clientAddr, len);
      return 0;
    }
  }

  return 1;
}

/* This function reads the entries in the database
 * Parameters: fileName - name of the file
 *             entries - pointer to the entries array
 * Return: total numbers of entries or -1 if error occurs.
 */
int readEntries(char *fileName, struct DataInfo *entries) {
  int totalEntries = 0;
  FILE *fp;
  // open the file
  fp = fopen(fileName, "r");
  if (fp == NULL) {
    return -1;
  }
  // read in entries from the database
  while (EOF != fscanf(fp, "%ld %d %d\n", &entries[totalEntries].subNo,
                       &entries[totalEntries].techNo,
                       &entries[totalEntries].paidCondition)) {
    totalEntries++;
  }
  fclose(fp);
  return totalEntries;
}
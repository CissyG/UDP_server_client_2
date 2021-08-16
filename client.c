/*************************************************************************
 * COEN 233 Assignment 2 - client.c
 *
 * Name: Xin Guan
 * Student ID: 1610150
 * Date: 05/29/2021
 *
 * This file defines a client that sends multiple access requests to a
 * server and receives responses from the server. It will resend the packet
 * multiple times if it does not receive the response in ack_timer.
 ************************************************************************/
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include "myPacket.h"

// function prototype
void sendAndWaitForResponse(int socketFd, uint8_t *packet, int packetLen,
                           struct sockaddr_in *servAddr);

int main() {
  int socketFd;
  struct sockaddr_in servAddr;

  // Create a socket
  socketFd = socket(AF_INET, SOCK_DGRAM, 0);
  if (socketFd < 0) {
    perror("Socket creation failed.");
    exit(EXIT_FAILURE);
  }

  // set server address and port
  memset(&servAddr, 0, sizeof(servAddr));
  int port = 2021;
  servAddr.sin_family = AF_INET;
  servAddr.sin_addr.s_addr = INADDR_ANY;
  servAddr.sin_port = htons(port);

  uint8_t packet[MAXLEN]; // buffer to store the outgoing packet
  int l;

  // request should be permitted
  printf("Requesting access %dG for subscriber %ld\n", 4, 4085546805UL);
  l = buildAccessPacket(packet, 0, 0, 4, 4085546805UL);
  sendAndWaitForResponse(socketFd, packet, l, &servAddr);
  // request that does not exist
  printf("Requesting access %dG for subscriber %ld\n", 4, 4085546806UL);
  l = buildAccessPacket(packet, 0, 0, 4, 4085546806UL);
  sendAndWaitForResponse(socketFd, packet, l, &servAddr);
  // request that has not been paid
  printf("Requesting access %dG for subscriber %ld\n", 3, 4086668821UL);
  l = buildAccessPacket(packet, 0, 0, 3, 4086668821UL);
  sendAndWaitForResponse(socketFd, packet, l, &servAddr);

  close(socketFd);
  return 0;
}

/* This function sends an access request to the server and wait for
 * response from the server. It will resend 3 times if it does not
 * receive a response within 3 seconds.
 * Parameters: socketFd - the socket identifier
 *             packet - buffer with the outgoing packet
 *             packetLen - the length of the packet
 *             servAddr = the address of server
 */
void sendAndWaitForResponse(int socketFd, uint8_t *packet, int packetLen,
                           struct sockaddr_in *servAddr) {
  uint8_t buffer[MAXLEN]; // buffer to store the received packet
  uint8_t data[MAXLEN]; // dummy data buffer
  int clientId, dataLen, segNo;
  socklen_t len;

  // resend loop if we do not receive the packet
  int count = 0;
  while (count < 3) {
    // send the request to server
    sendto(socketFd, packet, packetLen, 0, (const struct sockaddr *)servAddr,
           sizeof(*servAddr));

    // set wait time for 3 seconds
    fd_set sock_set;
    FD_ZERO(&sock_set);
    FD_SET(socketFd, &sock_set);
    struct timeval ack_timer = {0, 0};
    ack_timer.tv_sec = 3;
    if (select(socketFd + 1, &sock_set, NULL, NULL, &ack_timer) > 0) {
      // receive the packet within 3 seconds
      int n = recvfrom(socketFd, buffer, MAXLEN, 0, (struct sockaddr *)servAddr,
                       &len);
      enum PACKET_ERROR ret =
          readPacket(buffer, n, &clientId, data, &dataLen, &segNo);
      unsigned long int subNo;
      int techNo;
      parseData(data, &techNo, &subNo);
      if (ret == SUCCESS_ACK_OK) {
        // received access permission
        printf("Subscriber %ld is permitted to access technology %d.\n", subNo,
               techNo);
      } else if (ret == SUCCESS_NOT_EXIST) {
        // received subscriber does not exist message
        printf("Subscriber %ld with technology %d does not exist.\n", subNo,
               techNo);
      } else if (ret == SUCCESS_NOT_PAID) {
        // receive subscriber does not pay message
        printf("Subscriber %ld has not paid for technology %d.\n", subNo,
               techNo);
      }
      break;
    } else {
      // time out for the packet
      printf("Time out. ");
      count++;
      if (count < 3) {
        // resend the packet
        printf("Re-sending packet. \n");
      } else {
        // we have sent the packet three times, so do not resend anymore
        printf("Server does not respond\n");
      }
    }
  }
}
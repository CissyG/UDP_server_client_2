/*************************************************************************
 * COEN 233 Assignment 2 - myPacket.h
 *
 * Name: Xin Guan
 * Student ID: 1610150
 * Date: 05/29/2021
 *
 * This file is a header file. It contains a few enum definitions related
 * to the customized UDP packet. It also contains prototypes of functions
 * to read and build such packets.
 ************************************************************************/
#ifndef COEN233_PROJECT2_MYPACKET_H
#define COEN233_PROJECT2_MYPACKET_H

#include <stdlib.h>
#include <string.h>

#define MAXLEN 264 // maximum length of packet

// possible returns when reading a packet
enum PACKET_ERROR {
  LEN_MISMATCH = -4, // length mismatch error
  NO_END_ID = -3, // no end identifier error
  OTHER_ERROR = -1, // other errors
  SUCCESS_ACC = 0, // successfully received the access request
  SUCCESS_NOT_PAID = 1, // successfully received subscriber has not paid message
  SUCCESS_NOT_EXIST = 2, // successfully received subscriber does not exist message
  SUCCESS_ACK_OK = 3 // successfully received access permission message
};

// code for lookup result
enum VERIFICATION_CODE {
  ACC_OK = 1, // access is permitted
  SUB_NOT_EXIST = 2, // subscriber does not exist
  NOT_PAID = 3 // subscriber has not paid
};

/* This function reads a packet and returns whether it is successful.
 * Parameters: packet - buffer to store the packet
 *             nByte - the length of the packet
 *             clientId - pointer to the client Id
 *             data - pointer to data buffer for data packets
 *             dataLen - pointer to the length of data for data packets
 *             segNo - pointer to segment number for data packets
 * Return: see enum PACKET_ERROR for details
 */
enum PACKET_ERROR readPacket(uint8_t *packet, int nByte, int *clientId,
                             uint8_t *data, int *dataLen, int *segNo);

/* This function parses the data to get technology number and subscriber number
 * Parameters: data - pointer to the data to be parsed
 *             techNo - pointer to technology number
 *             subNo - pointer to subscriber number
 */
void parseData(uint8_t *data, int *techNo, unsigned long int *subNo);

/* This function builds an access request packet.
 * Parameters: packet - buffer to store the packet
 *             clientId - client Id
 *             segNo - segment number
 *             techNo - technology number
 *             subNo - subscriber number
 * Return: length of the packet
 */
int buildAccessPacket(uint8_t *packet, int clientId, int segmentNo, int techNo,
                      unsigned long int subNo);

/* This function builds a not paid packet.
 * Parameters: packet - buffer to store the packet
 *             clientId - client Id
 *             segNo - segment number
 *             techNo - technology number
 *             subNo - subscriber number
 * Return: length of the packet
 */
int buildNotPaidPacket(uint8_t *packet, int clientId, int segmentNo, int techNo,
                       unsigned long int subNo);

/* This function builds a not exist packet.
 * Parameters: packet - buffer to store the packet
 *             clientId - client Id
 *             segNo - segment number
 *             techNo - technology number
 *             subNo - subscriber number
 * Return: length of the packet
 */
int buildNotExistPacket(uint8_t *packet, int clientId, int segmentNo,
                        int techNo, unsigned long int subNo);

/* This function builds an access permitted packet.
 * Parameters: packet - buffer to store the packet
 *             clientId - client Id
 *             segNo - segment number
 *             techNo - technology number
 *             subNo - subscriber number
 * Return: length of the packet
 */
int buildAccessOkPacket(uint8_t *packet, int clientId, int segmentNo,
                        int techNo, unsigned long int subNo);
#endif // COEN233_PROJECT2_MYPACKET_H

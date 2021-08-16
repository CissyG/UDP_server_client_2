/*************************************************************************
 * COEN 233 Assignment 2 - myPacket.c
 *
 * Name: Xin Guan
 * Student ID: 1610150
 * Date: 05/29/2021
 *
 * This file defines functions to read and build customized UDP packets.
 ************************************************************************/
#include "myPacket.h"
#include <stdlib.h>
#include <string.h>

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
                             uint8_t *data, int *dataLen, int *segNo) {
  // verify the start of the packet
  if (packet[0] != 255 || packet[1] != 255) {
    return OTHER_ERROR;
  }

  *clientId = packet[2];
  if (packet[3] == 255 && packet[4] == 248) {
    // access request packet
    int segment = packet[5];
    *segNo = segment;

    int length = packet[6];
    // detect length mismatch packets
    if (length != (nByte - 9)) {
      return LEN_MISMATCH;
    }
    // copy data to the data buffer
    memcpy(data, packet + 7, length);
    *dataLen = length;

    // detect no end identifier packets
    if (packet[7 + length] != 255 || packet[8 + length] != 255) {
      return NO_END_ID;
    }
    return SUCCESS_ACC;
  } else if (packet[3] == 255 && packet[4] == 249) {
    // not paid packet
    int segment = packet[5];
    *segNo = segment;

    int length = packet[6];
    // detect length mismatch packets
    if (length != (nByte - 9)) {
      return LEN_MISMATCH;
    }
    // copy data to the data buffer
    memcpy(data, packet + 7, length);
    *dataLen = length;
    // detect no end identifier packets
    if (packet[7 + length] != 255 || packet[8 + length] != 255) {
      return NO_END_ID;
    }
    return SUCCESS_NOT_PAID;
  } else if (packet[3] == 255 && packet[4] == 250) {
    // not exist packet
    int segment = packet[5];
    *segNo = segment;

    int length = packet[6];
    // detect length mismatch packets
    if (length != (nByte - 9)) {
      return LEN_MISMATCH;
    }

    // copy data to the data buffer
    memcpy(data, packet + 7, length);
    *dataLen = length;

    // detect no end identifier packets
    if (packet[7 + length] != 255 || packet[8 + length] != 255) {
      return NO_END_ID;
    }
    return SUCCESS_NOT_EXIST;
  } else if (packet[3] == 255 && packet[4] == 251) {
    // access permitted packet
    int segment = packet[5];
    *segNo = segment;

    int length = packet[6];
    // detect length mismatch packets
    if (length != (nByte - 9)) {
      return LEN_MISMATCH;
    }

    // copy data to the data buffer
    memcpy(data, packet + 7, length);
    *dataLen = length;

    // detect no end identifier packets
    if (packet[7 + length] != 255 || packet[8 + length] != 255) {
      return NO_END_ID;
    }
    return SUCCESS_ACK_OK;
  } else {
    return OTHER_ERROR;
  }
}

/* This function parses the data to get technology number and subscriber number
 * Parameters: data - pointer to the data to be parsed
 *             techNo - pointer to technology number
 *             subNo - pointer to subscriber number
 */
void parseData(uint8_t *data, int *techNo, unsigned long int *subNo) {
  // technology number
  *techNo = data[0];
  // subscriber number
  *subNo = 0;
  for (int i = 0; i < 5; i++) {
    // from the right most byte to left
    *subNo += (((unsigned long int)data[5 - i]) << ((unsigned int)(8 * i)));
  }
}

/* This function builds an access request packet.
 * Parameters: packet - buffer to store the packet
 *             clientId - client Id
 *             segNo - segment number
 *             techNo - technology number
 *             subNo - subscriber number
 * Return: length of the packet
 */
int buildAccessPacket(uint8_t *packet, int clientId, int segmentNo, int techNo,
                      unsigned long int subNo) {
  // start identifier
  packet[0] = 255;
  packet[1] = 255;
  // client id
  packet[2] = clientId;
  // Acc_Per
  packet[3] = 255;
  packet[4] = 248;
  // segment number
  packet[5] = segmentNo;
  // length
  packet[6] = 6;
  //technology number
  packet[7] = techNo;
  // bytes of subscriber number
  for (int i = 4; i >= 0; i--) {
    packet[8 + i] = subNo % (1 << 8U);
    subNo = (subNo >> 8U);
  }
  // end identifier
  packet[13] = 255;
  packet[14] = 255;

  return 15;
}

/* This function builds a not paid packet.
 * Parameters: packet - buffer to store the packet
 *             clientId - client Id
 *             segNo - segment number
 *             techNo - technology number
 *             subNo - subscriber number
 * Return: length of the packet
 */
int buildNotPaidPacket(uint8_t *packet, int clientId, int segmentNo, int techNo,
                       unsigned long int subNo) {
  // start identifier
  packet[0] = 255;
  packet[1] = 255;
  // client id
  packet[2] = clientId;
  // Not paid
  packet[3] = 255;
  packet[4] = 249;
  // segment number
  packet[5] = segmentNo;
  // length
  packet[6] = 6;
  // technology number
  packet[7] = techNo;
  // bytes of subscriber number
  for (int i = 4; i >= 0; i--) {
    packet[8 + i] = subNo % (1 << 8);
    subNo = (subNo >> 8);
  }
  // end identifier
  packet[13] = 255;
  packet[14] = 255;

  return 15;
}

/* This function builds a not exist packet.
 * Parameters: packet - buffer to store the packet
 *             clientId - client Id
 *             segNo - segment number
 *             techNo - technology number
 *             subNo - subscriber number
 * Return: length of the packet
 */
int buildNotExistPacket(uint8_t *packet, int clientId, int segmentNo,
                        int techNo, unsigned long int subNo) {
  // start identifier
  packet[0] = 255;
  packet[1] = 255;
  // client id
  packet[2] = clientId;
  // Not exist
  packet[3] = 255;
  packet[4] = 250;
  // segment number
  packet[5] = segmentNo;
  // length
  packet[6] = 6;
  // technology number
  packet[7] = techNo;
  // bytes of subscriber number
  for (int i = 4; i >= 0; i--) {
    packet[8 + i] = subNo % (1 << 8);
    subNo = (subNo >> 8);
  }
  // end identifier
  packet[13] = 255;
  packet[14] = 255;

  return 15;
}

/* This function builds an access permitted packet.
 * Parameters: packet - buffer to store the packet
 *             clientId - client Id
 *             segNo - segment number
 *             techNo - technology number
 *             subNo - subscriber number
 * Return: length of the packet
 */
int buildAccessOkPacket(uint8_t *packet, int clientId, int segmentNo,
                        int techNo, unsigned long int subNo) {
  // start identifier
  packet[0] = 255;
  packet[1] = 255;
  // client id
  packet[2] = clientId;
  // Acc_Ok
  packet[3] = 255;
  packet[4] = 251;
  // segment number
  packet[5] = segmentNo;
  // length
  packet[6] = 6;
  // technology number
  packet[7] = techNo;
  // bytes of subscriber number
  for (int i = 4; i >= 0; i--) {
    packet[8 + i] = subNo % (1 << 8);
    subNo = (subNo >> 8);
  }
  // end identifier
  packet[13] = 255;
  packet[14] = 255;

  return 15;
}

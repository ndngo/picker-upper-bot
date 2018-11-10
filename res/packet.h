#pragma once
#ifndef _PACKET_H_
#define _PACKET_H_
#include <iostream>
#include <stdio.h>
#include <cstring>

/** specifies the types of command packets that can be sent to the
 * BeagleBone
 */
enum CmdType { DRIVE, SLEEP, STATUS, ARM, CLAW, ACK, NACK } ;

/**
 *  structure of the packet sent to the robot
 */
struct Header {
  unsigned int pktCount;
  unsigned char drive : 1;
  unsigned char status : 1;
  unsigned char sleep : 1;
  unsigned char arm : 1;
  unsigned char claw : 1;
  unsigned char ack : 1;
  unsigned char pad : 2;
  unsigned char length;
};

/**
 * the compiler allocates 8 bytes but only 6 is used
 */
const int HEADERSIZE = sizeof(Header) - 2;

struct CmdPacket {
  Header head;
  char * data;
  unsigned char CRC;
};

struct MotorBody {
  unsigned direction;
  unsigned char duration;
};

class Packet {
  CmdPacket cmdPacket;
  char * rawBuffer;
  int getSize();
public:
  Packet();
  Packet(char*);
  void setCmd(CmdType);
  void setBodyData(char*, int);
  void setPktCount(int);
  CmdType getCmd();
  int getLength();
  char* getBodyData();
  int getPktCount();
  bool checkCRC(char*, int);
  void calcCRC();
  char* genPacket();
  ~Packet();
};

#endif _PACKET_H_
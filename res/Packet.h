#pragma once
#ifndef _TCPPACKET_H_
#define _TCPPACKET_H_
#include <iostream>
#include <stdio.h>
#include <cstring>

/** specifies the types of command packets that can be sent to the
 * BeagleBone
 */
enum CmdType { DRIVE, SLEEP, STATUS, ARM, CLAW, ACK, NACK } ;

/**
 * direction constants for MotorBody
 */ 
const int FORWARD = 1;
const int BACKWARD = 2;
const int RIGHT = 3;
const int LEFT = 4;
const int UP = 5;
const int DOWN = 6;
const int OPEN = 7;
const int CLOSE = 8;

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
 * the compiler allocates 8 bytes to the size of Hder eabut only 6 is used
 * this corrects it
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

#endif
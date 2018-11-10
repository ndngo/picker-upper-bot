#include "packet.h"

/**
 * Packet initialized to safe state
 */
Packet::Packet() {
	cmdPacket.head.pktCount = 0;
	cmdPacket.head.drive = 0;
	cmdPacket.head.status = 0;
	cmdPacket.head.sleep = 0;
	cmdPacket.head.arm = 0;
	cmdPacket.head.claw = 0;
	cmdPacket.head.ack = 0;
	cmdPacket.head.pad = 0;
	cmdPacket.head.length = HEADERSIZE + sizeof(cmdPacket.CRC);
	cmdPacket.data = nullptr;
	cmdPacket.CRC = 0;
  rawBuffer = nullptr;
}

/**
 * parse raw buffer into Packet object
 * param: raw, address to where raw data to parse is stored
 */
Packet::Packet(char* raw) {
  char* p = raw;
  char* bitfields;

  // header
  memcpy(&cmdPacket.head.pktCount, p, sizeof(cmdPacket.head.pktCount));
  p += sizeof(cmdPacket.head.pktCount);

	bitfields = p;
	cmdPacket.head.drive = (*p & 0x01);
	cmdPacket.head.status = ((*p >> 1) & 0x01);
	cmdPacket.head.sleep = ((*p >> 2) & 0x01);
	cmdPacket.head.arm = ((*p >> 3) & 0x01);
	cmdPacket.head.claw = ((*p >> 4) & 0x01);
	cmdPacket.head.ack = ((*p >> 5) & 0x01);
	cmdPacket.head.pad = 0x00;
  p += sizeof(char);

  memcpy(&cmdPacket.head.length, p, sizeof(cmdPacket.head.length));
  
  // packet size is determined by the type of packet
  int size = getSize();

  // ACK packets are always size 0
  // header

  p += sizeof(char);

  // body
  cmdPacket.data = new char[size];
  memcpy(cmdPacket.data, p, size);
  p += size;

  // tail
  memcpy(&cmdPacket.CRC, p, sizeof(char));
}

/**
 * Sets the bitfield flags to match specified CmdType
 * param: cmdtype, the command type of the packet
 */
void Packet::setCmd(CmdType cmdtype) {
  switch(cmdtype) {
    case DRIVE:
      cmdPacket.head.drive = 1;
      cmdPacket.head.status = 0;
      cmdPacket.head.sleep = 0;
      cmdPacket.head.arm = 0;
      cmdPacket.head.claw = 0;
      cmdPacket.head.ack = 0;
      cmdPacket.head.length = HEADERSIZE + 2 + sizeof(cmdPacket.CRC);
      break;
    case SLEEP:
      cmdPacket.head.drive = 0;
      cmdPacket.head.status = 0;
      cmdPacket.head.sleep = 1;
      cmdPacket.head.arm = 0;
      cmdPacket.head.claw = 0;
      cmdPacket.head.ack = 0;
      cmdPacket.head.length = HEADERSIZE + 2 + sizeof(cmdPacket.CRC);
      break;
    case ARM:
      cmdPacket.head.drive = 0;
      cmdPacket.head.status = 0;
      cmdPacket.head.sleep = 0;
      cmdPacket.head.arm = 1;
      cmdPacket.head.claw = 0;
      cmdPacket.head.ack = 0;
      cmdPacket.head.length = HEADERSIZE + 2 + sizeof(cmdPacket.CRC);
      break;
    case CLAW:
      cmdPacket.head.drive = 1;
      cmdPacket.head.status = 0;
      cmdPacket.head.sleep = 0;
      cmdPacket.head.arm = 0;
      cmdPacket.head.claw = 1;
      cmdPacket.head.ack = 0;
      cmdPacket.head.length = HEADERSIZE + 2 + sizeof(cmdPacket.CRC);
      break;
    case ACK:
      cmdPacket.head.ack = 1;
      cmdPacket.head.length = HEADERSIZE + 2 + sizeof(cmdPacket.CRC);
      break;
  }
}

/**
 * Configres the body of the packet
 */
void Packet::setBodyData(char* raw, int size) {
  delete cmdPacket.data;
  cmdPacket.data = new char[size];
  cmdPacket.head.length = HEADERSIZE + size + sizeof(cmdPacket.CRC);
  memcpy(cmdPacket.data, raw, size);
}

/**
 * Sets the packet ID
 */
void Packet::setPktCount(int count) {
  cmdPacket.head.pktCount = count;
}

/**
 * Checks bitfield configuration and returns its associated command type
 * return: CmdType associated with bitfiend configuration,
 * return: NACK if bitfield is improperly configured
 */
CmdType Packet::getCmd() {
  char* p = (char*)&cmdPacket + sizeof(cmdPacket.head.pktCount);

  if	    (*p & 0x01)		   { return DRIVE; }
	else if ((*p >> 1) & 0x01) { return STATUS; }
	else if ((*p >> 2) & 0x01) { return SLEEP; }
	else if ((*p >> 3) & 0x01) { return ARM; }
	else if ((*p >> 4) & 0x01) { return CLAW; }
  else if ((*p >> 5) & 0x01) { return ACK; }
  else { return NACK; }
}

int Packet::getLength() {
  return cmdPacket.head.length;
}

char* Packet::getBodyData() {
  return cmdPacket.data;
}

int Packet::getPktCount() {
  return cmdPacket.head.pktCount;
}

/**
 * Generates the CRC value of the data and compares it to the known CRC
 * return: true if calculated and known CRC match
 * else false
 */
bool Packet::checkCRC(char* raw, int size) {
  char* p = (char*)&cmdPacket;
  unsigned char count = 0;
  for(int i= 0; i < size - 1;i++) {
    for(int j = 0; j < 8; j++) {
      ((*p >> j) & 0x01) ? count++ : count;
    }
  }
  return (count == *p);
}

/**
 * Generates the CRC packet and stores it in the CRC field
 */

void Packet::calcCRC() {
  char* p = (char*)&cmdPacket;
  unsigned char count = 0;
  for(int i = 0; i < HEADERSIZE; i++) {
    for(int j = 0; j < 8; j++) {
      ((*p >> j) & 0x01) ? count++ : count;
    }
    p++;
  }

  int size = getSize();

  p = cmdPacket.data;
  for(int i = 0; i < size; i++) {
    for(int j = 0; j < 8; j++) {
      ((*p >> j) & 0x01) ? count++ : count;
    }
    p++;
  }
  cmdPacket.CRC = count;
}

int Packet::getSize() {
  int size = 0;
  switch(getCmd()) {
    case ACK:
    case NACK:
    case SLEEP:
      size = 0;
      break;
    case DRIVE:
    case ARM:
    case CLAW:
      size = 2;
      break;
    case STATUS:
      size = cmdPacket.head.length - HEADERSIZE - sizeof(cmdPacket.CRC);
      break;
  }
  
  return size;
}

/**
 * Serializes the packet
 */
char* Packet::genPacket() {
  int size = getSize();

  rawBuffer = new char[cmdPacket.head.length];
  memset(rawBuffer, 0, cmdPacket.head.length);
  char* p = rawBuffer;

  //header
  memcpy(p, &cmdPacket.head, HEADERSIZE);
  p += HEADERSIZE;

  // body
  memcpy(p, cmdPacket.data, size);
  p += size;

  // tail
  memcpy(p, &cmdPacket.CRC, sizeof(cmdPacket.CRC));

  return rawBuffer;
}

/**
 * Destructor
 */
Packet::~Packet() {
  delete cmdPacket.data;
  delete rawBuffer;
  rawBuffer = nullptr;
  cmdPacket.data = nullptr;
}
/*
NAME:        ServerHelper.h
DESCRIPTION: This program is mainly for define include files, global variables, and function declaration
*/

/* Include files */ 

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/time.h>
#include <stdint.h>
#include <stdlib.h>
#include <strings.h>
#include <arpa/inet.h>

/* Global variable definitions */ 

extern const uint16_t StartOfPacketIdentifier;
extern const uint16_t EndOfPacketIdentifier;
extern const uint8_t ClientId;
extern const uint8_t Length;
extern const uint8_t WrongLength;
extern const uint16_t DATA ;
extern const uint16_t ACK ;
extern const uint16_t REJECT ;
extern const uint16_t REJECTOutOfSequence ;
extern const uint16_t REJECTLengthMismatch ;
extern const uint16_t REJECTEndOfPacketMissing ;
extern const uint16_t REJECTDuplicatePacket;

enum result
{
    OK = 0,
    OutOfSequence,
    LengthMismatch,
    EndOfPacketMissing,
    DuplicatePacket
};

/* function definitions */ 

char* ErrorPack(uint8_t id, char segNo, uint16_t rejectSubCode, int* cnt);
char* ACKPack(uint8_t id, char segNo, int* cnt);
int DataPacketFormatMatch(char* packet, int size, uint8_t *id, char *segNo);















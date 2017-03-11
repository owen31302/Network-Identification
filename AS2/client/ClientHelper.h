/*
NAME:        ClientHelper.h
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

extern const uint16_t Acc_Per;
extern const uint8_t _2G;
extern const uint8_t _3G;
extern const uint8_t _4G;
extern const uint8_t _5G;
extern const uint16_t Not_paid;
extern const uint16_t Not_exist;
extern const uint16_t Access_OK;

/* structure declaration */
struct Database{
   uint32_t number;
   uint8_t Technology;
   int paid;
} ; 

extern const struct Database users[4];


/* function definitions */ 

void ResponseCheck(char* packet);
char* sendPacket(int sockClient, char* packet, int packetSize, struct sockaddr_in server, int* size);
char* pack(int segNo, uint32_t SourceSubscriberNo, uint8_t Technology, int* cnt);
void TeleResponseCheck(char* packet, int size);



/*
NAME:        ServerHelper.c
DESCRIPTION: This program is mainly for (1) initialize the global variable in the header file.
										(2)	implementation of those function defined in the header file
*/

#include "ServerHelper.h"

const uint16_t StartOfPacketIdentifier = 0xAAAA;
const uint16_t EndOfPacketIdentifier = 0xFFFF;
const uint8_t ClientId = 0xCC;
const uint8_t Length = 0xFF;
const uint8_t WrongLength = 0xFA;
const uint16_t DATA = 0XFFF1;
const uint16_t ACK = 0XFFF2;
const uint16_t REJECT = 0XFFF3;
const uint16_t REJECTOutOfSequence = 0XFFF4;
const uint16_t REJECTLengthMismatch = 0XFFF5;
const uint16_t REJECTEndOfPacketMissing = 0XFFF6;
const uint16_t REJECTDuplicatePacket = 0XFFF7;


char* ErrorPack(uint8_t id, char segNo, uint16_t rejectSubCode, int* cnt){

	*cnt = sizeof(StartOfPacketIdentifier)+
					sizeof(id)+
					sizeof(REJECT)+
					sizeof(rejectSubCode)+
					sizeof(segNo)+
					sizeof(EndOfPacketIdentifier);

	char *packet = malloc(*cnt * sizeof(char));

	// start
	int shift = 0;
	memcpy(packet + shift, &StartOfPacketIdentifier, sizeof(StartOfPacketIdentifier));
	
	// id
	shift += sizeof(StartOfPacketIdentifier);
	memcpy(packet + shift, &id, sizeof(id));
	
	// REJECT
	shift += sizeof(id);
	memcpy(packet + shift, &REJECT, sizeof(REJECT));
	
	// REJECT subcode
	shift += sizeof(REJECT);
	memcpy(packet + shift, &rejectSubCode, sizeof(rejectSubCode));
	
	// Received segment no.
	shift += sizeof(rejectSubCode);
	memcpy(packet + shift, &segNo, sizeof(segNo));
	
	// End
	shift += sizeof(segNo);
	memcpy(packet + shift, &EndOfPacketIdentifier, sizeof(EndOfPacketIdentifier));

	return packet;
}
	
char* ACKPack(uint8_t id, char segNo, int* cnt){

	*cnt = sizeof(StartOfPacketIdentifier)+
					sizeof(id)+
					sizeof(ACK)+
					sizeof(segNo)+
					sizeof(EndOfPacketIdentifier);

	char *packet = malloc(*cnt * sizeof(char));

	// start
	int shift = 0;
	memcpy(packet + shift, &StartOfPacketIdentifier, sizeof(StartOfPacketIdentifier));
	
	// id
	shift += sizeof(StartOfPacketIdentifier);
	memcpy(packet + shift, &id, sizeof(id));
	
	// ACK
	shift += sizeof(id);
	memcpy(packet + shift, &ACK, sizeof(ACK));
	
	// Received segment no.
	shift += sizeof(ACK);
	memcpy(packet + shift, &segNo, sizeof(segNo));
	
	// End
	shift += sizeof(segNo);
	memcpy(packet + shift, &EndOfPacketIdentifier, sizeof(EndOfPacketIdentifier));

	return packet;
}

int DataPacketFormatMatch(char* packet, int size, uint8_t *id, char *segNo){

	/* ---- parse the packet according to the packet strcuture ---- */
	// start
	uint16_t startOfPacket;
	int shift = 0;
	memcpy(&startOfPacket, packet + shift, sizeof(startOfPacket));
	//printf("This is start : %x\n", startOfPacket);
	// id
	uint8_t ClientId;
	shift += sizeof(startOfPacket);
	memcpy(&ClientId, packet + shift, sizeof(ClientId));
	*id = ClientId;
	//printf("This is ClientId : %x\n", ClientId);
	// data type
	uint16_t dataType;
	shift += sizeof(ClientId);
	memcpy(&dataType, packet + shift, sizeof(dataType));
	//printf("This is dataType : %x\n", dataType);
	// seg no
	char packSegNo;
	shift += sizeof(dataType);
	memcpy(&packSegNo, packet + shift, sizeof(packSegNo));
	*segNo = packSegNo;
	//printf("This is packSegNo : %x\n", packSegNo);
	// length
	uint8_t Length;
	shift += sizeof(packSegNo);
	memcpy(&Length, packet + shift, sizeof(Length));
	//printf("This is Length : %x\n", Length);
	// payload
	shift += sizeof(Length);
	int payloadStart = shift;
		// do nothing
	
	// end
	uint16_t endOfPacket;
	int payloadEnd = size - sizeof(endOfPacket);
	//printf("payloadEnd: %d\n", payloadEnd);
	memcpy(&endOfPacket, packet + payloadEnd, sizeof(endOfPacket));
	//printf("This is endOfPacket : %x\n", endOfPacket);
	

	// payload calculation
	if(payloadEnd - payloadStart != (int) Length){
		return LengthMismatch;
	}

	// end identifier check
	if(endOfPacket != EndOfPacketIdentifier){
		return EndOfPacketMissing;
	}

	// success
	return OK;

}
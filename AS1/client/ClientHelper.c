/*
NAME:        ClientHelper.c
DESCRIPTION: This program is mainly for (1) initialize the global variable in the header file.
										(2)	implementation of those function defined in the header file
*/


#include "ClientHelper.h"

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

/* ---- ResponseCheck is used for check the response from server: ACK, Error(subcode) ---- */
void ResponseCheck(char* packet){

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
	//printf("This is ClientId : %x\n", ClientId);
	
	// ACK or reject
	uint16_t rejOrACK;
	shift += sizeof(ClientId);
	memcpy(&rejOrACK, packet + shift, sizeof(rejOrACK));
	//printf("This is rejOrACK : %x\n", rejOrACK);
	
	// reject subcode
	if(rejOrACK == REJECT){
		uint16_t subcode;
		shift += sizeof(rejOrACK);
		memcpy(&subcode, packet + shift, sizeof(subcode));
		switch(subcode){
			case REJECTOutOfSequence:
				printf("*** REJECTOutOfSequence\n");
				break;
			case REJECTLengthMismatch:
				printf("*** REJECTLengthMismatch\n");
				break;
			case REJECTEndOfPacketMissing:
				printf("*** REJECTEndOfPacketMissing\n");
				break;
			case REJECTDuplicatePacket:
				printf("*** REJECTDuplicatePacket\n");
				break;
		}
		shift += sizeof(subcode);
	}else{
		printf("*** Receive ACK.\n");
		shift += sizeof(rejOrACK);	
	}

	// seg no
	char packSegNo;
	memcpy(&packSegNo, packet + shift, sizeof(packSegNo));
	//printf("This is packSegNo : %x\n", packSegNo);
	shift += sizeof(packSegNo);	

	// end
	uint16_t endOfPacket;
	memcpy(&endOfPacket, packet + shift, sizeof(endOfPacket));
	//printf("This is endOfPacket : %x\n", endOfPacket);

}

/* ---- sendPacket is used for sending the packet, if server is not responding, will resend the packet for three times ---- */
char* sendPacket(int sockClient, char* packet, int packetSize, struct sockaddr_in server){
	int size = 10;
	char* recBuf = malloc(size * sizeof(char));

	for(int i = 0; i < 4; i++){
		sendto(sockClient, packet, packetSize, 0, (struct sockaddr *) &server, sizeof(server) );
		int rc = recv(sockClient, recBuf, size, 0);
		
		if(rc>0){
			return recBuf;
		}else if(i!=0 && rc <= 0){
			printf("Resend packet to server %d\n", i);	
		}
	}
	return NULL;
}

/* ---- pack is used for building the packet in the specific format ---- */
char* pack(int segNo, int* cnt){

	*cnt = sizeof(StartOfPacketIdentifier)+
					sizeof(ClientId)+
					sizeof(DATA)+
					1+
					sizeof(Length)+
					(int)Length+
					sizeof(EndOfPacketIdentifier);

	char *packet = malloc(*cnt * sizeof(char));

	// start
	int shift = 0;
	memcpy(packet + shift, &StartOfPacketIdentifier, sizeof(StartOfPacketIdentifier));
	
	// id
	shift += sizeof(StartOfPacketIdentifier);
	memcpy(packet + shift, &ClientId, sizeof(ClientId));
	
	// data type
	shift += sizeof(ClientId);
	memcpy(packet + shift, &DATA, sizeof(DATA));
	
	// seg no
	shift += sizeof(DATA);
	*(packet + shift) = segNo;
	
	// length
	shift++;
	// ---- Error Test: Length mismatch
	//memcpy(packet + shift, &WrongLength, sizeof(WrongLength));
	memcpy(packet + shift, &Length, sizeof(Length));
	
	// payload
	shift += sizeof(Length);
		// do nothing
	
	// end
	// ---- Error Test: Lost of end identifier
	//shift += (int)Length;
	//memcpy(packet + shift, &EndOfPacketIdentifier, sizeof(EndOfPacketIdentifier));

	return packet;
}
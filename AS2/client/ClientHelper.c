/*
NAME:        ClientHelper.c
DESCRIPTION: This program is mainly for (1) initialize the global variable in the header file.
										(2)	implementation of those function defined in the header file
*/


#include "ClientHelper.h"

const uint16_t StartOfPacketIdentifier = 0xFFFF;
const uint16_t EndOfPacketIdentifier = 0xFFFF;
const uint8_t ClientId = 0xFF;
const uint8_t Length = 0xFF;
const uint8_t WrongLength = 0xFA;
const uint16_t DATA = 0XFFF1;
const uint16_t ACK = 0XFFF2;
const uint16_t REJECT = 0XFFF3;
const uint16_t REJECTOutOfSequence = 0XFFF4;
const uint16_t REJECTLengthMismatch = 0XFFF5;
const uint16_t REJECTEndOfPacketMissing = 0XFFF6;
const uint16_t REJECTDuplicatePacket = 0XFFF7;

const uint16_t Acc_Per = 0xFFF8;
const uint8_t _2G = 0x02;
const uint8_t _3G = 0x03;
const uint8_t _4G = 0x04;
const uint8_t _5G = 0x05;
const uint16_t Not_paid = 0xFFF9;
const uint16_t Not_exist = 0xFFFA;
const uint16_t Access_OK = 0xFFFB;

// Database
// 4085546805 -> F3847F35
// 4086668821 -> F3959E15
// 4086808821 -> F397C0F5
const struct Database users[4] = {{0xF3847F35, _4G, 1}, {0xF3959E15, _3G, 0}, {0xF397C0F5, _2G, 1}, {0x12345678, _2G, 1}}; // 1 paid 0 unpaid

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
char* sendPacket(int sockClient, char* packet, int packetSize, struct sockaddr_in server, int *size){
	int recBufSize = 300;
	char* recBuf = malloc(recBufSize * sizeof(char));

	for(int i = 0; i < 4; i++){
		sendto(sockClient, packet, packetSize, 0, (struct sockaddr *) &server, sizeof(server) );
		int rc = recv(sockClient, recBuf, recBufSize, 0);
		*size = rc;
		if(rc>0){
			return recBuf;
		}else if(i!=0 && rc <= 0){
			printf("Resend packet to server %d\n", i);	
		}
	}
	return NULL;
}

/* ---- pack is used for building the packet in the specific format ---- */
char* pack(int segNo, uint32_t SourceSubscriberNo, uint8_t Technology, int* cnt){

	*cnt = sizeof(StartOfPacketIdentifier)+
					sizeof(ClientId)+
					sizeof(Acc_Per)+
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
	
	// Acc_Per
	shift += sizeof(ClientId);
	memcpy(packet + shift, &Acc_Per, sizeof(Acc_Per));

	// seg no
	shift += sizeof(Acc_Per);
	*(packet + shift) = segNo;
	
	// length
	shift++;
	memcpy(packet + shift, &Length, sizeof(Length));
	
	// Technology
	shift += sizeof(Length);
	memcpy(packet + shift, &Technology, sizeof(Technology));

	// SourceSubscriberNo
	shift += sizeof(Technology);
	memcpy(packet + shift, &SourceSubscriberNo, sizeof(SourceSubscriberNo));
	
	// end
	shift += ((int)Length - sizeof(Technology));
	memcpy(packet + shift, &EndOfPacketIdentifier, sizeof(EndOfPacketIdentifier));

	return packet;
}

/* ---- pack is used for parse the incoming packet ---- */
void TeleResponseCheck(char* packet, int size){

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
	
	// Acc_Per
	uint16_t Access;
	shift += sizeof(ClientId);
	memcpy(&Access, packet + shift, sizeof(Access));
	switch(Access){
		case Not_paid:
			printf("You need to pay!\n");
			break;
		case Not_exist:
			printf("Not exist!\n");
			break;
		case Access_OK:
			printf("Access OK!\n");
			break;
	}
	
	// seg no
	char packSegNo;
	shift += sizeof(Access);
	memcpy(&packSegNo, packet + shift, sizeof(packSegNo));
	//printf("This is packSegNo : %x\n", packSegNo);
	
	// length
	uint8_t Length;
	shift += sizeof(packSegNo);
	memcpy(&Length, packet + shift, sizeof(Length));
	//printf("This is Length : %d\n", (int)Length);
	
	// Technology
	shift += sizeof(Length);
	int payloadStart = shift;
	uint8_t Technology;
	memcpy(&Technology, packet + shift, sizeof(Technology));
	printf("This is Technology : %x\n", Technology);
	
	// SourceSubscriberNo
	shift += sizeof(Technology);
	uint32_t SubscriberNo;
	memcpy(&SubscriberNo, packet + shift, sizeof(uint32_t));
	printf("This is SubscriberNo : %ld\n", (long)SubscriberNo);
	
	// end
	uint16_t endOfPacket;
	int payloadEnd = size - sizeof(endOfPacket);
	memcpy(&endOfPacket, packet + payloadEnd, sizeof(endOfPacket));
	//printf("This is endOfPacket : %x\n", endOfPacket);
}













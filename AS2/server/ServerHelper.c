/*
NAME:        ServerHelper.c
DESCRIPTION: This program is mainly for (1) initialize the global variable in the header file.
										(2)	implementation of those function defined in the header file
*/

#include "ServerHelper.h"

const uint16_t StartOfPacketIdentifier = 0xFFFF;
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
const struct Database users[3] = {{0xF3847F35, _4G, 1}, {0xF3959E15, _3G, 0}, {0xF397C0F5, _2G, 1}}; // 1 paid 0 unpaid

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

int DataPacketFormatMatch(char* packet, int size, uint8_t *id, char *segNo, uint8_t* Technology, uint32_t* SubscriberNo){

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
	printf("This is ClientId : %x\n", ClientId);
	
	// Acc_Per
	uint16_t Access;
	shift += sizeof(ClientId);
	memcpy(&Access, packet + shift, sizeof(Access));
	printf("This is Access : %x\n", Access);
	
	// seg no
	char packSegNo;
	shift += sizeof(Access);
	memcpy(&packSegNo, packet + shift, sizeof(packSegNo));
	*segNo = packSegNo;
	//printf("This is packSegNo : %x\n", packSegNo);
	
	// length
	uint8_t Length;
	shift += sizeof(packSegNo);
	memcpy(&Length, packet + shift, sizeof(Length));
	//printf("This is Length : %d\n", (int)Length);
	
	// Technology
	shift += sizeof(Length);
	int payloadStart = shift;
	memcpy(Technology, packet + shift, sizeof(uint8_t));
	printf("This is Technology : %x\n", *Technology);
	
	// SourceSubscriberNo
	shift += sizeof(uint8_t);
	memcpy(SubscriberNo, packet + shift, sizeof(uint32_t));
	printf("This is SubscriberNo : %ld\n", (long)*SubscriberNo);
	
	// end
	uint16_t endOfPacket;
	int payloadEnd = size - sizeof(endOfPacket);
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

	// success, and check the user request
	for(int i = 0; i < sizeof(users); i++){
		if(*SubscriberNo == users[i].number){
			if(*Technology == users[i].Technology){
				if(users[i].paid){
					// Permitted
					return Permitted;
				}else{
					return NotPaid;
				}
			}else{
				// user not found 
				return NotExist;
			}
		}
	}
	return NotExist;
}

char* ACKSubscriber(uint8_t id, char segNo, uint16_t result, uint8_t Technology, uint32_t SubscriberNo, int* cnt){

	*cnt = sizeof(StartOfPacketIdentifier)+
					sizeof(id)+
					sizeof(result)+
					sizeof(segNo)+
					sizeof(Length)+
					(int)Length+
					sizeof(EndOfPacketIdentifier);

	char *packet = malloc(*cnt * sizeof(char));

	// start
	int shift = 0;
	memcpy(packet + shift, &StartOfPacketIdentifier, sizeof(StartOfPacketIdentifier));
	
	// id
	shift += sizeof(StartOfPacketIdentifier);
	memcpy(packet + shift, &id, sizeof(id));
	
	// result
	shift += sizeof(id);
	memcpy(packet + shift, &result, sizeof(result));
	
	// Received segment no.
	shift += sizeof(result);
	memcpy(packet + shift, &segNo, sizeof(segNo));
	
	// length
	shift += sizeof(segNo);
	memcpy(packet + shift, &Length, sizeof(Length));
	
	// Technology
	shift += sizeof(Length);
	memcpy(packet + shift, &Technology, sizeof(Technology));

	// SubscriberNo
	shift += sizeof(Technology);
	memcpy(packet + shift, &SubscriberNo, sizeof(SubscriberNo));
	
	// end
	shift += ((int)Length - sizeof(Technology));
	memcpy(packet + shift, &EndOfPacketIdentifier, sizeof(EndOfPacketIdentifier));

	return packet;
}


















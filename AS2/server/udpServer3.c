/*
Author:		 Yu-Cheng Lin
NAME:        udpServer2.c 
DESCRIPTION: This program is mainly for implementing reliable UDP transmission.
			 Server will receive the packet from the client.
			 Server will check the received file whether it is matched with the predefined format.
			 Server will send back the result to client.
*/

#include "ServerHelper.h"

int main(int argc, char **argv){
	int welcomeSocket;
	struct sockaddr_in server, client;
	int s_len = sizeof(client);;
	int rc;

	char packet[264];
	char* response;
	int responseSize;
	memset(packet, 0 ,sizeof(packet));
	uint8_t id;
	char segNo;
	int segCnt = 0;
	int lastSegNo = -1;

	uint32_t SourceSubscriberNo;
	uint8_t Technology;

	/* ---- Create the socket. The three arguments are: ---- */
	/* 1) Internet Domain 2) Datagram 3) Default protocol (UDP in this case) */
	welcomeSocket = socket(AF_INET, SOCK_DGRAM, 0);

	/* ---- Configure settings of the server address struct ---- */
	/* Address family = Internet */
	server.sin_family = AF_INET;
	/* Set port number, using htons to use proper btye order */
	server.sin_port = htons(12345);
	/* Set IP address to localhost */
	server.sin_addr.s_addr = htonl(INADDR_ANY);

	/* ---- bind info to socket ---- */
	bind( welcomeSocket, (struct sockaddr *) &server, sizeof(server));

	/* ---- wait for incoming message ---- */
	for(;;){
		int rv = recvfrom(welcomeSocket, &packet, sizeof(packet), 0, (struct sockaddr *) &client, (socklen_t *) &s_len);

		/* ---- send the message back to client ---- */
		printf("---- I got a message from client!\n");
		int resultCode = DataPacketFormatMatch(packet, rv, &id, &segNo, &Technology, &SourceSubscriberNo);


		/* ---- First, switch will check the packet length and the end identifier. ---- */
		/* ---- Second, if statement inside the OK case will check the sequence No. and duplicate. ---- */
		/* ---- Finally, if all the check point passed, will send ACK back to user. ---- */
		switch (resultCode){
			case OK:
				segCnt++;
				if((int)segNo == lastSegNo){
					printf("Duplicate!\n");
					response = ErrorPack(id, segNo, REJECTDuplicatePacket, &responseSize);
					sendto(welcomeSocket, response, responseSize, 0, (struct sockaddr *) &client, sizeof(client) );
				}

				if(segCnt == (int)segNo){
					printf("ACK.\n");
					response = ACKPack(id, segNo, &responseSize);
					sendto(welcomeSocket, response, responseSize, 0, (struct sockaddr *) &client, sizeof(client) );
					if(segCnt>5){
						segCnt = 1;
					}
				}else{
					printf("Wrong Sequence!\n");
					response = ErrorPack(id, segNo, REJECTOutOfSequence, &responseSize);
					sendto(welcomeSocket, response, responseSize, 0, (struct sockaddr *) &client, sizeof(client) );
				} 
				lastSegNo = (int)segNo;
				break;
			case LengthMismatch:
				printf("LengthMismatch found!\n");
				response = ErrorPack(id, segNo, REJECTLengthMismatch, &responseSize);
				sendto(welcomeSocket, response, responseSize, 0, (struct sockaddr *) &client, sizeof(client) );
				break;
			case EndOfPacketMissing:
				printf("EndOfPacketMissing found!\n");
				response = ErrorPack(id, segNo, REJECTEndOfPacketMissing, &responseSize);
				sendto(welcomeSocket, response, responseSize, 0, (struct sockaddr *) &client, sizeof(client) );
				break;
			case Permitted:
				printf("Permitted\n");
				response = ACKSubscriber(id, segNo, Access_OK, Technology, SourceSubscriberNo, &responseSize);
				sendto(welcomeSocket, response, responseSize, 0, (struct sockaddr *) &client, sizeof(client) );
				break;
			case NotPaid:
				printf("NotPaid\n");
				response = ACKSubscriber(id, segNo, Not_paid, Technology, SourceSubscriberNo, &responseSize);
				sendto(welcomeSocket, response, responseSize, 0, (struct sockaddr *) &client, sizeof(client) );
				break;
			case NotExist:
				printf("NotExist\n");
				response = ACKSubscriber(id, segNo, Not_exist, Technology, SourceSubscriberNo, &responseSize);
				sendto(welcomeSocket, response, responseSize, 0, (struct sockaddr *) &client, sizeof(client) );
				break;
		}
	}
	
	/* ---- close socket ---- */
	return 0;

}















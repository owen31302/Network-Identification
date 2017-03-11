/*
Author:		 Yu-Cheng Lin
NAME:        udpClient3.c 
DESCRIPTION: This program is the second part, client is verifying the Access Permission with server by implementing reliable UDP transmission.
			 This is mainly for checking (1) Subscriber Number (2) Corresponding Technology (3) this user paid or unpard for the service.
*/

#include "ClientHelper.h"

int main(int argc, char **argv){
	int sockClient;
	struct sockaddr_in server, client;
	int s_len = sizeof(client);;
	struct hostent *hp;
	struct timeval tv;

	char *packet;
	int packetSize = 0;
	char* recBuf;
	int recBufSize = 0;
	

	/* ---- Create the socket. The three arguments are: ---- */
	/* 1) Internet Domain 2) Datagram 3) Default protocol (UDP in this case) */
	sockClient = socket(AF_INET, SOCK_DGRAM, 0);

	/* ---- Configure settings of the client address struct ---- */
	/* Address family = Internet */
	client.sin_family = AF_INET;
	/* Set port number, using htons to use proper btye order */
	client.sin_port = htons(22345);
	/* Set IP address to localhost */
	hp = gethostbyname("localhost");
	bcopy( hp->h_addr, &(client.sin_addr.s_addr), hp->h_length);

	/* ---- bind info to socket ---- */
	bind( sockClient, (struct sockaddr *) &client, sizeof(client));

	/* ---- Configure settings of the server address struct ---- */
	/* Address family = Internet */
	server.sin_family = AF_INET;
	/* Set port number, using htons to use proper btye order */
	server.sin_port = htons(12345);
	/* Set IP address to localhost */
	bcopy( hp->h_addr, &(server.sin_addr.s_addr), hp->h_length);

	/* ---- Setup non blocking io, and specify the duration ---- */	
	tv.tv_usec = 0;
	tv.tv_sec = 2;
	setsockopt(sockClient, SOL_SOCKET, SO_RCVTIMEO, (char*) &tv, sizeof(struct timeval));
	
	/* ---- Check if server contains this subscriber and its status ---- */
	/* ---- send packet, and loop it 3 times if not receiving ACK or Error from Server ---- */
	for(int i = 0; i < 4; i++){
		printf("\n---- Check user %d with Server.\n", i+1);
		packet = pack(i+1, users[i].number, users[i].Technology, &packetSize);
		recBuf = sendPacket(sockClient, packet, packetSize, server, &recBufSize);
		if(recBuf != NULL){
			TeleResponseCheck(recBuf, recBufSize);
		}else{
			printf("Server does not respond.\n");
			break;
		}
	}

	printf("End of the program\n");	

	return 0;

}
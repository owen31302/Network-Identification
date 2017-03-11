/*
NAME:        udpClient2.c 
DESCRIPTION: This program is mainly for implementing reliable UDP transmission.
			 The client will resend the request three times if it does not receive the respose from the server within 2 secs.
			 The client will re-try for three times, so the maximum total duration will be 2*(1+3) = 8 sec.
*/

#include "ClientHelper.h"
//http://stackoverflow.com/questions/34120658/how-to-include-helper-functions-in-c

int main(int argc, char **argv){
	int sockClient;
	struct sockaddr_in server, client;
	int s_len = sizeof(client);;
	struct hostent *hp;
	struct timeval tv;

	char *packet;
	int packetSize = 0;
	char* recBuf;
	

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
	
	
	// send packet, and loop it 3 times if not receiving ACK or Error from Server
	for(int i = 0; i < 5; i++){
		printf("Send packet %d to Server.\n", i+1);
		
		packet = pack(i+1, &packetSize);

		// ---- Error Test: Not in sequence
		/*if(i == 4){
			printf("Change the pack sequence.\n");
			packet = pack(i+2, &packetSize);
		}*/

		// ---- Error Test: duplicate
		/*if(i == 2){
			printf("Change the pack sequence.\n");
			packet = pack(i, &packetSize);
		}*/

		recBuf = sendPacket(sockClient, packet, packetSize, server);
		if(recBuf != NULL){
			ResponseCheck(recBuf);
		}else{
			printf("Server does not respond.\n");
			break;
		}
	}
	

	printf("End of the program\n");	

	return 0;

}
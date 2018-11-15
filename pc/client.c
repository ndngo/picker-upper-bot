// Client side C/C++ program to demonstrate Socket programming 
#include <arpa/inet.h>
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#define PORT 8080 

int main(int argc, char *argv[]) 
{ 
	struct sockaddr_in address; 
    struct command
    {
        // w/a/s/d movement
        // e/r for pick up/putdown
        unsigned char movement[1];
        unsigned char distance[4];
    };
	int sock = 0, valread; 
	struct sockaddr_in serv_addr; 
	char test[1]; 
	char buffer[1024] = {0}; 
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
	{ 
		printf("\n Socket creation error \n"); 
		return -1; 
	} 

	memset(&serv_addr, '0', sizeof(serv_addr)); 

	serv_addr.sin_family = AF_INET; 
	serv_addr.sin_port = htons(PORT); 
	
	// Convert IPv4 and IPv6 addresses from text to binary form 
	if(inet_pton(AF_INET, argv[1], &serv_addr.sin_addr)<=0) 
	{ 
		printf("\nInvalid address/ Address not supported \n"); 
		return -1; 
	} 

	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
	{ 
		printf("\nConnection Failed \n"); 
		return -1; 
	} 
    struct command c;
	int d;

	while(1)
	{
		printf("Enter command wasd for movement er for pick up or down : ");
		scanf("%c", &c.movement);

		d = 0;
		printf("Enter distance if applicable (wasd movement) : ");
		scanf("%d", &d);
		
		//empty the input buffer
		getchar();
		//convert integer to unsigned char[4]
		sprintf(c.distance, "%d", d);

		char sendPacket[sizeof c];

		//used to clear distance buffer
		char empty[] = {'\0','\0','\0','\0'};

		//Copy the struct to the send packet
		memcpy( sendPacket, &c, sizeof c);
		
		//this actually clears the buffer of c.distance
		memcpy(c.distance, empty, sizeof c);

		send(sock , sendPacket, sizeof c , 0 ); 
		printf("Command sent.\n"); 

		//Receive ack from server
		valread = read( sock , buffer, 1024); 
		printf("%s\n",buffer ); 
	}
	return 0; 
} 


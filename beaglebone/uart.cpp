#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <iostream>
#include <chrono>
#include <thread>
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
// BB-UART serial information
#define PORTuart "/dev/ttyO4"
#define PORTtcp 8080 
#define BAUDRATE B115200

struct PktDef
{
  unsigned forward : 1;
  unsigned backwards : 1;
  unsigned left : 1;
  unsigned right : 1;
  unsigned armUp : 1;
  unsigned armDn : 1;
  unsigned gripOpen : 1;
  unsigned gripClose : 1;
};


void txBytes(int &uart, void *buffer, size_t size) {
  if (uart != -1) {
    std::cout << "sending: " << (char*)buffer << std::endl;
    int count = write(uart, buffer, size);
    if (count < 0) {
      std::cout << "error:failed to tx data" << std::endl;
    }
  }

}

void rxBytes(int &uart, void *buffer, size_t maxSize) {
  if (uart != -1) {
    int rxLength = read(uart, buffer, maxSize);
    if (rxLength == 0) {
      std::cout << "error: error occured during UART read" << "std::endl";
    }
  }
}

struct command
{
  // w/a/s/d movement
  // e/r for pick up/putdown
  unsigned char movement[1];
  unsigned char distance[4];
};

int main() {
  #pragma region Uart Connection setup
  // open uart connection
  int uart_stream = -1;
  uart_stream = open(PORTuart, O_RDWR);
  if (uart_stream == -1) {
    std::cout << "Failed to open BB-UART" << std::endl;
    return(-1);
  }

  std::cout << "BB-UART Opened" << std::endl;


  // configure BB-UART
  struct termios options;
  tcgetattr(uart_stream, &options);
  options.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD; //sets baud rate
  options.c_iflag = IGNPAR;
  options.c_oflag = 0;
  options.c_lflag = 0;
  tcflush(uart_stream, TCIFLUSH);
  tcsetattr(uart_stream, TCSANOW, &options);

  char rxBuffer[1];
  char txBuffer[1];
  #pragma endregion

  
  #pragma region tcp port/socket setup
  int server_fd, new_socket, valread; 
	struct sockaddr_in address; 
	int opt = 1; 
	int addrlen = sizeof(address); 
	char *hello = "Hello from server"; 
	
	// Creating socket file descriptor 
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
	{ 
		perror("socket failed"); 
		exit(EXIT_FAILURE); 
	} 
	
	// Forcefully attaching socket to the port 8080 
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, 
												&opt, sizeof(opt))) 
	{ 
		perror("setsockopt"); 
		exit(EXIT_FAILURE); 
	} 
	address.sin_family = AF_INET; 
	address.sin_addr.s_addr = INADDR_ANY; 
	address.sin_port = htons( PORTtcp ); 
	
	// Forcefully attaching socket to the port 8080 
	if (bind(server_fd, (struct sockaddr *)&address, 
								sizeof(address))<0) 
	{ 
		perror("bind failed"); 
		exit(EXIT_FAILURE); 
	} 
	if (listen(server_fd, 3) < 0) 
	{ 
		perror("listen"); 
		exit(EXIT_FAILURE); 
	} 
	if ((new_socket = accept(server_fd, (struct sockaddr *)&address, 
					(socklen_t*)&addrlen))<0) 
	{ 
		perror("accept"); 
		exit(EXIT_FAILURE); 
	} 
#pragma endregion
	
  struct command c;
	char buffer[sizeof c];
	while(1)
	{
		valread = read( new_socket , buffer, sizeof c); 
		printf("The buffer before we convert to struct: %s\n",buffer ); 
		int x = 0;
		for (int i = 1; i < sizeof buffer; i++){
			printf("Buffer stream index %d value : %c\n", i, buffer[i]);
            if(buffer[i] != '\0')
				x = x * 10 + buffer[i] - '0';
		}
		printf("The int value: %d\n", x);

		send(new_socket , hello , strlen(hello) , 0 ); 
		printf("Command has been received\n"); 
    
    txBuffer[0] = buffer[0];
    if( x == 0 )
      txBytes(uart_stream, txBuffer, sizeof(txBuffer));
    else
    {
      for(int i = 0; i < x; i++){
        txBytes(uart_stream, txBuffer, sizeof(txBuffer));
      }
    }
//    rxBytes(uart_stream, rxBuffer, sizeof(txBuffer));
//    std::cout << rxBuffer << std::endl;
    //std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
  return 0;
}

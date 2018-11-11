#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

// BB-UART serial information
#define PORT "/dev/ttyO4"
#define BAUDRATE B115200

// define protocol
struct PktDef {
  unsigned Forward :   1;
  unsigned Backwards : 1;
  unsigned Left :      1;
  unsigned Right :     1;
  unsigned ArmUp :     1;
  unsigned ArmDn :     1;
  unsigned GripOpen :  1;
  unsigned GripClose : 1;
};

void txBytes(int &uart, void *buffer, size_t size) {
  if(uart != -1) {
    int count = write(uart, buffer, size);
    if(count < 0) {
      std::cout << "ERROR: failed to Tx data" << std::endl;
    }
  }
}

void rxBytes(int &uart, void *buffer, size_t maxSize) {
  if(uart != -1) {
    int rx_length = read(uart, buffer, maxSize);
    if(rx_length == 0) {
      std::cout << "ERROR: An error occured during the UART read" << std::endl;
    }
  }

}

int main() {
  
  std::cout << "Hello world!" << std::endl;
  
  // opens the UART stream
  int uart_stream = -1;
  uart_stream = open(PORT, O_RDWR);
  if (uart_stream == -1) {
    std::cout << "Failed to open BB-UART" << std::endl;
    return(-1);
  }
  std::cout << "BB-UART Opened" << std::endl;  

  // configures UART connection
  struct termios options;
  tcgetattr(uart_stream, &options);
  options.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
  options.c_iflag = IGNPAR;
  options.c_oflag = 0;
  options.c_lflag = 0;
  tcflush(uart_stream, TCIFLUSH);
  tcsetattr(uart_stream, TCSANOW, &options);
  
  txBytes(uart_stream, (void*)"hello mr. robot", sizeof("hello mr. robot"));

  return 0;
}

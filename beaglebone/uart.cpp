#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <iostream>
#include <chrono>
#include <thread>

// BB-UART serial information
#define PORT "/dev/ttyO4"
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

char* receiveCommand() {
  char txBuffer[256];
  std::cin >> txBuffer[0];
  txBuffer[1] = '\0';
  return txBuffer;
 }

int main() {

  // open uart connection
  int uart_stream = -1;
  uart_stream = open(PORT, O_RDWR);
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

  char rxBuffer[256];

  while (1) {
    char* txBuffer = receiveCommand();
    txBytes(uart_stream, txBuffer, sizeof(txBuffer));
    rxBytes(uart_stream, rxBuffer, sizeof(txBuffer));
    std::cout << rxBuffer << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  return 0;
}

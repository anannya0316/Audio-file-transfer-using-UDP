//This code uses the Transmission Control Protocol (TCP) as the underlying transport protocol.
//TCP provides reliable, ordered, and error-checked delivery of data between applications running on hosts 
//communicating over an IP network.

//In the code, the socket is created using the socket() function with the SOCK_STREAM parameter, 
//which creates a stream socket for a reliable, connection-oriented protocol. The bind() function binds the 
//socket to a specific IP address and port number, and the listen() function puts the socket in a passive mode,
//waiting for an incoming connection request from a client.

//When a client connects to the server, the accept() function returns a new socket descriptor for the connection. 
//The serve_file() function then reads a filename from the client, opens the file in binary mode for reading using
// fopen() function and reads the contents of the file using fread() function. The contents of the file are then 
//written to the client using write() function.

//Since TCP is a reliable, connection-oriented protocol, it ensures that the data is delivered correctly and in 
//order. The server sends the data in small chunks (32 bytes in this case) to the client, and the client 
//acknowledges receipt of each chunk. If the client does not acknowledge the receipt of a chunk, the server 
//retransmits that chunk until it receives an acknowledgment.

//The code includes the necessary header files for socket programming and defines some constants for
// maximum file size and buffer size.

#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#define MAX 1000000
#define BUFFER_SIZE 32

//This function initializes the file transfer by reading the name of the file requested by the client from 
//the socket connection and returning it.
/*This code defines a function named init that takes an integer new_socket as an argument and returns 
a character pointer.

Within the function, a character array named buff is defined with a size of BUFFER_SIZE. The bzero function is
then used to set all the bytes in buff to zero. bzero is a function from the C standard library that sets a 
specified number of bytes in a buffer to zero.

A character pointer named filename is then declared and assigned memory using the malloc function, which 
dynamically allocates memory of size 100 bytes.

The read function is then used to read data from the new_socket file descriptor and store it in buff.
read is a function from the C standard library that reads a specified number of bytes from a file descriptor 
into a buffer.

Finally, the strcpy function is used to copy the contents of buff to filename, and filename is returned 
from the function.

Overall, this function initializes a buffer named buff by setting all its bytes to zero using bzero, reads 
data from a socket into buff, dynamically allocates memory for filename, and copies the data from buff to 
filename.*/

char *init(int new_socket)
{
  char buff[BUFFER_SIZE];
  bzero(buff, sizeof(buff));     //sets a specified number of bytes in a buffer to zero. 
  char *filename = (char *)malloc(100);
  read(new_socket, buff, sizeof(buff));
  strcpy(filename, buff);
  return (char *)filename;
}

//The serve_file() function takes the client's requested file name and opens the file for reading in binary mode
// ("rb"). If the file does not exist, an error message is printed, and the function exits.
//If the file exists, the function reads the file in chunks of size BUFFER_SIZE using fread() and sends each 
//chunk to the client using write() until the entire file has been sent. If an error occurs while writing to the
// socket, the function prints an error message and exits.
void serve_file(int new_socket){
  char buff[BUFFER_SIZE];
  bzero(buff, sizeof(buff));
  char filename[100];
  strcpy(filename, init(new_socket));
  printf("File requested by client -> %s\n", filename);
  FILE *fptr;
  if ((fptr = fopen(filename, "rb")) == NULL){                   //rb is read binary
    printf("Incorrect file name. No such file exists.\n");
    write(new_socket, buff, sizeof(buff));
    exit(EXIT_FAILURE);

  }
  
  while(1){
    size_t num_read = fread(buff, 1, BUFFER_SIZE, fptr); 
    if (num_read == 0) // end of file.

        break;
    int n = write(new_socket, buff, num_read);
    if (n < 0){ // Error
      perror("ERROR writing to socket");
      exit(1);

    }
    else if (n == 0) 
        break;

  }
  bzero(buff, sizeof(buff));
  fclose(fptr);

}

// Driver function

int main(int argc, char **argv){   //argc is no of command lines passed in code and argv stores the commands in an array
  int server_fd, new_socket, len;  //fd is file descriptor, new socket represents that new socket is created once client server connection is established, len stores length of socket address
  struct sockaddr_in servaddr, cli;    //internet socket address, address and port no
  int p_server_port = 8000;    //8000 is default port no
  if (argc == 2){
    p_server_port = atoi(argv[1]);      //converts i/p port no of string format to int
  }

  else {
    if (argc < 2)
      printf("Insufficient input parameters\n");
    else
      printf("Too many input parameters\n");
    exit(EXIT_FAILURE);

  }
  // Socket file descriptor

// initializes a socket using the socket() function with the SOCK_STREAM parameter,
// which creates a stream socket for a reliable, connection-oriented protocol. 
  server_fd = socket(AF_INET, SOCK_STREAM, 0);        //afnet is used to define that socket is created in IPv4
  if (server_fd == -1){
    printf("Socket creation failed...\n");
    exit(EXIT_FAILURE);
  }

  bzero(&servaddr, sizeof(servaddr));
  // Assign IP and PORT
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");     //special IP for loopback purpose
  servaddr.sin_port = htons(p_server_port);

  // Binding newly created socket to given IP
  if ((bind(server_fd, (struct sockaddr *)&servaddr, sizeof(servaddr))) != 0){
    printf("Socket bind failed...\n");
    exit(EXIT_FAILURE);
  }

  // server is ready to listen

  if ((listen(server_fd, 5)) != 0){    //5 is the max attempts to listen
    printf("Listen failed...\n");
    exit(EXIT_FAILURE);
  } 

  len = sizeof(cli);
  // Accept the data packet from client
  new_socket = accept(server_fd, (struct sockaddr *)&cli, &len);
  if (new_socket < 0){
    printf("Server accept failed...\n");
    exit(EXIT_FAILURE);
  }

  else{
    printf("Client connected at PORT -> %d\n", p_server_port);
  }

  // Function for chatting between client and server
  serve_file(new_socket);

  // Closing the socket after successful communication
  close(server_fd);

}
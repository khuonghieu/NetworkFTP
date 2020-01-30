// Name: Hieu Khuong
// CIS 4307 - Distributed Systems
// Spring 2020
// Homework 2 - Network Communication

#include <arpa/inet.h>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 9089
#define SERVERIP "127.0.0.1"

using namespace std;

int main(int argc, char *argv[]) {

  // Argument checking
  if (argc < 4) {
    cout << "Not enough arguments" << endl;
    return 1;
  }
  char *serverIP = argv[1];
  int port = atoi(argv[2]);
  char *fileName = argv[3];
  char *directory = argv[4];
  cout << "Server IP: " << serverIP << endl;
  cout << "Port number: " << port << endl;
  cout << "File name request is " << fileName << endl;
  cout << "Directory to save is " << directory << endl;

  // Network connection handler
  int client_socket, valread, stringLen, recvMsgSize;
  struct sockaddr_in serv_addr;
  char buffer[255];

  // Create socket
  if ((client_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
    printf("Socket creation error \n");
    return -1;
  }
  cout << "Done socket create" << endl;

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = inet_addr(serverIP);
  serv_addr.sin_port = htons(port);

  // Connect to server
  if (connect(client_socket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) <
      0) {
    printf("\nConnection Failed \n");
    return -1;
  }
  cout << "Connected to server" << serverIP << endl;

  // Send buffer to server
  stringLen = strlen(fileName);
  if (send(client_socket, fileName, stringLen, 0) != stringLen) {
    printf("Send different byte than expected \n");
    return -1;
  }
  cout << "Done sending filename " << fileName << endl;

  // Create file and download the content
  char *dirName = strcat(directory, "/");
  char *filePath = strcat(dirName, fileName);
  FILE *receiveFile = fopen(filePath, "w");
  cout << "Created file to write " << string(filePath) << endl;

  // Receive the file content
  while (recv(client_socket, buffer, sizeof(buffer), 0) != 0) {
    fprintf(receiveFile, buffer);
    memset(&buffer, 0, sizeof(buffer));
  }
  fclose(receiveFile);
  cout << "done writing" << endl;

  close(client_socket);

  return 0;
}
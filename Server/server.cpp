// Name: Hieu Khuong
// CIS 4307 - Distributed Systems
// Spring 2020
// Homework 2 - Network Communication

#include <arpa/inet.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <list>
#include <map>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define MAXSTRING 255
#define MAXCACHE 64000000

using namespace std;
namespace fs = std::filesystem;

// Trim a buffer string from whitespaces
const std::string WHITESPACE = " \n\r\t\f\v";
std::string ltrim(const std::string &s) {
  size_t start = s.find_first_not_of(WHITESPACE);
  return (start == std::string::npos) ? "" : s.substr(start);
}
std::string rtrim(const std::string &s) {
  size_t end = s.find_last_not_of(WHITESPACE);
  return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}
std::string trim(const std::string &s) { return rtrim(ltrim(s)); }

// Get file size
int getFileSize(FILE *fptr) {
  fseek(fptr, 0L, SEEK_END);
  int sz = ftell(fptr);
  rewind(fptr);
  return sz;
}
int getFileSizeWithName(const char *fileName, char *directory) {
  string filePath = string(fs::current_path()) + "/" + string(directory) + "/" +
                    string(fileName);
  FILE *fptr = fopen(filePath.c_str(), "r");
  if (fptr == NULL) {
    return 0;
  } else {
    int sz = getFileSize(fptr);
    return sz;
  }
}

// Check cache overload status
bool cacheOverload(int currentSize, int fileSize) {
  if (currentSize + fileSize > MAXCACHE) {
    return true;
  } else {
    return false;
  }
}
// Linked List LRU methods
int deleteUntilAvailable(list<char *> *lru, int sz, char *directory,
                         map<char *, char *> *cache) {
  int remain = sz;
  while (remain > 0) {
    char *element = *(lru->end());
    int size = getFileSizeWithName(element, directory);
    remain = sz - size;
    lru->pop_back();
    cache->erase(element);
  }
  return remain;
}
// Cache methods
bool alreadyInCache(map<char *, char *> *cache, char *fileName) {
  map<char *, char *>::iterator iter = cache->find(fileName);
  if (iter != cache->end()) {
    return true;
  } else {
    return false;
  }
}

int main(int argc, char *argv[]) {

  // Argument checking
  if (argc < 2) {
    cout << "Not enough arguments" << endl;
    return 1;
  }
  int port = atoi(argv[1]);
  char *directory = argv[2];
  cout << "Port number: " << port << endl;
  cout << "Host directory is " << directory << endl;

  // Directory checking
  if (fs::exists(directory) && fs::is_directory(directory)) {
    cout << "Directory exist" << endl;
  } else {
    perror("Directory not exist");
    exit(EXIT_FAILURE);
  }

  // Cache represented as a map
  map<char *, char *> cache;
  // LRU linked list
  list<char *> lru;
  // Current size of cache
  int currentCacheSize = 0;

  // Network connection handlers
  int server_fd, client_socket, bufferSize, clientAddr, recvMsgSize;
  struct sockaddr_in address;
  int addrlen = sizeof(address);
  char *fileNameBuffer = (char *)malloc(sizeof(char) * MAXSTRING);
  const char *hello = "Hello from server";
  int opt = 1;

  // Creating socket file descriptor
  if ((server_fd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
    perror("socket failed");
    exit(EXIT_FAILURE);
  }
  cout << "Done socket create" << endl;

  // Forcefully attaching socket to the port, prevent 'socket already in use'
  // error
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt,
                 sizeof(opt))) {
    perror("setsockopt");
    exit(EXIT_FAILURE);
  }

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = htonl(INADDR_ANY);
  address.sin_port = htons(port);
  // Bind socket to the port 9089
  if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
    perror("bind failed");
    exit(EXIT_FAILURE);
  }
  cout << "Done binding" << endl;

  // Listen to chosen port
  if (listen(server_fd, 5) < 0) {
    perror("listen failed");
    exit(EXIT_FAILURE);
  }
  cout << "Listening ..." << endl;

  // Infinite loop to accept incoming connections
  while (true) {

    // Accept a connection
    if ((client_socket = accept(server_fd, (struct sockaddr *)&address,
                                (socklen_t *)&addrlen)) < 0) {
      perror("accept failed");
      exit(EXIT_FAILURE);
    }
    cout << "Accepted connection from " << inet_ntoa(address.sin_addr) << endl;

    // Receive buffer from client
    if ((recvMsgSize = recv(client_socket, fileNameBuffer,
                            sizeof(fileNameBuffer) * 2, 0)) < 0) {
      perror("Received fail");
      exit(EXIT_FAILURE);
    }
    cout << "Client " << inet_ntoa(address.sin_addr) << " requesting file "
         << string(fileNameBuffer) << endl;

    // File already in cache
    if (alreadyInCache(&cache, fileNameBuffer)) {
      char *sendBuffer = cache.at(fileNameBuffer);
      send(client_socket, sendBuffer, sizeof(sendBuffer), 0);
      lru.remove(fileNameBuffer);
      lru.push_front(fileNameBuffer);
      cout << "Done send file alrdy in cache" << endl;
    }
    // Not in cache
    else {
      // Look up for requested file then send the content
      string filePath = string(fs::current_path()) + "/" + string(directory) +
                        "/" + fileNameBuffer;
      FILE *fptr = fopen(filePath.c_str(), "r");
      char *value = (char *)malloc(sizeof(char) * 64000000);
      if (fptr == NULL) {
        cout << "Cannot open file" << endl;
      } else {
        // Count file size
        int sz = getFileSize(fptr);
        cout << "File size: " << sz << " bytes" << endl;
        // send file content
        char readLine[255];

        cout << "start strcat value" << endl;
        while (fgets(readLine, sizeof(readLine), fptr) != NULL) {
          value = strcat(value, readLine);
        }
        cout << "done strcat value, size: " << sizeof(value) << endl;
        // Cache overload, delete some until good for adding

        if (cacheOverload(currentCacheSize, sz)) {
          cout << "overloaded" << endl;
          currentCacheSize += deleteUntilAvailable(&lru, sz, directory, &cache);
          cout << "deleted done" << endl;
          lru.push_front(fileNameBuffer);
          cout << "done push file name to lru" << endl;
          cache.insert(pair<char *, char *>(fileNameBuffer, value));
          cout << "done insert filename and value to map" << endl;
        }
        // Cache still good to add more
        else {
          cout << "Not over loaded" << endl;
          lru.push_front(fileNameBuffer);
          cout << "done push lru" << endl;
          cache.insert(pair<char *, char *>(fileNameBuffer, value));
          cout << "done insert filename and value to map" << endl;
        }
        send(client_socket, &value, sizeof(value), 0);
        memset(value, 0, sizeof(value));
      }
      memset(fileNameBuffer,0,sizeof(fileNameBuffer));
    }
    close(client_socket);
  }
  return 0;
}

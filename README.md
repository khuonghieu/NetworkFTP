# Student Information:
Name: Hieu Khuong  
Course: CIS4307 - Distributed Systems  
Spring 2020  
Homework 2 - Network File Transfer

# Structure of program:
There are 2 Python files: tcp_client.py (TCP client for retrieving files) and tcp_server.py (TCP server for serving the requested files).
Flow of the program:
1. The server would open a socket with a certain port and bind to to that port. Server will listen for any connection to that socket.
2. The client would connect to the server using a certain IP address and port pair. Client would send the requested file name to server, then waiting for server response.
3. When server received the file name, it will look up in the directory. If the file exists, it would load the file content into cache. Then the content would be sent to client.
4. Client receives the content and write to a file created in a certain directory.
# Cache implementation:
The cache has 3 parts:
1. cache: A map to store the content of the file with the key:value format. The key is the file name, and the value is the file content.
2. lru: A list to mimic the LRU (Least Recently Used) cache to store the file name. When a file is accessed, it would be pushed to the front of the list. When the cache is overloaded and a new file is accessed, the LRU would pop out file names from the tail until enough memory is available for the new file.
3. currentSize: An integer to store the current value of cache memory. The cache memory limit is 64mb.
# Execute the code:
1. Open terminal
2. Run server with parameters: 
$python3 tcp_server.py [port] [directory to find files] 
3. Open another terminal
4. Run client with parameters:
$python3 tcp_client.py [ip address] [port] [file name] [directory to save]

# Test cases:
Test case 1: Request an invalid file (file not exist in server directory).
- Test result: server print out "File X does not exist in the server" and close the connection.

Test case 2: Client tries to connect to an inactive server.
- Test result: client print out "Server dead or not found" and exit.

Test case 3: Client use different port number from server.
- Test result: client print out "Server dead or not found" and exit.

Test case 4: Server use invalid directory
- Server print out "Cannot find directory" and exit

Test case 5: Client use invalid directory
- Client print out "Cannot find directory" and exit

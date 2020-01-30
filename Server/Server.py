#!/usr/bin/python3

import socket
import os
import sys

MAXCACHE = 8000000

#Get size of a file
def getSize(filename):
    return os.path.getsize(filename)

#Load a file content into a byte buffer
def fileContent(filename):
    #content = b""
    with open(filename, 'r') as f:
        content = f.read().encode()
    return content

#Check if a file already in cache/lru
def alreadyInCache(cache, filename):
    if filename in cache.keys():
        return True
    else:
        return False

#Add file to cache and push file to top of lru
def addToCacheAndLRU(lru,cache,filename, currentSize):
    if(filename in lru):
        lru.remove(filename)
    lru.insert(0,filename)
    cache[filename] = fileContent(filename)
    currentSize+=getSize(filename)
    return currentSize

#Pop lru and delete cache until enough size available for new file
def deleteUntilAvailable(lru, cache, currentSize, filename):
    while(currentSize + getSize(filename)>MAXCACHE):
        popped = lru.pop()
        cache.pop(popped)
        currentSize -= getSize(popped)
        print("removed", popped)
    return currentSize

def main():

    #Arg checking and assign
    if(len(sys.argv) < 2):
        print("Not enough argument")
        exit
    port = int(sys.argv[1])
    directory = sys.argv[2]

    #Cache and LRU and current size
    cache = {}
    lru = []
    currentSize = 0 

    #Directory checking and redirecting
    if os.path.exists(directory):
        # Change the current working Directory
        os.chdir(directory)
    else:
        print("Can't find dir")
    print(os.getcwd())

    #Create socket
    s = socket.socket()
    print("Socket successfully created")

    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    #Binding    
    s.bind(('', port))
    print("socket binded to", port)

    # Enable listening mode
    s.listen(5)
    print("socket is listening")

    # a forever loop until we interrupt it or an error occurs
    while True:

        # Establish connection with client.
        c, addr = s.accept()
        #Take filename from client
        filename = c.recv(1024).decode()
        print("Client with ip", addr, "requested",filename)
        
        #Check cache
        if(alreadyInCache(cache,filename)):
            c.send(cache[filename])
            print("Cache hit. File",filename,"sent to client")
        else:
            #Cache overload
            if (currentSize + getSize(filename) > MAXCACHE):
                currentSize = deleteUntilAvailable(lru,cache,currentSize,filename)
            currentSize = addToCacheAndLRU(lru,cache,filename,currentSize)
            c.send(cache[filename])
            print("Cache miss. File",filename,"sent to client")
        # Close the connection with the client
        c.close()
        print("Current cache:", currentSize,"bytes")
        print()

if __name__ == '__main__':
    main()

#!/usr/bin/python3

import socket
import os
import sys


def main():


    if(len(sys.argv) < 4):
        print("Not enough argument")
        exit

    serverIP = sys.argv[1]
    port = int(sys.argv[2])
    fileName = sys.argv[3]
    directory = sys.argv[4]

    print(os.getcwd())
    if os.path.exists(directory):
        # Change the current working Directory
        os.chdir(directory)
    else:
        print("Can't find dir")
    print(os.getcwd())

    # Create a socket object
    s = socket.socket()
    # connect to the server on local computer
    s.connect((serverIP, port))

    s.send(fileName.encode())
    f = open(fileName, 'w')
    # receive data from the server
    data = s.recv(1024)
    while(data):
        f.write(data.decode())
        data = s.recv(1024)
    # close the connection
    f.close()
    s.close()

if __name__ == '__main__':
    main()

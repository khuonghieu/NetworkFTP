#!/usr/bin/python3

import socket
import os
import sys


def main():

    #Argv checking and assign
    if(len(sys.argv) < 4):
        print("Not enough argument")
        exit

    serverIP = sys.argv[1]
    port = int(sys.argv[2])
    fileName = sys.argv[3]
    directory = sys.argv[4]


    #Change directory to the chosen one in argv
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

    #Send file name
    s.send(fileName.encode())
    # receive data from the server
    data = s.recv(1024)
    if(data):
        f = open(fileName, 'w')
    else:
        s.close()
        return
    #Write to file
    while(data):
        f.write(data.decode())
        data = s.recv(1024)
    # close the connection
    f.close()
    s.close()
    return

if __name__ == '__main__':
    main()

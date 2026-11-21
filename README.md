# Tcp Port Scanner

tldr: A simple program that uses tcp scanning to check for open ports.  

## Overview

As a preface, this program is not intended to be used for any malicious intent. This is a simple port scanner that uses tcp scanning to check whether a port is open. It takes 4 command line arguments: ./script, ipv4 address, port starting range, port ending range. The source code below is well documented to allow the user to understand the innerworkings of this program. 

## Prerequisites

This program contains 2 separate scripts that serve different purposes. 1 is for scanner itself called 'port-scanner.c' and another for testing the scanner using a deployed server called 'server.c'. This program will ONLY run on Linux, apologies for any Windows users out there. The reasoning being is that the entire infrastructure code relies on Linux, such as the POSIX Socket API which allows the scanner to test communication using sockets. Note that the port-scanner program takes in 4 command-line arguments. These 4 arguments are as followed: ./script, ipv4 address, starting-port-range, ending-port-range. 

Please ensure that your machine has a minimum of 10 total KiB of space for both files. Once downloaded, follow the instructions:

1: Copy both downloaded files into a new directory (This directory will be used to compile and run both scripts later on)  
2: Using a terminal, switch into the new directory to confirm both files are inside using ls -l  
3: For the purpose of this demonstration, the GCC will be used (GNU Compiler Collection) to compile both programs.  
4: run the following command to compile the server script: gcc server.c -o server -lpthread  
5: run the following command to compile the client script: gcc port-scanner.c -o port-scanner -lpthread  
6: Execute the compiled server script first: ./server (Note that the order in which you execute the programs is important since the server has to be listening on the port before the scanner checks it.)  
7: Execute the compiled port-scanner script next specifying 4 command line arguments: ./port-scanner 127.0.0.1 8001 8002

After a small delay, the scanner will either output a message indicating the port is open or throw a "destination network unreachable...". To test out functionality, a server script has been provided to simulate a successful scan. Since the server is running on the local machine, the loopback address can be specified as the 2nd command line argument. Port 8001 is used in step 7 since the server is listening on it. As seen from the header files, This program implements multi-threading to allow 2 way communication between both nodes. Outside of the main function, a separate worker thread is created to handling the sending and receiving portion of script. Both portions are encased in a while loop to allow communication so long as the client does not type in quit. Since the program implements threading, an additional parameter needs to added when compiling the program: gcc server.c -o server '-lpthread'. 

## FAQ

Here of some the odd and unintended quirks you may encounter while messing around with the programs. 

1: The server.c script is merely used to see the scanner in action, if you check out the source code the program, you'll notice it is geared towards a chat program. That's because this program was previously used in a another project of mine. You'll notice that after the scanner detects the open port, a prompt on the server side to enter in a message will be displayed to the terminal, this output can be ignored. 

2: Ensure that the command line ipv4 address entered is valid, "destination network unreachable" occurs even when an incorrect ipv4 address or port is entered incorrectly. There are certain measures implemented such as making sure there are 4 arguments passed or entering in an invalid range (3000 - 100) but not everything has been taken into consideration.

## Resources

This project couldn't have been done without the help of the following resources:  
GeeksforGeeks - Understanding TCP 
Beej's Guide to Network Programming  
Socket Programming - Dan Gookins  
Socket Programming - Jacob Sorber

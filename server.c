#include <pthread.h>
#include<stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <stdint.h>

/*
 Server Script 
3rd iteration of tcp-chat client & server!
Based on previous iteration, the server could only output 1 message before terminating the session.
This new iteration expands on the previous build to incorperate threading.
This was a new concept but it allowed me to enable two way communication with the feature to termiate the session when needed to.
I'm sure there is an easier way to enable two way communicaiton without threading...
*/



/*
A worker thread function responsible for handling the communication for the server
It uses the client_socket which was created during accept()
Our other socket (network_socket) is listening for incoming connections
*/
void *communication_thread(void *client_socket) {

bool flag = true;
char  message[1024];
char receive[1024];

/* 
the flag's job to check whether the client  wants to quit at any given moment.
while the flag is true, the program to continue to allow 2 way communication.
 */
while(flag) {
	
printf("receiving message...\n");


/*
program is designed so that client initates a response and and server catches it first before sending anything back
worker thread function can only have void * passed into, since the recv() and send()
requires an int as the socket file descriptor for the first parameter, we need to
defereference and cast the value as an int
 */

int change_void = *(int *)client_socket; 

int  receiving = recv(change_void, receive, sizeof(receive), 0);

//check to see if the client typed 'quit' to exit program 
//strcmp compare two strings and 0 indicated success
if(strcmp(receive, "quit") == 0) {

      printf("client has quit...\n");
      flag = false;
      break;
      close(change_void);
    }

if(receiving == -1) {

   perror("failed to receive message...\n");
   exit(1);

     } else if (receiving == 0) {

   printf("connection terminated...\n");
   
     } else if (receiving > 0) {
 
//received string needs to be '\0' 
//This is because send() recv() do not automatically null terminate it 	     
   receive[receiving] = '\0';
   printf("message received: %s\n", receive);

     } else {
	
   break;

     }


//sending portion of the script

printf("sending message...\n");

printf("Enter a message:\n");

fgets(message, sizeof(message), stdin);


//work thread function can only have void * passed into, since our send()
//requires  an int as the socket file descriptor for the first parameter, we need to
//defereference and cast the value as an int

int change_void2 = *(int *)client_socket;

int sending = send(change_void2, message, strlen(message), 0);

if(sending == -1) {

   perror("failed to send message...\n");
   exit(1);

     } else if (sending > 0) {
     
     message[sending] = '\0';
     
     } 

  }
     return NULL;
}

 


//main thread 

int main() {

 
//*reference is a pointer to struct addrinfo hints

   
struct addrinfo hints;
struct addrinfo *reference;
int backlog = 1;

struct sockaddr_storage client_address;
socklen_t client_size;

char receive[1024];

//setting the server configuration, tcp, ipv4/ipv6, listening on 8080...

printf("configuring host...\n");

//memeset() is used to clear up the function and ensure no garbage memory is present
memset(&hints, 0, sizeof(hints));
hints.ai_family = AF_UNSPEC;
hints.ai_socktype = SOCK_STREAM;
hints.ai_flags = AI_PASSIVE;

int configuration = getaddrinfo(NULL, "8001", &hints, &reference);

//error handling to check whether the function failed (this will appear mulitple times throughout the code)
if(configuration != 0) {
 
    perror("failed to configure host");
    exit(1);
  }

printf("creating socket...\n");

//reference pointer to the struct hints members
//creating our network socket
int network_socket = socket(
	reference->ai_family,
	reference->ai_socktype,
	reference->ai_protocol
	);

if(network_socket == -1) {
  
   perror("failed to create socket");
   exit(1);  

}

//binding our socket to a port along with ip address information
printf("binding socket...\n");

int binding = bind(
	network_socket,
	reference->ai_addr,
	reference->ai_addrlen
	);

if(binding == -1) {

    perror("failed to bind socket...\n");
    exit(1);
  }

//listening for incoming connections
//backlog refers to the number of connections that can wait in a queue (it's set to 1)
printf("listening...\n");

int listening = listen(
	network_socket,
	backlog
        );

if(listening == -1) {

   perror("failed to startup listening state...\n");
   exit(1);   
  }

/*
Accepting connections
function takes in our network_socket and returns a new socket used to communicate with the client
struct sockaddr_storage client_address contains information about incoming connection
socklen_t = integer datatype used to represent addresses

 */

printf("accepting connection...\n");

//Ensuring that client_size will write no more than what is specified inside of sockaddr_storage
//casting a pointer  struct sockaddr * to address of another struct sockaddr_storage

client_size = sizeof(client_address);
int client_socket = accept(
	network_socket,
        (struct sockaddr *) &client_address, 
	&client_size
        );

if(client_socket == -1) {

   perror("failed to accept incoming connection...\n");
   exit(1);
  }


//Initializing our worker thread
pthread_t texting_thread;

//creating our worker thread using teh communcation_thread function
//4th argument is parameter that our communication_thread uses/gets passed in
pthread_create(&texting_thread, NULL, communication_thread, (void *)&client_socket);

//forces main to wait untill this thread has finished running
pthread_join(texting_thread, NULL);





//release the memory used by the getaddrinfo() stored in *reference
freeaddrinfo(reference);


//close our listening network socket
close(network_socket);

return 0;

}

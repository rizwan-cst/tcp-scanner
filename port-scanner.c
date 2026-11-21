#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>

//custom struct used to store cli arguments, a server ip, starting port range and ending port range
struct scanner_configuration {

        char *server_ip;
        int starting_port;
        int ending_port;
};


//worker thread that loops through the port ranges, a fresh network_socket is created each time to test against each port
//memory address of our scanner_configuration config is passed as argument but it is initally treated as a void 
void *scanning_thread(void *custom_struct) {

struct sockaddr_in my_address;

//casting the void address to the struct scanner_configuration
//*config will allow us to access the members inside of the struct
struct scanner_configuration *config = (struct scanner_configuration *)custom_struct;

//setting up our scanner to be tested against an address & port 
printf("Scanning host on: %s from ports %d to %d\n", config->server_ip, config->starting_port, config->ending_port);


int starting_range = config->starting_port;
int ending_range = config->ending_port;

//In every iteration, we take create a new socket to be tested against the port to check if it is availible. 
for(; starting_range <= ending_range; starting_range++) {

int network_socket = socket(AF_INET, SOCK_STREAM, 0);

//making sure the struct is empty before filling up
memset(&my_address, 0, sizeof(my_address));
my_address.sin_family = AF_INET;
my_address.sin_port = htons(starting_range);
my_address.sin_addr.s_addr = inet_addr(config->server_ip);

//if a connection is successful, the port will declared as opened
int connection = connect(network_socket, (struct sockaddr *)&my_address, sizeof(my_address));

    if(connection == 0) {
    
    printf("OPEN PORT: %d\n", starting_range);
    close(network_socket);
    } else if (connection == -1) {

	    perror("destination network unreachable...\n");
	    close(network_socket);
     }

  
  }

  return NULL;

}



/*
Port scanner that takes 4 cli arguments to check whether ports are open
argc is number of arguments
argv array that indexes arguments
*/
int main(int argc, char *argv[]) {

struct addrinfo hints;
struct addrinfo *reference;

//restricting the user to only using 4 command line arguments (Ipv4 and a port range). Note that the script execution ./portscanner counts as cli argument.	

if (argc != 4) {
	
printf("Please enter an ipv4 address along with a range of ports...\n");	
exit(1);

  }

//passing our command line arguments int the custom struct
//cli arguments for the port ranges are orginally defined as strings of ASCII characters
//atoi(ASCII to Integer)  converts the strings into a whole number
struct scanner_configuration config;
config.server_ip = argv[1];  //remote ip
config.starting_port = atoi(argv[2]); //starting port
config.ending_port = atoi(argv[3]); //ending port


//if user enters an invalid port range
if(config.ending_port < config.starting_port) {

printf("invalid range, the ending port range cannot be smaller than the starting range...\n");	
exit(1);

  }


//creating a thread handler
pthread_t scanner_id;

//running the thread, 4th argument is the address of our config struct
pthread_create(&scanner_id, NULL, scanning_thread, (void *)&config);

//forces the main to wait unitll this thread is completed
pthread_join(scanner_id, NULL);

   return 0;
}






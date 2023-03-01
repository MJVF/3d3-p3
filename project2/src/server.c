#include "server.h"

int main(int argc , char *argv[])
{
    //User defined port to initiate connections
    unsigned short PORT;
	if(argc==2)
		PORT = htons(atoi(argv[1])); // Converts int to network byte order
	else{
		printf("NO PORT DEFINED...\nProgram Terminating");
		return -1;
	}


    //from <netinet/in.h>, this struct handles connections to inet addresses
    struct sockaddr_in addr;
        addr.sin_family = AF_INET; //Address family for IPv4 connections
        addr.sin_port = PORT; //Define socket port
        addr.sin_addr.s_addr = INADDR_ANY; //To avoid binding socket to a specific IP address, better porting from machine to machine
    int length_addr = sizeof(addr);

    //define socket descriptor o utilise send() and recv(), SOCK_STREAM and not SOCK_DGRAM as we want to connect to client
    int sock = socket(AF_INET,SOCK_STREAM, 0);
    bind(sock, (struct sockaddr *)&addr, sizeof(addr)); //Bond Socket to PORT
    printf("PORT %d Open, ready to connect!\n", atoi(argv[1]));



    //Prepare to trade information with client
    fd_set incoming;
    int cl_sock;
    char listings[4096];

    //Enter infinite loop to begin accepting connection requests and data transmission
    while(1){

        //Reset socket inbetween different client communications
        FD_ZERO(&incoming);
        FD_SET(sock, &incoming);
        

    }


    return 0;
}
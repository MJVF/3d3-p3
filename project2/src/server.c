#include "server.h"

int main(int argc , char *argv[])
{
    //User defined port to initiate connections
    unsigned short PORT;
	if(argc>=2)
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

    //define socket descriptor o utilise send() and recv(), SOCK_STREAM and not SOCK_DGRAM as we want to connect to client
    int sock = socket(AF_INET,SOCK_STREAM, 0);
    //Allow for multiple client connections
    int option = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *)&option, sizeof(option));
    //Bind Socket to PORT
    bind(sock, (struct sockaddr *)&addr, sizeof(addr));
    printf("PORT %d Open, ready to connect!\n", atoi(argv[1]));



    //Prepare to trade information with client
    fd_set incoming;
    int cl_sock;
    int inc_sock;
    char listings[4096];
    int length_addr = sizeof(addr);

    //Enter infinite loop to begin accepting connection requests and data transmission
    while(1){

        //Reset socket inbetween different client communications
        FD_ZERO(&incoming);
        FD_SET(sock, &incoming);

        //define different socked descriptors
        int max_sd = sock;
        int new_sd = cl_sock;
        if(cl_sock > 0)
            FD_SET(new_sd, &incoming);
        if(new_sd > sock)
            max_sd = new_sd;

        //Wait for activity on potential sockets, produce error if errno code is not EINTR (i.e failure to select socket)
        if(select(max_sd + 1, &incoming, NULL, NULL, NULL) < 0 && errno != EINTR){
            printf("Error in connecting to client over port...\n");
        }

        //If connection is succesful, begin bit stream between client and server
        if(FD_ISSET(sock, &incoming)){
            
            //Accept the incoming socket request and send message to the client
            inc_sock = accept(sock, (struct sockaddr *)&addr, (socklen_t*)&length_addr);
            send(inc_sock, argv[2], strlen(argv[2]), 0);
        
            cl_sock = inc_sock;
        
        
        }

    }


    return 0;
}
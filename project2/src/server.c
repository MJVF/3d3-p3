#include "server.h"




void write_file(int sockfd)
{
    int n; 
    FILE *fp;
    char *filename = "recv.txt";
    char buffer[SIZE];

    fp = fopen(filename, "w");
    if(fp==NULL)
    {
        perror("[-]Error in creating file.");
        exit(1);
    }
    while(1)
    {
        n = recv(sockfd, buffer, SIZE, 0);
        if(n<=0)
        {
            break;
            return;
        }
        fprintf(fp, "%s", buffer);
        bzero(buffer, SIZE);
    }
    return;
    
}


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

    //define socket descriptor to utilise send() and recv(), SOCK_STREAM and not SOCK_DGRAM as we want to connect to client
    int server_sock = socket(AF_INET, SOCK_STREAM, 0);
    //Allow for multiple client connections
    int option = 1;
    setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, (char *)&option, sizeof(option));
    //Bind Socket to the specified PORT
    bind(server_sock, (struct sockaddr *)&addr, sizeof(addr));
    printf("PORT %d Open, ready to connect!\n", atoi(argv[1]));
    //Setting listening backlog
    listen(server_sock, 5);


    //Prepare to trade information with client
    fd_set incoming;
    int client_sock = 0;
    int incoming_sock = 0;
    char listings[4096];
    int length_addr = sizeof(addr);

    //Enter infinite loop to begin accepting connection requests and data transmission
    while(1){

        //Reset socket inbetween different client communications
        FD_ZERO(&incoming);
        FD_SET(server_sock, &incoming);

        //define different socked descriptors
        int max_sd = server_sock;
        int new_sd = client_sock;
        if(client_sock > 0)
            FD_SET(new_sd, &incoming);
        if(new_sd > server_sock)
            max_sd = new_sd;

        //Wait for activity on potential sockets, produce error if errno code is not EINTR (i.e failure to select socket)
        if(select(max_sd + 1, &incoming, NULL, NULL, NULL) < 0 && errno != EINTR){
            printf("Error in connecting to client over port...\n");
        }

        //If connection is succesful, begin bit stream between client and server
        if(FD_ISSET(server_sock, &incoming)){
            
            //Accept the incoming socket request and send message to the client
            incoming_sock = accept(server_sock, (struct sockaddr *)&addr, (socklen_t*)&length_addr);
            //int valread = read(incoming_sock, listings, 4096);
            //printf("%s\n", listings);
            //send(incoming_sock, argv[2], strlen(argv[2]), 0);

            write_file(incoming_sock);

            new_sd = client_sock;
        
        
        }

    }


    return 0;
}
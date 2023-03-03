#include "server.h"

//colours for terminal printing
#define CNRM  "\x1B[0m"
#define CRED  "\x1B[31m"
#define CGRN  "\x1B[32m"
#define CBLU  "\x1B[34m"

#define client_limit 50

int main(int argc , char *argv[]){

    //Aesthetic detail
    printf("%s  ____  _____ ____     %s        _____           _           _   ___  \n", CGRN, CBLU);
    printf("%s |___ \\|  __ \\___ \\    %s       |  __ \\         (_)         | | |__ \\ \n", CGRN, CBLU);
    printf("%s   __) | |  | |__) | %s ______ %s | |__) | __ ___  _  ___  ___| |_   ) |\n", CGRN, CNRM, CBLU);
    printf("%s  |__ <| |  | |__ <  %s|______|%s |  ___/ '__/ _ \\| |/ _ \\/ __| __| / / \n", CGRN, CNRM, CBLU);
    printf("%s  ___) | |__| |__) |   %s       | |   | | | (_) | |  __/ (__| |_ / /_ \n", CGRN, CBLU);
    printf("%s |____/|_____/____/    %s       |_|   |_|  \\___/| |\\___|\\___|\\__|____|\n", CGRN, CBLU);
    printf("                                             _/ |                   \n");
    printf("                                            |__/                    %s\n\n", CNRM);

    //User defined port to initiate server
    unsigned short PORT;
	if(argc==2)
		PORT = htons(atoi(argv[1])); // Converts int to network byte order
	else{
		printf("NO PORT DEFINED...\nProgram Terminating");
		exit(EXIT_FAILURE);
	}

    //track if intiialisation functions have failed.
    int error = 0;

    //from <netinet/in.h>, this struct handles connections to inet addresses
    struct sockaddr_in addr;
        addr.sin_family = AF_INET; //Address family for IPv4 connections
        addr.sin_port = PORT; //Define socket port
        addr.sin_addr.s_addr = INADDR_ANY; //To avoid binding socket to a specific IP address, better porting from machine to machine

    //define socket descriptor to utilise send() and recv(), SOCK_STREAM and not SOCK_DGRAM as we want to connect to client
    int server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if(!server_sock){
        printf("%s[-]ERROR: Failed to create Socket... Aborting!%s\n", CRED, CNRM);
        exit(EXIT_FAILURE);
    }printf("%s[+]Socket initiated%s\n", CGRN, CNRM);

    //Allow for multiple client connections
    int option = 1;
    setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, (char *)&option, sizeof(option));

    //Bind Socket to the specified PORT
    error = bind(server_sock, (struct sockaddr *)&addr, sizeof(addr));
    if(error < 0){
        printf("%s[-]ERROR: Failed to bind Socket to PORT... Aborting!%s\n", CRED, CNRM);
        exit(EXIT_FAILURE);
    }printf("%s[+]Socket %s%d%s succesfully bound to PORT %s%d%s\n", CGRN, CBLU, server_sock, CGRN, CBLU, ntohs(PORT),CNRM);

    //Setting listening backlog
    listen(server_sock, 5);
    
    printf("%s[+]Server open on PORT %s%d%s, listening for clients.%s\n", CGRN, CBLU, ntohs(PORT), CGRN, CNRM);

    //Initiliase variables in preparation to transfer information with clients
    fd_set incoming;
    int incoming_sock = 0;
    int length_addr = sizeof(addr);
    int client_sock[client_limit] = { 0 };

    int new_sd;
    int max_sd;
    int received;
    int amount = 0;

    char listings[4096];
    FILE * write;
    //write = fopen("resources.txt", "a+");

    //Enter infinite loop to begin accepting connection requests and data transmission
    while(1){

        //Reset socket inbetween different client communications
        FD_ZERO(&incoming);
        FD_SET(server_sock, &incoming);

        //define different socked descriptors
        max_sd = server_sock;
        
        int i;
        for(i = 0; i < client_limit; i++){
            new_sd = client_sock[i];

            if(new_sd > 0)
                FD_SET(new_sd, &incoming);
            
            if(new_sd > server_sock)
                max_sd = new_sd;
        }

        //Wait for activity on potential sockets, produce error if errno code is not EINTR (i.e failure to select socket)
        if(select(max_sd + 1, &incoming, NULL, NULL, NULL) < 0 && errno != EINTR){
            printf("%s[-]Error in connecting to client over port...%s\n", CRED, CNRM);
        }

        //If connection is succesful, begin bit stream between client and server
        if(FD_ISSET(server_sock, &incoming)){
            
            //Accept the incoming socket request and send message to the client
            incoming_sock = accept(server_sock, (struct sockaddr *)&addr, (socklen_t*)&length_addr);
            printf("\n%s[*]Connection requested on socket %s%d%s\n   port: %s%d%s\n     ip: %s%s%s\n\n",CGRN,CBLU, incoming_sock ,CGRN,CBLU, ntohs(addr.sin_port) ,CGRN,CBLU, inet_ntoa(addr.sin_addr) ,CNRM);
            received = read(incoming_sock, listings, 4096);

            //count the number of resource listings received
            for(i = 0; i < 4096; i++){
                if(listings[i] == '\n' || listings[i] == '\r')
                    amount++;
                else if(listings[i] == '\0')
                    break;
            }
            write = fopen("resources.txt", "a+");
            fprintf(write, "%s\n", listings);
            fclose(write);
            

            /*for(i = 0; i < client_limit; i++){
                if(!client_sock[i]){
                    client_sock[i] = incoming_sock;
                    break;
                }
            }*/
            char receipt[64];
            sprintf(receipt, "\n%d", amount);
            strcat(receipt, " resource listing(s) succesfully received!\n\0");

            send(incoming_sock, receipt, strlen(receipt), 0);
            printf("%s[+]Confirmation Receipt for %s%d%s listing(s) sent to Client!%s\n", CGRN, CBLU, amount, CGRN, CNRM);
            amount = 0;
            //listings = '\0';

        }
        
        close(incoming_sock);

    }
    //fclose(write);

    return 0;
}
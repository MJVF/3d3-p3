#include "server.h"

//colours for terminal printing
#define CNRM  "\x1B[0m"
#define CRED  "\x1B[31m"
#define CGRN  "\x1B[32m"
#define CBLU  "\x1B[34m"

#define MAX 2048

int main(int argc , char *argv[]){

    aesthetic();

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

    //from <arpa/inet.h>, this struct handles connections to inet addresses
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
    int client_sock = 0;

    int new_sd;
    int max_sd;
    int amount = 0;

    int i;
    char c;
    int auth;
    char listings[MAX];
    FILE* database;
    char receipt[64];
    char resources[MAX];
    char data[MAX];

    //Enter infinite loop to begin accepting connection requests and data transmission
    while(1){
        init:
        //Reset socket inbetween different client communications
        FD_ZERO(&incoming);
        FD_SET(server_sock, &incoming);

        //define different socket descriptors
        max_sd = server_sock;
        new_sd = client_sock;

        if(new_sd > 0)
            FD_SET(new_sd, &incoming);
            
        if(new_sd > server_sock)
            max_sd = new_sd;

        //Wait for activity on potential sockets, produce error if errno code is not EINTR (i.e failure to select socket)
        if(select(max_sd + 1, &incoming, NULL, NULL, NULL) < 0 && errno != EINTR){
            printf("%s[-]Error in connecting to client over port...%s\n", CRED, CNRM);
        }

        //If connection is succesful, begin bit stream between client and server
        if(FD_ISSET(server_sock, &incoming)){
            
            //Accept the incoming socket request and send message to the client
            incoming_sock = accept(server_sock, (struct sockaddr *)&addr, (socklen_t*)&length_addr);
            printf("\n%s[*]Connection requested on socket %s%d%s\n   port: %s%d%s\n     ip: %s%s%s\n\n",CGRN,CBLU, incoming_sock ,CGRN,CBLU, ntohs(addr.sin_port) ,CGRN,CBLU, inet_ntoa(addr.sin_addr) ,CNRM);

            if(read(incoming_sock, listings, MAX) == -1){
                printf("%s[-]Error while receiving data through port %s%i%s... Data stream interrupted, closing socket!%s\n", CRED, CBLU, atoi(argv[1]), CRED, CNRM);
                close(incoming_sock);
                goto init;
            }
            
            //Authentify client's credentials
            bzero(receipt, 64);
            auth = checkUser(listings);
            if(auth < 0){ //auth = -1, no local repository of valid credentials
                close(incoming_sock);   
                return 0;
            }
            if(auth > 0){ //auth = 1, invalid client credentials
                printf("%s[-]Warning Invalid Credentials, terminating connection!%s\n", CRED, CNRM);
                sprintf(receipt, "%s\n[-]Invalid Credentials... Access Denied!%s\n", CRED, CNRM);
                send(incoming_sock, receipt, strlen(receipt), 0);
                close(incoming_sock);
                goto init;
            }
            printf("%s[+]Authentification Success, beginning data stream with client!%s\n", CGRN, CNRM); //auth = 0, valid credentials
            sprintf(receipt, "%s\n[+]Credentials Validated, initiating communications!%s\n", CGRN, CNRM);
            send(incoming_sock, receipt, strlen(receipt), 0);
            bzero(listings, MAX);

            
            //receive the clients resource information
            if(read(incoming_sock, listings, MAX) == -1){
                printf("%s[-]Error while receiving data through port %s%i%s... Data stream interrupted, closing socket!%s\n", CRED, CBLU, atoi(argv[1]), CRED, CNRM);
                close(incoming_sock);
                goto init;
            }

            //count the number of resource listings received
            for(i = 0; i < MAX; i++){
                if(listings[i] == '\n' || listings[i] == '\r')
                    amount++;
                else if(listings[i] == '\0')
                    break;
            }
            database = fopen("data/server_resources.txt", "a+");
            fprintf(database, "%s\n", listings);
            fclose(database);

            bzero(receipt, 64);
            sprintf(receipt, "\n%d", amount);
            strcat(receipt, " resource listing(s) succesfully received!\n");


            database = fopen("data/server_resources.txt", "r");
            i = 0;
            bzero(resources, MAX);
            while(c != EOF){
                c = fgetc(database);
                if(c == EOF)
                    break;
                resources[i] = c;
                i++;
            }
            resources[i] = '\0';

            bzero(data, MAX);
            strcat(data, receipt);
            strcat(data, resources);

            send(incoming_sock, data, strlen(data), 0);
            printf("%s[+]Updated client's database!\n[+]Confirmation Receipt for %s%d%s listing(s) sent to Client!%s\n", CGRN, CBLU, amount, CGRN, CNRM);
            amount = 0;

        }
        
        close(incoming_sock);

    }

    return 0;
}


int checkUser(char details[2048]){
    char name[64];
    char pass[64];
    char in_name[64];
    char in_pass[64];
    bzero(in_name, 64);
    bzero(in_pass, 64);
    
    char c;
    FILE* credentials = fopen("data/trusted_credentials.txt", "r");

    if(!credentials){
        printf("%s[-]Error! trusted_credentials.txt not found... Terminating Program!%s\n", CRED, CNRM);
        fclose(credentials);
        return -1;
    }

    //Read in the credentials sent by the client requesting to connect to the server
    int i = 0;
    int j = 0;
    while(details[i] != ','){ //Username
        in_name[i] = details[i];
        i++;
    }in_name[i] = '\0';
    i++;
    while(details[i]){ //Password
        in_pass[j] = details[i];
        i++;
        j++;
    }in_pass[j] = '\0';
    
    //Compare the clients credentials to the local list of trusted credentials
    while(c != EOF){
        i = 0;
        j = 0;
        bzero(name, 64);
        bzero(pass, 64);
        c = fgetc(credentials);
        while(c != ',' && c != EOF){ //Username
            name[i] = c;
            i++;
            c = fgetc(credentials);
        }name[i] = '\0';
        c = fgetc(credentials);
        while(c != '\n' && c != '\r' && c != EOF){ //Password
            pass[j] = c;
            j++;
            c = fgetc(credentials);
        }pass[j] = '\0';

        if(strcmp(in_name, name) == 0 && strcmp(in_pass, pass) == 0){
            fclose(credentials);
            return 0;
        }
    }
    fclose(credentials);
    return 1; //No credentials match
}



void aesthetic(){
    //Aesthetic detail
    printf("%s  ____  _____ ____     %s        _____           _           _   ___  \n", CGRN, CBLU);
    printf("%s |___ \\|  __ \\___ \\    %s       |  __ \\         (_)         | | |__ \\ \n", CGRN, CBLU);
    printf("%s   __) | |  | |__) | %s ______ %s | |__) | __ ___  _  ___  ___| |_   ) |\n", CGRN, CNRM, CBLU);
    printf("%s  |__ <| |  | |__ <  %s|______|%s |  ___/ '__/ _ \\| |/ _ \\/ __| __| / / \n", CGRN, CNRM, CBLU);
    printf("%s  ___) | |__| |__) |   %s       | |   | | | (_) | |  __/ (__| |_ / /_ \n", CGRN, CBLU);
    printf("%s |____/|_____/____/    %s       |_|   |_|  \\___/| |\\___|\\___|\\__|____|\n", CGRN, CBLU);
    printf("                                             _/ |                   \n");
    printf("                                            |__/                    %s\n\n", CNRM);
}
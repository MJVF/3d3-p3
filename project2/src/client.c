#include "client.h"

//colours for terminal printing
#define CNRM  "\x1B[0m"
#define CRED  "\x1B[31m"
#define CGRN  "\x1B[32m"
#define CYLW  "\x1B[33m"
#define CBLU  "\x1B[34m"

int main(int argc, char* agrgv[]){
    //If main town Server is down, clients can interact with one another to distribute the decentralised database.
    char init;
    printf("%s[*]Host for another client? [y/N]%s\n", CYLW, CNRM);
    scanf("%c", &init);
    if(init == 'y'){
        beginServer();
        return 0;
    }
    beginClient();
    return 0;
}



void beginServer(){

    

    return;
}

void beginClient(){

    //User input IP and PORT to connect to.
    char inet[16];
    int port;
    
    printf("%s[*]Enter Server IP:%s ", CGRN, CNRM);
    scanf("%s", inet);
    printf("%s[*]Enter communication PORT:%s ", CGRN, CNRM);
    scanf("%i", &port);
    unsigned short PORT = htons(port);

    //from <arpa/inet.h>, this struct handles connections to inet addresses
    struct sockaddr_in addr;
        addr.sin_family = AF_INET; //Address family for IPv4 connections
        addr.sin_port = PORT; //Define socket port
        addr.sin_addr.s_addr = inet_addr(inet); //Connecting to specified IP address

    //define socket descriptor to utilise send() and recv(), SOCK_STREAM and not SOCK_DGRAM as we want to connect to server
    int client_sock = socket(AF_INET, SOCK_STREAM, 0);
    if(!client_sock){
        printf("%s[-]ERROR: Failed to create Socket... Aborting!%s\n", CRED, CNRM);
        exit(EXIT_FAILURE);
    }printf("%s[+]Socket initiated%s\n", CGRN, CNRM);

    int numFail = 0;
    while(numFail < 5){ //Attempt to connect to server at most 5 times before giving up
        if(connect(client_sock, (struct sockaddr*)&addr, sizeof(addr)) != -1)
            break;
        numFail++;
        if(numFail == 5){
            printf("%s[-]Couldnt not Connect... Exiting program!%s\n", CRED, CNRM);
            return;
        }
        printf("%s[-]Error Connecting, attempt %i/5, Retrying connection%s\n", CRED, numFail, CNRM);
        sleep(0.5);
    }printf("%s[+]Connection to host succesful!%s\n", CGRN, CNRM);

    FILE* read = fopen()
    close(client_sock);

    return;
}
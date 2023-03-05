#include "client.h"

//colours for terminal printing
#define CNRM  "\x1B[0m"
#define CRED  "\x1B[31m"
#define CGRN  "\x1B[32m"
#define CYLW  "\x1B[33m"
#define CBLU  "\x1B[34m"

#define MAX 2048

int main(int argc, char* agrgv[]){

    aesthetic();

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
        printf("%s[-]Error Connecting, attempt %i/5, Retrying connection%s\n", CRED, numFail, CNRM);
        if(numFail == 5){
            printf("%s[-]Could not Connect... Exiting program!%s\n", CRED, CNRM);
            return;
        }
        sleep(1);
    }printf("%s[+]Connection to host succesful!%s\n", CGRN, CNRM);

    FILE*  resources= fopen("data/my_resources.txt", "r");
    char data[MAX] = {0};
    char c;
    int i = 0;
    while(c != EOF){
        c = fgetc(resources);
        if(c == EOF)
            break;
        data[i] = c;
        i++;
    }
    data[i+1] = '\0';
    if(send(client_sock, data, sizeof(data), 0) == -1){
        printf("%s[-]Unknown result of file transmission... Local Error detected!%s\n", CRED, CNRM);
    }else printf("%s[+]File succesfuly sent to host%s\n", CGRN, CNRM);

    char received[MAX];
    if(read(client_sock, received, MAX) == -1){
        printf("%s[-]Error while receiving data through port %s%i%s... Data stream interrupted!%s\n", CRED, CBLU, port, CRED, CNRM);
    }else printf("%s\n", received);
    
    close(client_sock);

    printf("%s[+]Succesfuly disconnected from server... program safely terminating!%s\n", CGRN, CNRM);

    return;
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
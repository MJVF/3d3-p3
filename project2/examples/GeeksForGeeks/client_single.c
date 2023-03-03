// Client side C/C++ program to demonstrate Socket
// programming
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#define PORT 8888
#define SIZE 1024


void send_file(FILE *fp, int sockfd)
{
    char data[SIZE] = {0};

    while(fgets(data, SIZE, fp)!=NULL)
    {
        if(send(sockfd, data, sizeof(data), 0)== -1)
        {
            perror("[-] Error in sendung data");
            exit(1);
        }
        bzero(data, SIZE);
    }
}



int main(int argc, char const* argv[])
{
	int status, valread, client_fd;
	struct sockaddr_in serv_addr;
	char* hello = "Hello from client";
	char buffer[1024] = { 0 };
	if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("\n Socket creation error \n");
		return -1;
	}

	FILE *fp;
    char *filename = "test.txt";
	fp = fopen(filename, "r");

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);

	// Convert IPv4 and IPv6 addresses from text to binary
	// form
	if (inet_pton(AF_INET, argv[1], &serv_addr.sin_addr)
		<= 0) {
		printf(
			"\nInvalid address/ Address not supported \n");
		return -1;
	}

	if ((status
		= connect(client_fd, (struct sockaddr*)&serv_addr,
				sizeof(serv_addr)))
		< 0) {
		printf("\nConnection Failed \n");
		return -1;
	}
	//send(client_fd, hello, strlen(hello), 0);
	send_file(fp,client_fd);
	printf("Hello message sent\n");
	valread = read(client_fd, buffer, 1024);
	printf("%s\n", buffer);

	// closing the connected socket
	close(client_fd);
	return 0;
}

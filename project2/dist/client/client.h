#ifndef C_H_
#define C_H_

//The usual suspects
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//System library for system sockets
#include <sys/socket.h>
//Libraries for handling inet addresses
#include <netinet/in.h>
#include <arpa/inet.h>
//Libraries for socket in/out 
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/time.h>

void beginServer();
void beginClient();
int checkUser(char details[2048]);
void aesthetic();


#endif
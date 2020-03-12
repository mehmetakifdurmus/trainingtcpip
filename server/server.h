#ifndef SERVER_H
#define SERVER_H

#define PORT 1337
#define true 1
#define false 0
#include <pthread.h>
#include <malloc.h>
#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>

extern int run;

extern int server_fd, addrlen;
extern struct sockaddr_in address;

typedef struct _clientnode
{
    pthread_t thread;
    char client[8];
    struct _clientnode* next;
    int socket;
} clientnode;

static clientnode* head = NULL;

clientnode* initClientList();
clientnode* addClient(clientnode* _head);
void removeClient(clientnode* _head, const char* _client);
void clearClientList(clientnode* _head);
void printClient(clientnode* _head);
void initServer();
void terminateServer();
void* accepter();
void accepterClose();
void* messenger(void* _clientnode);

#endif // SERVER_H

// Server side C/C++ program to demonstrate Socket programming
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#define PORT 1337
#include "threadlist.h"
#define true 1
#define false 0
#include <signal.h>

static threadnode* head = NULL;
static int run = 1;

static int server_fd, addrlen;
static struct sockaddr_in address;
static pthread_mutex_t mutex;

void handle_sigint(int sig)
{
    printf("\nCaught signal %d\nClosing server...\n", sig);
    pthread_mutex_unlock(&mutex);
}

void* messenger(void* _thread_ptr)
{
    threadnode* thread_ptr = (threadnode*) _thread_ptr;
    char buffer[1024];
    buffer[0] = 0;
    long valread;
    valread = read(thread_ptr->socket , thread_ptr->cl_name, 8);
    printf("%s has entered the chat.\n", thread_ptr->cl_name);
    while(true)
    {
        valread = read(thread_ptr->socket , buffer, 1024);
        if(strcmp(buffer, "exit") == 0)
        {
            printf("%s has left the chat.\n", thread_ptr->cl_name);
            send(thread_ptr->socket , "bye", 3, 0);
            break;
        }
        else if(strlen(buffer) == 0)
        {
            printf("%s has dropped from the chat.\n", thread_ptr->cl_name);
            break;
        }
        else
        {
            threadnode* ptr = head->next;
            printf("%s : %s, size : %ld\n", thread_ptr->cl_name, buffer, strlen(buffer));
            if(buffer[0] == '/')
            {
                if(buffer[1] == 'b')
                {
                    while(true)
                    {
                        if(ptr == NULL)
                            break;
//                        else if(ptr == thread_ptr)
//                        {
//                            ptr = ptr->next;
//                            continue;
//                        }
                        else
                        {
                            send(ptr->socket , &buffer[3], strlen(&buffer[3]) , 0);
                        }
                        ptr = ptr->next;
                    }
                    buffer[0] = 0;
                }
                else
                {
                    (void) buffer;
                }

            }
        }
        buffer[0] = 0;
    }

    close(thread_ptr->socket);
    removeThread(head, thread_ptr->cl_name);
    (void) valread;
    return NULL;
}

void* accepter()
{
    int new_socket;
    threadnode* thread_ptr;
    while(run)
    {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        else
        {

            thread_ptr = addThread(head);
            thread_ptr->socket = new_socket;
            //                        messenger(thread_ptr);
            pthread_create(&thread_ptr->thread, NULL, &messenger, (void*) thread_ptr);
        }

    }
    return NULL;
}

int main()
{
    pthread_mutex_init(&mutex, NULL);
    pthread_mutex_lock(&mutex);
    signal(SIGINT, handle_sigint);
    int opt = 1;
    addrlen = sizeof(address);

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 1337
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                   &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );

    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *)&address,
             sizeof(address))<0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    head = initThreadList();
    pthread_t accepter_thread;
    pthread_create(&accepter_thread, NULL, &accepter, NULL);

    head = initThreadList();
    strcpy(head->cl_name, "HEAD");
    pthread_mutex_lock(&mutex);

    {
        pthread_cancel(accepter_thread);
        //stop threads
        threadnode* current = head;
        threadnode* tmp;
        while (current->next != NULL)
        {
            tmp = current->next;
            pthread_cancel(current->thread);
            current = tmp;
        }

    }

    clearThreadList(head);

    return 0;
}

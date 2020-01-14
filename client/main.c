// Client side C/C++ program to demonstrate Socket programming
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#define PORT 1337

int run = 1;
char buffer[1024];
char name[8];

void* messenger(void* _socket)
{
    int socket = *((int*) _socket);
    long valread;
    while(run)
    {
        valread = read(socket , buffer, 1024);
        if (valread <= 0)
        {
            run = 0;
            break;
        }
        if (strcmp(buffer, "bye") == 0)
            break;
        printf("Incoming Message: %s\n", buffer);
        memset(buffer, 0x00, valread);
    }
    return NULL;
}

int main()
{
    memset(buffer, 0x00, 1024);
    memset(name, 0x00, 8);
    printf("Enter your name here (8 character): ");
    gets(name);
    while(strcmp(name, "exit") == 0)
    {
        memset(name, 0x00, 8);
        printf("Exit is reserved\nPlease enter another name here (8 character): ");
        gets(name);
    }

    int sock = 0;
    struct sockaddr_in serv_addr;
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }

    send(sock , name , strlen(name) , 0);

    pthread_t messenger_thread;
    pthread_create(&messenger_thread, NULL, &messenger, (void*) &sock);

    char sender[1024];
    size_t sender_len = 0;
    memset(sender, 0x00, 1024);
    char to[8];
    memset(to, 0x00, 8);

    while(run)
    {
        gets(sender);

        //Leave
        if (strcmp(sender, "exit") == 0)
        {
            run = 0;
            sender_len = strlen(sender);
            send(sock, sender, sender_len, 0);
        }
        else
        {
            if(sender[0] == '/' && sender[1] == 'm')
            {
                int i = 0;
                for(; i < 8; ++i)
                {
                    if(sender[3 + i] != ' ')
                    {
                        to[i] = sender[3 + i];
                    }
                    else
                        break;
                }
                to[i] = 0;
                printf("Your message to %s:%s, size:%ld\n", to, &sender[4 + i], strlen(&sender[4 + i]));
                memset(to, 0x00, 8);
            }
            else
                printf("Your message:%s, size:%ld\n", sender, strlen(sender));

            sender_len = strlen(sender);
            send(sock, sender, sender_len, 0);
            memset(sender, 0x00, sender_len);
        }
    }
    pthread_join(messenger_thread, NULL);

    return 0;
}

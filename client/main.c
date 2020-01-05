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
    char filename[18];
    strcpy(filename, "/home/mad/");
    strcpy(&filename[10], name);
    FILE * fp;
    fp = fopen (filename,"w");
    while(run)
    {
        valread = read(socket , buffer, 1024);
        if (strcmp(buffer, "bye") == 0)
            break;
        fprintf(fp, "%s\n", buffer);
    }
    fclose (fp);
    (void) valread;
    return NULL;
}

int main()
{
    memset(buffer, 0x00, 1024);
    memset(name, 0x00, 8);
    printf("Enter your name here (8 character): ");
    scanf("%s", name);
//    while(strcmp(name, "exit") == 0)
//    {
//        memset(name, 0x00, 8);
//        printf("Exit is reserved\nPlease enter another name here (8 character): ");
//        scanf("%s", name);
//    }

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

    while(run)
    {
        scanf("%s", sender);
        printf("Your message:%s, size:%ld\n", sender, strlen(sender));
        if (strcmp(sender, "exit") == 0)
            run = 0;
        sender_len = strlen(sender);
        send(sock, sender, sender_len, 0);
        memset(sender, 0x00, sender_len);
    }
    pthread_join(messenger_thread, NULL);

    return 0;
}

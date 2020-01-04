// Client side C/C++ program to demonstrate Socket programming
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#define PORT 1337

int work = 1;
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
    while(work)
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
    buffer[0] = 0;
    printf("Enter your name here (8 character): ");
    scanf("%s", name);
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

    while(work)
    {
        scanf("%s", sender);
        if (strcmp(sender, "exit") == 0)
            work = 0;
        send(sock ,sender ,strlen(sender) , 0);
        sender[0] = '\0';
    }
//    strcpy(sender, "/bWtf");
//    printf("%s\n", sender);
//    send(sock ,sender ,strlen(sender) , 0);
//    sender[0] = 0;
//    strcpy(sender, "exit");
//    work = 0;
//    send(sock ,sender ,strlen(sender) , 0);

    pthread_join(messenger_thread, NULL);

    return 0;
}

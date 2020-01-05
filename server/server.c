#include "server.h"

extern int run;

extern int server_fd, addrlen;
extern struct sockaddr_in address;

clientnode* initClientList()
{
    clientnode* _head = (clientnode*)malloc(sizeof(clientnode));
    _head->next = NULL;
    return _head;
}

clientnode* addClient(clientnode* _head)
{
    clientnode* current = _head;
    while(current->next != NULL)
        current = current->next;
    current->next = initClientList();
    memset(current->next->client, 0x00, 8);
    return current->next;
}

void removeClient(clientnode* _head, const char* _client)
{
    clientnode* previous = _head;
    clientnode* current = head;
    while(current != NULL)
    {
        if(strcmp(current->client, _client) == 0)
        {
            previous->next = current->next;
            free(current);
            break;
        }
        else
        {
            previous = current;
            current = current->next;
        }
    }
}

void clearClientList(clientnode* _head)
{
    clientnode* current = _head;
    clientnode* tmp;
    while(current != NULL)
    {
        tmp = current->next;
        free(current);
        current = tmp;
    }
}

void printClient(clientnode* _head)
{
    clientnode* current = _head;
    while(current != NULL)
    {
        printf("Client : %s\n", current->client);
        current = current->next;
    }
}

void initServer()
{
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

    head = initClientList();
    strcpy(head->client, "HEAD");

    //    pthread_t accepter_thread;
    //    pthread_create(&accepter_thread, NULL, &accepter, NULL);
    //debug
    accepter();

}

void* accepter()
{
    int new_socket;
    clientnode* client_ptr;
    while(run)
    {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        else
        {
            client_ptr = addClient(head);
            client_ptr->socket = new_socket;

            if(read(client_ptr->socket , client_ptr->client, 8) >= 0)
            {
                if(strcmp(client_ptr->client, "exit") == 0)
                {
                    printf("Accept: Exiting...\n");
                    run = false;
                    break;
                }
                else
                {
                    //            pthread_create(&client_ptr->thread, NULL, &messenger, (void*) client_ptr);
                    //debug
                    messenger(client_ptr);
                }

            }
        }

    }
    return NULL;
}

void accepterClose()
{
    int sock = 0;
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return;
    }

    if (connect(sock, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        printf("\nConnection Failed \n");
        return;
    }
    send(sock , "exit", 5, 0);
}

void* messenger(void* _clientnode)
{
    clientnode* client_ptr = (clientnode*) _clientnode;
    char buffer[1024];
    memset(buffer, 0x00, 1024);
    long valread;
    printf("%s has entered the chat.\n", client_ptr->client);
    while(true)
    {
        valread = read(client_ptr->socket , buffer, 1024);
        if(strcmp(buffer, "exit") == 0)
        {
            printf("%s has left the chat.\n", client_ptr->client);
            send(client_ptr->socket , "bye", 3, 0);
            break;
        }
        else if(strlen(buffer) == 0)
        {
            printf("%s has dropped from the chat.\n", client_ptr->client);
            break;
        }
        else
        {
            clientnode* ptr = head->next;
            if(buffer[0] == '/' && buffer[1] == 'm')
            {
                printf("%s : %s, size : %ld\n", client_ptr->client, buffer, strlen(buffer));
            }
            else
            {
                while(ptr != NULL)
                {
//                    if(ptr == client_ptr)
//                    {
//                        ptr = ptr->next;
//                        continue;
//                    }
//                    else
//                    {
//                        send(ptr->socket , &buffer[3], strlen(&buffer[3]) , 0);
//                    }
                    send(ptr->socket , &buffer[3], strlen(&buffer[3]) , 0);
                    ptr = ptr->next;
                }
            }

        }
        memset(buffer, 0x00, valread);
    }
    close(client_ptr->socket);
    removeClient(head, client_ptr->client);
    return NULL;
}

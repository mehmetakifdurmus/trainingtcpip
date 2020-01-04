#ifndef THREADLIST_H
#define THREADLIST_H
#include <pthread.h>
#include <malloc.h>
#include <string.h>
#include <stdio.h>

typedef struct _threadnode
{
    pthread_t thread;
    char cl_name[8];
    struct _threadnode* next;
    int socket;
} threadnode;

threadnode* initThreadList()
{
    threadnode* head = (threadnode*)malloc(sizeof(threadnode));
    head->next = NULL;
    head->cl_name[0] = 0;
    return head;
}

threadnode* addThread(threadnode* head)
{
    threadnode* current = head;
    while(current->next != NULL)
        current = current->next;
    current->next = initThreadList();    
    return current->next;
}

void removeThread(threadnode* head, const char* cl_name)
{
    threadnode* previous = head;
    threadnode* current = head;
    while(current != NULL)
    {
        if(strcmp(current->cl_name, cl_name) == 0)
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

void clearThreadList(threadnode* head)
{
    threadnode* current = head;
    threadnode* tmp;
    while (current->next != NULL)
    {
        tmp = current->next;
        free(current);
        current = tmp;
    }
    //final current
    free(current);
}

void printThread(threadnode* head)
{
    threadnode* current = head;
    while(current != NULL)
    {
        printf("NODE : %s\n", current->cl_name);
        current = current->next;
    }
}



#endif // THREADLIST_H

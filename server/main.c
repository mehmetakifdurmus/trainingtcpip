// Server side C/C++ program to demonstrate Socket programming
#include <unistd.h>
#include <string.h>
#include "server.h"
#include <signal.h>

static pthread_mutex_t mutex;
extern int run;

extern clientnode* head;

void handle_signal(int sig)
{
    //unused
    if(sig == SIGINT)
    {
        printf("\nCaught signal SIGINT\nClosing server...\n");
        accepterClose();
        pthread_mutex_unlock(&mutex);
    }
}

int main()
{
    pthread_mutex_init(&mutex, NULL);
    pthread_mutex_lock(&mutex);
    signal(SIGINT, handle_signal);
    initServer();

    pthread_mutex_lock(&mutex);
    /*
    {
        //stop threads
        clientnode* current = head;
        clientnode* tmp;
        while (current->next != NULL || current == NULL)
        {
            tmp = current->next;
            pthread_cancel(current->thread);
            current = tmp;
        }

    }
*/
    clearClientList(head);

    return 0;
}

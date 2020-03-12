// Server side C/C++ program to demonstrate Socket programming
#include <unistd.h>
#include <string.h>
#include "server.h"
#include <signal.h>

pthread_mutex_t mutex;

void handle_signal(int sig)
{
    //unused
    if(sig == SIGINT)
    {
        printf("\nCaught signal SIGINT\nClosing server...\n");
        exit(0);
    }
}

int main()
{
    signal(SIGINT, handle_signal);
    initServer();

    //Server cannot know the crash!
    //this function is useless
    clearClientList(head);
    return 0;
}

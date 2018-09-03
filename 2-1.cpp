#include <cstdio>
#include <cstdlib>
#include <pthread.h>
#include <unistd.h>
#include "helper.h"

void *threadFunc(void *arg)
{
    for (int i = 0; i < 3; i++)
    {
        VPRINTF("I'm threadFunc: %d\n", i);
        sleep(1);
    }

    return NULL;
}


int p2_1_first_thread(int argv, char *argc[])
{
    VPRINTF("Hello world\n");

    pthread_t thread;

    if (pthread_create(&thread, NULL /* attr */, threadFunc, NULL) != 0)
    {
        VPRINTF("Error: Failed to create new thread.\n");
        exit(1);
    }

    for (int i = 0; i < 5; i++)
    {
        VPRINTF("I'm main: %d\n", i);
        sleep(1);
    }

    return 0;
}

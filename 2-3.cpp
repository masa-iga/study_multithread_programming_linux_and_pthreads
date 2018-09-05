#include <cstdio>
#include <cstdlib>
#include <pthread.h>
#include <unistd.h>
#include "helper.h"

static void anotherFunc(int n)
{
	if (n == 1)
	{
		VPRINTF("Hasta la vista, baby.\n");
#if 1
		pthread_exit(NULL); // the thread finishes
#else
		exit(0); // the program finishes
#endif
	}
}


static void *threadFunc(void *arg)
{
	for (int i = 0; i < 3; i++)
	{
		VPRINTF("I'm threadFunc: %d\n", i);
		anotherFunc(i);
		sleep(1);
	}

	return NULL;
}


int p2_3_first_thread2(int argv, char *argc[])
{
    VPRINTF("Hello world\n");

	pthread_t thread;

	if (pthread_create(&thread, NULL, threadFunc, NULL) != 0)
	{
		VPRINTF("Error: Failed to create new thread.\n");
		exit(1);
	}

	for (int i = 0; i < 5; ++i)
	{
		VPRINTF("I'm main: %d\n", i);
		sleep(1);
	}

    return 0;
}

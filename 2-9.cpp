#include <cstdio>
#include <cstdlib>
#include <pthread.h>
#include <unistd.h>
#include "helper.h"
#include <cstdint>

/* Sample for thread join and return */

namespace {

void *threadFunc(void *arg)
{
	int n = static_cast<int>(reinterpret_cast<int64_t>(arg));

	for (int i = 0; i < n; i++)
	{
		VPRINTF("I'm threadFunc: %d\n", i);
		sleep(1);
	}

	return reinterpret_cast<void*>(n);
}

} // anonymouse namespace


int p2_9_first_thread4(int argc, char *argv[])
{
    VPRINTF("Hello world\n");

	int n;

	if (argc > 1)
		n = atoi(argv[1]);
	else
		n = 3;


	pthread_t thread;

	if (pthread_create(&thread, NULL, threadFunc, reinterpret_cast<void*>(n)) != 0)
	{
		VPRINTF("error: failed to create new thread.\n");
		exit(1);
	}

	for (int i = 0; i < 5; i++)
	{
		VPRINTF("I'm main: %d\n", i);
		sleep(1);
	}

	int ret;

	if (pthread_join(thread, (void**)&ret) != 0)
	{
		VPRINTF("error: failed to wait for the thread termination.\n");
		exit(1);
	}

	VPRINTF("threadFunc has been terminated with number %d\n", ret);
	VPRINTF("Bye.\n");

    return 0;
}

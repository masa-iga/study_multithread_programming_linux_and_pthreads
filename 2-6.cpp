#include <cstdio>
#include <cstdlib>
#include <pthread.h>
#include <unistd.h>
#include "helper.h"
#include <cstdint>

/* Sample for thread id and thread sync */

namespace {

void *threadFunc(void *arg)
{
	int n = static_cast<int>(reinterpret_cast<int64_t>(arg));

	for (int i = 0; i < n; ++i)
	{
		VPRINTF("I'm threadFunc: %d\n", i);
		sleep(1);
	}

	return nullptr;
}

} // anonymouse namespace


int p2_6_first_thread3(int argc, char *argv[])
{
    VPRINTF("Hello world\n");

	int n;
	if (argc > 1)
		n = atoi(argv[1]);
	else
		n = 8;

	pthread_t thread;
	if (pthread_create(&thread, NULL, threadFunc, reinterpret_cast<void*>(n)) != 0)
	{
		VPRINTF("error: failed to create new thread.\n");
		exit(1);
	}

	for (int i = 0; i < 5; i++)
	{
		VPRINTF("I'm main: %d\n", i);;
		sleep(1);
	}

	if (pthread_join(thread, NULL) != 0)
	{
		VPRINTF("error: failed to wait for the thread termination.\n");
		exit(1);
	}

	VPRINTF("Byte.\n");

    return 0;
}

#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <pthread.h>
#include <unistd.h>
#include "helper.h"

/* Sample of thread attribute: stack size */

namespace {

	const int32_t kArrayLength = 100 * 1000 * 1000;
	//const int32_t kArrayLength = 100;

	void *threadFunc(void *arg)
	{
		double table[kArrayLength];

		for (int i = 0; i < kArrayLength; i++)
		{
			table[i] = static_cast<double>(i) * 3.14f;
		}

		return nullptr;
	}

} // anonymouse namespace


int p2_10_stack_size(int argc, char *argv[])
{
    VPRINTF("Hello world\n");

	pthread_attr_t attr;
	pthread_attr_init(&attr);

	const int32_t stackSize = kArrayLength * sizeof(double) + 100000;

	if (pthread_attr_setstacksize(&attr, stackSize) != 0)
	{
		VPRINTF("failed to set stack size\n");
		exit(1);
	}

	pthread_t thread;

	if (pthread_create(&thread, &attr, threadFunc, NULL) != 0)
	{
		VPRINTF("error: failed to create new thread.\n");
		exit(1);
	}

	if (pthread_join(thread, NULL) != 0)
	{
		VPRINTF("error: failed to wait for the thread termination.\n");
		exit(1);
	}

    return 0;
}

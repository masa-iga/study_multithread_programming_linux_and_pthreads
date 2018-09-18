#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <pthread.h>
#include <unistd.h>
#include "helper.h"

/* Sample of thread attribute: stack size */

namespace {

	int g_varA;

	void *threadFunc(void *arg)
	{
		int32_t n = static_cast<int32_t>(reinterpret_cast<int64_t>(arg)); // 64 bit to 32 bit
		int varB;

		varB = 4 * n;
		VPRINTF("g_varA=%d (0x%p), varB=%d (0x%p)\n", g_varA, &g_varA, varB, &varB);

		g_varA = 5 * n;
		VPRINTF("g_varA=%d (0x%p), varB=%d (0x%p)\n", g_varA, &g_varA, varB, &varB);

		sleep(2);

		VPRINTF("g_varA=%d (0x%p), varB=%d (0x%p)\n", g_varA, &g_varA, varB, &varB);

		varB = 6 * n;
		VPRINTF("g_varA=%d (0x%p), varB=%d (0x%p)\n", g_varA, &g_varA, varB, &varB);

		return nullptr;
	}

} // anonymouse namespace


int p3_2_variables(int argc, char *argv[])
{
    VPRINTF("Hello world\n");

	int varB;
	g_varA = 1; varB = 2;

	VPRINTF("g_varA=%d (0x%p), varB=%d (0x%p)\n", g_varA, &g_varA, varB, &varB);

	pthread_t thread1, thread2;

	pthread_create(&thread1, NULL, threadFunc, reinterpret_cast<void *>(1));

	sleep(1);

	varB = 3;

	VPRINTF("g_varA=%d (0x%p), varB=%d (0x%p)\n", g_varA, &g_varA, varB, &varB);

	pthread_create(&thread2, NULL, threadFunc, reinterpret_cast<void*>(2));

	pthread_join(thread1, NULL);
	pthread_join(thread2, NULL);

	VPRINTF("g_varA=%d (0x%p), varB=%d (0x%p)\n", g_varA, &g_varA, varB, &varB);


    return 0;
}

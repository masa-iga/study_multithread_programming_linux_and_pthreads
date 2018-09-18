#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <pthread.h>
#include <unistd.h>
#include "helper.h"

namespace {

	const uint32_t kStringSize = 32;

	char *g_varA;

	void *threadFunc(void *arg)
	{
		int n = static_cast<int>(reinterpret_cast<int64_t>(arg));

		snprintf(g_varA, kStringSize, "Hello I'm No.%d", n);

		VPRINTF("sets g_varA as '%s' (n = %d)\n", g_varA, n);

		sleep(2);
		
		VPRINTF("after 2 secs. g_varA is '%s' (n = %d)\n", g_varA, n);

		return nullptr;
	}

} // anonymouse namespace


int p3_3_variables2(int argc, char *argv[])
{
    VPRINTF("Hello world\n");

	g_varA = reinterpret_cast<char*>(malloc(kStringSize));

	strcpy(g_varA, "Good morning");;

	VPRINTF("g_varA is '%s'\n", g_varA);

	pthread_t thread1;
	pthread_create(&thread1, NULL, threadFunc, (void*)1);

	sleep(1);

	VPRINTF("g_varA is '%s'\n", g_varA);

	pthread_t thread2;
	pthread_create(&thread2, NULL, threadFunc, (void *)2);

	pthread_join(thread1, NULL);
	pthread_join(thread2, NULL);

	VPRINTF("g_varA is '%s'\n", g_varA);

	free(g_varA);

    return 0;
}

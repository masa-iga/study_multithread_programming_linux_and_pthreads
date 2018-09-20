#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <pthread.h>
#include <unistd.h>
#include "helper.h"
#include <sys/wait.h> // waitpid()

namespace
{
	int g_varA;

	void processFunc(int n)
	{
		int varB = n;
		VPRINTF("n = %d: g_varA address 0x%p, barB address 0x%p\n", n, &g_varA, &varB);

		VPRINTF("n = %d: g_varA = %d, barB = %d\n", n, g_varA, varB);

		g_varA = 5 * n;
		VPRINTF("n = %d: g_varA = %d, barB = %d\n", n, g_varA, varB);

		sleep(2);

		VPRINTF("n = %d: g_varA = %d, barB = %d\n", n, g_varA, varB);

		varB = 6 * n;
		VPRINTF("n = %d: g_varA = %d, barB = %d\n", n, g_varA, varB);

		exit(0);
	}

} // anonymouse namespace


int p3_6_variablesProc(int argc, char *argv[])
{
    VPRINTF("Hello world\n");

	g_varA = 1;
	int varB = 2;

	VPRINTF("g_varA address 0x%p, barB address 0x%p\n", &g_varA, &varB);

	VPRINTF("g_varA = %d, barB = %d\n", g_varA, varB);

	pid_t process1;
	if ((process1 = fork()) == 0)
	{
		// this is child process
		processFunc(1);
	}

	// this is main process
	sleep(1);

	varB = 3;

	VPRINTF("g_varA = %d, barB = %d\n", g_varA, varB);

	pid_t process2;
	if ((process2 = fork() == 0))
	{
		// this is child process
		processFunc(2);
	}

	// this is main process
	waitpid(process1, NULL /* status */, 0 /* options */);
	waitpid(process2, NULL /* status */, 0 /* options */);

	VPRINTF("g_varA = %d, barB = %d\n", g_varA, varB);

    return 0;
}

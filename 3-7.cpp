#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/wait.h> // waitpid()
#include "helper.h"

namespace
{
    void processFunc(void)
    {
        const char *msg = "processFunc writes to standard output\n";

        write(1 /* fd; standard output */, msg, strlen(msg));

        VPRINTF("close standard output\n");
        close(1);

        int fd = open("fdProcess.txt", O_WRONLY| O_CREAT | O_TRUNC, 0744);
        if (fd < 0)
        {
            VPRINTF("open() failed. (%d)\n", fd);
        }
        VPRINTF("fd = %d\n", fd);

        msg = "processFunc writes to fdProcess.txt\n";

        write(1, msg, strlen(msg));

        sleep(2);

        exit(0);
    }

} // anonymouse namespace


int p3_7_fdProcess(int argc, char *argv[])
{
    VPRINTF("Hello world\n");

    pid_t process;

    if ((process = fork() == 0))
    {
        // this is child process
        processFunc();
    }

    // this is main process
    sleep(1);

    const char *msg = "main writes to standard output\n";
    write(1, msg, strlen(msg));

    waitpid(process, NULL /* status */, 0 /* option */);

    return 0;
}

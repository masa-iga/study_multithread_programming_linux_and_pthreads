#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
//#include <sys/wait.h> // waitpid()
#include "helper.h"

namespace
{
    const int32_t kDataSize = 10 * 1000 * 1000;
    const int32_t kThreads = 10;

    int32_t g_data[kDataSize];

    void setRandomData()
    {
        for (int i = 0; i < kDataSize; i++)
        {
            g_data[i] = rand();
        }
    }

    int32_t getMax(size_t start_index, size_t end_index)
    {
        int32_t max = g_data[start_index];

        for (size_t i = start_index; i <= end_index; i++)
        {
            if (max < g_data[i])
                max = g_data[i];
        }

        return max;
    }

    void *threadFunc(void *arg)
    {
        const int32_t n = static_cast<int32_t>(reinterpret_cast<uint64_t>(arg));

        const size_t start_index = (kDataSize / kThreads) * n;
        const size_t end_index = start_index + (kDataSize / kThreads) - 1;

        const int32_t max = getMax(start_index, end_index);

        return reinterpret_cast<void*>(max);
    }

} // anonymouse namespace


int p3_9_maxData(int argc, char *argv[])
{
    VPRINTF("Hello world\n");

    srand(static_cast<unsigned int>(time(NULL)));
    setRandomData();


    pthread_t threads[kThreads];

    for (int32_t i = 0; i < kThreads; i++)
    {
        if (pthread_create(&threads[i], NULL, threadFunc, reinterpret_cast<void*>(i)) != 0)
        {
            VPRINTF("error: failed to create new thread\n");
            exit(1);
        }
    }


    int32_t res[kThreads];

    for (int32_t i = 0; i < kThreads; i++)
    {
        if (pthread_join(threads[i], (void**)&(res[i])) != 0)
        {
            VPRINTF("error: failed to wait for the thread termination (thread # %d)\n", i);
            exit(1);
        }
    }


    int32_t max = res[0];

    for (int32_t i = 1; i < kThreads; i++)
    {
        if (max < res[i])
            max = res[i];
    }

    VPRINTF("Max value is %d\n", max);

    return 0;
}

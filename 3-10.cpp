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

    static_assert(kDataSize > 0, "kDataSize must be larger than 0");
    static_assert(kThreads > 0, "kThreads must be larger than 0");
    static_assert(kDataSize % kThreads == 0, "kDataSize must be power of kThreads");

    int32_t g_data[kDataSize];

    pthread_key_t g_start_index_key;
    pthread_key_t g_end_index_key;

    void setRandomData()
    {
        VPRINTF("RAND_MAX is 0x%x\n", RAND_MAX);

        for (int32_t i = 0; i < kDataSize; i++)
        {
            g_data[i] = rand();
        }
    }

    int32_t getMax()
    {
        size_t start_index = reinterpret_cast<size_t>(pthread_getspecific(g_start_index_key));
        size_t end_index = reinterpret_cast<size_t>(pthread_getspecific(g_end_index_key));

        int32_t max = g_data[start_index];

        for (size_t i = start_index; i < end_index; i++)
        {
            if (max < g_data[i])
            {
                max = g_data[i];
            }
        }

        VPRINTF("max 0x%x start %zd end %zd\n", max, start_index, end_index);

        return max;
    }

    void *threadFunc(void *arg)
    {
        int32_t n = static_cast<int32_t>(reinterpret_cast<int64_t>(arg));

        pthread_setspecific(g_start_index_key, reinterpret_cast<void*>((kDataSize/kThreads) * n) /* value */);
        pthread_setspecific(g_end_index_key, reinterpret_cast<void*>((kDataSize/kThreads) * (n + 1) - 1));

        int32_t max = getMax();

        return reinterpret_cast<void*>(max);
    }
} // anonymouse namespace


int p3_10_maxData2(int argc, char *argv[])
{
    VPRINTF("Hello world\n");

    srand(static_cast<unsigned int>(time(NULL)));
    setRandomData();

    pthread_key_create(&g_start_index_key, NULL /* destructor */);
    pthread_key_create(&g_end_index_key, NULL /* destructor */);

    pthread_t threads[kThreads];

    for (int32_t i = 0; i < kThreads; i++)
    {
        if (pthread_create(&threads[i], nullptr, threadFunc, reinterpret_cast<void*>(i)) != 0)
        {
            VPRINTF("error: failed to create new thread (thread# %d)\n", i);
            exit(1);
        }
    }

    int32_t res[kThreads];

    for (int32_t i = 0; i < kThreads; i++)
    {
        if (pthread_join(threads[i], reinterpret_cast<void**>(&res[i])) != 0)
        {
            VPRINTF("error: failed to wait for the thread termination (thread# %d)\n", i);
            exit(1);
        }
    }

    int32_t max = res[0];
    for (int32_t i = 1; i < kThreads; i++)
    {
        if (max < res[i])
            max = res[i];
    }

    VPRINTF("max value is 0x%x\n", max);

    return 0;
}

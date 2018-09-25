#include <cstdio>
#include <cstdlib>
#include <cstdint>
//#include <cstring>
//#include <unistd.h>
#include <pthread.h>
#include "helper.h"

namespace
{
    const int32_t kMaxPrimeNumbers = 100 * 1000;
    const int32_t kNumThread = 1000;

    int32_t g_primeNumbers[kMaxPrimeNumbers];
    int32_t g_nPrimeNumber;

    pthread_mutex_t g_usingPrimeNumber;

    bool isPrimeNumber(int32_t m)
    {
        for (int32_t i = 0; i < g_nPrimeNumber; ++i)
        {
            if (m % g_primeNumbers[i] == 0)
                return false;
        }

        return true;
    }

    int32_t countPrimeNumbers(int32_t n)
    {
        pthread_mutex_lock(&g_usingPrimeNumber);

        g_nPrimeNumber = 0;

        for (int32_t i = 2; i <= n; ++i)
        {
            if (isPrimeNumber(i))
            {
                if (g_nPrimeNumber >= kMaxPrimeNumbers)
                {
                    VPRINTF("oops, too many prime numbers.\n");
                    exit(1);
                }

                g_primeNumbers[g_nPrimeNumber] = i;
                g_nPrimeNumber++;
            }
        }

        int32_t ret = g_nPrimeNumber;

        pthread_mutex_unlock(&g_usingPrimeNumber);

        return ret;
    }

    void *threadFunc(void *arg)
    {
        int32_t n = static_cast<int32_t>(reinterpret_cast<int64_t>(arg));

        int x = countPrimeNumbers(n);

        if (x != 25)
        {
            VPRINTF("Number of prime numbers under %d is %d!?\n", n, x);
        }

        return nullptr;
    }

} // anonymouse namespace


int p4_4_primeNumber5(int argc, char *argv[])
{

    pthread_mutex_init(&g_usingPrimeNumber, NULL /* attr */);

    pthread_t threads[kNumThread];

    for (int32_t i = 0; i < kNumThread; ++i)
    {
        if (pthread_create(&threads[i], NULL, threadFunc, (void*)100) != 0)
        {
            VPRINTF("can't create thread (%d)\n", i);
            exit(1);
        }
    }

    for (int32_t i = 0; i < kNumThread; ++i)
    {
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_destroy(&g_usingPrimeNumber);

    VPRINTF("done\n");

    return 0;
}

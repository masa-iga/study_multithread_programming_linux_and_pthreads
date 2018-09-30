#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <pthread.h>
#include "helper.h"
#include <cerrno>

namespace
{

    const int32_t kMaxPrimeNumbers = 100 * 1000;
    const int32_t kNumberOfThread = 6;

    int32_t g_primeNumbers[kMaxPrimeNumbers];
    int32_t g_nPrimeNumber;
    int32_t g_primeNumberChecked;

    pthread_mutex_t g_usingPrimeNumber;

    bool isPrimeNumber(int32_t m)
    {
        for (int32_t i = 0; i < g_nPrimeNumber; ++i)
        {
            if (g_primeNumbers[i] > (m/2))
            {
                return true;
            }

            if ((m % g_primeNumbers[i]) == 0)
            {
                return false;
            }
        }

        return true;
    }

    void generatePrimeNumbers(int32_t n)
    {
        pthread_mutex_lock(&g_usingPrimeNumber);

        if (n <= g_primeNumberChecked)
        {
            pthread_mutex_unlock(&g_usingPrimeNumber);
            return;
        }

        for (int32_t i = g_primeNumberChecked + 1; i <= n; ++i)
        {
            if (isPrimeNumber(i))
            {
                if (g_nPrimeNumber >= kMaxPrimeNumbers)
                {
                    VPRINTF("Oops, too many prime numbers\n");
                    return;
                }

                g_primeNumbers[g_nPrimeNumber] = i;
                g_nPrimeNumber++;
            }
        }

        g_primeNumberChecked = n;

        pthread_mutex_unlock(&g_usingPrimeNumber);
    }

    int32_t countPrimeNumbers(int32_t n)
    {
        generatePrimeNumbers(n);

        int32_t count = 0;

        for (int32_t i = 0; i < g_nPrimeNumber; ++i)
        {
            if (g_primeNumbers[i] > n)
            {
                break;
            }

            ++count;
        }

        return count;
    }

    void *threadFunc(void *arg)
    {
        int32_t n = static_cast<int32_t>(reinterpret_cast<int64_t>(arg));
        int32_t x = countPrimeNumbers(n);

        VPRINTF("number of prime numbers under %d is %d\n", n, x);

        return nullptr;
    }

} // anonymouse namespace


int p4_6_fastPrimeNumber(int argc, char *argv[])
{
    const int32_t number_list[kNumberOfThread] = { 1, 10, 100, 1000, 10000, 100000 };

    g_nPrimeNumber = 0;
    g_primeNumberChecked = 1;

    pthread_mutex_init(&g_usingPrimeNumber, NULL);

    pthread_t threads[kNumberOfThread];

    for (int32_t i = 0; i < kNumberOfThread; ++i)
    {
        if (pthread_create(&threads[i], NULL, threadFunc, reinterpret_cast<void*>(number_list[i])) != 0)
        {
            VPRINTF("can't create thread (%d)\n", i);
            exit(1);
        }
    }

    for (int32_t i = 0; i < kNumberOfThread; ++i)
    {
        if (pthread_join(threads[i], NULL) != 0)
        {
            VPRINTF("error: pthread_join() failed. errno %d\n", errno);
            exit(1);
        }
    }

    pthread_mutex_destroy(&g_usingPrimeNumber);

    VPRINTF("Done.\n");

    return 0;
}

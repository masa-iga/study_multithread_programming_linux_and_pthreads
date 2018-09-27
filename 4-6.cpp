#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <pthread.h>
#include "helper.h"

namespace
{
    const int32_t kMaxPrimeNumbers = 100 * 1000;

    int32_t g_primeNumbers[kMaxPrimeNumbers];
    int32_t g_nPrimeNumber;
    int32_t g_primeNumberChecked;


    bool isPrimeNumber(int32_t m)
    {
        for (int32_t i = 0; i < g_nPrimeNumber; ++i)
        {
            if (g_primeNumbers[i] > (m/2))
            {
                return true;
            }

            if (m % g_primeNumbers[i] == 0)
            {
                return false;
            }
        }

        return true;
    }

    void generatePrimeNumbers(int32_t n)
    {
        if (n <= g_primeNumberChecked)
        {
            return;
        }

        for (int32_t i = g_primeNumberChecked; i <= n; ++i)
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

            count++;
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
    const int32_t number_list[6] = { 1, 10, 100, 1000, 10000, 100000 };

    g_nPrimeNumber = 0;
    g_primeNumberChecked = 1;

    pthread_t threads[6];

    for (int i = 0; i < 6; ++i)
    {
        if (pthread_create(&threads[i], NULL, threadFunc, reinterpret_cast<void*>(number_list[i])) != 0)
        {
            VPRINTF("can't create thread (%d)\n", i);
            exit(1);
        }
    }

    return 0;
}

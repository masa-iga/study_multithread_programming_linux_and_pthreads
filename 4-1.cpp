#include <cstdio>
#include <cstdlib>
#include <cstdint>
//#include <cstring>
//#include <unistd.h>
//#include <pthread.h>
#include "helper.h"

namespace
{
    const int32_t kMaxPrimeNumbers = 100 * 1000;
    int32_t g_primeNumber[kMaxPrimeNumbers];
    int32_t g_nPrimeNumber;

    bool isPrimeNumber(int32_t m)
    {
        for (int i = 0; i < g_nPrimeNumber; ++i)
        {
            if (m % g_primeNumber[i] == 0)
                return false;
        }

        return true;
    }

    int32_t countPrimeNumbers(int n)
    {
        g_nPrimeNumber = 0;

        for (int i = 2; i <= n; ++i)
        {
            if (isPrimeNumber(i))
            {
                if (g_nPrimeNumber > kMaxPrimeNumbers)
                {
                    VPRINTF("Oops, too many prime numbers\n");
                    exit(1);
                }

                g_primeNumber[g_nPrimeNumber] = i;
                g_nPrimeNumber++;
            }
        }

        return g_nPrimeNumber;
    }
} // anonymouse namespace


int p4_1_primeNumber(int argc, char *argv[])
{
    int32_t n = 100 * 1000;

    int32_t x = countPrimeNumbers(n);
    VPRINTF("Number of prime numbers under %d is %d\n", n, x);

    n = 200 * 1000;

    x = countPrimeNumbers(n);
    VPRINTF("Number of prime numbers under %d is %d\n", n, x);

    return 0;
}

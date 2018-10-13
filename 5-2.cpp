#include <cstdio>
#include <cstdlib>
#include <pthread.h>
#include <unistd.h>
#include "helper.h"

pthread_mutex_t g_mutex;
pthread_cond_t g_cond;

namespace {

    void* threadFunc(void *arg)
    {
        VPRINTF("start.\n");

        sleep(2);

        pthread_mutex_lock(&g_mutex);
        {
            VPRINTF("wait for signal.\n");

            if (pthread_cond_wait(&g_cond, &g_mutex) != 0)
            {
                VPRINTF("error on pthread_cond_wait\n");
                exit(1);
            }

            VPRINTF("got a signal\n");
        }
        pthread_mutex_unlock(&g_mutex);

        return NULL;
    }

} // namespace anonymouse


int p5_2_condWait(int argc, char *argv[])
{
    pthread_mutex_init(&g_mutex, NULL);
    pthread_cond_init(&g_cond, NULL);

    pthread_t thread;
    pthread_create(&thread, NULL, threadFunc, NULL);

    sleep(3);

    VPRINTF("send signal\n");
    pthread_cond_signal(&g_cond);

    pthread_join(thread, NULL);

    pthread_cond_destroy(&g_cond);
    pthread_mutex_destroy(&g_mutex);

    return 0;
}

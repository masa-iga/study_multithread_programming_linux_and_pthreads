#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <pthread.h>
#include <cmath>
#include "helper.h"

namespace
{
    typedef struct
    {
        char    mark;
        double  x;
        double  y;
        double  angle;
        double  speed;
		pthread_mutex_t mutex;
    } Fly;

    const int32_t kWidth = 78;
    const int32_t kHeight = 23;
    const int32_t kMaxFly = 6;
    const char * const kFlyMarkList = "o@*+.#";
    const int32_t kDrawCycle = 50;
    const float kMinSpeed = 1.0f;
    const float kMaxSpeed = 20.0f;

    bool g_stop_request;

    Fly g_fly_list[kMaxFly];


    void mSleep(int32_t msec)
    {
        struct timespec ts;
        ts.tv_sec = msec/1000;
        ts.tv_nsec = (msec % 1000) * 1000 * 1000;
        nanosleep(&ts, NULL);
    }


    double randDouble(double min_value, double max_value)
    {
        return min_value + (double)rand()/((double)RAND_MAX + 1) * (max_value - min_value);
    }


    void clearScreen()
    {
        //fputs("\033[2J", stdout); // this escape code clears screen; this does not work in C++
		std::system("clear");
    }


    void moveCursor(int32_t x, int32_t y)
    {
        //printf("\033[%d;%dH", y, x); // this does not work in C++
    }


    void flyInitRandom(Fly *fly, char mark_)
    {
        fly->mark = mark_;
        fly->x = randDouble(0, static_cast<double>(kWidth - 1));
        fly->y = randDouble(0, static_cast<double>(kHeight - 1));
        fly->angle = randDouble(0, M_2_PI);
        fly->speed = randDouble(kMinSpeed, kMaxSpeed);
		pthread_mutex_init(&fly->mutex, NULL);
    }


	double flyDistance(Fly *fly, double x, double y)
	{
		pthread_mutex_lock(&fly->mutex);

		const double dx = x - fly->x;
		const double dy = y - fly->y;
		const double d = sqrt(dx * dx + dy * dy);

		pthread_mutex_unlock(&fly->mutex);

		return d;
	}


    void flyMove(Fly *fly)
    {
		pthread_mutex_lock(&fly->mutex);

		fly->x += cos(fly->angle);
		fly->y += sin(fly->angle);

		/* x方向の縁にぶつかったら方向を変える */
		if (fly->x < 0)
		{
			fly->x = 0;
			fly->angle = M_PI - fly->angle;
		}
		else if (fly->x > kWidth - 1)
		{
			fly->x = kWidth - 1;
			fly->angle = M_PI - fly->angle;
		}

		/* y方向の縁にぶつかったら方向を変える */
		if (fly->y < 0)
		{
			fly->y = 0;
			fly->angle = -(fly->angle);
		}
		else if (fly->y > kHeight - 1)
		{
			fly->y = kHeight - 1;
			fly->angle = -(fly->angle);
		}

		pthread_mutex_unlock(&fly->mutex);

		for (int32_t i = 0; i < kMaxFly; ++i)
		{
			if ((g_fly_list[i].mark != fly->mark) &&
				(flyDistance(&g_fly_list[i], fly->x, fly->y) < 2.0))
			{
				fly->angle += M_PI;
				break;
			}
		}

    }


    bool isFlyAt(Fly &fly, int32_t x, int32_t y)
    {
		pthread_mutex_lock(&fly.mutex);
		bool ret = (static_cast<int>(fly.x) == x) && (static_cast<int>(fly.y) == y);
		pthread_mutex_unlock(&fly.mutex);

		return ret;
    }


    void *doMove(void *arg)
    {
        Fly *fly = (Fly*)arg;

        while (!g_stop_request)
        {
			flyMove(fly);
            mSleep(static_cast<int>(1000.0 / fly->speed));
        }

        return nullptr;
    }


    void drawScreen()
    {
        moveCursor(0, 0);

		for (int32_t y = 0; y < kHeight; ++y)
		{
			for (int32_t x = 0; x < kWidth; ++x)
			{
				char ch = 0;

				/* if thre is a fly at (x, y), show its mark */
				for (int32_t i = 0; i < kMaxFly; ++i)
				{
					if (isFlyAt(g_fly_list[i], x, y))
					{
						ch = g_fly_list[i].mark;
						break;
					}
				}

				if (ch != 0)
				{
					putchar(ch);
				}
				else if ((y == 0) || (y == kHeight - 1))
				{
					/* print upper/lower edge */
					putchar('-');
				}
				else if ((x == 0) || (x == kWidth - 1))
				{
					/* print right/left edge */
					putchar('|');
				}
				else
				{
					putchar(' ');
				}
			}

			putchar('\n');
		}
    }


    void *doDraw(void *arg)
    {
		while (!g_stop_request)
		{
			drawScreen();
			mSleep(kDrawCycle);
		}

        return nullptr;
    }

} // anonymouse namespace


int p4_11_fly3(int argc, char *argv[])
{
	g_stop_request = false;

    srand(static_cast<unsigned int>(time(NULL)));

    clearScreen();

    for (int32_t i = 0; i < kMaxFly; ++i)
    {
        flyInitRandom(&g_fly_list[i], kFlyMarkList[i]);
    }


	/* Fly move */
    pthread_t move_thread[kMaxFly];

    for (int32_t i = 0; i < kMaxFly; ++i)
    {
        pthread_create(&move_thread[i], NULL, doMove, reinterpret_cast<void*>(&g_fly_list[i]));
    }


	/* Rendering */
	pthread_t draw_thread;
	pthread_create(&draw_thread, NULL, doDraw, NULL);


	/* メインスレッドは何か入力されるのを待つだけ */
	char buf[40];
	fgets(buf, sizeof(buf), stdin);
	g_stop_request = true;


	pthread_join(draw_thread, NULL);
	for (int32_t i = 0; i < kMaxFly; ++i)
	{
		pthread_join(move_thread[i], NULL);
		pthread_mutex_destroy(&g_fly_list[i].mutex);
	}


    return 0;
}

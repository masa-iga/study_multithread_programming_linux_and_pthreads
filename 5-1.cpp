#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <string.h>
#include <pthread.h>
#include <cmath>
#include "helper.h"

namespace
{
	const int32_t kWidth = 78;
	const int32_t kHeight = 23;
	const int32_t kMaxFly = 1;
	const int32_t kDrawCycle = 50;


	typedef struct
	{
		char	mark;
		double	x;
		double	y;
		double	angle;
		double	speed;
		double	destX;
		double	destY;
		bool	busy;
		pthread_mutex_t	mutex;
        pthread_cond_t cond;
	} Fly;


	Fly	g_fly_list[kMaxFly];
	bool g_stop_request;


	void mSleep(int32_t msec)
	{
        struct timespec ts;
        ts.tv_sec = msec / 1000;
        ts.tv_nsec = (msec % 1000) * (1000 * 1000);

        nanosleep(&ts, NULL);
	}


	void clearScreen()
	{
        //fputs("\033[2J", stdout); // this escape code clears screen; this does not work in C++
		std::system("clear");
	}


	void moveCursor(int32_t x, int32_t y)
	{
        printf("\033[%d;%dH", y, x);
	}


    void saveCursor()
    {
        printf("\0337");
        //system("tput sc");
    }

    void restoreCursor()
    {
        printf("\0338");
        //system("tput rc");
    }

	void flyInitCenter(Fly *fly, char mark_)
	{
		fly->mark = mark_;
		pthread_mutex_init(&fly->mutex, NULL);
        pthread_cond_init(&fly->cond, NULL);
		fly->x = static_cast<double>(kWidth) / 2.0f;
		fly->y = static_cast<double>(kHeight) / 2.0f;
		fly->angle = 0.0f;
		fly->speed = 2.0f;
		fly->destX = fly->x;
		fly->destY = fly->y;
		fly->busy = false;

		VPRINTF("x %f y %f\n", fly->x, fly->y);
	}


	void flyDestroy(Fly *fly)
	{
        pthread_mutex_destroy(&fly->mutex);
        pthread_cond_destroy(&fly->cond);
	}
	

	/* multi thread safe */
	void flyMove(Fly *fly)
	{
		pthread_mutex_lock(&fly->mutex);
		{
			fly->x += cos(fly->angle);
			fly->y += sin(fly->angle);
		}
		pthread_mutex_unlock(&fly->mutex);
	}


	bool isFlyAt(Fly *fly, int32_t x, int32_t y)
	{
        bool res;

        pthread_mutex_lock(&fly->mutex);
        {
			const int32_t fly_x_int32 = static_cast<int32_t>(fly->x);
			const int32_t fly_y_int32 = static_cast<int32_t>(fly->y);
            res = (fly_x_int32 == x) && (fly_y_int32 == y);
			//VPRINTF("res %d : is fly (%f, %f) at (%d, %d)?\n", res, fly->x, fly->y, x, y);
        }
        pthread_mutex_unlock(&fly->mutex);

		return res;
	}


	/* multi thread safe */
	/* adjust direction and speed for destination */
	void flySetDirection(Fly *fly)
	{
		pthread_mutex_lock(&fly->mutex);
		{
			const double dx = fly->destX - fly->x;
			const double dy = fly->destY - fly->y;

			fly->angle = atan2(dy, dx);
			fly->speed = sqrt(dx * dx + dy * dy) / 5.0f;

			// speed up if it is too slow since difficult to recognize
			if (fly->speed < 2.0f)
			{
				fly->speed = 2.0f;
			}
		}
		pthread_mutex_unlock(&fly->mutex);
	}


	/* multi thread safe */
	double flyDistanceToDestination(Fly *fly)
	{
		double res;

		pthread_mutex_lock(&fly->mutex);
		{
			const double dx = fly->destX - fly->x;
			const double dy = fly->destY - fly->y;
			res = sqrt(dx * dx + dy * dy);
		}
		pthread_mutex_unlock(&fly->mutex);

		return res;
	}


	bool flySetDestination(Fly *fly, double x, double y)
	{
        /* cannot set while the fly is moving */
        if (fly->busy)
            return false;

        pthread_mutex_lock(&fly->mutex);
        {
            fly->destX = x;
            fly->destY = y;
			pthread_cond_signal(&fly->cond);
        }
        pthread_mutex_unlock(&fly->mutex);

		return true;
	}

	void flyWaitForSetDestination(Fly *fly)
	{
		pthread_mutex_lock(&fly->mutex);
		{
			if (pthread_cond_wait(&fly->cond, &fly->mutex) != 0)
			{
				VPRINTF("fatal error on pthread_cond_wait.\n");
				exit(1);
			}
		}
		pthread_mutex_unlock(&fly->mutex);
	}


	void *doMove(void *arg)
	{
		Fly *fly = reinterpret_cast<Fly*>(arg);

		while (!g_stop_request)
		{
			/* wait until destination is set */
			fly->busy = false;

#if 0
			while ((flyDistanceToDestination(fly) < 1.0f) && !g_stop_request)
			{
				mSleep(100);
			}
#else
			flyWaitForSetDestination(fly);

			if (flyDistanceToDestination(fly) < 1.0f)
			{
				continue;
			}
#endif

			fly->busy = true;

			/* set direction for destination */
			flySetDirection(fly);

			/* keep moving until get to the destination */
			while ((flyDistanceToDestination(fly) >= 1.0f) && !g_stop_request)
			{
				flyMove(fly);
				mSleep(static_cast<int32_t>(1000.0f / fly->speed));
			}
		}

		return nullptr;
	}


	void drawScreen()
	{
        //saveCursor();

        //moveCursor(0, 0);

        for (int32_t y = 0; y < kHeight; ++y)
        {
            for (int32_t x = 0; x < kWidth; ++x)
            {
                char ch = 0;

                /* draw the fly's mark if it is located at (x, y) */
                for (int32_t i = 0; i < kMaxFly; ++i)
                {
                    if (isFlyAt(&g_fly_list[i], x, y))
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
                    putchar('-');
                }
                else if ((x == 0) || (x == kWidth - 1))
                {
                    putchar('|');
                }
                else
                {
                    putchar(' ');
                }
            }

            putchar('\n');
        }

        //restoreCursor();

        fflush(stdout);
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


int p5_1_soloFly(int argc, char *argv[])
{

	/* Initialize */
	clearScreen();
	flyInitCenter(&g_fly_list[0], '@');
	g_stop_request = false;


	pthread_t moveThread;
	pthread_create(&moveThread, NULL, doMove, reinterpret_cast<void*>(&g_fly_list[0]));

    pthread_t drawThread;
    pthread_create(&drawThread, NULL, doDraw, NULL);

    while (true)
    {
        printf("Destination? ");
        fflush(stdout);

        char buf[40];
        fgets(buf, sizeof(buf), stdin);

        if (strncmp(buf, "stop", 4) == 0)
        {
            break;
        }

        /* parse coordinate */
        char *cp;
        double dest_x = strtod(buf, &cp);
        double dest_y = strtod(cp, &cp);

		//VPRINTF("input x %f y %f\n", dest_x, dest_y);

        if (!flySetDestination(&g_fly_list[0], dest_x, dest_y))
        {
            printf("The fly is now busy now. Try later. Now the fly is at (%f, %f)\n", g_fly_list[0].x, g_fly_list[0].y);
        }
    }

    g_stop_request = true;

    pthread_join(drawThread, NULL);
    pthread_join(moveThread, NULL);

    flyDestroy(&g_fly_list[0]);

    return 0;
}

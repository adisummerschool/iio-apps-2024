#include <iio.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define URI "ip:10.76.84.102"
#define THRESHOLD 25

int main()
{
    struct iio_context* ctx;
    ctx = iio_create_context_from_uri(URI);

    if (!ctx)
    {
        perror("cannot get ctx!\n");
        exit(-1);
    }

    struct iio_device* dev;
    dev = iio_context_find_device(ctx, "ad5592r_s");

    if (!dev)
    {
        perror("cannot get device!\n");
        exit(-2);
    }

    iio_device_attr_write(dev, "en", "1");

    struct iio_channel* chan[6];

	while(1)
	{
		int val[6];
		for (int i = 0 ; i < sizeof(chan) / sizeof(chan[0]); ++i)
		{
			char attr_name[15];
			snprintf(attr_name, sizeof(attr_name), "voltage%d", i);
			chan[i] = iio_device_find_channel(dev, attr_name, false);

			if (!chan[i])
			{
				printf("cannot get channel %d!\n", i);
				exit(-3);
			}

			char* attr = iio_channel_find_attr(chan[i], "raw");

			if (!attr)
			{
				printf("cannot get attribute raw from channel %d!\n", i);
				exit(-4);
			}

			char value[20];
			iio_channel_attr_read(chan[i], "raw", value, sizeof(value));

			val[i] = atoi(value);
		}

		printf("X - ");
		if (val[0] <= THRESHOLD && val[1] <= THRESHOLD)
		{
			printf("OK\n");
		}
		else if (val[0] > THRESHOLD)
		{
			printf("Turn Right\n");
		}
		else if (val[1] > THRESHOLD)
		{
			printf("Turn Left\n");
		}

		printf("Y - ");
		if (val[2] <= THRESHOLD && val[3] <= THRESHOLD)
		{
			printf("OK\n");
		}
		else if (val[2] > THRESHOLD)
		{
			printf("Turn Right\n");
		}
		else if (val[3] > THRESHOLD)
		{
			printf("Turn Left\n");
		}

		printf("Z - ");
		if (val[4] <= THRESHOLD && val[5] <= THRESHOLD)
		{
			printf("OK\n");
		}
		else if (val[4] > THRESHOLD)
		{
			printf("Turn Right\n");
		}
		else if (val[5] > THRESHOLD)
		{
			printf("Turn Left\n");
		}

		printf("%d %d   %d %d   %d %d\n", val[0], val[1], val[2], val[3], val[4], val[5]);

		sleep(1);
		printf("\033[A");
		printf("\033[K");
		printf("\033[A");
		printf("\033[K");
		printf("\033[A");
		printf("\033[K");
		printf("\033[A");
		printf("\033[K");
	}

    iio_context_destroy(ctx);
    return 0;
}
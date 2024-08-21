#include <iio.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>

#define NO_OF_CHANNELS 6
#define URI "ip:10.76.84.102"
#define N 11
#define ALPHA (2.0 / (N + 1))
#define K 4
#define G 2200
#define MINIMUM_SHOCK_VALUE 0.25

void shift_elems(int vals[N][NO_OF_CHANNELS / 2], int channel_no)
{
    for (int t = 0; t < N - 1; ++t)
    {
        vals[t][channel_no] = vals[t + 1][channel_no];
    }

    return;
}

float mean(int vals[N][NO_OF_CHANNELS / 2], int channel_no)
{
    float sum = 0;
    for (int t = 0; t < N; ++t)
    {
        sum += vals[t][channel_no];
    }

    return (float)(sum / (float)N);
}

float standard_deviation(int vals[N][NO_OF_CHANNELS / 2], int channel_no, float mean)
{
    float sum = 0;
    for (int t = 0; t < N; ++t)
    {
        sum += ((vals[t][channel_no] - mean) * (vals[t][channel_no] - mean));
    }

    return (float)sqrt(sum / (float)N);
}

void clamp(int* val)
{
    if (*val < -G)
    {
        *val = -G;
    }
    else if (*val > G)
    {
        *val = G;
    }

    return;
}

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
        iio_context_destroy(ctx);
        exit(-2);
    }

    iio_device_attr_write(dev, "en", "1");

    struct iio_channel* chan[NO_OF_CHANNELS];
    for (int i = 0; i < NO_OF_CHANNELS; ++i)
    {
        char attr_name[15];
        snprintf(attr_name, sizeof(attr_name), "voltage%d", i);
        chan[i] = iio_device_find_channel(dev, attr_name, false);
        if (!chan[i])
        {
            printf("cannot get channel %d!\n", i);
            iio_context_destroy(ctx);
            exit(-3);
        }
    }

    int vals[N][NO_OF_CHANNELS / 2];

    // fill the "queue"
    int n = 0;
    while (n < N)
    {
        int val[NO_OF_CHANNELS];
        for (int i = 0; i < NO_OF_CHANNELS; ++i)
        {
            char* attr = iio_channel_find_attr(chan[i], "raw");
            if (!attr)
            {
                printf("cannot get attribute raw from channel %d!\n", i);
                iio_context_destroy(ctx);
                exit(-4);
            }

            char value[20];
            iio_channel_attr_read(chan[i], "raw", value, sizeof(value));

            val[i] = atoi(value);
        }

        vals[n][0] = val[0] - val[1]; clamp(&vals[n][0]);
        vals[n][1] = val[2] - val[3]; clamp(&vals[n][1]);
        vals[n][2] = val[4] - val[5]; clamp(&vals[n][2]);

        ++n;
    }

    float old_EMA_x = mean(vals, 0);
    float old_EMA_y = mean(vals, 1);
    float old_EMA_z = mean(vals, 2);

    // read new values
	while(1)
	{
        int val[NO_OF_CHANNELS];
        for (int i = 0; i < NO_OF_CHANNELS; ++i)
        {
            char* attr = iio_channel_find_attr(chan[i], "raw");
            if (!attr)
            {
                printf("cannot get attribute raw from channel %d!\n", i);
                iio_context_destroy(ctx);
                exit(-4);
            }

            char value[20];
            iio_channel_attr_read(chan[i], "raw", value, sizeof(value));

            val[i] = atoi(value);
        }

        int new_val_x = val[0] - val[1]; clamp(&new_val_x);
        int new_val_y = val[2] - val[3]; clamp(&new_val_y);
        int new_val_z = val[4] - val[5]; clamp(&new_val_z);

        float sigma_x = standard_deviation(vals, 0, mean(vals, 0));
        float sigma_y = standard_deviation(vals, 1, mean(vals, 1));
        float sigma_z = standard_deviation(vals, 2, mean(vals, 2));

        float upper_th_x = old_EMA_x + K * sigma_x;
        float lower_th_x = old_EMA_x - K * sigma_x;

        float upper_th_y = old_EMA_y + K * sigma_y;
        float lower_th_y = old_EMA_y - K * sigma_y;

        float upper_th_z = old_EMA_z + K * sigma_z;
        float lower_th_z = old_EMA_z - K * sigma_z;

        bool shock_x = false;
		printf("X - ");
        if (new_val_x < lower_th_x || new_val_x > upper_th_x)
        {
            float shock_value_x = fabs(new_val_x - old_EMA_x) / (float)G;
            if (shock_value_x > MINIMUM_SHOCK_VALUE)
            {
                printf("shock detected: %.2fG\n", shock_value_x);
                shock_x = true;
            }
            else
            {
                printf("no shock\n");
            }
        }
        else
        {
            printf("no shock\n");
        }

        bool shock_y = false;
		printf("Y - ");
        if (new_val_y < lower_th_y || new_val_y > upper_th_y)
        {
            float shock_value_y = fabs(new_val_y - old_EMA_y) / (float)G;
            if (shock_value_y > MINIMUM_SHOCK_VALUE)
            {
                printf("shock detected: %.2fG\n", shock_value_y);
                shock_y = true;
            }
            else
            {
                printf("no shock\n");
            }
        }
        else
        {
            printf("no shock\n");
        }

        bool shock_z = false;
		printf("Z - ");
        if (new_val_z < lower_th_z || new_val_z > upper_th_z)
        {
            float shock_value_z = fabs(new_val_z - old_EMA_z) / (float)G;
            if (shock_value_z > MINIMUM_SHOCK_VALUE)
            {
                printf("shock detected: %.2fG\n", shock_value_z);
                shock_z = true;
            }
            else
            {
                printf("no shock\n");
            }
        }
        else
        {
            printf("no shock\n");
        }

        //printf("%d %d %d %d %d %d\n", val[0], val[1], val[2], val[3], val[4], val[5]);
		printf("%d %.2f %.2f %.2f\n", new_val_z, old_EMA_z, lower_th_z, upper_th_z);

	    usleep(10000); // 10 milliseconds

        if (shock_x || shock_y || shock_z)
        {
            usleep(500000); // half a second
        }

		printf("\033[A");
		printf("\033[K");
		printf("\033[A");
		printf("\033[K");
		printf("\033[A");
		printf("\033[K");
		printf("\033[A");
		printf("\033[K");

        //if (!shock_x)
        //{
            old_EMA_x = ALPHA * new_val_x + (1 - ALPHA) * old_EMA_x;
            shift_elems(vals, 0);
            vals[N - 1][0] = new_val_x;
        //}
        //if (!shock_y)
        //{
            old_EMA_y = ALPHA * new_val_y + (1 - ALPHA) * old_EMA_y;
            shift_elems(vals, 1);
            vals[N - 1][1] = new_val_y;
        //}
        //if (!shock_z)
        //{
            old_EMA_z = ALPHA * new_val_z + (1 - ALPHA) * old_EMA_z;
            shift_elems(vals, 2);
            vals[N - 1][2] = new_val_z;
        //}
	}

    iio_context_destroy(ctx);
    return 0;
}
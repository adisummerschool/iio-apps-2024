#include <iio.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>

#define NO_OF_CHANNELS 6
#define URI "ip:10.76.84.102"
#define K 2
#define G 2200
#define MINIMUM_SHOCK_VALUE 0.2
#define MAX_ADC 4095.0
#define VOLT_PER_LSB (2.4 / MAX_ADC)
#define SAMPLES_COUNT 64
#define ALPHA (2.0 / (SAMPLES_COUNT + 1))
#define CYCLIC false
#define GROUP_SIZE 8

void shift_elems(int16_t vals[][NO_OF_CHANNELS / 2], int channel_no)
{
    for (int i = 0; i < SAMPLES_COUNT - GROUP_SIZE; ++i)
    {
        vals[i][channel_no] = vals[i + GROUP_SIZE][channel_no];
    }

    return;
}

double mean(int16_t vals[][NO_OF_CHANNELS / 2], int chnanel_no)
{
    double sum = 0;
    for (int i = 0; i < SAMPLES_COUNT; ++i)
    {
        sum += (double)vals[i][chnanel_no];
    }

    return (double)(sum / SAMPLES_COUNT);
}

double standard_deviation(int16_t vals[][NO_OF_CHANNELS / 2], int channel_no, double mean)
{
    double sum = 0;
    for (int i = 0; i < SAMPLES_COUNT; ++i)
    {
        sum += ((vals[i][channel_no] - mean) * (vals[i][channel_no] - mean));
    }

    return (double)sqrt(sum / SAMPLES_COUNT);
}

void clamp(int16_t* val)
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
        perror("cannot get context!\n");
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

        iio_channel_enable(chan[i]);
    }

    struct iio_buffer* buf;
    buf = iio_device_create_buffer(dev, SAMPLES_COUNT, CYCLIC);
    if (!buf)
    {
        printf("cannot create buffer!\n");
        iio_context_destroy(ctx);
        exit(-4);
    }

    // fill the "queue"
    int16_t vals[SAMPLES_COUNT][NO_OF_CHANNELS / 2];

    int bytes_read = iio_buffer_refill(buf);
    if (bytes_read < 0)
    {
        printf("error when reading from buffer!\n");
        iio_context_destroy(ctx);
        exit(-5);
    }
    int i = 0;
    for (void* ptr = iio_buffer_start(buf); ptr < iio_buffer_end(buf); ptr += iio_buffer_step(buf))
    {
        vals[i][0] = (*(uint16_t*)(ptr + 0 * sizeof(uint16_t))) - (*(uint16_t*)(ptr + 1 * sizeof(uint16_t))); clamp(&vals[i][0]);
        vals[i][1] = (*(uint16_t*)(ptr + 2 * sizeof(uint16_t))) - (*(uint16_t*)(ptr + 3 * sizeof(uint16_t))); clamp(&vals[i][1]);
        vals[i][2] = (*(uint16_t*)(ptr + 4 * sizeof(uint16_t))) - (*(uint16_t*)(ptr + 5 * sizeof(uint16_t))); clamp(&vals[i][2]);
        ++i;
    }

    double old_EMA_x = mean(vals, 0);
    double old_EMA_y = mean(vals, 1);
    double old_EMA_z = mean(vals, 2);

    // read new values
	while(1)
	{
        bytes_read = iio_buffer_refill(buf);
        if (bytes_read < 0)
        {
            printf("error when reading from buffer!\n");
            iio_context_destroy(ctx);
            exit(-5);
        }
        i = 0;

        int16_t val[SAMPLES_COUNT][NO_OF_CHANNELS / 2];
        for (void* ptr = iio_buffer_start(buf); ptr < iio_buffer_end(buf); ptr += iio_buffer_step(buf))
        {
            val[i][0] = (*(uint16_t*)(ptr + 0 * sizeof(uint16_t))) - (*(uint16_t*)(ptr + 1 * sizeof(uint16_t))); clamp(&val[i][0]);
            val[i][1] = (*(uint16_t*)(ptr + 2 * sizeof(uint16_t))) - (*(uint16_t*)(ptr + 3 * sizeof(uint16_t))); clamp(&val[i][1]);
            val[i][2] = (*(uint16_t*)(ptr + 4 * sizeof(uint16_t))) - (*(uint16_t*)(ptr + 5 * sizeof(uint16_t))); clamp(&val[i][2]);
            ++i;
        }

        int16_t new_val[GROUP_SIZE][NO_OF_CHANNELS / 2];
        for (i = 0; i < SAMPLES_COUNT; ++i)
        {
            new_val[i % GROUP_SIZE][0] = val[i][0];
            new_val[i % GROUP_SIZE][1] = val[i][1];
            new_val[i % GROUP_SIZE][2] = val[i][2];

            if ((i + 1) % GROUP_SIZE == 0)
            {
                double new_val_x = mean(new_val, 0);
                double new_val_y = mean(new_val, 1);
                double new_val_z = mean(new_val, 2);

                double sigma_x = standard_deviation(vals, 0, mean(vals, 0));
                double sigma_y = standard_deviation(vals, 1, mean(vals, 1));
                double sigma_z = standard_deviation(vals, 2, mean(vals, 2));

                double upper_th_x = old_EMA_x + K * sigma_x;
                double lower_th_x = old_EMA_x - K * sigma_x;

                double upper_th_y = old_EMA_y + K * sigma_y;
                double lower_th_y = old_EMA_y - K * sigma_y;

                double upper_th_z = old_EMA_z + K * sigma_z;
                double lower_th_z = old_EMA_z - K * sigma_z;

                bool shock_x = false;
                printf("X - ");
                if (new_val_x < lower_th_x || new_val_x > upper_th_x)
                {
                    double shock_value_x = fabs(new_val_x - old_EMA_x) / (double)G;
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
                    double shock_value_y = fabs(new_val_y - old_EMA_y) / (double)G;
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
                    double shock_value_z = fabs(new_val_z - old_EMA_z) / (double)G;
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

                printf("%.2f %.2lf %.2lf %.2lf\n", new_val_x, old_EMA_x, lower_th_x, upper_th_x);

                
                usleep(1000); // 1 milliseconds
                /*
                if (shock_x || shock_y || shock_z)
                {
                    usleep(500000); // half a second
                }
                */

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
                    for (int j = 0; j < GROUP_SIZE; ++j)
                    {
                        vals[SAMPLES_COUNT - 1 - j][0] = new_val[j][0];
                    }
                //}
                //if (!shock_y)
                //{
                    old_EMA_y = ALPHA * new_val_y + (1 - ALPHA) * old_EMA_y;
                    shift_elems(vals, 1);
                    for (int j = 0; j < GROUP_SIZE; ++j)
                    {
                        vals[SAMPLES_COUNT - 1 - j][1] = new_val[j][1];
                    }
                //}
                //if (!shock_z)
                //{
                    old_EMA_z = ALPHA * new_val_z + (1 - ALPHA) * old_EMA_z;
                    shift_elems(vals, 2);
                    for (int j = 0; j < GROUP_SIZE; ++j)
                    {
                        vals[SAMPLES_COUNT - 1 - j][2] = new_val[j][2];
                    }
                //}
            }
        }
	}

    iio_context_destroy(ctx);
    return 0;
}
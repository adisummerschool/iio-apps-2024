#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <iio.h>

#define URI "ip:10.76.84.236"
#define CHANS_NB 6

#define SAMPLES_CNT 1024
#define CYCLIC false
#define THRESHOLD 20
#define MAX_ADC 4095

int main()
{
    struct iio_context *ctx = iio_create_context_from_uri(URI);
    if (!ctx)
    {
        perror("cannot get ctx");
        return -1;
    }

    char *dev_name = "ad5592r_s";
    struct iio_device *dev;
    dev = iio_context_find_device(ctx, dev_name);
    if (!dev)
    {
        perror("cannot get device");
        return -1;
    }

    iio_device_attr_write(dev, "en", "1");

    struct iio_channel *chans[CHANS_NB];

    for (int i = 0; i < CHANS_NB; i++)
    {
        chans[i] = iio_device_get_channel(dev, i);
        if (!chans[i])
        {
            perror("cannot get channel");
            return -1;
        }
        iio_channel_enable(chans[i]);
    }

    struct iio_buffer *buf = iio_device_create_buffer(dev, SAMPLES_CNT, CYCLIC);
    if (!buf)
    {
        perror("cannot get buffer");
        return -1;
    }
    int bytes_read = iio_buffer_refill(buf);

    uint32_t refrence_avg[CHANS_NB] = {0};

    for (void *ptr = iio_buffer_start(buf); ptr < iio_buffer_end(buf); ptr += iio_buffer_step(buf))
    {
        uint16_t x_poz = *(uint16_t *)(ptr + 0 * sizeof(uint16_t));
        uint16_t x_neg = *(uint16_t *)(ptr + 1 * sizeof(uint16_t));
        uint16_t y_poz = *(uint16_t *)(ptr + 2 * sizeof(uint16_t));
        uint16_t y_neg = *(uint16_t *)(ptr + 3 * sizeof(uint16_t));
        uint16_t z_poz = *(uint16_t *)(ptr + 4 * sizeof(uint16_t));
        uint16_t z_neg = *(uint16_t *)(ptr + 5 * sizeof(uint16_t));

        refrence_avg[0] += x_poz;
        refrence_avg[1] += x_neg;
        refrence_avg[2] += y_poz;
        refrence_avg[3] += y_neg;
        refrence_avg[4] += z_poz;
        refrence_avg[5] += z_neg;
    }

    for (int i = 0; i < CHANS_NB; i++)
    {
        refrence_avg[i] /= SAMPLES_CNT;
    }

    uint32_t reference_G = refrence_avg[4];

    while (true)
    {
        int new_bytes_read = iio_buffer_refill(buf);

        uint32_t new_avg[CHANS_NB] = {0};

        for (void *ptr = iio_buffer_start(buf); ptr < iio_buffer_end(buf); ptr += iio_buffer_step(buf))
        {
            uint16_t x_poz = *(uint16_t *)(ptr + 0 * sizeof(uint16_t));
            uint16_t x_neg = *(uint16_t *)(ptr + 1 * sizeof(uint16_t));
            uint16_t y_poz = *(uint16_t *)(ptr + 2 * sizeof(uint16_t));
            uint16_t y_neg = *(uint16_t *)(ptr + 3 * sizeof(uint16_t));
            uint16_t z_poz = *(uint16_t *)(ptr + 4 * sizeof(uint16_t));
            uint16_t z_neg = *(uint16_t *)(ptr + 5 * sizeof(uint16_t));

            new_avg[0] += x_poz;
            new_avg[1] += x_neg;
            new_avg[2] += y_poz;
            new_avg[3] += y_neg;
            new_avg[4] += z_poz;
            new_avg[5] += z_neg;
        }

        double g_values[CHANS_NB];
        for (int i = 0; i < CHANS_NB; i++)
        {
            new_avg[i] /= SAMPLES_CNT;
            g_values[i] = new_avg[i] / (MAX_ADC * 1.0);

            // uint32_t diff = abs(new_avg[i] - refrence_avg[i]) / reference_G;
            uint32_t diff = abs(new_avg[i] - refrence_avg[i]);
            if (diff > THRESHOLD)
            {
                printf("\ndiff = %d, i = %d ||||||| ", diff, i);
                if (i == 0 || i == 1)
                    printf("x: ");
                else if (i == 2 || i == 3)
                    printf("y: ");
                else
                    printf("z: ");
                printf("shock\n");
            }
        }

        printf("refrence_avg: %4d %4d %4d %4d %4d %4d\n",
               refrence_avg[0], refrence_avg[1], refrence_avg[2], refrence_avg[3], refrence_avg[4], refrence_avg[5]);

        printf("     new_avg: %4d %4d %4d %4d %4d %4d\n",
               new_avg[0], new_avg[1], new_avg[2], new_avg[3], new_avg[4], new_avg[5]);

        printf("\n---------------------------------------------\n");
    }

    iio_context_destroy(ctx);

    return 0;
}

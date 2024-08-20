#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iio.h>
#include <unistd.h>
const int samples_count = 1024;
typedef struct
{
    uint16_t xpoz;
    uint16_t xneg;
    uint16_t ypoz;
    uint16_t yneg;
    uint16_t zpoz;
    uint16_t zneg;

} buffer_element;

#define URI "ip:10.76.84.242"

int main(int argc, char *argv[])
{
    struct iio_context *ctx;
    struct iio_device *dev;
    struct iio_buffer *buf;
    struct iio_channel *channels[6];
    bool cyclic = false;
    ctx = iio_create_context_from_uri(URI);
    if (!ctx)
    {
        printf("\n no context\n");
        return -1;
    }

    dev = iio_context_find_device(ctx, "iio:device0");
    if (!dev)
    {
        printf("\n no device \n");
        return -1;
    }
    iio_device_attr_write(dev, "en", "1");
    for (int i = 0; i < 6; i++)
    {
        channels[i] = iio_device_get_channel(dev, i);
        if (!channels[i])
        {
            printf("\n Channel not fund \n");
            return -1;
        }
        iio_channel_enable(channels[i]);
    }

    buf = iio_device_create_buffer(dev, samples_count, cyclic);
    if (!buf)
    {
        printf("\n cannot create buffer \n");
        return -1;
    }
    int bytes_read = iio_buffer_refill(buf);
    uint16_t values[6];
    uint32_t sum_xpoz = 0;
    uint32_t sum_xneg = 0;
    uint32_t sum_ypoz = 0;
    uint32_t sum_yneg = 0;
    uint32_t sum_zpoz = 0;
    uint32_t sum_zneg = 0;

    for (void *ptr = iio_buffer_start(buf); ptr < iio_buffer_end(buf); ptr += iio_buffer_step(buf))
    {
        uint32_t xpoz = *(uint16_t *)(ptr + 0 * sizeof(uint16_t));
        sum_xpoz = sum_xpoz + xpoz;
        //printf(" %d", xpoz);

        uint32_t xneg = *(uint16_t *)(ptr + 1 * sizeof(uint16_t));
        sum_xneg = sum_xneg + xneg;
        //printf(" %d", xneg);

        uint32_t ypoz = *(uint16_t *)(ptr + 2 * sizeof(uint16_t));
        sum_ypoz = sum_ypoz + ypoz;
        //printf(" %d", ypoz);

        uint32_t yneg = *(uint16_t *)(ptr + 3 * sizeof(uint16_t));
        sum_yneg = sum_yneg + yneg;
       // printf(" %d", yneg);

        uint32_t zpoz = *(uint16_t *)(ptr + 4 * sizeof(uint16_t));
        sum_zpoz = sum_zpoz + zpoz;
        //printf(" %d", zpoz);

        uint32_t zneg = *(uint16_t *)(ptr + 5 * sizeof(uint16_t));
        sum_zneg = sum_zneg + zneg;
        //printf(" %d", zneg);
    }
    uint32_t avg_xpoz = sum_xpoz / samples_count;
    printf(" MediaXpoz: %d", avg_xpoz);

    uint32_t avg_xneg = sum_xneg / samples_count;
    printf(" MediaXneg: %d", avg_xneg);

    uint32_t avg_ypoz = sum_ypoz / samples_count;
    printf(" MediaYpoz: %d", avg_ypoz);

    uint32_t avg_yneg = sum_yneg / samples_count;
    printf(" MediaYneg: %d", avg_yneg);

    uint32_t avg_zpoz = sum_zpoz / samples_count;
    printf(" MediaZpoz: %d", avg_zpoz);

    uint32_t avg_zneg = sum_zneg / samples_count;
    printf(" MediaZneg: %d", avg_zneg);
}
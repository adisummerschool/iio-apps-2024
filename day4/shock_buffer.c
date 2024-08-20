#include <iio.h>
#include <stdio.h>

#define URI "ip:10.76.84.243"

const int samples_count = 1024;
const int max_adc = 4095;

typedef struct
{
    uint16_t xpoz;
    uint16_t xneg;
    uint16_t ypoz;
    uint16_t yneg;
    uint16_t zpoz;
    uint16_t zneg;

} buffer_element;

int main()
{
    struct iio_context *ctx;
    struct iio_device *dev;
    struct iio_buffer *buf;
    struct iio_channel *channels[6];
    bool cyclic = false;

    ctx = iio_create_context_from_uri(URI);
    dev = iio_context_find_device(ctx, "iio:device0");

    if (!ctx){
        printf("\n no context\n");
        return -1;
    }

    if (!dev){
        printf("\n no device \n");
        return -1;
    }

    iio_device_attr_write(dev, "en", "1");
    
    for(int i = 0 ; i < 6; i++)
    {
        channels[i] = iio_device_get_channel(dev, i);
        if(!channels[i])
        {
            printf("\n Channel not found! \n");
            return -1;
        }
        iio_channel_enable(channels[i]);
    }

    buf = iio_device_create_buffer(dev, samples_count, cyclic);

    if (!buf){
        printf("\n cannot create buffer \n");
        return -1;
    }

    int bytes_read = iio_buffer_refill(buf);
    u_int16_t values[6];

    printf("Average value from buffers:");

    // X

    printf("\n");
    printf("\033[34m");
    uint32_t sum_xpoz = 0;
    for(void *ptr = iio_buffer_start(buf); ptr < iio_buffer_end(buf); ptr += iio_buffer_step(buf))
    {
        uint16_t xpoz = *(uint16_t *)(ptr + 0 * sizeof(uint16_t));
        sum_xpoz = sum_xpoz + xpoz;
    }
    uint32_t avg_xpoz = sum_xpoz / samples_count;
    double chn0_accel = 2 * (double)avg_xpoz / max_adc;
    printf("+X AVG: %d -- ACCEL: %.3f [g]", avg_xpoz, chn0_accel);

    printf("\n");
    printf("\033[32m");
    uint32_t sum_xneg = 0;
    for(void *ptr = iio_buffer_start(buf); ptr < iio_buffer_end(buf); ptr += iio_buffer_step(buf))
    {
        uint16_t xneg = *(uint16_t *)(ptr + 1 * sizeof(uint16_t));
        sum_xneg = sum_xneg + xneg;
    }
    uint32_t avg_xneg = sum_xneg / samples_count;
    double chn1_accel = 2 * (double)avg_xneg / max_adc;
    printf("-X AVG: %d -- ACCEL: %.3f [g]", avg_xneg, chn1_accel);

    // Y

    printf("\n");
    printf("\033[36m");
    uint32_t sum_ypoz = 0;
    for(void *ptr = iio_buffer_start(buf); ptr < iio_buffer_end(buf); ptr += iio_buffer_step(buf))
    {
        uint16_t ypoz = *(uint16_t *)(ptr + 2 * sizeof(uint16_t));
        sum_ypoz = sum_ypoz + ypoz;
    }
    uint32_t avg_ypoz = sum_ypoz / samples_count;
    double chn2_accel = 2 * (double)avg_ypoz / max_adc;
    printf("+Y AVG: %d -- ACCEL: %.3f [g]", avg_ypoz, chn2_accel);

    printf("\n");
    printf("\033[31m");
    uint32_t sum_yneg = 0;
    for(void *ptr = iio_buffer_start(buf); ptr < iio_buffer_end(buf); ptr += iio_buffer_step(buf))
    {
        uint16_t yneg = *(uint16_t *)(ptr + 3 * sizeof(uint16_t));
        sum_yneg = sum_yneg + yneg;
    }
    uint32_t avg_yneg = sum_yneg / samples_count;
    double chn3_accel = 2 * (double)avg_yneg / max_adc;
    printf("-Y AVG: %d -- ACCEL: %.3f [g]", avg_yneg, chn3_accel);

    // Z

    printf("\n");
    printf("\033[35m");
    uint32_t sum_zpoz = 0;
    for(void *ptr = iio_buffer_start(buf); ptr < iio_buffer_end(buf); ptr += iio_buffer_step(buf))
    {
        uint16_t zpoz = *(uint16_t *)(ptr + 4 * sizeof(uint16_t));
        sum_zpoz = sum_zpoz + zpoz;
    }
    uint32_t avg_zpoz = sum_zpoz / samples_count;
    double chn4_accel = 2 * (double)avg_zpoz / max_adc;
    printf("+Z AVG: %d -- ACCEL: %.3f [g]", avg_zpoz, chn4_accel);

    printf("\n");
    printf("\033[33m");
    uint32_t sum_zneg = 0;
    for(void *ptr = iio_buffer_start(buf); ptr < iio_buffer_end(buf); ptr += iio_buffer_step(buf))
    {
        uint16_t zneg = *(uint16_t *)(ptr + 5 * sizeof(uint16_t));
        sum_zneg = sum_zneg + zneg;
    }
    uint32_t avg_zneg = sum_zneg / samples_count;
    double chn5_accel = 2 * (double)avg_zneg / max_adc;
    printf("-Z AVG: %d -- ACCEL: %.3f [g]", avg_zneg, chn5_accel);

    return 0;
}
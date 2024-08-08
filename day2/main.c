#include <iio.h>
#include <stdio.h>
#include <unistd.h>

#define URI "ip:10.76.84.102"

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

    struct iio_channel* chan;
    chan = iio_device_find_channel(dev, "voltage0", false);

    if (!chan)
    {
        perror("cannot get channel!\n");
        exit(-3);
    }

    char* attr = iio_channel_find_attr(chan, "raw");

    if (!attr)
    {
        perror("cannot get attribute!\n");
        exit(-4);
    }

    for (int i = 0; i < 10; ++i)
    {
        char char_i[20];
        snprintf(char_i, sizeof(char_i), "%d", i + 1);

        iio_channel_attr_write(chan, "raw", char_i);
        sleep(1);

        char value[20];
        iio_channel_attr_read(chan, "raw", value, sizeof(value));
        printf("%s\n", value);
    }

    iio_context_destroy(ctx);
    return 0;
}
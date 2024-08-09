#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iio.h>

#define URI "ip:10.76.84.236"

int main()
{
    unsigned int major;
    unsigned int minor;
    char git_tag[8];
    iio_library_get_version(&major, &minor, git_tag);

    printf("version: %d, %d, %s\n", major, minor, git_tag);

    struct iio_context *ctx = iio_create_context_from_uri(URI);
    if (!ctx)
    {
        perror("cannot get ctx");
        return -1;
    }

    int nr = iio_context_get_attrs_count(ctx);
    printf("nr atribute: %d\n", nr);

    for (int i = 0; i < nr; i++)
    {
        const char *name;
        const char *value;
        int atr = iio_context_get_attr(ctx, i, &name, &value);
        printf("%s = %s\n", name, value);
    }

    char *dev_name = "iio:device0";
    struct iio_device *dev;
    dev = iio_context_find_device(ctx, dev_name);
    if (!dev)
    {
        perror("cannot get device");
        return -1;
    }

    char *chan_name = "voltage0";
    struct iio_channel *chn = iio_device_find_channel(dev, chan_name, false);
    if (!chn)
    {
        perror("cannot get channel");
        return -1;
    }

    for (int i = 0; i < 10; i++)
    {
        const char *name = "raw";
        char *value1;
        value1[0] = i + '0';
        iio_channel_attr_write(chn, name, value1);
        char value2[20];
        iio_channel_attr_read(chn, name, value2, sizeof(value2));
        printf("\t%s\n", value2);
        sleep(1);
    }

    return 0;
}
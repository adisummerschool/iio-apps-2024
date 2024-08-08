#define URI "ip:10.76.84.245"
#define DEVICE_NAME "ad5592r_s"
#define CHANNEL_NAME "voltage0"
#define CHANNEL_ATTRIBUTE "raw"
#define EN_ATTRIBUTE "en"

#include <iio.h>
#include <stdio.h>
#include <unistd.h>

int main()
{
    // unsigned int major, minor;
    // char git_tag[8];
    // iio_library_get_version(&major, &minor, git_tag);

    // printf("major: %d; minor: %d; git_tag: %s\n", major, minor, git_tag);

    struct iio_context *ctx = iio_create_context_from_uri(URI);

    if (!ctx)
    {
        printf("Context unavailable!!!\n");
        return -1;
    }
    printf("Connection established!\n");

    // unsigned int attrCount = iio_context_get_attrs_count(ctx);

    // for (int i = 0; i < attrCount; i++)
    // {
    //     const char *name, *value;
    //     iio_context_get_attr(ctx, i, &name, &value);
    //     printf("name: %s; value: %s\n", name, value);
    // }

    struct iio_device *device = iio_context_find_device(ctx, DEVICE_NAME);
    if (!device)
    {
        printf("No device with name \"%s\"", DEVICE_NAME);
        return -2;
    }

    char *enable = "1";
    iio_device_attr_write(device, EN_ATTRIBUTE, enable);

    struct iio_channel *channel = iio_device_find_channel(device, CHANNEL_NAME, false);
    if (!channel)
    {
        printf("No channel with name \"%s\"", CHANNEL_NAME);
        return -3;
    }

    for (int i = 1; i <= 10; i++)
    {
        char value[20];
        sprintf(value, "%d", i);
        iio_channel_attr_write(channel, CHANNEL_ATTRIBUTE, value);

        char dst[100];
        iio_channel_attr_read(channel, CHANNEL_ATTRIBUTE, dst, sizeof(dst));
        printf("%s\n", dst);

        sleep(1);
    }

    enable = "0";
    iio_device_attr_write(device, EN_ATTRIBUTE, enable);

    iio_context_destroy(ctx);

    return 0;
}
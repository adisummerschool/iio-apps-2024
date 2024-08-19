#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iio.h>

#define URI "ip:10.76.84.236"
#define THRESHOLD 20

int main()
{
    unsigned int major;
    unsigned int minor;
    char git_tag[8];
    iio_library_get_version(&major, &minor, git_tag);

    // printf("version: %d, %d, %s\n", major, minor, git_tag);

    struct iio_context *ctx = iio_create_context_from_uri(URI);
    if (!ctx)
    {
        perror("cannot get ctx");
        return -1;
    }

    int nr = iio_context_get_attrs_count(ctx);
    // printf("nr atribute: %d\n", nr);

    for (int i = 0; i < nr; i++)
    {
        const char *name;
        const char *value;
        int atr = iio_context_get_attr(ctx, i, &name, &value);
        //  printf("%s = %s\n", name, value);
    }

    char *dev_name = "iio:device0";
    struct iio_device *dev;
    dev = iio_context_find_device(ctx, dev_name);
    if (!dev)
    {
        perror("cannot get device");
        return -1;
    }

    while (true)
    {
        char *chan_0_name = "voltage0";
        char *chan_1_name = "voltage1";
        char *chan_2_name = "voltage2";
        char *chan_3_name = "voltage3";
        char *chan_4_name = "voltage4";
        char *chan_5_name = "voltage5";

        struct iio_channel *chan_0 = iio_device_find_channel(dev, chan_0_name, false);
        struct iio_channel *chan_1 = iio_device_find_channel(dev, chan_1_name, false);
        struct iio_channel *chan_2 = iio_device_find_channel(dev, chan_2_name, false);
        struct iio_channel *chan_3 = iio_device_find_channel(dev, chan_3_name, false);
        struct iio_channel *chan_4 = iio_device_find_channel(dev, chan_4_name, false);
        struct iio_channel *chan_5 = iio_device_find_channel(dev, chan_5_name, false);

        if (!chan_0 || !chan_1 || !chan_2 || !chan_3 || !chan_4 || !chan_5)
        {
            perror("cannot get channel");
            return -1;
        }

        const char *name = "raw";
        char chan_0_data[20];
        char chan_1_data[20];
        char chan_2_data[20];
        char chan_3_data[20];
        char chan_4_data[20];
        char chan_5_data[20];

        iio_channel_attr_read(chan_0, name, chan_0_data, sizeof(chan_0_data));
        iio_channel_attr_read(chan_1, name, chan_1_data, sizeof(chan_1_data));
        iio_channel_attr_read(chan_2, name, chan_2_data, sizeof(chan_2_data));
        iio_channel_attr_read(chan_3, name, chan_3_data, sizeof(chan_3_data));
        iio_channel_attr_read(chan_4, name, chan_4_data, sizeof(chan_4_data));
        iio_channel_attr_read(chan_5, name, chan_5_data, sizeof(chan_5_data));

        int chan_0_num = atoi(chan_0_data);
        int chan_1_num = atoi(chan_1_data);
        int chan_2_num = atoi(chan_2_data);
        int chan_3_num = atoi(chan_3_data);
        int chan_4_num = atoi(chan_4_data);
        int chan_5_num = atoi(chan_5_data);

        // printf("%d %d %d %d %d %d\n", chan_0_num, chan_1_num, chan_2_num, chan_3_num, chan_4_num, chan_5_num);
        printf("x: ");
        if (chan_0_num < THRESHOLD) // x+
        {
            if (chan_1_num < THRESHOLD) // x-
            {
                printf("OK\n");
            }
            else
            {
                printf("turn right\n");
            }
        }
        else
        {
            printf("turn left\n");
        }

        printf("y: ");
        if (chan_2_num < THRESHOLD)
        {
            if (chan_3_num < THRESHOLD)
            {
                printf("OK\n");
            }
            else
            {
                printf("turn right\n");
            }
        }
        else
        {
            printf("turn left\n");
        }

        printf("z: ");
        if (chan_4_num < THRESHOLD)
        {
            if (chan_5_num < THRESHOLD)
            {
                printf("OK\n");
            }
            else
            {
                printf("turn right\n");
            }
        }
        else
        {
            printf("turn left\n");
        }

        printf("%d %d %d %d %d %d\n", chan_0_num, chan_1_num, chan_2_num, chan_3_num, chan_4_num, chan_5_num);
        printf("------------------\n");
        sleep(1);
    }
    return 0;
}
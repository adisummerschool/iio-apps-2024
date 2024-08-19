#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iio.h>

#define URI "ip:10.76.84.236"
#define THRESHOLD 0.01

int main()
{
    struct iio_context *ctx = iio_create_context_from_uri(URI);
    if (!ctx)
    {
        perror("cannot get ctx");
        return -1;
    }

    char *dev_name = "iio:device0";
    struct iio_device *dev;
    dev = iio_context_find_device(ctx, dev_name);
    if (!dev)
    {
        perror("cannot get device");
        return -1;
    }

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
    int raw = chan_4_num;

    while (true)
    {
        printf("init: %4d %4d %4d %4d %4d %4d\n", chan_0_num, chan_1_num, chan_2_num, chan_3_num, chan_4_num, chan_5_num);

        struct iio_channel *chan_0_aux = iio_device_find_channel(dev, chan_0_name, false);
        struct iio_channel *chan_1_aux = iio_device_find_channel(dev, chan_1_name, false);
        struct iio_channel *chan_2_aux = iio_device_find_channel(dev, chan_2_name, false);
        struct iio_channel *chan_3_aux = iio_device_find_channel(dev, chan_3_name, false);
        struct iio_channel *chan_4_aux = iio_device_find_channel(dev, chan_4_name, false);
        struct iio_channel *chan_5_aux = iio_device_find_channel(dev, chan_5_name, false);

        if (!chan_0_aux || !chan_1_aux || !chan_2_aux || !chan_3_aux || !chan_4_aux || !chan_5_aux)
        {
            perror("cannot get channel");
            return -1;
        }

        const char *name = "raw";
        char chan_0_data_aux[20];
        char chan_1_data_aux[20];
        char chan_2_data_aux[20];
        char chan_3_data_aux[20];
        char chan_4_data_aux[20];
        char chan_5_data_aux[20];

        iio_channel_attr_read(chan_0_aux, name, chan_0_data_aux, sizeof(chan_0_data_aux));
        iio_channel_attr_read(chan_1_aux, name, chan_1_data_aux, sizeof(chan_1_data_aux));
        iio_channel_attr_read(chan_2_aux, name, chan_2_data_aux, sizeof(chan_2_data_aux));
        iio_channel_attr_read(chan_3_aux, name, chan_3_data_aux, sizeof(chan_3_data_aux));
        iio_channel_attr_read(chan_4_aux, name, chan_4_data_aux, sizeof(chan_4_data_aux));
        iio_channel_attr_read(chan_5_aux, name, chan_5_data_aux, sizeof(chan_5_data_aux));

        int chan_0_num_aux = atoi(chan_0_data_aux);
        int chan_1_num_aux = atoi(chan_1_data_aux);
        int chan_2_num_aux = atoi(chan_2_data_aux);
        int chan_3_num_aux = atoi(chan_3_data_aux);
        int chan_4_num_aux = atoi(chan_4_data_aux);
        int chan_5_num_aux = atoi(chan_5_data_aux);

        double chan_0_g = abs(chan_0_num_aux - chan_0_num) / (raw * 1.0);
        double chan_1_g = abs(chan_1_num_aux - chan_1_num) / (raw * 1.0);
        double chan_2_g = abs(chan_2_num_aux - chan_2_num) / (raw * 1.0);
        double chan_3_g = abs(chan_3_num_aux - chan_3_num) / (raw * 1.0);
        double chan_4_g = abs(chan_4_num_aux - chan_4_num) / (raw * 1.0);
        double chan_5_g = abs(chan_5_num_aux - chan_5_num) / (raw * 1.0);

        printf("loop: %4d %4d %4d %4d %4d %4d\n", chan_0_num_aux, chan_1_num_aux, chan_2_num_aux, chan_3_num_aux, chan_4_num_aux, chan_5_num_aux);
        printf("G: %.4f %.4f %.4f %.4f %.4f %.4f\n", chan_0_g, chan_1_g, chan_2_g, chan_3_g, chan_4_g, chan_5_g);
        printf("\tx: ");
        if (chan_0_g > THRESHOLD)
        {
            printf("detected shock: %.4fG\n", chan_0_g);
        }
        else if (chan_1_g > THRESHOLD)
        {
            printf("detected shock: %.4fG\n", chan_1_g);
        }
        else
        {
            printf("no shock\n");
        }

        printf("\ty: ");
        if (chan_2_g > THRESHOLD)
        {
            printf("detected shock: %.4fG\n", chan_2_g);
        }
        else if (chan_3_g > THRESHOLD)
        {
            printf("detected shock: %.4fG\n", chan_3_g);
        }
        else
        {
            printf("no shock\n");
        }

        printf("\tz: ");
        if (chan_4_g > THRESHOLD)
        {
            printf("detected shock: %.4fG\n", chan_4_g);
        }
        else if (chan_5_g > THRESHOLD)
        {
            printf("detected shock: %.4fG\n", chan_5_g);
        }
        else
        {
            printf("no shock\n");
        }

        printf("------------------------------------\n");
        sleep(1);
    }

    return 0;
}
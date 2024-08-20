#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <iio.h>

#define URI "ip:10.76.84.236"
#define CHANS_NB 6
#define MAX_ADC 4095
#define VOLT_PER_LSB 2.4 / MAX_ADC

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

    iio_device_attr_write(dev, "en", "1");

    char *chan_name[CHANS_NB] = {"voltage0", "voltage1", "voltage2", "voltage3", "voltage4", "voltage5"};
    struct iio_channel *chans[CHANS_NB];

    for (int i = 0; i < CHANS_NB; i++)
    {
        chans[i] = iio_device_find_channel(dev, chan_name[i], false);
        if (!chans[i])
        {
            perror("cannot get channel");
            return -1;
        }
    }

    const char *attr_name = "raw";
    while (true)
    {
        char chan_data[CHANS_NB][20];
        int chan_raw[CHANS_NB];
        double chan_voltage[CHANS_NB];
        double chan_g_values[CHANS_NB];

        for (int i = 0; i < CHANS_NB; i++)
        {
            iio_channel_attr_read(chans[i], attr_name, chan_data[i], sizeof(chan_data[i]));
            chan_raw[i] = atoi(chan_data[i]);
            chan_voltage[i] = chan_raw[i] * VOLT_PER_LSB;
            chan_g_values[i] = chan_raw[i] / (MAX_ADC * 1.0);
        }

        printf("     raw: ");
        for (int i = 0; i < CHANS_NB; i++)
        {
            printf("%8d ", chan_raw[i]);
        }

        printf("\n");
        printf("raw to V: ");
        for (int i = 0; i < CHANS_NB; i++)
        {
            printf("%7.4fV ", chan_voltage[i]);
        }

        printf("\n");
        printf("   accel:\n");
        for (int i = 0; i < CHANS_NB; i++)
        {
            printf("           %fg\n", chan_g_values[i]);
        }
        printf("\n----------------------------------------------------------------\n");
        sleep(1);
    }
    iio_context_destroy(ctx);

    return 0;
}

#define URI "ip:10.76.84.245"
#define DEVICE_NAME "ad5592r_s"
#define CHANNEL_ATTRIBUTE "raw"
#define EN_ATTRIBUTE "en"
#define AXIS "+x\t-x\t+y\t-y\t+z\t-z\n"
#define THRESHOLD 50

#include <iio.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

bool keepRunning = true;

void handler(int dummy)
{
    keepRunning = false;
}

int main()
{
    signal(SIGINT, handler);
    system("clear");

    struct iio_context *ctx = iio_create_context_from_uri(URI);

    if (!ctx)
    {
        printf("Context unavailable!!!\n");
        return -1;
    }
    printf("Connection established!\n");

    struct iio_device *device = iio_context_find_device(ctx, DEVICE_NAME);
    if (!device)
    {
        printf("No device with name \"%s\"", DEVICE_NAME);
        return -2;
    }

    char *enable = "1";
    iio_device_attr_write(device, EN_ATTRIBUTE, enable);

    while (keepRunning)
    {
        printf("%s", AXIS);

        unsigned int chanNumbers = iio_device_get_channels_count(device);
        for (unsigned int i = 0; i < chanNumbers; i++)
        {
            struct iio_channel *channel = iio_device_get_channel(device, i);

            char channelData[100];
            iio_channel_attr_read(channel, CHANNEL_ATTRIBUTE, channelData, sizeof(channelData));
            printf("%s\t", channelData);
        }
        printf("\n");

        sleep(2);
        printf("\033[A");
        printf("\033[K");
        printf("\033[A");
        printf("\033[K");

        // struct iio_channel *channel1 = iio_device_get_channel(device, 0);
        // struct iio_channel *channel2 = iio_device_get_channel(device, 1);
        // struct iio_channel *channel3 = iio_device_get_channel(device, 2);
        // struct iio_channel *channel4 = iio_device_get_channel(device, 3);
        // struct iio_channel *channel5 = iio_device_get_channel(device, 4);
        // struct iio_channel *channel6 = iio_device_get_channel(device, 5);

        // char channel1Data[100];
        // iio_channel_attr_read(channel1, CHANNEL_ATTRIBUTE, channel1Data, sizeof(channel1Data));
        // char channel2Data[100];
        // iio_channel_attr_read(channel2, CHANNEL_ATTRIBUTE, channel2Data, sizeof(channel2Data));
        // char channel3Data[100];
        // iio_channel_attr_read(channel3, CHANNEL_ATTRIBUTE, channel3Data, sizeof(channel3Data));
        // char channel4Data[100];
        // iio_channel_attr_read(channel4, CHANNEL_ATTRIBUTE, channel4Data, sizeof(channel4Data));
        // char channel5Data[100];
        // iio_channel_attr_read(channel5, CHANNEL_ATTRIBUTE, channel5Data, sizeof(channel5Data));
        // char channel6Data[100];
        // iio_channel_attr_read(channel6, CHANNEL_ATTRIBUTE, channel6Data, sizeof(channel6Data));

        // if (atoi(channel1Data) > THRESHOLD)
        // {
        //     printf("x: turn left\n");
        // }
        // else
        // {
        //     if (atoi(channel2Data) > THRESHOLD)
        //     {
        //         printf("x: turn right\n");
        //     }
        //     else
        //     {
        //         printf("x: OK\n");
        //     }
        // }
        // if (atoi(channel3Data) > THRESHOLD)
        // {
        //     printf("y: turn left\n");
        // }
        // else
        // {
        //     if (atoi(channel4Data) > THRESHOLD)
        //     {
        //         printf("y: turn right\n");
        //     }
        //     else
        //     {
        //         printf("y: OK\n");
        //     }
        // }
        // if (atoi(channel5Data) > THRESHOLD)
        // {
        //     printf("z: turn left\n");
        // }
        // else
        // {
        //     if (atoi(channel6Data) > THRESHOLD)
        //     {
        //         printf("z: turn right\n");
        //     }
        //     else
        //     {
        //         printf("z: OK\n");
        //     }
        // }
        // sleep(2);

        // printf("\033[A");
        // printf("\033[K");
        // printf("\033[A");
        // printf("\033[K");
        // printf("\033[A");
        // printf("\033[K");
    }

    enable = "0";
    iio_device_attr_write(device, EN_ATTRIBUTE, enable);

    iio_context_destroy(ctx);
    system("clear");

    printf("Done! Connection closed!\n");

    return 0;
}
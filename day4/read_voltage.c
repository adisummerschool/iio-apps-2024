#define URI "ip:10.76.84.245"
#define DEVICE_NAME "ad5592r_s"
#define CHANNEL_ATTRIBUTE "raw"
#define EN_ATTRIBUTE "en"
#define VOLTAGE "voltage"
#define CHANNELS_NUMBER 6
#define NUMBERS_OF_LINES_TO_REMOVE 12

#include <iio.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

const int MAX_ADC = 4095;
const double VOLTS_PER_LSB = 2.4 / MAX_ADC;

bool keepRunning = true;

void handler(int dummy)
{
    keepRunning = false;
}

char *voltageName(int x)
{
    switch (x)
    {
    case 0:
        return "voltage0";
    case 1:
        return "voltage1";
    case 2:
        return "voltage2";
    case 3:
        return "voltage3";
    case 4:
        return "voltage4";
    default:
        return "voltage5";
    }
}

void removeLines(int n) {
    for(int i = 0; i < n; i++) {
        printf("\033[A");
        printf("\033[K");
    }
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

    struct iio_channel *channels[CHANNELS_NUMBER];
    for (unsigned int i = 0; i < CHANNELS_NUMBER; i++)
    {
        channels[i] = iio_device_find_channel(device, voltageName(i), false);
    }

    if (!channels[0] || !channels[1] || !channels[2] || !channels[3] || !channels[4] || !channels[5])
    {
        printf("Channel not found");
        return -3;
    }

    while (keepRunning)
    {
        for (unsigned int i = 0; i < CHANNELS_NUMBER; i++)
        {
            char channelData[100];
            iio_channel_attr_read(channels[i], CHANNEL_ATTRIBUTE, channelData, sizeof(channelData));

            int channelRawVal = atoi(channelData);
            double channelVoltageVal = (double)channelRawVal * VOLTS_PER_LSB;
            double channelGVal = (double)channelRawVal / MAX_ADC * 2;

            printf("Voltage on channel %d is: %.2f [V]\nG value on channel %d is: %.2f\n", i, channelVoltageVal, i, channelGVal);
        }

        sleep(1);
        removeLines(NUMBERS_OF_LINES_TO_REMOVE);
    }

    enable = "0";
    iio_device_attr_write(device, EN_ATTRIBUTE, enable);

    iio_context_destroy(ctx);
    system("clear");

    printf("Done! Connection closed!\n");

    return 0;
}
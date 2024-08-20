#define URI "ip:10.76.84.245"
#define DEVICE_NAME "ad5592r_s"
#define CHANNEL_ATTRIBUTE "raw"
#define EN_ATTRIBUTE "en"
#define CHANNELS_NUMBER 6
#define SAMPLE_COUNT 1024
#define CYCLIC_STATUS false
#define AXIS "+x\t\t-x\t\t+y\t\t-y\t\t+z\t\t-z\n"

#include <iio.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

const int MAX_ADC = 4095;
const double VOLTS_PER_LSB = 2.4 / MAX_ADC;

bool keepRunning = true;

typedef struct
{
    uint16_t xpoz;
    uint16_t xneg;
    uint16_t ypoz;
    uint16_t yneg;
    uint16_t zpoz;
    uint16_t zneg;
} bufferElement;

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

void removeLines(int n)
{
    for (int i = 0; i < n; i++)
    {
        printf("\033[A");
        printf("\033[K");
    }
}

double axisAvg(uint32_t value)
{
    return ((float)value / SAMPLE_COUNT);
}

int main()
{
    signal(SIGINT, handler);
    system("clear");

    bool firstRead = true;

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
        if (!channels[i])
        {
            printf("Channel not found");
            return -3;
        }
        iio_channel_enable(channels[i]);
    }

    struct iio_buffer *buffer = iio_device_create_buffer(device, SAMPLE_COUNT, CYCLIC_STATUS);

    while (keepRunning)
    {
        int readBytes = iio_buffer_refill(buffer);
        uint32_t axisSum[CHANNELS_NUMBER] = {0};
        double averages[CHANNELS_NUMBER] = {0.0};

        for (int i = 0; i < CHANNELS_NUMBER; i++)
        {
            for (void *ptr = iio_buffer_start(buffer); ptr < iio_buffer_end(buffer); ptr += iio_buffer_step(buffer))
            {
                axisSum[i] += *(uint16_t *)(ptr + i * sizeof(uint16_t));
            }
        }

        printf("%s", AXIS);
        for (int i = 0; i < CHANNELS_NUMBER; i++)
        {
            averages[i] = axisAvg(axisSum[i]);
            printf("%.2f\t\t", averages[i]);
        }
        printf("\n");

        removeLines(2);
    }

    enable = "0";
    iio_device_attr_write(device, EN_ATTRIBUTE, enable);

    iio_context_destroy(ctx);
    system("clear");

    printf("Done! Connection closed!\n");

    return 0;
}
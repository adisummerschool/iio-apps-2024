#define URI "ip:10.76.84.245"
#define DEVICE_NAME "ad5592r_s"
#define CHANNEL_ATTRIBUTE "raw"
#define EN_ATTRIBUTE "en"
#define THRESHOLD 400
#define G1 (float)2000
#define SHOCKS_SIZE 3
#define LAST_ELEM_IN_HISTORY 9
#define HISTORY_MAX_LENGHT 10

#include <iio.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

bool keepRunning = true;

void handler(int dummy)
{
    keepRunning = false;
}

bool isShock(unsigned int previous, unsigned int current)
{
    if (abs(current - previous) > THRESHOLD)
    {
        return true;
    }

    return false;
}

int main()
{
    signal(SIGINT, handler);
    system("clear");

    unsigned int averages[6] = {0};
    char axis[3][5] = {"x: ", "y: ", "z: "};
    bool firstRead = true;
    unsigned int historiesLenght[6] = {0};
    unsigned int shocksHistory[6][10] = {0};

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
        unsigned int channelNumbers = iio_device_get_channels_count(device);
        char channelsData[6][100];
        for (unsigned int i = 0; i < channelNumbers; i++)
        {
            struct iio_channel *channel = iio_device_get_channel(device, i);
            iio_channel_attr_read(channel, CHANNEL_ATTRIBUTE, channelsData[i], sizeof(channelsData[i]));
        }

        if (!firstRead)
        {
            bool shocks[3] = {false};
            unsigned int newAverages[6] = {0};
            for (unsigned int i = 0; i < channelNumbers; i++)
            {
                unsigned int currentValue = atoi(channelsData[i]);
                if (isShock(averages[i], currentValue))
                {
                    if (historiesLenght[i] >= HISTORY_MAX_LENGHT)
                    {
                        bool noShockInHistory = true;
                        for (unsigned int j = 0; j < LAST_ELEM_IN_HISTORY && noShockInHistory; j++)
                        {
                            if (isShock(shocksHistory[i][j], shocksHistory[i][j + 1]))
                            {
                                noShockInHistory = false;
                            }
                        }
                        historiesLenght[i] = 0;

                        if(noShockInHistory && !isShock(shocksHistory[i][LAST_ELEM_IN_HISTORY], currentValue)) {
                            newAverages[i] = currentValue;
                            shocks[i / 2] = false;
                        }
                        else {
                            shocks[i / 2] = true;
                        }
                    }
                    else
                    {
                        shocks[i / 2] = true;
                        shocksHistory[i][historiesLenght[i]] = currentValue;
                        historiesLenght[i]++;
                    }
                }
                else
                {
                    newAverages[i] = currentValue;
                    historiesLenght[i] = 0;
                }
            }

            for (int i = 0; i < SHOCKS_SIZE; i++)
            {
                printf("%s", axis[i]);
                float shockImpact = abs(averages[i] - atoi(channelsData[i])) / G1;
                if (shocks[i])
                {
                    printf("detected shock of %.2fG\n", shockImpact);
                }
                else
                {
                    printf("no shock\n");
                    averages[(i * 2)] = newAverages[(i * 2)];
                    averages[(i * 2) + 1] = newAverages[(i * 2) + 1];
                }
            }
            sleep(0.5);

            printf("\033[A");
            printf("\033[K");
            printf("\033[A");
            printf("\033[K");
            printf("\033[A");
            printf("\033[K");
        }
        else
        {
            for (unsigned int i = 0; i < channelNumbers; i++)
            {
                averages[i] = atoi(channelsData[i]);
            }
            firstRead = false;
        }
    }

    enable = "0";
    iio_device_attr_write(device, EN_ATTRIBUTE, enable);

    iio_context_destroy(ctx);
    system("clear");

    printf("Done! Connection closed!\n");

    return 0;
}
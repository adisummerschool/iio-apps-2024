#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <iio.h>

#include <termios.h>
#include <fcntl.h>

#define URI "ip:10.76.84.236"
#define THRESHOLD 10
#define CHANS_NB 6

int kbhit(void) // https://stackoverflow.com/questions/32390617/get-keyboard-interrupt-in-c
{
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF)
    {
        ungetc(ch, stdin);
        return 1;
    }

    return 0;
}

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
    char chan_data[CHANS_NB][20];
    int chan_num[CHANS_NB];

    for (int i = 0; i < CHANS_NB; i++)
    {
        iio_channel_attr_read(chans[i], attr_name, chan_data[i], sizeof(chan_data[i]));
        chan_num[i] = atoi(chan_data[i]);
    }

    int raw = chan_num[4];

    while (!kbhit())
    {
        struct iio_channel *new_chans[CHANS_NB];
        for (int i = 0; i < CHANS_NB; i++)
        {
            new_chans[i] = iio_device_find_channel(dev, chan_name[i], false);
            if (!new_chans[i])
            {
                perror("cannot get channel");
                return -1;
            }
        }

        char new_chan_data[CHANS_NB][20];
        int new_chan_num[CHANS_NB];

        for (int i = 0; i < CHANS_NB; i++)
        {
            iio_channel_attr_read(new_chans[i], attr_name, new_chan_data[i], sizeof(new_chan_data[i]));
            new_chan_num[i] = atoi(new_chan_data[i]);
        }

        double chan_g_values[CHANS_NB];

        for (int i = 0; i < CHANS_NB; i++)
        {
            chan_g_values[i] = abs(new_chan_num[i] - chan_num[i]) / (raw * 1.0);
        }

        printf("valori initiale:\n\t");
        for (int i = 0; i < CHANS_NB; i++)
        {
            printf("%7d ", chan_num[i]);
        }
        printf("\n");

        printf("valori noi:\n\t");
        for (int i = 0; i < CHANS_NB; i++)
        {
            printf("%7d ", new_chan_num[i]);
        }
        printf("\n");

        printf("valori G:\n\t");
        for (int i = 0; i < CHANS_NB; i++)
        {
            printf("%7.4f ", chan_g_values[i]);
        }
        printf("\n");

        for (int i = 0; i < CHANS_NB; i++)
        {
            if (abs(new_chan_num[i] - chan_num[i]) >= THRESHOLD)
            {
                printf("\n\n");
                if (i == 0 || i == 1)
                    printf("x: ");
                else if (i == 2 || i == 3)
                    printf("y: ");
                else
                    printf("z: ");
                printf("shock\n\n");
            }
        }

        printf("------------------------------------\n");
        sleep(1);
    }

    iio_context_destroy(ctx);

    return 0;
}
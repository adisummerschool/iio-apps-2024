#include <stdio.h>
#include <stdlib.h>
#include <iio.h>
#include <string.h>
#include <unistd.h>

#define URI "ip:10.76.84.239"
#define MAX_ADC 4095
#define VOLTS_PER_LSB (2.4 / MAX_ADC)
#define NUM_CHANNELS 6

int main() {
    struct iio_context *ctx = iio_create_context_from_uri(URI);
    if (!ctx) {
        perror("Creating context from URI failed");
        return -1;
    }

    struct iio_device *dev = iio_context_find_device(ctx, "iio:device0");
    if (!dev) {
        perror("Device not found");
        iio_context_destroy(ctx);
        return -1;
    }

    if (iio_device_attr_write(dev, "en", "1") < 0) {
        perror("Failed to enable device");
        iio_context_destroy(ctx);
        return -1;
    }

    struct iio_channel *channels[NUM_CHANNELS];
    const char *channel_names[] = {"voltage0", "voltage1", "voltage2", "voltage3", "voltage4", "voltage5"};

    // Initialize channels
    for (int i = 0; i < NUM_CHANNELS; i++) {
        channels[i] = iio_device_find_channel(dev, channel_names[i], false);
        if (!channels[i]) {
            fprintf(stderr, "Channel %s not found\n", channel_names[i]);
            iio_context_destroy(ctx);
            return -1;
        }
    }

    char chn_str[NUM_CHANNELS][100];
    int chn_raw[NUM_CHANNELS];
    double chn_volts[NUM_CHANNELS];
    double g_chn[NUM_CHANNELS];

    while (1) {
        for (int i = 0; i < NUM_CHANNELS; i++) {
            if (iio_channel_attr_read(channels[i], "raw", chn_str[i], sizeof(chn_str[i])) >= 0) {
                chn_raw[i] = atoi(chn_str[i]);
                chn_volts[i] = chn_raw[i] * VOLTS_PER_LSB;
                g_chn[i] = (double)chn_raw[i] / MAX_ADC;

                printf("Voltage on channel %d: %f V\n", i, chn_volts[i]);
                printf("g-force on channel %d: %f g\n\n", i, g_chn[i]);
            } else {
                fprintf(stderr, "Failed to read channel %s\n", channel_names[i]);
            }
        }
            \
        printf("---------------------------------\n");
        sleep(2);
    }

    iio_context_destroy(ctx);
    return 0;
}


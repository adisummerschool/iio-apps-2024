
#include <stdio.h>
#include <stdlib.h>
#include <iio.h>
#include <string.h>
#include <unistd.h>

#define URI "ip:10.76.84.239"

const int MAX_ADC = 4095;
const double VOLTS_PER_LSB = 2.4 / MAX_ADC;

int main(){

    struct iio_context *ctx = iio_create_context_from_uri(URI);
    if (!ctx) {
        perror("Creating context from URI failed");
        return -1;
    }

    struct iio_device *dev = iio_context_find_device(ctx, "iio:device0");
    if (!dev) {
        perror("Device not found");
        return -1;
    }

    iio_device_attr_write(dev, "en", "1");
    if (iio_device_attr_write(dev, "en", "1") < 0) {
        perror("Failed to enable device");
        return -1;
    }

    struct iio_channel *ch0 = iio_device_find_channel(dev, "voltage0", false);
    struct iio_channel *ch1 = iio_device_find_channel(dev, "voltage1", false);
    struct iio_channel *ch2 = iio_device_find_channel(dev, "voltage2", false);
    struct iio_channel *ch3 = iio_device_find_channel(dev, "voltage3", false);
    struct iio_channel *ch4 = iio_device_find_channel(dev, "voltage4", false);
    struct iio_channel *ch5 = iio_device_find_channel(dev, "voltage5", false);

    if (!ch0 || !ch1 || !ch2 || !ch3 || !ch4 || !ch5) {
    perror("Channel not found");
    return -1;
    }

    char chn0_str[100];
    char chn1_str[100];
    char chn2_str[100];
    char chn3_str[100];
    char chn4_str[100];
    char chn5_str[100];

    int chn0_raw;
    int chn1_raw;
    int chn2_raw;
    int chn3_raw;
    int chn4_raw;
    int chn5_raw;

    while (true){

        iio_channel_attr_read(ch0, "raw", chn0_str, sizeof(chn0_str));
        chn0_raw = atoi(chn0_str);

        iio_channel_attr_read(ch1, "raw", chn1_str, sizeof(chn1_str));
        chn1_raw = atoi(chn1_str);

        iio_channel_attr_read(ch2, "raw", chn2_str, sizeof(chn2_str));
        chn2_raw = atoi(chn2_str);

        iio_channel_attr_read(ch3, "raw", chn3_str, sizeof(chn3_str));
        chn3_raw = atoi(chn3_str);

        iio_channel_attr_read(ch4, "raw", chn4_str, sizeof(chn4_str));
        chn4_raw = atoi(chn4_str);

        iio_channel_attr_read(ch5, "raw", chn5_str, sizeof(chn5_str));
        chn5_raw = atoi(chn5_str);

        double chn0_volts = chn0_raw * VOLTS_PER_LSB;
        double chn1_volts = chn1_raw * VOLTS_PER_LSB;
        double chn2_volts = chn2_raw * VOLTS_PER_LSB;
        double chn3_volts = chn3_raw * VOLTS_PER_LSB;
        double chn4_volts = chn4_raw * VOLTS_PER_LSB;
        double chn5_volts = chn5_raw * VOLTS_PER_LSB;

        printf("Voltage on channel 0: %f V\n", chn0_volts);
        printf("Voltage on channel 1: %f V\n", chn1_volts);
        printf("Voltage on channel 2: %f V\n", chn2_volts);
        printf("Voltage on channel 3: %f V\n", chn3_volts);
        printf("Voltage on channel 4: %f V\n", chn4_volts);
        printf("Voltage on channel 5: %f V\n\n", chn5_volts);

        double g_chn0 = (double) chn0_raw/MAX_ADC;
        double g_chn1 = (double) chn1_raw/MAX_ADC;
        double g_chn2 = (double) chn2_raw/MAX_ADC;
        double g_chn3 = (double) chn3_raw/MAX_ADC;
        double g_chn4 = (double) chn4_raw/MAX_ADC;
        double g_chn5 = (double) chn5_raw/MAX_ADC;

        printf("g-force on channel 0: %f g\n", g_chn0);
        printf("g-force on channel 1: %f g\n", g_chn1);
        printf("g-force on channel 2: %f g\n", g_chn2);
        printf("g-force on channel 3: %f g\n", g_chn3);
        printf("g-force on channel 4: %f g\n", g_chn4);
        printf("g-force on channel 5: %f g\n", g_chn5);


        printf("---------------------------------\n");

        sleep(2);  
    }

    iio_context_destroy(ctx);


    return 0;
}


/*
#include <stdio.h>
#include <stdlib.h>
#include <iio.h>
#include <string.h>
#include <unistd.h>

#define URI "ip:10.76.84.239"

const int MAX_ADC = 4095;
const double VOLTS_PER_LSB = 2.4 / MAX_ADC;
const int NUM_CHANNELS = 6;  // Number of channels to read

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
    char *channel_names[] = {"voltage0", "voltage1", "voltage2", "voltage3", "voltage4", "voltage5"};
    char chn_str[NUM_CHANNELS][100];
    int chn_raw[NUM_CHANNELS];
    double chn_volts[NUM_CHANNELS];

    while (true){
        // Initialize channels
        for (int i = 0; i < NUM_CHANNELS; i++) {
            channels[i] = iio_device_find_channel(dev, channel_names[i], false);
            if (!channels[i]) {
                fprintf(stderr, "Channel %s not found\n", channel_names[i]);
                iio_context_destroy(ctx);
                return -1;
            }
        }

        // Read channel values, convert to volts, and print
        for (int i = 0; i < NUM_CHANNELS; i++) {
            if (iio_channel_attr_read(channels[i], "raw", chn_str[i], sizeof(chn_str[i])) >= 0) {
                chn_raw[i] = atoi(chn_str[i]);
                chn_volts[i] = chn_raw[i] * VOLTS_PER_LSB;
                printf("Voltage on channel %d: %f V\n", i, chn_volts[i]);
            } else {
                fprintf(stderr, "Failed to read channel %s\n", channel_names[i]);
            }
        }

        printf("---------------------------------\n");

        sleep(2);  // Sleep for 1 second before the next reading
    }

    iio_context_destroy(ctx);
    return 0;
}
*/
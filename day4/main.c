#include <stdio.h>
#include <stdlib.h>
#include <iio.h>
#include <string.h>
#include <unistd.h>

#define URI "ip:10.76.84.103"

const int MAX_ADC = 4095;
const double VOLTS_PER_LSB = 2.4 / MAX_ADC;

int main() {
    struct iio_context *ctx = iio_create_context_from_uri(URI);
    if (!ctx) {
        printf("\n No context found\n");
        return -1;
    }

    struct iio_device *dev = iio_context_find_device(ctx, "iio:device0");
    if (!dev) {
        printf("\n No device found\n");
        return -1;
    }

    // Enable the device
    iio_device_attr_write(dev, "en", "1");

    // Find channels voltage0 to voltage5
    struct iio_channel *channels[6];
    channels[0] = iio_device_find_channel(dev, "voltage0", false);
    channels[1] = iio_device_find_channel(dev, "voltage1", false);
    channels[2] = iio_device_find_channel(dev, "voltage2", false);
    channels[3] = iio_device_find_channel(dev, "voltage3", false);
    channels[4] = iio_device_find_channel(dev, "voltage4", false);
    channels[5] = iio_device_find_channel(dev, "voltage5", false);

    // Ensure all channels are found
    for (int i = 0; i < 6; i++) {
        if (!channels[i]) {
            fprintf(stderr, "Failed to find voltage%d\n", i);
            return -1;
        }
    }

    // Main loop to read and calculate voltage
   while (1) {
        for (int i = 0; i < 6; i++) {
            char chn_str[100];

            // Read raw data from the channel
            if (iio_channel_attr_read(channels[i], "raw", chn_str, sizeof(chn_str)) < 0) {
                fprintf(stderr, "Failed to read from channel voltage%d\n", i);
                continue;
            }

            // Convert raw data to integer
            int chn_raw = atoi(chn_str);

            // Calculate the voltage from the raw value
            double chn_voltage = chn_raw * VOLTS_PER_LSB;

            // Calculate the acceleration (normalized value between 0 and 1)
            double chn_acceleration = ((double)chn_raw / MAX_ADC) * 2;

            // Print the voltage and acceleration for the current channel
            printf("CHANNEL voltage%d: VOLTAGE = %f [V], ACCELERATION = %f [g]\n", i, chn_voltage, chn_acceleration);
        }

        printf ("\n");
        // Sleep for a second to prevent the loop from running too fast (optional)
        sleep(2);
    }

    if (iio_channel_is_enabled(channels[0]) == 0) {
    printf("Channel 0 is disabled.\n");
} else {
    printf("Channel 0 is enabled.\n");
}


    // Cleanup
    iio_context_destroy(ctx);

    return 0;
}

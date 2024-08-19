#include <stdio.h>
#include <stdlib.h>
#include <iio.h>
#include <unistd.h>  // For sleep


#define URI "ip:10.76.84.239"
#define DEVICE_NAME "iio:device0"
#define CHANNEL_NAME0 "voltage0"
#define CHANNEL_NAME1 "voltage1"
#define CHANNEL_NAME2 "voltage2"
#define CHANNEL_NAME3 "voltage3"
#define CHANNEL_NAME4 "voltage4"
#define CHANNEL_NAME5 "voltage5"

#define THRESHOLD 50

void read_channels(struct iio_context *ctx) {
    struct iio_device *device = iio_context_find_device(ctx, DEVICE_NAME);

    struct iio_channel *channel0 = iio_device_find_channel(device, CHANNEL_NAME0, false);
    struct iio_channel *channel1 = iio_device_find_channel(device, CHANNEL_NAME1, false);
    struct iio_channel *channel2 = iio_device_find_channel(device, CHANNEL_NAME2, false);
    struct iio_channel *channel3 = iio_device_find_channel(device, CHANNEL_NAME3, false);
    struct iio_channel *channel4 = iio_device_find_channel(device, CHANNEL_NAME4, false);
    struct iio_channel *channel5 = iio_device_find_channel(device, CHANNEL_NAME5, false);

    int initial_raw0, initial_raw1, initial_raw2, initial_raw3, initial_raw4, initial_raw5;
    int raw0, raw1, raw2, raw3, raw4, raw5;

    char buf[256];

    // Take initial readings
    if (iio_channel_attr_read(channel0, "raw", buf, sizeof(buf)) >= 0) initial_raw0 = atoi(buf);
    printf("initial_raw0: %d\n", initial_raw0);
    if (iio_channel_attr_read(channel1, "raw", buf, sizeof(buf)) >= 0) initial_raw1 = atoi(buf);
    printf("initial_raw1: %d\n", initial_raw1);
    if (iio_channel_attr_read(channel2, "raw", buf, sizeof(buf)) >= 0) initial_raw2 = atoi(buf);
    printf("initial_raw2: %d\n", initial_raw2);
    if (iio_channel_attr_read(channel3, "raw", buf, sizeof(buf)) >= 0) initial_raw3 = atoi(buf);
    printf("initial_raw3: %d\n", initial_raw3);
    if (iio_channel_attr_read(channel4, "raw", buf, sizeof(buf)) >= 0) initial_raw4 = atoi(buf);
    printf("initial_raw4: %d\n", initial_raw4);
    if (iio_channel_attr_read(channel5, "raw", buf, sizeof(buf)) >= 0) initial_raw5 = atoi(buf);
    printf("initial_raw5: %d\n", initial_raw5);
    
    printf("-------------------\n");

    while (1) {
        // Read current values for each channel
        if (iio_channel_attr_read(channel0, "raw", buf, sizeof(buf)) >= 0) raw0 = atoi(buf);
        if (iio_channel_attr_read(channel1, "raw", buf, sizeof(buf)) >= 0) raw1 = atoi(buf);
        if (iio_channel_attr_read(channel2, "raw", buf, sizeof(buf)) >= 0) raw2 = atoi(buf);
        if (iio_channel_attr_read(channel3, "raw", buf, sizeof(buf)) >= 0) raw3 = atoi(buf);
        if (iio_channel_attr_read(channel4, "raw", buf, sizeof(buf)) >= 0) raw4 = atoi(buf);
        if (iio_channel_attr_read(channel5, "raw", buf, sizeof(buf)) >= 0) raw5 = atoi(buf);

        // Calculate differences and convert to g
        double diff_x0 = (double) (raw0 - initial_raw0);
        double avg_x0 = diff_x0 / initial_raw4;
        double diff_y0 = (double) (raw2 - initial_raw2);
        double avg_y0 = diff_y0 / initial_raw4;
        double diff_z0 = (double) (raw4 - initial_raw4);
        double avg_z0 = diff_z0 / initial_raw4;
        double diff_x1 = (double) (raw1 - initial_raw1);
        double avg_x1 = diff_x1 / initial_raw4;
        double diff_y1 = (double) (raw3 - initial_raw3);
        double avg_y1 = diff_y1 / initial_raw4;
        double diff_z1 = (double) (raw5 - initial_raw5);
        double avg_z1 = diff_z1 / initial_raw4;


    

        
        // Check if the difference exceeds the threshold
        if (diff_x0 > 50) {
            printf("Shock on X+ axis: %.2f g\n", avg_x0);
        }
        if (diff_y0 > 50) {
            printf("Shock on Y+ axis: %.2f g\n", avg_y0);
        }
        if (diff_z0 > 50) {
            printf("Shock on Z+ axis: %.2f g\n", avg_z0);
        }

        if (diff_x1 > 50) {
            printf("Shock on X- axis: %.2f g\n", avg_x1);
        }
        if (diff_y1 > 50) {
            printf("Shock on Y- axis: %.2f g\n", avg_y1);
        }
        if (diff_z1 > 50) {
            printf("Shock on Z- axis: %.2f g\n", avg_z1);
        }


        sleep(0.2); // Sleep for 1 second before the next reading
    }
}

int main() {
    struct iio_context *ctx = iio_create_context_from_uri(URI);
    if (!ctx) {
        perror("Creating context from URI failed");
        return -1;
    }

    read_channels(ctx);

    iio_context_destroy(ctx);

    return 0;
}

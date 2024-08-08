#include <stdio.h>
#include <stdlib.h>
#include <iio.h>
#include <unistd.h>  // For sleep

#define URI "ip:10.76.84.239"
#define DEVICE_NAME "iio:device1"
#define CHANNEL_NAME "temp0"

void read_and_calculate_temperature(struct iio_context *ctx) {
    struct iio_device *device = iio_context_find_device(ctx, DEVICE_NAME);

    struct iio_channel *channel = iio_device_find_channel(device, CHANNEL_NAME, false);

    for (int i = 1; i <= 10; i++) {
        char buf[256];
        int raw_temp;
        int offset_temp;
        double scale_temp;
        double temp;

        if (iio_channel_attr_read(channel, "raw", buf, sizeof(buf)) < 0) {
            perror("Reading raw attribute failed");
            return;
        }
        raw_temp = atoi(buf);
        printf("RAW_TEMP: %d\n", raw_temp);

        if (iio_channel_attr_read(channel, "offset", buf, sizeof(buf)) < 0) {
            perror("Reading offset attribute failed");
            return;
        }
        offset_temp = atoi(buf);
        printf("OFFSET_TEMP: %d\n", offset_temp);

        if (iio_channel_attr_read(channel, "scale", buf, sizeof(buf)) < 0) {
            perror("Reading scale attribute failed");
            return;
        }
        scale_temp = atof(buf);
        printf("SCALE_TEMP: %.6f\n", scale_temp);


        temp = (raw_temp + offset_temp) * scale_temp / 1000.0;
        printf("Temperature: %.2f °C\n\n", temp);

        sleep(1);
    }
}

int main() {
    struct iio_context *ctx = iio_create_context_from_uri(URI);
    if (!ctx) {
        perror("Creating context from URI failed");
        return -1;
    }

    read_and_calculate_temperature(ctx);

    iio_context_destroy(ctx);
    return 0;
}

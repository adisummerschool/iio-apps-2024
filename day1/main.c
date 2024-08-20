#include <stdio.h>
#include <iio.h>
#include <unistd.h>
#define URI "ip:10.76.84.103"
#define DEVICE_NAME "iio:device0"
#define CHANNEL_NAME "voltage0"
#define ATTRIBUTE_NAME "raw"
int main() {
    struct iio_context *ctx = iio_create_context_from_uri(URI);
    if (!ctx) {
        perror("Creating context from URI failed");
        return -1;
    }
    struct iio_device *device = iio_context_find_device(ctx, DEVICE_NAME);
    if (!device) {
        fprintf(stderr, "Device %s not found\n", DEVICE_NAME);
        iio_context_destroy(ctx);
        return -1;
    }
    struct iio_channel *channel = iio_device_find_channel(device, CHANNEL_NAME, false);
    if (!channel) {
        fprintf(stderr, "Channel %s not found\n", CHANNEL_NAME);
        iio_context_destroy(ctx);
        return -1;
    }
    char buf[16];
    for (int value = 1; value <= 10; value++) {
        snprintf(buf, sizeof(buf), "%d", value);
        iio_channel_attr_write(channel, ATTRIBUTE_NAME, buf);
        printf("Set %s to %d\n", ATTRIBUTE_NAME, value);
        iio_channel_attr_read(channel, ATTRIBUTE_NAME, buf, sizeof(buf));
        printf("Read %s: %s\n", ATTRIBUTE_NAME, buf);
        sleep(1);
    }
    return 0;
}
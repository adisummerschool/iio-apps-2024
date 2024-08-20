#include <stdio.h>
#include <stdlib.h>
#include <iio.h>
#include <string.h>
#include <unistd.h>

#define URI "ip:10.76.84.239"
#define CYCLIC false
#define THRESHOLD 1000
const int sample_buffer_size = 16;

int main() {
    struct iio_context *ctx = iio_create_context_from_uri(URI);
    if (!ctx) {
        perror("Creating context from URI failed");
        return -1;
    }

    struct iio_device *dev = iio_context_find_device(ctx, "ad5592r_s");   
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

    struct iio_channel *channels[6];
    for (int i = 0; i < 6; i++) {
        channels[i] = iio_device_get_channel(dev, i);
        if (!channels[i]) {
            fprintf(stderr, "Channel %d not found\n", i);
            iio_context_destroy(ctx);
            return -1;
        }
        iio_channel_enable(channels[i]);
    }

    struct iio_buffer *buf = iio_device_create_buffer(dev, sample_buffer_size, CYCLIC);
    if (!buf) {
        perror("Failed to create buffer");
        iio_context_destroy(ctx);
        return -1;
    }

    int initial_raw_values[6] = {0};

    // Take initial readings from the buffer
    if (iio_buffer_refill(buf) < 0) {
        perror("Failed to refill buffer");
        iio_context_destroy(ctx);
        return -1;
    }

    for (void *ptr = iio_buffer_start(buf); ptr < iio_buffer_end(buf); ptr += iio_buffer_step(buf)) {
        for (int i = 0; i < 6; i++) {
            initial_raw_values[i] += *(uint16_t *)(ptr + i * sizeof(uint16_t));
        }
    }

    for (int i = 0; i < 6; i++) {
        initial_raw_values[i] /= sample_buffer_size;
        printf("Initial avg value for channel %d: %d\n", i, initial_raw_values[i]);
    }

    printf("-------------------\n");

    while (1) {
        double sum_values[6] = {0};

        if (iio_buffer_refill(buf) < 0) {
            perror("Failed to refill buffer");
            iio_context_destroy(ctx);
            return -1;
        }

        for (void *ptr = iio_buffer_start(buf); ptr < iio_buffer_end(buf); ptr += iio_buffer_step(buf)) {
            for (int i = 0; i < 6; i++) {
                sum_values[i] += *(uint16_t *)(ptr + i * sizeof(uint16_t));
            }
        }

        for (int i = 0; i < 6; i++) {
            uint16_t avg_value = sum_values[i] / sample_buffer_size;
            printf("avg_value for channel %d: %d\n", i, avg_value);

            if (abs(avg_value - initial_raw_values[i]) >= THRESHOLD) {
                printf("Shock detected on channel %d\n", i);
                printf("Recalibrating the device...\n");

                initial_raw_values[i] = avg_value;
                sleep(5);
            }
        }

        printf("---------------------------------\n");
        sleep(1);  // Adjust sleep duration as needed
    }

    iio_context_destroy(ctx);
    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <iio.h>
#include <string.h>
#include <unistd.h>
#define URI "ip:10.76.84.103"
#define threshold 200
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
    char* ch_name[6] = {"voltage0", "voltage1", "voltage2", "voltage3", "voltage4", "voltage5"};
    char initial_ch[6][100];
    for (int i = 0; i < 6; i++){
        iio_channel_attr_read(iio_device_find_channel(dev, ch_name[i], false), "raw", initial_ch[i], sizeof(initial_ch[i]));
        printf("Initial raw value char %d: %s\n", i, initial_ch[i]);
    }
    int initial_raw_values[6];
    for (int i = 0; i < 6; i++){
        initial_raw_values[i] = atoi(initial_ch[i]);
        printf("Initial raw value %d: %d\n", i, initial_raw_values[i]);
    }
    char detected_str[6][100];
    int detected_raw_str[6];
    while (1){
        for (int i = 0; i < 6; i++){
            iio_channel_attr_read(iio_device_find_channel(dev, ch_name[i], false), "raw", detected_str[i], sizeof(detected_str[i]));
            detected_raw_str[i] = atoi(detected_str[i]);
            if (abs(detected_raw_str[i] - initial_raw_values[i]) >= threshold){
                printf("Shock detected on channel %d\n", i);
                printf("Recalibrate the device\n");
                sleep(5);
                iio_channel_attr_read(iio_device_find_channel(dev, ch_name[i], false), "raw", initial_ch[i], sizeof(initial_ch[i]));
                initial_raw_values[i] = atoi(initial_ch[i]);
            }
            initial_raw_values[i] = detected_raw_str[i];
        }
    }
    iio_context_destroy(ctx);
    return 0;
}
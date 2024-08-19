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

void read_channels(struct iio_context *ctx){

    struct iio_device *device = iio_context_find_device(ctx, DEVICE_NAME);

    struct iio_channel *channel0 = iio_device_find_channel(device, CHANNEL_NAME0, false);
    struct iio_channel *channel1 = iio_device_find_channel(device, CHANNEL_NAME1, false);
    struct iio_channel *channel2 = iio_device_find_channel(device, CHANNEL_NAME2, false);
    struct iio_channel *channel3 = iio_device_find_channel(device, CHANNEL_NAME3, false);
    struct iio_channel *channel4 = iio_device_find_channel(device, CHANNEL_NAME4, false);
    struct iio_channel *channel5 = iio_device_find_channel(device, CHANNEL_NAME5, false);

    int raw0, raw1, raw2, raw3, raw4, raw5;

    while(true){
        char buf[256];

        ////////////////////////x////////////////////////
        if (iio_channel_attr_read(channel0, "raw", buf, sizeof(buf)) < 0) {
            perror("Reading raw attribute failed");
            return;
        }
        raw0 = atoi(buf);
        printf("Channel 0: %d\n", raw0);

        if (iio_channel_attr_read(channel1, "raw", buf, sizeof(buf)) < 0) {
            perror("Reading raw attribute failed");
            return;
        }
        raw1 = atoi(buf);
        printf("Channel 1: %d\n", raw1);

        if (raw0 > THRESHOLD){
            printf("x: turn -x\n");
        }

        if (raw1 > THRESHOLD){
            printf("x: turn +x\n");
        }

        if (raw0 < THRESHOLD && raw1 < THRESHOLD){
            printf("x: OK\n");
        }

        ////////////////////////y////////////////////////
        if (iio_channel_attr_read(channel2, "raw", buf, sizeof(buf)) < 0) {
            perror("Reading raw attribute failed");
            return;
        }
        raw2 = atoi(buf);
        printf("Channel 2: %d\n", raw2);

        if (iio_channel_attr_read(channel3, "raw", buf, sizeof(buf)) < 0) {
            perror("Reading raw attribute failed");
            return;
        }
        raw3 = atoi(buf);
        printf("Channel 3: %d\n", raw3);

        if (raw2 > THRESHOLD){
            printf("y: turn -y\n");
        }

        if (raw3 > THRESHOLD){
            printf("y: turn +y\n");
        }

        if (raw2 < THRESHOLD && raw3 < THRESHOLD){
            printf("y: OK\n");
        }

        ////////////////////////z////////////////////////
        if (iio_channel_attr_read(channel4, "raw", buf, sizeof(buf)) < 0) {
            perror("Reading raw attribute failed");
            return;
        }
        raw4 = atoi(buf);
        printf("Channel 4: %d\n", raw4);

        if (iio_channel_attr_read(channel5, "raw", buf, sizeof(buf)) < 0) {
            perror("Reading raw attribute failed");
            return;
        }
        raw5 = atoi(buf);
        printf("Channel 5: %d\n", raw5);

        if (raw4 > THRESHOLD){
            printf("z: turn -z\n-----------------\n");
        }

        if (raw5 > THRESHOLD){
            printf("z: turn +z\n-----------------\n");
        }

        if (raw4 < THRESHOLD && raw5 < THRESHOLD){
            printf("z: OK\n-----------------\n");
        }

        sleep(0.1);
    }
}



int main(){

    struct iio_context *ctx = iio_create_context_from_uri(URI);
    if (!ctx) {
        perror("Creating context from URI failed");
        return -1;
    }

    read_channels(ctx);

    iio_context_destroy(ctx);

    return 0;
}
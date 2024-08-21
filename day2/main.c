#include <iio.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define URI "ip:10.76.84.247"

int main()
{
    struct iio_context *ctx;
    ctx = iio_create_context_from_uri(URI);

    if(!ctx){
        perror("Cannot get context");
    }

    char dev_name[] = "ad5592r_s";
    struct iio_device *dev =  iio_context_find_device(ctx,dev_name);	

    if(!dev){
        perror("Cannot find device!");
    }

    struct iio_channel *channel = iio_device_find_channel(dev, "voltage0", false);
    if(!channel){
        perror("Cannot find channel -> voltage0");
    }

    char val_r[3], val_w[3];

    if(iio_device_attr_write(dev, "en", "1") == 0)
        perror("Error setting enable!");

    for(int i = 11; i <=20; i++){
        if(sprintf(val_w, "%d", i) == 0){
            perror("Error conv integer!");
        }

        if( iio_channel_attr_write(channel, "raw", val_w) == 0 ){
            perror("Cannot write to channel -> voltage0 -> raw");
        }

        if( iio_channel_attr_read(channel, "raw", val_r, 1024) == 0){
            perror("Cannot read attr value!");
        }

        printf("Value written: %s\n", val_r);
        sleep(1);
    }
}
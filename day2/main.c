#include <iio.h>
#include <stdio.h>

#define URI "ip:10.76.84.254"

int main(){
    struct iio_context * ctx;
    ctx = iio_create_context_from_uri(URI);

    if(!ctx){
        printf("%s", "cannot get ctx");
    }
   
    struct iio_device *dev;
    dev = iio_context_find_device(ctx, "ad5592r_s"); 

    printf("%s", iio_device_get_name(dev));    

    struct iio_channel *chn = NULL;

    struct iio_chanel *en = NULL;
    iio_device_attr_write(dev,"en","1");

    bool ok;
    char val_read[1024];
    char val_write[1024];

    for(int i=0;i<iio_device_get_channels_count(dev);i++){
        chn = iio_device_get_channel(dev,i);

        if(!chn){
            printf("ERROR");
        }

        sprintf(val_write, "%d", i);
        iio_channel_attr_write(chn,"raw",val_write);
        iio_channel_attr_read(chn,"raw",val_read,1024);

        printf("\n[%d] ", i);
        printf("%s", val_read);
    }


    //iio_chanel (voltage0)
    //for(int i=0;i<10;i++){
    //  write to chanel attribute i
    //  read and print chanel attribute
    //  sleep 1
    //}

}
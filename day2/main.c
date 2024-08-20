#include <iio.h>
#include <stdio.h>

#define URI "ip:10.76.84.240"

int main()
{
    unsigned int major;
    unsigned int minor;
    char git_tag[8];
   
    const char *name;
    const char *value;

    iio_library_get_version(&major, &minor, git_tag);
    printf("%d,%d, %s\n", major, minor, git_tag);

    struct iio_context * ctx;
    ctx=iio_create_context_from_uri(URI);
    
    if(!ctx){
        perror("cannot get ctx");

    }
    int attributes=iio_context_get_attrs_count(ctx);
    printf("nr of attr: %d\n", attributes);

    
    for(int i=0; i<attributes;i++){
        iio_context_get_attr(ctx,i,&name,&value);
        printf("nume:%s , val: %s\n\n", name, value);
    }

    char dev_name[]="ad5592r-s";
    struct iio_device *dev= iio_context_find_device (ctx,dev_name);
    if(!dev){
        perror("cannot get device");

    }

     struct iio_channel *channel =iio_device_find_channel(dev,"voltage0",false);
     if(!channel){
         perror("cannot get channel");

     }

     char val_ir[10];
     char val_iw[10];

     if(iio_device_attr_write(dev, "en", "1")==0){
        perror("Enable on 0");
     }

    for(int i=0; i<=10;i++){
     sprintf(val_iw,"%d",i);
    
     if(iio_channel_attr_write(channel, "raw",val_iw)==0){
        perror("no writing on channel");
     }

     if(iio_channel_attr_read(channel, "raw",val_ir,1024)==0){
        perror("no reading on channel");
     }

     printf("Value on channel: %s\n", val_ir);
     sleep(1);

}
}
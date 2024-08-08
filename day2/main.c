#include <stdio.h>
#include <iio.h>
#include <string.h>

#define URI "ip:10.76.84.237"

int main(){
    
    int ret;

    unsigned int major;
    unsigned int minor;
    char git_tag[8];
    
    iio_library_get_version(&major, &minor, git_tag);

    printf("%d, %d, %s\n", major, minor, git_tag);

    struct iio_context * ctx;
    ctx = iio_create_context_from_uri(URI);

    if(!ctx){
        printf("%s", "cannot get ctx");
    }
 
    struct iio_device * dev = iio_context_find_device(ctx, "ad5592r_s");
    if(!dev){
        fprintf(stderr, "Unable to find device\n");
        return -1;
    }

    struct iio_channel *chn = iio_device_find_channel(dev, "voltage0", false);
    if(!chn){
        fprintf(stderr, "Unable to find channel\n");
        return -1;
    }

    ret = iio_device_attr_write(dev, "en", "1");
    if (ret < 0) {
        fprintf(stderr, "Unable to enable the channel 'voltage0'.\n");
        iio_context_destroy(ctx);
        return -1;
    }
    printf("Channel 'voltage0' enabled successfully.\n");

    char attr_name[20];
    char attr_value[20];
    char read_value[20];

    /* const char * iio_channel_get_name(const struct iio_channel *chn)
    {
        return chn->name;
    } */
    char value[20];

    for(int i=0; i<10; i++){
        
        // snprintf(attr_name, sizeof(attr_name), "attr%d", i+1);
        // snprintf(attr_value, sizeof(attr_value), "%d", i);

        value[0] = i+'0';
        ret = iio_channel_attr_write(chn, "raw", value);
        fflush(stdout);
        if(ret < 0){
            printf(stderr, "Unable to write attribute %s\n", "raw");
        }
        else{
            printf("Attribute written: %s = %s \n", "raw", value);
        }

        ret = iio_channel_attr_read(chn, "raw", read_value, sizeof(read_value));
        if(ret < 0){
            printf(stderr, "Unable to read attribute %s\n", "raw");
        }
        else{
            printf("Attribute read: %s = %s \n", "raw", read_value);
        }

        sleep (1);
    }

    iio_context_destroy(ctx);
    return 0;
}
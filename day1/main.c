#define URI "ip:10.76.84.244"
#include <iio.h>
#include <stdio.h>


int main(void){
    unsigned int major, minor;
    char git_tag[8];
    iio_library_get_version(&major, &minor, git_tag);
    printf("%d %d %s\n", major, minor, git_tag);

    struct iio_context *ctx = iio_create_context_from_uri(URI);
    if(!ctx)
    {
        printf("Context unavailable\n");
        return -1;
    }
    printf("Conection established\n");

    struct iio_device *iio_device_ = iio_context_find_device(ctx, "iio-ad5592r_s");

    //atribute
    unsigned int atr_count =  iio_device_get_attrs_count(iio_device_);
    printf("\nAttributes: %d\n", atr_count);
    for(int i=0; i<atr_count; i++)
    {
        const char *attribute = iio_device_get_attr(iio_device_, i);
        printf("%s\n", attribute);
    }

    iio_device_attr_write(iio_device_, "en", "1");

    //scriere canale
    unsigned int chan_count = iio_device_get_channels_count(iio_device_);

    for(int i=0; i<chan_count; i++)
    {
        char value[10];
        struct iio_channel *iio_channel_ = iio_device_get_channel(iio_device_, i);
        sprintf(value, "%d", i*2);
        iio_channel_attr_write(iio_channel_, "raw", value);
    }

    //citire canale
    printf("\nChannels: %d\n", chan_count);

    for(int i=0; i<chan_count; i++)
    {
        char dst[100];
        struct iio_channel *iio_channel_ = iio_device_get_channel(iio_device_, i);
        const char *channel_name = iio_channel_get_id(iio_channel_);
        ssize_t dev_attr = iio_channel_attr_read(iio_channel_, "raw" , dst, sizeof(dst));
        printf("%s : %s\n", channel_name, dst);
    }
    return 0;
}
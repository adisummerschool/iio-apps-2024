#define URI "ip:10.76.84.242"
#include <iio.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#define threshold 50
#define shockthreshold 400
#define oneg 2000
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
    struct iio_device *iio_device_ = iio_context_find_device(ctx, "ad5592r_s");
    //atribute
    unsigned int atr_count =  iio_device_get_attrs_count(iio_device_);
    printf("\nAttributes: %d\n", atr_count);
    for(int i=0; i<atr_count; i++)
    {
        const char *attribute = iio_device_get_attr(iio_device_, i);
        printf("%s\n", attribute);
    }
    iio_device_attr_write(iio_device_, "en", "1");
    //citire canale
    unsigned int chan_count = iio_device_get_channels_count(iio_device_);
    printf("\nChannels: %d\n", chan_count);
    char dst[6][100] = {"0", "0", "0", "0", "2000", "0"};
    char dst_prev[6][100];
    bool shock;
    while(true)
    {
        for(int i=0; i<chan_count; i++)
        {
            strcpy(dst_prev[i], dst[i]);
            struct iio_channel *iio_channel_ = iio_device_get_channel(iio_device_, i);
            const char *channel_name = iio_channel_get_id(iio_channel_);
            ssize_t dev_attr = iio_channel_attr_read(iio_channel_, "raw" , dst[i], sizeof(dst));
            //printf("%s : %s\n", channel_name, dst[i]);
        }
        //calibrare
        /*if(atoi(dst[0])>threshold)
            printf("X turn right\n");
        else if(atoi(dst[1])>threshold)
            printf("X turn left\n");
        else
            printf("X ok\n");
        if(atoi(dst[2])>threshold)
            printf("Y turn right\n");
        else if(atoi(dst[3])>threshold)
            printf("Y turn left\n");
        else
            printf("Y ok\n");
        if(atoi(dst[4])<oneg-threshold || atoi(dst[4])>oneg+threshold)
            printf("Z turn right\n");
        else if(atoi(dst[5])>threshold)
            printf("Z turn left\n");
        else
            printf("Z ok\n");
        printf("\033[A");
        printf("\033[K");
        printf("\033[A");
        printf("\033[K");
        printf("\033[A");
        printf("\033[K");*/
        //sleep(1);
        if(abs(atoi(dst[0])-atoi(dst_prev[0])) > shockthreshold || abs(atoi(dst[1])-atoi(dst_prev[1])) > shockthreshold)
        {
            printf("X: detected shock: %f G\n", (float) (atoi(dst[0])-atoi(dst[1]))/oneg);
            shock=true;
        }
        else
            printf("X: no shock\n");
        if(abs(atoi(dst[2])-atoi(dst_prev[2])) > shockthreshold || abs(atoi(dst[3])-atoi(dst_prev[3])) > shockthreshold)
        {
            printf("Y: detected shock: %f G\n", (float) (atoi(dst[2])-atoi(dst[3]))/oneg);
            shock=true;
        }
        else
            printf("Y: no shock\n");
        if(abs(atoi(dst[4])-atoi(dst_prev[4])) > shockthreshold || abs(atoi(dst[5])-atoi(dst_prev[5])) > shockthreshold)
        {
            printf("Z: detected shock: %f G\n", (float) (atoi(dst[4])-atoi(dst[5]))/oneg);
            shock=true;
        }
        else
            printf("Z: no shock\n");
        printf("\033[A");
        printf("\033[K");
        printf("\033[A");
        printf("\033[K");
        printf("\033[A");
        printf("\033[K");
        if(shock)
            sleep(1);
        shock=false;
    }
    return 0;
}












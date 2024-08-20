#include <iio.h>

#define URI "ip:10.76.84.107"

int main()
{
    unsigned int major;
    unsigned int minor;
    char git_tag[8];

    iio_library_get_version(&major, &minor, git_tag);

    printf("Library info: %d, %d, %s\n\n", major, minor, git_tag);

    struct iio_context* ctx;
    ctx = iio_create_context_from_uri(URI);

    if (!ctx)
    {
        perror("cannot get ctx!\n");
    }

    int n = iio_context_get_attrs_count(ctx);
    printf("no. of attr of context: %d\n",n);
    // for (int i = 0; i < n; ++i)
    // {
    //     char* name;
    //     char* value;

    //     iio_context_get_attr(ctx, i, &name, &value);
    //     printf("%d) %s, %s\n", i, name, value);
    // }

    char namedev[]="ad5592r_s";
    struct iio_device *dvc;


    dvc = iio_context_find_device(ctx,namedev);
    if (!dvc)
    {
        perror("cannot get dvc!\n");
    }

    int no_channels = iio_device_get_channels_count(dvc);
    printf("no. channels of device: %d\n",no_channels);

    iio_device_attr_write(dvc,"en","1");

     struct iio_channel *channel0;
     channel0= iio_device_find_channel(dvc,"voltage0",false);
     struct iio_channel *channel1;
     channel1= iio_device_find_channel(dvc,"voltage1",false);
     struct iio_channel *channel2;
     channel2= iio_device_find_channel(dvc,"voltage2",false);
     struct iio_channel *channel3;
     channel3= iio_device_find_channel(dvc,"voltage3",false);
     struct iio_channel *channel4;
     channel4= iio_device_find_channel(dvc,"voltage4",false);
     struct iio_channel *channel5;
     channel5= iio_device_find_channel(dvc,"voltage5",false);
    // char *raw_chann0 = iio_channel_find_attr(channel0,"raw");

    char dst[6][10];
    int initials[6];
    iio_channel_attr_read(channel0, "raw", dst[0], 10);
    iio_channel_attr_read(channel1, "raw", dst[1], 10);
    iio_channel_attr_read(channel2, "raw", dst[2], 10);
    iio_channel_attr_read(channel3, "raw", dst[3], 10);
    iio_channel_attr_read(channel4, "raw", dst[4], 10);
    iio_channel_attr_read(channel5, "raw", dst[5], 10);
    sleep(1);
    printf("initials: ");
    for(int i=0;i<6;i++){
        initials[i]=atoi(dst[i]);
        printf("%d ",initials[i]);
    }
    printf("\n");
    
    char c[10];
    int difs[6];
    while(1){
        iio_channel_attr_read(channel0, "raw", dst[0], 10);
        iio_channel_attr_read(channel1, "raw", dst[1], 10);
        iio_channel_attr_read(channel2, "raw", dst[2], 10);
        iio_channel_attr_read(channel3, "raw", dst[3], 10);
        iio_channel_attr_read(channel4, "raw", dst[4], 10);
        iio_channel_attr_read(channel5, "raw", dst[5], 10);

        for(int i=0;i<6;i++){
            difs[i]=atoi(dst[i])-initials[i];
            printf(" diferenta: %d; ",difs[i]);
        }

        printf("\nX_AXIS:");
        if(atoi(dst[0])>=0 && atoi(dst[0])<=50){
            if(atoi(dst[1])>=0 && atoi(dst[1])<=50){
                printf("OK\n");
            }
            else{
                printf("turn left\n");
            }
        }
        else{
            printf("turn right\n");
        }
        printf("Y_AXIS:");
        if(atoi(dst[2])>=0 && atoi(dst[2])<=50){
            if(atoi(dst[3])>=0 && atoi(dst[3])<=50){
                printf("OK\n");
            }
            else{
                printf("turn left\n");
            }
        }
        else{
            printf("turn right\n");
        }
        printf("Z_AXIS:");
        if(atoi(dst[4])>=0 && atoi(dst[4])<=50){
            if(atoi(dst[5])>=0 && atoi(dst[5])<=50){
                printf("OK\n");
            }
            else{
                printf("turn left\n");
            }
        }
        else{
            printf("turn right\n");
        }
        printf("ch0: %s     ch1: %s     ch2: %s     ch3: %s     ch4: %s     ch5: %s\n", dst[0], dst[1], dst[2], dst[3], dst[4], dst[5]);
        printf("\n");
        
        sleep(1);
    }
    iio_context_destroy(ctx);
    return 0;
}
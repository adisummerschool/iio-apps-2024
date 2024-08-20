#include <iio.h>
#include <stdio.h>
#include <unistd.h>

#define URI "ip:10.76.84.107"
#define THRESH 200

int main()
{
    struct iio_context* ctx;
    ctx = iio_create_context_from_uri(URI);

    if (!ctx)
    {
        perror("cannot get ctx!\n");
    }

    // int n = iio_context_get_attrs_count(ctx);
    // printf("no. of attr of context: %d\n",n);

    char namedev[]="ad5592r_s";
    struct iio_device *dvc;

    dvc = iio_context_find_device(ctx,namedev);
    if (!dvc)
    {
        perror("cannot get dvc!\n");
    }

    // int no_channels = iio_device_get_channels_count(dvc);
    // printf("no. channels of device: %d\n",no_channels);

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
    char *ch_name[6]={"voltage0", "voltage1", "voltage2", "voltage3", "voltage4", "voltage5"};

    // iio_channel_attr_read(channel0, "raw", dst[0], sizeof(dst[0]));
    // iio_channel_attr_read(channel1, "raw", dst[1], 10);
    // iio_channel_attr_read(channel2, "raw", dst[2], 10);
    // iio_channel_attr_read(channel3, "raw", dst[3], 10);
    // iio_channel_attr_read(channel4, "raw", dst[4], 10);
    // iio_channel_attr_read(channel5, "raw", dst[5], 10);
    for(int i=0;i<6;i++){
        iio_channel_attr_read(iio_device_find_channel(dvc,ch_name[i],false),"raw",dst[i],sizeof(dst[i]));//*dst[i] ??
    }
    printf("\ninitials: ");
    for(int i=0;i<6;i++){
        initials[i]=atoi(dst[i]);
        printf("%d ",initials[i]);
    }
    printf("\n");

    //sleep(1);
    
    char c[10];
    int difs[6];
    char det_val[6][10];
    int det_val_raw[6];

    while(1){

        for(int i=0;i<6;i++){
            iio_channel_attr_read(iio_device_find_channel(dvc,ch_name[i],false),"raw",det_val[i],sizeof(det_val[i]));
            det_val_raw[i]=atoi(det_val[i]);
            printf("\nDetected value: %d\n",det_val_raw[i]);
            if((det_val_raw[i]-initials[i])>=THRESH){
                    printf("\nSHOCK!!!!!\n");
                    printf("\nRecalibrate\n");
                    sleep(5);
                    iio_channel_attr_read(iio_device_find_channel(dvc,ch_name[i],false),"raw",dst[i],sizeof(dst[i]));
                    initials[i]=atoi(dst[i]);
            }
            // else{
            //     printf("\nno shock\n");
            // }
            initials[i]= det_val_raw[i];
        }

        // printf("\nX_AXIS:");
        // if(abs(difs[0])>500 || abs(difs[1])>500){
        //     if(abs(difs[0])>abs(difs[1]))
        //         printf("detected shock: %.3fG ",abs(difs[0])*1.0/2000.0);
        //     else
        //         printf("detected shock: %.3fG ",abs(difs[1])*1.0/2000.0);
        // }
        // else
        //     printf("no shock");
        // printf("\nY_AXIS:");
        // if(abs(difs[2])>500 || abs(difs[3])>500)
        // {
        //     if(abs(difs[2])>abs(difs[3]))
        //         printf("detected shock: %.3fG ",abs(difs[2])*1.0/2000.0);
        //     else
        //         printf("detected shock: %.3fG ",abs(difs[3])*1.0/2000.0);
        // }
        // else
        //     printf("no shock");
        // printf("\nZ_AXIS:");
        // if(abs(difs[4])>500 || abs(difs[5])>500)
        // {
        //     if(abs(difs[4])>abs(difs[5]))
        //         printf("detected shock: %.3fG ",abs(difs[4])*1.0/2000.0);
        //     else
        //         printf("detected shock: %.3fG ",abs(difs[5])*1.0/2000.0);
        // }
        // else
        //     printf("no shock");
        // printf("\nch0: %s     ch1: %s     ch2: %s     ch3: %s     ch4: %s     ch5: %s\n", dst[0], dst[1], dst[2], dst[3], dst[4], dst[5]);
        // printf("\n");
        
        //sleep(1);
    }
    iio_context_destroy(ctx);
    return 0;
}
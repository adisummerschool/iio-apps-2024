#include <iio.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

#define URI "ip:10.76.84.107"
#define THRESH 200

const int max_adc = 4095;
const double volt_per_lsb = 2.4/4095;

struct iio_context* ctx;

void my_handler(int sig){
    signal(SIGINT, my_handler);
    printf("Caught signal\n");
    iio_context_destroy(ctx);
    exit(0); 

}


int main()
{
    signal(SIGINT, my_handler);
    ctx = iio_create_context_from_uri(URI);

    if (!ctx)
    {
        perror("cannot get ctx!\n");
    }

    char namedev[]="ad5592r_s";
    struct iio_device *dvc;

    dvc = iio_context_find_device(ctx,namedev);
    if (!dvc)
    {
        perror("cannot get dvc!\n");
    }

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

    if( !channel0 || !channel1 || !channel2 || !channel3 || !channel4 || !channel5)
        perror("\n channel not found \n");    

    char initials_str[6][10];
    int initials[6];
    char *ch_name[6]={"voltage0", "voltage1", "voltage2", "voltage3", "voltage4", "voltage5"};
    double ch_voltage[6];
    double ch_g[6];
    while(1){

        printf("reads:");
        for(int i=0;i<6;i++){
            iio_channel_attr_read(iio_device_find_channel(dvc,ch_name[i],false),"raw",initials_str[i],sizeof(initials_str[i]));
            initials[i]=atoi(initials_str[i]);
            printf("%d ",initials[i]);
        }
        printf("\n");
        
        for(int i=0; i<6; i++){
            ch_voltage[i] = initials[i] * volt_per_lsb;
            printf("VOLTAGE ON CH%d IS %lf [V]\n",i,ch_voltage[i]);
            ch_g[i] = ch_voltage[i] / (max_adc * 1.0);
            printf("ACCEL ON CH%d IS %lf [g]\n",i,ch_voltage[i]);
        }
        printf("\n");
        sleep(1);
        printf("\033[A");
        printf("\033[K");
        printf("\033[A");
        printf("\033[K");
        printf("\033[A");
        printf("\033[K");
        printf("\033[A");
        printf("\033[K");
        printf("\033[A");
        printf("\033[K");
        printf("\033[A");
        printf("\033[K");
        printf("\033[A");
        printf("\033[K");
        printf("\033[A");
        printf("\033[K");
        printf("\033[A");
        printf("\033[K");
        printf("\033[A");
        printf("\033[K");
        printf("\033[A");
        printf("\033[K");
        printf("\033[A");
        printf("\033[K");
        printf("\033[A");
        printf("\033[K");
        printf("\033[A");
        printf("\033[K");
    }
     
    
    return 0;
}
#include <iio.h>
#include <stdio.h>
#include <unistd.h>

#define URI "ip:10.76.84.254"

const double voltage_per_lsb = 2.4/4095;

int main(){
    struct iio_context * ctx;
    ctx = iio_create_context_from_uri(URI);

    if(!ctx){
        printf("%s", "cannot get ctx");
    }

    struct iio_device *dev;
    dev = iio_context_find_device(ctx, "iio:device0"); 

    if(!dev){
        printf("\n no device \n");
    }

    printf("%s", iio_device_get_name(dev));    

    struct iio_channel *chn = NULL;

    iio_device_attr_write(dev,"en","1");

    bool ok;
    char val_read[1024];
    char val_write[1024];

    int val_chans[8] = {0};

    while(1){
        for(int i=0;i<iio_device_get_channels_count(dev);i++){
            chn = iio_device_get_channel(dev,i);

            if(!chn){
                printf("ERROR");
            }

            iio_channel_attr_read(chn,"raw",val_read,1024);

            val_chans[i] = atoi(val_read);
        }

        double v_x_neg = (double)(val_chans[0])*voltage_per_lsb;
        double v_x_poz = (double)(val_chans[1])*voltage_per_lsb;
        double v_y_neg = (double)(val_chans[2])*voltage_per_lsb;
        double v_y_poz = (double)(val_chans[3])*voltage_per_lsb;
        double v_z_neg = (double)(val_chans[4])*voltage_per_lsb;
        double v_z_poz = (double)(val_chans[5])*voltage_per_lsb;

        double g_x_neg = (double)(val_chans[0])/2000;
        double g_x_poz = (double)(val_chans[1])/2000;
        double g_y_neg = (double)(val_chans[2])/2000;
        double g_y_poz = (double)(val_chans[3])/2000;
        double g_z_neg = (double)(val_chans[4])/2000;
        double g_z_poz = (double)(val_chans[5])/2000;
        
        printf("\n X neg: %f [V]  |  %f [G]", v_x_neg, g_x_neg);
        printf("\n X poz: %f [V]  |  %f [G]", v_x_poz, g_x_poz);
        printf("\n Y neg: %f [V]  |  %f [G]", v_y_neg, g_y_neg);
        printf("\n Y poz: %f [V]  |  %f [G]", v_y_poz, g_y_poz);
        printf("\n Z neg: %f [V]  |  %f [G]", v_z_neg, g_z_neg);
        printf("\n Z poz: %f [V]  |  %f [G]", v_z_poz, g_z_poz);
        printf("\n-------------------------------------------");

        sleep(1);
    }
}
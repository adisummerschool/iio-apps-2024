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

    int treshold = 100;
    int treshold_G = 400;

    int val_chans[8] = {0};
    int nr_read = 0;

    int prev_val_x_poz[10] = {0};
    int prev_val_x_neg[10] = {0};
    int prev_val_y_poz[10] = {0};
    int prev_val_y_neg[10] = {0};
    int prev_val_z_poz[10] = {0};
    int prev_val_z_neg[10] = {0};

    while(true){    
        for(int i=0;i<iio_device_get_channels_count(dev);i++){
            chn = iio_device_get_channel(dev,i);

            if(!chn){
                printf("ERROR");
            }

            iio_channel_attr_read(chn,"raw",val_read,1024);

            val_chans[i] = atoi(val_read);

            printf("\n[Channel %d] ", i);
            printf("%s", val_read);
        }
        if(nr_read == 0){
            for(int i=0;i<10;i++){
                prev_val_x_neg[i] = val_chans[0];
                prev_val_x_poz[i] = val_chans[1];
                prev_val_y_neg[i] = val_chans[2];
                prev_val_y_poz[i] = val_chans[3];
                prev_val_z_neg[i] = val_chans[4];
                prev_val_z_poz[i] = val_chans[5];
            }
        }

        printf("\n-------------------------------\n");

        printf("\nX: ");
        if(abs(val_chans[0] - val_chans[1]) <= treshold){
            printf(" OK");
        }
        else{
            if(val_chans[0] - val_chans[1] > treshold){
                printf(" Turn Left");
            }
            else{
                printf(" Turn Right");
            }
        }

        printf("\nY: ");
        if(abs(val_chans[2] - val_chans[3]) <= treshold){
            printf(" OK");
        }
        else{
            if(val_chans[2] - val_chans[3] > treshold){
                printf(" Turn Left");
            }
            else{
                printf(" Turn Right");
            }
        }

        printf("\nZ: ");
        if(abs(val_chans[4] - val_chans[5]) <= treshold){
            printf(" OK");
        }
        else{
            if(val_chans[4] - val_chans[5] > treshold){
                printf(" Turn Left");
            }
            else{
                printf(" Turn Right");
            }
        }
        
        printf("\n-------------------------------\n");

        int average_x_neg = 0;
        int average_x_poz = 0;
        int average_y_neg = 0;
        int average_y_poz = 0;
        int average_z_neg = 0;
        int average_z_poz = 0;

        for(int i=0; i<10; i++){
            average_x_neg = average_x_neg + prev_val_x_neg[i];
            average_x_poz = average_x_poz + prev_val_x_poz[i];
            average_y_neg = average_y_neg + prev_val_y_neg[i];
            average_y_poz = average_y_poz + prev_val_y_poz[i];
            average_z_neg = average_z_neg + prev_val_z_neg[i];
            average_z_poz = average_z_poz + prev_val_z_poz[i];
        }

        int shock_det = 0;

        int average_val_x = abs(val_chans[0] - average_x_neg/10) + abs(val_chans[1] - average_x_poz/10);
        int average_val_y = abs(val_chans[2] - average_y_neg/10) + abs(val_chans[3] - average_y_poz/10);
        int average_val_z = abs(val_chans[4] - average_z_neg/10) + abs(val_chans[5] - average_z_poz/10);

        printf("\nAverage Z neg: %d", average_z_neg);
        printf("\nAverage Z poz: %d", average_z_poz);

        printf("\nAverage X: %d", average_val_x);
        printf("\nAverage Y: %d", average_val_y);
        printf("\nAverage Z: %d", average_val_z);

        printf("\n X: ");
        if(average_val_x > treshold_G){
            printf("%f G", (float)(average_val_x)/2000.0);
            shock_det = 1;
        }
        else{
            printf("no shock");
        }

        printf("\n Y: ");
        if(average_val_y > treshold_G){
            printf("%f G", (float)(average_val_y)/2000.0);
            shock_det = 1;
        }
        else{
            printf("no shock");
        }

        printf("\n Z: ");
        if(average_val_z > treshold_G){
            printf("%f G", (float)(average_val_z)/2000.0);
            shock_det = 1;
        }
        else{
            printf("no shock");
        }

        printf("\n");
        usleep(1000);

        if(shock_det == 1){
            sleep(1);
            printf("\nRECALIBRATE\n");
        }

        prev_val_x_poz[nr_read % 10] = val_chans[1];
        prev_val_x_neg[nr_read % 10] = val_chans[0];
        prev_val_y_poz[nr_read % 10] = val_chans[3];
        prev_val_y_neg[nr_read % 10] = val_chans[2];
        prev_val_z_poz[nr_read % 10] = val_chans[5];
        prev_val_z_neg[nr_read % 10] = val_chans[4];
        
        nr_read++;
    }
}
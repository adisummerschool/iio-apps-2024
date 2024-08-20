#define URI "ip:10.76.84.252"
#include <iio.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>
#define threshold 50
#define anglethreshold 200
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

    struct iio_device *iio_device_ = iio_context_find_device(ctx, "iio-ad5592r_s");
    if(!iio_device_)
    {
        printf("Device iio-ad5592r_s not found\n");
        return -1;
    }
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
    time_t time_shock = time(NULL);
    double anglex, angley;
    int optiune=0;
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
        if(!optiune)
        {
            printf("Optiune: \n");
            printf("1. Calibrare\n");
            printf("2. Detectie soc\n");
            printf("3. Unghi\n");
            scanf("%d", &optiune);
        }
        switch (optiune)
        {
            case 1:
                if(atoi(dst[0])>threshold)
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
                printf("\033[K");
                break;

            case 2:
                double xg = atoi(dst[0])>atoi(dst[1]) ? atoi(dst[0]) : -atoi(dst[1]);
                double yg = atoi(dst[2])>atoi(dst[3]) ? atoi(dst[2]) : -atoi(dst[3]);
                double zg = atoi(dst[4])>atoi(dst[5]) ? atoi(dst[4]) : -atoi(dst[5]);
                double xg_prev = atoi(dst_prev[0])>atoi(dst_prev[1]) ? atoi(dst_prev[0]) : -atoi(dst_prev[1]);
                double yg_prev = atoi(dst_prev[2])>atoi(dst_prev[3]) ? atoi(dst_prev[2]) : -atoi(dst_prev[3]);
                double zg_prev = atoi(dst_prev[4])>atoi(dst_prev[5]) ? atoi(dst_prev[4]) : -atoi(dst_prev[5]);

                if(time(NULL)-time_shock>=1)
                {
                    if(abs(xg-xg_prev) > shockthreshold)
                    {
                        printf("X: detected shock: %lf G\n", (xg-xg_prev)/oneg);
                        shock=true;
                    }
                    else
                        printf("X: no shock\n");

                    if(abs(yg-yg_prev) > shockthreshold )
                    {
                        printf("Y: detected shock: %f G\n", (yg-yg_prev)/oneg);
                        shock=true;
                    }
                    else
                        printf("Y: no shock\n");

                    if(abs(zg-zg_prev) > shockthreshold)
                    {
                        printf("Z: detected shock: %f G\n", (zg-zg_prev)/oneg);
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
                
                }
                if(shock)
                    time_shock=time(NULL);
                shock=false;   
                break;
                
            case 3:
                double x = atoi(dst[0])>atoi(dst[1]) ? atoi(dst[0]) : -atoi(dst[1]);
                double y = atoi(dst[2])>atoi(dst[3]) ? atoi(dst[2]) : -atoi(dst[3]);
                double z = atoi(dst[4])>atoi(dst[5]) ? atoi(dst[4]) : -atoi(dst[5]);

            
                anglex = abs(x)>anglethreshold || abs(z)>anglethreshold ? 180/3.14*atan2(x,z) : 0;
                angley = abs(y)>anglethreshold || abs(z)>anglethreshold ? 180/3.14*atan2(y,z) : 0;       

                if(time(NULL)-time_shock>=1)
                {
                printf("X: %lf°\n", anglex);
                printf("Y: %lf°\n", angley);

                printf("\033[A");
                printf("\033[K");
                printf("\033[A");
                printf("\033[K");
                }
                time_shock=time(NULL);
                    break;
                default:
                    break;
            }
    }
    return 0;
}
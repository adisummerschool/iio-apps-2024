#include <iio.h>
#include <stdio.h>

#define URI "ip:10.76.84.243"

const int max_adc = 4095;
const double voltage_per_lbs = 2.4 / 4095;

int main()
{
    struct iio_context* ctx = iio_create_context_from_uri(URI);

    if (!ctx){
        printf("\n no context\n");
        }
    struct iio_device *dev = iio_context_find_device(ctx, "iio:device0");

    if (!dev){
        printf("\n no device \n");
        }

    iio_device_attr_write(dev, "en", "1");

    struct iio_channel * chn; // iio_channel structure

    int a = 0;
    while(true)
    {
        for (int i = 0; i < 6; i++)
        {
            chn = iio_device_get_channel(dev, i);

            char strr[100];
            iio_channel_attr_read(chn, "raw", strr, 100);
            int raw = atoi(strr);

            a = a + 1;
            a = a % 8;

            if(a == 1) printf("\033[30m");
            if(a == 2) printf("\033[34m");
            if(a == 3) printf("\033[32m");
            if(a == 4) printf("\033[36m");
            if(a == 5) printf("\033[31m");
            if(a == 6) printf("\033[35m");
            if(a == 7) printf("\033[33m");
            if(a == 8) printf("\033[37m");

            double chn_voltage = raw * voltage_per_lbs;
            double chn_accel = (double)raw / max_adc;
            printf("\nVOLTAGE IS %.3f [V] ACCEL IS %.3f [g] -- On Channel %d!",
             chn_voltage, chn_accel, i);  
        }
        sleep(1);
        printf("\033[A\033[K");
        printf("\033[A\033[K");
        printf("\033[A\033[K");
        printf("\033[A\033[K");
        printf("\033[A\033[K");
        printf("\033[A\033[K");
    }

    return 0;
}
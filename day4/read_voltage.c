#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iio.h>
#include <unistd.h>

#define URI "ip:10.76.84.244"

const int max_adc = 4095;
const double volts_per_lsb = 2.4/4095;

int main(int argc, char *argv[]){
    struct iio_context* ctx = iio_create_context_from_uri(URI);
    if(!ctx){
        printf("\n no context \n");
        return -1;
    }

    struct iio_device *dev = iio_context_find_device(ctx, "iio:device0");
    if(!dev){
        printf("\n no device \n");
        return -1;
    }

    iio_device_attr_write(dev, "en", "1");
    struct iio_channel *ch[6] = {
            iio_device_find_channel(dev, "voltage0", false),
            iio_device_find_channel(dev, "voltage1", false),
            iio_device_find_channel(dev, "voltage2", false),
            iio_device_find_channel(dev, "voltage3", false),
            iio_device_find_channel(dev, "voltage4", false),
            iio_device_find_channel(dev, "voltage5", false)
            };

    if(!ch[0] || !ch[1] || !ch[2] || !ch[3] || !ch[4] || !ch[5]){
        printf("\n channel not found \n");
        return -1;
    }
    
    char chn_str[6][100];

    while(true){
        for(int i = 0; i < 6; i++){
            iio_channel_attr_read(ch[i], "raw", chn_str[i], sizeof(chn_str[i]));
            int chn_raw = atoi(chn_str[i]);
            double chn_voltage = chn_raw * volts_per_lsb;
            printf("CHANNEL %d ---------- VOLTAGE IS %.4f [V] %f G\n", i, chn_voltage, chn_raw/2000.0);

        }
        printf("------------------------------------------\n");
        sleep(5);
    }

    iio_context_destroy(ctx);
    return 0;
}
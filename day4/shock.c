#include <stdio.h>
#include <stdlib.h>
#include <iio.h>
#include <string.h>
#include <unistd.h>
#define URI "ip:10.76.84.103"
#define NO_CH                   6
const int MAX_ADC = 4095;
const double VOLTS_PER_LSB = 2.4 / MAX_ADC;



const int samples_count = 100;

typedef struct 
{
    uint16_t xpoz;
    uint16_t xneg;
    uint16_t ypoz;
    uint16_t yneg;
    uint16_t zpoz;
    uint16_t zneg;


}buffer_element;

double buf_avg(uint16_t buf){


}


int main( int argc, char *argv[]){
    struct iio_context *ctx;
    struct iio_device *dev;
    struct iio_buffer *buf;
    struct iio_channel *channels[6];
    bool cyclic = false;

    ctx = iio_create_context_from_uri(URI);

    if (!ctx) {
        printf("Creating context from URI failed");
        return -1;
    }

    dev = iio_context_find_device(ctx, "ad5592r_s");
    iio_device_attr_write(dev, "en", "1");

    if (!dev) {
        printf("Device not found");
        return -1;
    }


    for (int i = 0; i < 6; i++){
        channels[i] = iio_device_get_channel(dev, i);
        if(!channels[i]){
            printf("\n ch not found");
            return -1;
        }
       iio_channel_enable(channels[i]);
    }

    buf = iio_device_create_buffer(dev, samples_count, cyclic);
    
    int bytes_read = iio_buffer_refill(buf);
    uint16_t values[6];

    if(!buf){
        printf("\n cannot create buffer \n");
        return -1;
    } 

    // for(void *ptr = iio_buffer_start(buf); ptr < iio_buffer_end(buf); ptr+= iio_buffer_step(buf)){

    //     uint16_t xpoz = *(uint16_t *) (ptr + 0 * sizeof(uint16_t));
    //     printf("%d ", xpoz);
    //       uint16_t xneg = *(uint16_t *) (ptr + 1 * sizeof(uint16_t));
    //     printf("%d \n", xneg);

    //     uint16_t ypoz = *(uint16_t *) (ptr + 2 * sizeof(uint16_t));
    //     printf("%d ", ypoz);
    //     uint16_t yneg = *(uint16_t *) (ptr + 3 * sizeof(uint16_t));
    //     printf("%d \n", yneg);

    //       uint16_t zpoz = *(uint16_t *) (ptr + 4 * sizeof(uint16_t));
    //     printf("%d ", zpoz);
    //     uint16_t zneg = *(uint16_t *) (ptr + 5 * sizeof(uint16_t));
    //     printf("%d \n", zneg);
    // }

    
uint16_t val[NO_CH] = {0};
    int sum[NO_CH] = {0}, j=0;
    double avg [NO_CH], accel, volt;
    for (void *ptr = iio_buffer_start(buf); ptr < iio_buffer_end(buf); ptr += iio_buffer_step(buf) )
    {
        for (int i=0; i < NO_CH; i++)
        {
            val[i] = *(uint16_t*) (ptr + i * sizeof(uint16_t));
            sum [i] += val[i];
        }
    }
    for (int i=0; i < NO_CH; i++)
    {
        avg[i] = (double)sum[i] / (double)samples_count;
        accel = avg[i] / MAX_ADC;
        volt = avg[i] * VOLTS_PER_LSB;
        printf("Avg on channel %d is\t%.3f[RAW]\t%.3F[G]\t%.3F[V]\n", i, avg[i], 2*accel, volt);
    }
  

 iio_context_destroy(ctx);
    return 0;
}
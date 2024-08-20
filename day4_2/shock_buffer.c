#include <iio.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#define URI "ip:10.76.84.254"
#define THRESH 200
const int max_adc = 4095;
const double volt_per_lsb = 2.4/4095;
const int samples_count = 1024; //pot fi 1024

typedef struct {
    uint16_t xpoz;
    uint16_t xneg;
    uint16_t ypoz;
    uint16_t yneg;
    uint16_t zpoz;
    uint16_t zneg;
}buffer_element;

struct iio_context* ctx;

int main(){
    ctx = iio_create_context_from_uri(URI);
    if (!ctx){
        perror("cannot get ctx!\n");
    }

    struct iio_device *dvc;
    
    dvc = iio_context_find_device(ctx,"ad5592r_s");
    if (!dvc){
        perror("cannot get dvc!\n");
    }

    iio_device_attr_write(dvc,"en","1");
    struct iio_channel *channels[6];
    
    for(int i=0; i<6; i++){
        channels[i] = iio_device_get_channel(dvc,i);
        if(!channels[i]){
            perror("\n channel not found \n");
        }
        iio_channel_enable(channels[i]);
    }

    bool cyclic = false;
    struct iio_buffer *buf;
    
    buf = iio_device_create_buffer(dvc, samples_count, cyclic);
    if(!buf){
        perror("cannot create buffer!");
    }

    int bytes_read = iio_buffer_refill(buf);
    uint16_t values[6];
    
    for(void *ptr = iio_buffer_start(buf); ptr<iio_buffer_end(buf); ptr += iio_buffer_step(buf)){
        uint16_t xneg = *(uint16_t *)(ptr + 0 *sizeof(uint16_t));
        uint16_t xpoz = *(uint16_t *)(ptr + 1 *sizeof(uint16_t));
        uint16_t yneg = *(uint16_t *)(ptr + 2 *sizeof(uint16_t));
        uint16_t ypoz = *(uint16_t *)(ptr + 3 *sizeof(uint16_t));
        uint16_t zneg = *(uint16_t *)(ptr + 4 *sizeof(uint16_t));
        uint16_t zpoz = *(uint16_t *)(ptr + 5 *sizeof(uint16_t));
        printf("reads: %d %d %d %d %d %d \n", xneg, xpoz, yneg, ypoz, zneg, zpoz);
    }
    
    iio_context_destroy(ctx);
    return 0;
}
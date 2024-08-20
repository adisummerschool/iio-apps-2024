#include <iio.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#define URI "ip:10.76.84.240"
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

uint32_t sum_xpoz;
uint32_t sum_xneg;
uint32_t sum_ypoz;
uint32_t sum_yneg;
uint32_t sum_zpoz;
uint32_t sum_zneg;

void buf_avg(buffer_element *buf){
    sum_xpoz += buf->xpoz;
    sum_xneg += buf->xneg;
    sum_ypoz += buf->ypoz;
    sum_yneg += buf->yneg;
    sum_zpoz += buf->zpoz;
    sum_zneg += buf->zneg;
}

int main()
{
    //signal(SIGINT, my_handler);
    struct iio_context* ctx;    
    struct iio_device *dvc;
    struct iio_buffer *buf;
    struct iio_channel *channels[6];
    bool cyclic = false;
    
    ctx = iio_create_context_from_uri(URI);
    if (!ctx)
    {
        perror("cannot get ctx!\n");
    }

    dvc = iio_context_find_device(ctx,"ad5592r_s");
    if (!dvc)
    {
        perror("cannot get dvc!\n");
    }

    iio_device_attr_write(dvc,"en","1");

    for(int i=0; i<6; i++){
        channels[i] = iio_device_get_channel(dvc,i);
        if( !channels[i])
            perror("\n channel not found \n");
        iio_channel_enable(channels[i]);
    }


    buf = iio_device_create_buffer(dvc, samples_count, cyclic);
    if(!buf){
        perror("cannot create buffer!");
    }
    
    int bytes_read = iio_buffer_refill(buf);
    uint16_t values[6];


    buffer_element buffer;
    for(void *ptr = iio_buffer_start(buf); ptr<iio_buffer_end(buf); ptr += iio_buffer_step(buf)){
        buffer.xpoz = *(uint16_t *)(ptr + 0 *sizeof(uint16_t));
        buffer.xneg = *(uint16_t *)(ptr + 1 *sizeof(uint16_t));
        buffer.ypoz = *(uint16_t *)(ptr + 2 *sizeof(uint16_t));
        buffer.yneg = *(uint16_t *)(ptr + 3 *sizeof(uint16_t));
        buffer.zpoz = *(uint16_t *)(ptr + 4 *sizeof(uint16_t));
        buffer.zneg = *(uint16_t *)(ptr + 5 *sizeof(uint16_t));

        printf("Raw Values: %d %d %d %d %d %d \n", buffer.xpoz, buffer.xneg, buffer.ypoz, buffer.yneg, buffer.zpoz, buffer.zneg);
        buf_avg(&buffer);
    }

    buffer_element avg_val;
    avg_val.xpoz = sum_xpoz / samples_count;
    avg_val.xneg = sum_xneg / samples_count;
    avg_val.ypoz = sum_ypoz / samples_count;
    avg_val.yneg = sum_yneg / samples_count;
    avg_val.zpoz = sum_zpoz / samples_count;
    avg_val.zneg = sum_zneg / samples_count;

    printf("Average Values: %d %d %d %d %d %d \n", avg_val.xpoz, avg_val.xneg, avg_val.ypoz, avg_val.yneg, avg_val.zpoz, avg_val.zneg);

    iio_context_destroy(ctx);
    return 0;
}
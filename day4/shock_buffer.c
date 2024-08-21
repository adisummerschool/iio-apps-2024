// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <iio.h>
// #include <unistd.h>


// #define URI "ip:10.76.84.244"

// const int samples_count = 4096;

// typedef struct 
// {
//     uint16_t xpoz;
//     uint16_t xneg;
//     uint16_t ypoz;
//     uint16_t yneg;
//     uint16_t zpoz;
//     uint16_t zneg;
// }buffer_element;



// int main(int argc, char *argv[])
// {
//     struct iio_context* ctx;
//     struct iio_device *dev;
//     struct iio_buffer *buf;
//     struct iio_channel *channels[6];
//     bool cyclic = false;

//     ctx = iio_create_context_from_uri(URI);
//     if (!ctx){
//         printf("\n no context\n");
//         return -1;
//     }

//     dev = iio_context_find_device(ctx, "iio:device0");
//     //struct iio_device *dev = iio_context_find_device(ctx, "ad5592r_s");

//     if (!dev){
//         printf("\n no device \n");
//         return -1;
//     }

//     iio_device_attr_write(dev, "en", "1");

//     for(int i = 0; i < 6; i++){
//         channels[i] = iio_device_get_channel(dev, i);
//         if(!channels[i]){
//             printf("\n Channel not found \n");
//             return -1;
//         }
//         iio_channel_enable(channels[i]);
//     }

//     buf = iio_device_create_buffer(dev, samples_count, cyclic);
//     if(!buf){
//         printf("\n cannot create buffer \n");
//         return -1;
//     }

//     int bytes_read = iio_buffer_refill(buf);

//     for(void *ptr = iio_buffer_start(buf); ptr < iio_buffer_end(buf); ptr += iio_buffer_step(buf)){
//         uint16_t xpoz = *(uint16_t *) (ptr + 0 * sizeof(uint16_t));
//         uint16_t xneg = *(uint16_t *) (ptr + 1 * sizeof(uint16_t));
//         uint16_t ypoz = *(uint16_t *) (ptr + 2 * sizeof(uint16_t));
//         uint16_t yneg = *(uint16_t *) (ptr + 3 * sizeof(uint16_t));
//         uint16_t zpoz = *(uint16_t *) (ptr + 4 * sizeof(uint16_t));
//         uint16_t zneg = *(uint16_t *) (ptr + 5 * sizeof(uint16_t));
//         printf(" X poz -> %d \n X neg -> %d \n Y poz -> %d \n", xpoz, xneg, ypoz);
//     }

//     iio_context_destroy(ctx);
//     return 0;
// }

#include <iio.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#define URI "ip:10.76.84.244"
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


// void my_handler(int sig){
//     signal(SIGINT, my_handler);
//     printf("Caught signal\n");
//     iio_context_destroy(ctx);
//     exit(0);
// }

uint32_t sum_xpoz;
uint32_t sum_xneg;
uint32_t sum_ypoz;
uint32_t sum_yneg;
uint32_t sum_zpoz;
uint32_t sum_zneg;

void buf_sum(buffer_element *buf){
    sum_xpoz += buf->xpoz;
    sum_xneg += buf->xneg;
    sum_ypoz += buf->ypoz;
    sum_yneg += buf->yneg;
    sum_zpoz += buf->zpoz;
    sum_zneg += buf->zneg;
}

void buf_avg(buffer_element *avg_val){
        avg_val->xpoz = sum_xpoz / samples_count;
        avg_val->xneg = sum_xneg / samples_count;
        avg_val->ypoz = sum_ypoz / samples_count;
        avg_val->yneg = sum_yneg / samples_count;
        avg_val->zpoz = sum_zpoz / samples_count;
        avg_val->zneg = sum_zneg / samples_count;
}

void init_sum(){
    sum_xpoz = 0;
    sum_xneg = 0;
    sum_ypoz = 0;
    sum_yneg = 0;
    sum_zpoz = 0;
    sum_zneg = 0;
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
    
    buffer_element buffer;
    buffer_element initial_avg_buf;
    buffer_element avg_buf;

    for(void *ptr = iio_buffer_start(buf); ptr<iio_buffer_end(buf); ptr += iio_buffer_step(buf)){
        buffer.xpoz = *(uint16_t *)(ptr + 0 *sizeof(uint16_t));
        buffer.xneg = *(uint16_t *)(ptr + 1 *sizeof(uint16_t));
        buffer.ypoz = *(uint16_t *)(ptr + 2 *sizeof(uint16_t));
        buffer.yneg = *(uint16_t *)(ptr + 3 *sizeof(uint16_t));
        buffer.zpoz = *(uint16_t *)(ptr + 4 *sizeof(uint16_t));
        buffer.zneg = *(uint16_t *)(ptr + 5 *sizeof(uint16_t));

        printf("Initial Raw Values: %d %d %d %d %d %d \n", buffer.xpoz, buffer.xneg, buffer.ypoz, buffer.yneg, buffer.zpoz, buffer.zneg);
        buf_sum(&buffer);
    }

    buf_avg(&initial_avg_buf);
    printf("\n\n| AVERAGE VALUES: %d  %d  %d  %d  %d  %d |\n", initial_avg_buf.xpoz, initial_avg_buf.xneg, initial_avg_buf.ypoz, initial_avg_buf.yneg, initial_avg_buf.zpoz, initial_avg_buf.zneg);


    int div_samp = 0;

    while(true){
        int bytes_read = iio_buffer_refill(buf);
        uint16_t values[6];

        for(void *ptr = iio_buffer_start(buf); ptr<iio_buffer_end(buf); ptr += iio_buffer_step(buf)){
            buffer.xpoz = *(uint16_t *)(ptr + 0 *sizeof(uint16_t));
            buffer.xneg = *(uint16_t *)(ptr + 1 *sizeof(uint16_t));
            buffer.ypoz = *(uint16_t *)(ptr + 2 *sizeof(uint16_t));
            buffer.yneg = *(uint16_t *)(ptr + 3 *sizeof(uint16_t));
            buffer.zpoz = *(uint16_t *)(ptr + 4 *sizeof(uint16_t));
            buffer.zneg = *(uint16_t *)(ptr + 5 *sizeof(uint16_t));

            //printf("Raw Values: %d %d %d %d %d %d \n", buffer.xpoz, buffer.xneg, buffer.ypoz, buffer.yneg, buffer.zpoz, buffer.zneg);
            buf_sum(&buffer);

            div_samp++;
            if(div_samp == 32){
                buf_avg(&avg_buf);


                initial_avg_buf = avg_buf;
                init_sum();
            }
        }

        

        


    }
    iio_context_destroy(ctx);
    return 0;
}
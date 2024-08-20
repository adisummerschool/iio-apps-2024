#include <iio.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

#define URI "ip:10.76.84.107"
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

// void my_handler(int sig){
//     signal(SIGINT, my_handler);
//     printf("Caught signal\n");
//     iio_context_destroy(ctx);
//     exit(0);
// }

int main()
{
    //signal(SIGINT, my_handler);
    ctx = iio_create_context_from_uri(URI);
    if (!ctx)
    {
        perror("cannot get ctx!\n");
    }

    struct iio_device *dvc;
    dvc = iio_context_find_device(ctx,"ad5592r_s");
    if (!dvc)
    {
        perror("cannot get dvc!\n");
    }

    iio_device_attr_write(dvc,"en","1");

    struct iio_channel *channels[6];
    for(int i=0; i<6; i++){
        channels[i] = iio_device_get_channel(dvc,i);
        if( !channels[i])
            perror("\n channel not found \n");
        iio_channel_enable(channels[i]);
    }
    printf("aici e ok");

    bool cyclic = false;
    struct iio_buffer *buf;
    buf = iio_device_create_buffer(dvc, samples_count, cyclic);
    if(!buf){
        perror("cannot create buffer!");
    }
    int bytes_read = iio_buffer_refill(buf);
    uint16_t values[6];

    printf("si aici");

    uint16_t xpoz = 0;
    uint16_t xneg = 0;
    uint16_t ypoz = 0;
    uint16_t yneg = 0;
    uint16_t zpoz = 0;
    uint16_t zneg = 0;
    int avgs_refs[6];
    int sums[6];
    for(void *ptr = iio_buffer_start(buf); ptr<iio_buffer_end(buf); ptr += iio_buffer_step(buf)){
        xpoz = *(uint16_t *)(ptr + 0 *sizeof(uint16_t));
        xneg = *(uint16_t *)(ptr + 1 *sizeof(uint16_t));
        ypoz = *(uint16_t *)(ptr + 2 *sizeof(uint16_t));
        yneg = *(uint16_t *)(ptr + 3 *sizeof(uint16_t));
        zpoz = *(uint16_t *)(ptr + 4 *sizeof(uint16_t));
        zneg = *(uint16_t *)(ptr + 5 *sizeof(uint16_t));
        printf("reads: %d %d %d %d %d %d \n", xpoz, xneg, ypoz, yneg, zpoz, zneg);
        //printf("reads: %d ;", xpoz);
        sums[0]+=xpoz;
        sums[1]+=xneg;
        sums[2]+=ypoz;
        sums[3]+=yneg;
        sums[4]+=zpoz;
        sums[5]+=zneg;
    }

    for(int i=0;i<6;i++){
            avgs_refs[i] = sums[i] / samples_count;
            printf(" avg %d: %d ",i,avgs_refs[i]);
    }
    //printf("\n NEXT: \n");

    // int avgs_det[6];
    // while(1){
    //     sums[0] = 0;
    //     sums[1] = 0;
    //     sums[2] = 0;
    //     sums[3] = 0;
    //     sums[4] = 0;
    //     sums[5] = 0;
    //     for(void *ptr = iio_buffer_start(buf); ptr<iio_buffer_end(buf); ptr += iio_buffer_step(buf)){
    //         xpoz = *(uint16_t *)(ptr + 0 *sizeof(uint16_t));
    //         xneg = *(uint16_t *)(ptr + 1 *sizeof(uint16_t));
    //         ypoz = *(uint16_t *)(ptr + 2 *sizeof(uint16_t));
    //         yneg = *(uint16_t *)(ptr + 3 *sizeof(uint16_t));
    //         zpoz = *(uint16_t *)(ptr + 4 *sizeof(uint16_t));
    //         zneg = *(uint16_t *)(ptr + 5 *sizeof(uint16_t));

    //         printf("reads: %d %d %d %d %d %d ", xpoz, xneg, ypoz, yneg, zpoz, zneg);

    //         sums[0]+=xpoz;
    //         sums[1]+=xneg;
    //         sums[2]+=ypoz;
    //         sums[3]+=yneg;
    //         sums[4]+=zpoz;
    //         sums[5]+=zneg;
    //         //printf("reads: %d ;", xpoz);

    //     }

    //     for(int i=0;i<6;i++){
    //         avgs_det[i] = sums[i] / samples_count;
    //         printf(" avg %d: %d ",i,avgs_det[i]);
    //         if((avgs_det[i]-avgs_refs[i])>=THRESH){
    //                 printf("\nSHOCK!!!!!\n");
    //                 printf("\nRecalibrate\n");
    //                 sleep(5);
    //                // iio_channel_attr_read(iio_device_find_channel(dvc,ch_name[i],false),"raw",dst[i],sizeof(dst[i]));
    //                // initials[i]=atoi(dst[i]);
    //         }

    //         avgs_refs[i]= avgs_det[i];
    //     }
    // }
   
    iio_context_destroy(ctx);
    return 0;
}
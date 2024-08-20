#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <iio.h>

#define URI 					"ip:10.76.84.253"
#define DEVICE_NAME 			"ad5592rs"
#define RAW			        	"raw"
#define EN 						"en"
#define THRESHOLD				400
#define NO_CH                   6

const int samples_count = 16;
const int MAX_ADC = 4095;
const double VOLTS_PER_LSB = 2.4 / MAX_ADC;

typedef struct 
{
    uint16_t xpoz;
    uint16_t xneg;
    uint16_t ypoz;
    uint16_t yneg;
    uint16_t zpoz;
    uint16_t zneg;

}buffer_element;

// double buf_avg(uint16_t buf)
// {
//     for (int i=0; i<samples_count; )
    
// }

int main()
{
//context
    struct iio_context *ctx = iio_create_context_from_uri(URI);
    if (!ctx)   {
        printf("%s\n", "Cannot get context");
        return -1;
    }

//device
    struct iio_device *cora = iio_context_find_device(ctx, DEVICE_NAME);
    if (!cora)
    {
        printf("Device could not be found!");
        iio_context_destroy(ctx);
        return -1;
    }
//enable
	iio_device_attr_write(cora, EN, "1");

//channels
	struct iio_channel *ch[NO_CH];
    int i;

    for (int i = 0; i < NO_CH; i++)
    {
        ch[i] = iio_device_get_channel(cora, i);
        if (!ch[i])
        {
            printf("Channel %d could not be  found!\n", i);
            iio_context_destroy(ctx);
            return -1;
        }
        iio_channel_enable(ch[i]);
    }
    
//buffer
    bool cyclic = false;
    struct iio_buffer *buf = iio_device_create_buffer(cora, samples_count, cyclic);
    if (!buf)
    {
        printf("Buffer cannot be created!\n");
        return -1;
    }

    int bytes_read = iio_buffer_refill(buf);
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

    
    iio_device_attr_write(cora, EN, "0");
    iio_context_destroy(ctx);

    return 0;
}

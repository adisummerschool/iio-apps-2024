#include <stdio.h>
#include <stdlib.h>
#include <iio.h>
#include <string.h>
#include <unistd.h>
#define URI                     "ip:10.76.84.253"
#define DEVICE_NAME 			"ad5592rs"
#define ATTR_NAME				"raw"
#define EN 						"en"
#define THRESHOLD				400
#define NO_CH                   6

const int MAX_ADC = 4095;
const double VOLTS_PER_LSB = 2.4 / MAX_ADC;

int main()
{
//context
    struct iio_context *ctx = iio_create_context_from_uri(URI);
    if (!ctx)   {
        printf("%s\n", "Cannot get context");
        return -1;
    }

//device
    struct iio_device *cora = iio_context_find_device(ctx, "ad5592rs");
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
  	char dst[10];

    for (int i = 0; i < NO_CH; i++)
    {
        ch[i] = iio_device_get_channel(cora, i);
        if (!ch[i])
        {
            printf("Channel %d could not be found!\n", i);
            iio_context_destroy(ctx);
            return -1;
        }
    }

//read G
    int val[NO_CH] = {0};
    double accel, volt;
	while(true)
    {
		for ( i = 0; i < NO_CH ; i++ )
		{
			iio_channel_attr_read(ch[i], ATTR_NAME, dst, sizeof(dst));
			val[i] = atoi(dst);
            accel = (double)val[i] / MAX_ADC;
            volt = (double)val[i] * VOLTS_PER_LSB;
            printf("Accel on channel %d is %.3f [g] {%.3f V}\n", i,  2*accel, volt);
		}
        printf("\n");
        sleep(2);
    }
    
    iio_device_attr_write(cora, EN, "0");
    iio_context_destroy(ctx);

    return 0;
}
#include <iio.h>
#include <unistd.h>


#define URI "ip:10.76.84.103"

const int max_adc = 4095;
const double volts_per_lsb = 2.4/4095;

int main(int argc, char *argv[])
{
  struct iio_context* ctx = iio_create_context_from_uri(URI);
  if (!ctx){
        printf("\n no context\n");
        return -1;
      }
    struct iio_device *dev = iio_context_find_device(ctx, "iio:device0");

    if (!dev){
        printf("\n no device \n");
        return -1;
      }

    iio_device_attr_write(dev, "en", "1");

    struct iio_channel *ch0 = iio_device_find_channel(dev, "voltage0", false);
    struct iio_channel *ch1 = iio_device_find_channel(dev, "voltage1", false);
    struct iio_channel *ch2 = iio_device_find_channel(dev, "voltage2", false);
    struct iio_channel *ch3 = iio_device_find_channel(dev, "voltage3", false);
    struct iio_channel *ch4 = iio_device_find_channel(dev, "voltage4", false);
    struct iio_channel *ch5 = iio_device_find_channel(dev, "voltage5", false);

    if( !ch0 || !ch1 || !ch2 || !ch3 || !ch4 || !ch5) {
        printf("\n channel not found \n");
        return -1;
      }

    char chn0_str[100];
    iio_channel_attr_read(ch0,"raw", chn0_str, sizeof(chn0_str));
    int chn0_raw = atoi(chn0_str);

    double chn0_accel = chn0_raw / max_adc;

    printf("\n Accel is %f [g]\n", chn0_accel);



  return 0;
}

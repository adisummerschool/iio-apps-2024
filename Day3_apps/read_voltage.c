#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iio.h>
#include <unistd.h>

#define URI "ip:10.76.84.242"

const int max_adc = 4095;
const double volts_per_lsb = 2.4 / 4095;
int main(int agrc, char *argv[])
{

    struct iio_context *ctx = iio_create_context_from_uri(URI);

    if (!ctx)
    {
        printf("\n no context\n");
    }
    struct iio_device *dev = iio_context_find_device(ctx, "iio:device0");

    if (!dev)
    {
        printf("\n no device \n");
    }

    iio_device_attr_write(dev, "en", "1");

    struct iio_channel *ch0 = iio_device_find_channel(dev, "voltage0", false);
    struct iio_channel *ch1 = iio_device_find_channel(dev, "voltage1", false);
    struct iio_channel *ch2 = iio_device_find_channel(dev, "voltage2", false);
    struct iio_channel *ch3 = iio_device_find_channel(dev, "voltage3", false);
    struct iio_channel *ch4 = iio_device_find_channel(dev, "voltage4", false);
    struct iio_channel *ch5 = iio_device_find_channel(dev, "voltage5", false);
    if (!ch0 || !ch1 || !ch2 || !ch3 || !ch4 || !ch5)
    {
        printf("\n Channel not found \n");
        return -1;
    }

    char chn0_str[100];
    char chn1_str[100];
    char chn2_str[100];
    char chn3_str[100];
    char chn4_str[100];
    char chn5_str[100];
    while (true)
    {
        // ch0
        iio_channel_attr_read(ch0, "raw", chn0_str, sizeof(chn0_str));
        int chn0_raw = atoi(chn0_str);
        double chn0_voltage = chn0_raw * volts_per_lsb;
        double chn0_accel = (double)chn0_raw / max_adc;
        printf("\n VOLTAGE IS: %3f [V] ACCEL IS %3f [g] --CH0\n", chn0_voltage, chn0_accel);
        // ch1
        iio_channel_attr_read(ch1, "raw", chn1_str, sizeof(chn1_str));
        int chn1_raw = atoi(chn1_str);
        double chn1_voltage = chn1_raw * volts_per_lsb;
        double chn1_accel = (double)chn1_raw / max_adc;
        printf("\n VOLTAGE IS: %3f [V] ACCEL IS %3f [g] --CH1\n", chn1_voltage,chn1_accel);
        // ch2
        iio_channel_attr_read(ch2, "raw", chn2_str, sizeof(chn2_str));
        int chn2_raw = atoi(chn2_str);
        double chn2_voltage = chn2_raw * volts_per_lsb;
        double chn2_accel = (double)chn2_raw / max_adc;
        printf("\n VOLTAGE IS: %3f [V] ACCEL IS %3f [g] --CH2\n", chn2_voltage,chn2_accel);
        // ch3
        iio_channel_attr_read(ch3, "raw", chn3_str, sizeof(chn3_str));
        int chn3_raw = atoi(chn3_str);
        double chn3_voltage = chn3_raw * volts_per_lsb;
        double chn3_accel = (double)chn3_raw / max_adc;
        printf("\n VOLTAGE IS: %3f [V] ACCEL IS %3f [g] --CH3\n", chn3_voltage,chn3_accel);
        // ch4
        iio_channel_attr_read(ch4, "raw", chn4_str, sizeof(chn4_str));
        int chn4_raw = atoi(chn4_str);
        double chn4_voltage = chn4_raw * volts_per_lsb;
        double chn4_accel = (double)chn4_raw / max_adc;
        printf("\n VOLTAGE IS: %3f [V] ACCEL IS %3f [g] --CH4\n", chn4_voltage,chn4_accel);
        // ch5
        iio_channel_attr_read(ch5, "raw", chn5_str, sizeof(chn5_str));
        int chn5_raw = atoi(chn5_str);
        double chn5_voltage = chn5_raw * volts_per_lsb;
        double chn5_accel = (double)chn5_raw / max_adc;
        printf("\n VOLTAGE IS: %3f [V] ACCEL IS %3f [g] --CH5\n", chn5_voltage,chn5_accel);

        sleep(1);
    }
}
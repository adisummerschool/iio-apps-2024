#include <iio.h>
#include <stdio.h>

#define URI "ip:10.76.84.243"

int main()
{
    unsigned int major;
    unsigned int minor;
    char git_tag[8];

    iio_library_get_version(&major, &minor, git_tag);
    printf("Library version: %d, %d, %s\n", major, minor, git_tag);

    struct  iio_context * ctx;
    ctx = iio_create_context_from_uri(URI);

    if (!ctx) {
        printf("%s","cannot get ctx");
    }

    
    struct iio_device * dev; // iio_device structure
    struct iio_channel * chn; // iio_channel structure

    dev = iio_context_find_device(ctx, "ad5592r_s");
    chn = iio_device_find_channel(dev, "voltage0", false);

    // enable
    iio_device_attr_write(dev, "en", "1");

    printf("Numere yay!\n");

    for(int i = 0; i < 10; i++)
    {
        char strw[20];
        sprintf(strw, "%d", i + 1);
        iio_channel_attr_write(chn, "raw", strw);

        char strr[20];
        iio_channel_attr_read(chn, "raw", strr, 4);
        printf("%s\n", strr);
        sleep(1);
    }
}
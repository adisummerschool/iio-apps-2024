#include "lib.h"
#include <iio.h>
#define CHANNEL_NAME "voltage0"
#define URI "ip:10.76.84.253"
#define DEVICE_NAME "ad5592rs"
#define ATTR_NAME "raw"
#define EN "en"
int main()
{
    unsigned int major;
    unsigned int minor;
    int attr;
    char git_tag[8];

    iio_library_get_version(&major, &minor, git_tag);
    printf("Library get version: %d, %d, %s\n", major, minor, git_tag);

    struct iio_context *ctx = iio_create_context_from_uri(URI);

    if (!ctx)   {
        printf("%s\n", "Cannot get context");
        return -1;
    }
    else
    {
        printf ("%s\n", "Context established\n");
    }

    //int iio_context_get_attrs_count(const struct iio_context *ctx);
    //int iio_context_get_attr(const struct iio_context *ctx, unsigned int index, const char **name, const char **value)
    
    int no_attr = iio_context_get_attrs_count(ctx);
    printf("No. of attributes: %d\n", no_attr);

    const char *value, *name;

    for (int i=0; i<no_attr; i++)
    {
        iio_context_get_attr(ctx, i, &name, &value);
        printf("Attribute %s: %s\n", name, value);
    }

    struct iio_device *cora = iio_context_find_device(ctx, "ad5592rs");
    if (!cora)
    {
        printf("Device could not be found!");
        iio_context_destroy(ctx);
        return -1;
    }


    int no_ch = iio_device_get_channels_count(cora);
    printf("\nNumber of channels is: %d\n", no_ch);
    struct iio_channel *voltage0;

    for (int i = 0; i < no_ch; i++)
    {
        voltage0 = iio_device_get_channel(cora, i);
        if (!voltage0)
        {
            printf("Channel %d could not be found!\n", i);
            iio_context_destroy(ctx);
            return -1;
        }
        printf("Name of ch. %d is : %s\n", i, iio_channel_get_id(voltage0));
    }

    no_attr = iio_device_get_attrs_count(cora);
    printf("\n");
    for (int i = 0; i < no_attr; i++)
    {
        printf("Device attribute %d is %s\n", i, iio_device_get_attr(cora, i));
    }

    iio_device_attr_write(cora, EN, "1");

    char dst[10];
    char attr_value[10];
    iio_device_attr_read(cora, EN, attr_value, sizeof(attr_value));
    printf("\nEnable is %s\n", attr_value);


    voltage0 = iio_device_get_channel(cora, 1); //channel 1 is voltage0

    name = iio_channel_get_attr(voltage0, 0);
    printf("Channel 1 has \"%s\" attribute\n", name);

    for (int i=0; i<10; i++)
    {
        sprintf(attr_value, "%d", i);
        iio_channel_attr_write(voltage0, ATTR_NAME, attr_value);

        iio_channel_attr_read(voltage0, ATTR_NAME, dst, sizeof(dst));
        printf("\n%s\n", dst);
        sleep(1);
    }
    
    iio_device_attr_write(cora, EN, "0");
    iio_context_destroy(ctx);

    return 0;
}
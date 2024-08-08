#include <iio.h>

#define URI "ip:10.76.84.107"

int main()
{
    unsigned int major;
    unsigned int minor;
    char git_tag[8];

    iio_library_get_version(&major, &minor, git_tag);

    printf("Library info: %d, %d, %s\n\n", major, minor, git_tag);

    struct iio_context* ctx;
    ctx = iio_create_context_from_uri(URI);

    if (!ctx)
    {
        perror("cannot get ctx!\n");
    }

    int n = iio_context_get_attrs_count(ctx);
    printf("no. of attr of context: %d\n",n);
    // for (int i = 0; i < n; ++i)
    // {
    //     char* name;
    //     char* value;

    //     iio_context_get_attr(ctx, i, &name, &value);
    //     printf("%d) %s, %s\n", i, name, value);
    // }

    char namedev[]="ad5592r_s";
    struct iio_device *dvc;


    dvc = iio_context_find_device(ctx,namedev);
    if (!dvc)
    {
        perror("cannot get dvc!\n");
    }

    int no_channels = iio_device_get_channels_count(dvc);
    printf("no. channels of device: %d\n",no_channels);

    iio_device_attr_write(dvc,"en","1");

     struct iio_channel *channel0;
     channel0= iio_device_find_channel(dvc,"voltage0",false);

    // char *raw_chann0 = iio_channel_find_attr(channel0,"raw");
    char dst[10][10];
    char c[10];
    for (int i = 0; i < 10; i++) {
        sprintf(c, "%d", i+1);  // Convertim i la șir de caractere
        iio_channel_attr_write(channel0, "raw", c);
        iio_channel_attr_read(channel0, "raw", dst[i], 10);
        printf("value: %s\n", dst[i]);
        sleep(1);
    }
    iio_context_destroy(ctx);
    return 0;
}
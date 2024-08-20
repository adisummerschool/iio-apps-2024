#include <iio.h>
#include <stdio.h>

#define URI "ip:10.76.84.243"

void sleep_ms(int milliseconds)
{
    // Convert milliseconds to microseconds
    usleep(milliseconds * 1000);
}

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
//  chn = iio_device_find_channel(dev, "voltage0", false);

    // enable
    iio_device_attr_write(dev, "en", "1");

    printf("Calibrate your placuta!\n");

    int thr = 400;
    //for(int i = 0; i < 10; i++)
    int a[6];

    for(int i = 0; i < 6; i++)
    {
        chn = iio_device_get_channel(dev, i);

        char strr[100];
        iio_channel_attr_read(chn, "raw", strr, 100);
        a[i] = atoi(strr);
    //  printf("\nvoltage%d : %s", i, strr);
    }
    
    while(true)
    {
    //  char strw[20];                                                                               
    //  sprintf(strw, "%d", i + 1);
    //  iio_channel_attr_write(chn, "raw", strw);

        printf("\n");
        int b[6];
        for(int i = 0; i < 6; i++)
        {
            chn = iio_device_get_channel(dev, i);

            char strr[100];
            iio_channel_attr_read(chn, "raw", strr, 100);
            b[i] = atoi(strr);
        //  printf("\nvoltage%d : %s", i, strr);
        }

        printf("\nX Axis: ");
        int x1 = abs(a[0] - b[0]);
        int x2 = abs(a[1] - b[1]);
        int x = x1 + x2;
        if(x > thr) printf("Ba Shock! %.2fG", (float)x / 2048);
        else printf("No shock!");
        
        printf("\nY Axis: ");
        int y1 = abs(a[2] - b[2]);
        int y2 = abs(a[3] - b[3]);
        int y = y1 + y2;
        if(y > thr) printf("Ba Shock! %.2fG", (float)y / 2048);
        else printf("No shock!");

        printf("\nZ Axis: ");
        int z1 = abs(a[4] - b[4]);
        int z2 = abs(a[5] - b[5]);
        int z = z1 + z2;
        if(z > thr) printf("Ba Shock! %.2fG", (float)z / 2048);
        else printf("No shock!");

        a[0] = b[0];
        a[1] = b[1];
        a[2] = b[2];
        a[3] = b[3];
        a[4] = b[4];
        a[5] = b[5];

        sleep_ms(1000);
    }
}
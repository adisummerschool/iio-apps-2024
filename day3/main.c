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
//  chn = iio_device_find_channel(dev, "voltage0", false);

    // enable
    iio_device_attr_write(dev, "en", "1");

    printf("Calibrate your placuta!\n");

    int thr = 50;
    //for(int i = 0; i < 10; i++)
    while(true)
    {
    //  char strw[20];                                                                               
    //  sprintf(strw, "%d", i + 1);
    //  iio_channel_attr_write(chn, "raw", strw);

        printf("\n");
        int a[6];
        for(int i = 0; i < 6; i++)
        {
            chn = iio_device_get_channel(dev, i);

            char strr[100];
            iio_channel_attr_read(chn, "raw", strr, 100);
            a[i] = atoi(strr);
        //  printf("\nvoltage%d : %s", i, strr);
        }

        printf("\nX Axis: ");
        int x = a[0] - a[1];
        if(abs(x) < thr) printf("Ba e OK!");
        else if(x > 0) printf("turn left!");
             else printf("turn right!");
        
        printf("\nY Axis: ");
        int y = a[2] - a[3];
        if(abs(y) < thr) printf("Ba e OK!");
        else if(y > 0) printf("turn left!");
             else printf("turn right!");
 

        printf("\nZ Axis: ");
        int z = a[4] - a[5];
        if(abs(z) < thr) printf("Ba e OK!");
    //  else if(z > 0) printf("turn left! + %d + %d\n", a[4], a[5]);
        else if(z > 0) printf("turn left!");
             else printf("turn right!");
             
        sleep(1);
    }
}
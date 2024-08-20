#include <stdio.h>
#include <unistd.h>
#include <iio.h>
#define URI 					"ip:10.76.84.253"
#define DEVICE_NAME 			"ad5592rs"
#define ATTR_NAME				"raw"
#define EN 						"en"
#define THRESHOLD				400

int main()
{
    unsigned int major;
    unsigned int minor;
    char git_tag[8];
    iio_library_get_version(&major, &minor, git_tag);
    printf("Library get version: %d, %d, %s\n", major, minor, git_tag);

//context
    struct iio_context *ctx = iio_create_context_from_uri(URI);
    if (!ctx)   {
        printf("%s\n", "Cannot get context");
        return -1;
    }
    else
    {
        printf ("%s\n", "Context established\n");
	}
    
//attributes
    int no_attr = iio_context_get_attrs_count(ctx);
    printf("No. of attributes: %d\n", no_attr);
    const char *value, *name;
    for (int i=0; i<no_attr; i++)
    {
        iio_context_get_attr(ctx, i, &name, &value);
        printf("Attribute %s: %s\n", name, value);
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
    char attr_value[10];
    iio_device_attr_read(cora, EN, attr_value, sizeof(attr_value));
    printf("\nEnable is %s\n", attr_value);
//channels
    int no_ch = iio_device_get_channels_count(cora);
    printf("\nNumber of channels is: %d\n", no_ch);

    struct iio_channel *ch0, *ch1, *ch2, *ch3, *ch4, *ch5;
	struct iio_channel *ch[6];

    for (int i = 0; i < no_ch; i++)
    {
        ch[i] = iio_device_get_channel(cora, i);
        if (!ch[i])
        {
            printf("Channel %d could not be found!\n", i);
            iio_context_destroy(ctx);
            return -1;
        }
        printf("Name of ch. %d is : %s\n", i, iio_channel_get_id(ch[i]));
    }
    

//shock detect
	int i, init_val[6], curr_val[6];
  	char dst[10];
	for ( i = 0; i < 6 ; i++ )
	{
		iio_channel_attr_read(ch[i], ATTR_NAME, dst, sizeof(dst));
		init_val[i] = atoi(dst);
	}
   
	while(true)
    {
		for ( i = 0; i < 6 ; i++ )
		{
			iio_channel_attr_read(ch[i], ATTR_NAME, dst, sizeof(dst));
			curr_val[i] = atoi(dst);
			if ((curr_val[i] - init_val[i]) > THRESHOLD)
			{
				printf("SHOCK DETECTED %.2f\n", abs(curr_val[i] - init_val[i])/2000.0);
				sleep(1);
			}
			else
				printf("OK %d\n", curr_val[i]);
			init_val[i] = curr_val[i];
		}
    }
    
    iio_device_attr_write(cora, EN, "0");
    iio_context_destroy(ctx);

    return 0;
}
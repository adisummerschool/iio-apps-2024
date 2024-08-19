#include <stdio.h>
#include <unistd.h>
#include <iio.h>
#define CHANNEL_NAME 			"volt0"
#define URI 					"ip:10.76.84.253"
#define DEVICE_NAME 			"ad5592rs"
#define ATTR_NAME				"raw"
#define EN 						"en"
#define THRESHOLD				1000

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
    struct iio_channel *volt0, *volt1, *volt2, *volt3, *volt4, *volt5;

    for (int i = 0; i < no_ch; i++)
    {
        volt0 = iio_device_get_channel(cora, i);
        if (!volt0)
        {
            printf("Channel %d could not be found!\n", i);
            iio_context_destroy(ctx);
            return -1;
        }
        printf("Name of ch. %d is : %s\n", i, iio_channel_get_id(volt0));
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


    volt0 = iio_device_get_channel(cora, 0); //channel 1 is volt0
	volt1 = iio_device_get_channel(cora, 1);
	volt2 = iio_device_get_channel(cora, 2);
	volt3 = iio_device_get_channel(cora, 3);
	volt4 = iio_device_get_channel(cora, 4);
	volt5 = iio_device_get_channel(cora, 5);
    
	int x_poz, x_neg, y_poz, y_neg, z_poz, z_neg;
	int x_poz_prev, x_neg_prev, y_poz_prev, y_neg_prev, z_poz_prev, z_neg_prev;

	iio_channel_attr_read(volt0, ATTR_NAME, dst, sizeof(dst));
	x_poz_prev=atoi(dst);

	iio_channel_attr_read(volt1, ATTR_NAME, dst, sizeof(dst));
	x_neg_prev=atoi(dst);

	iio_channel_attr_read(volt2, ATTR_NAME, dst, sizeof(dst));
	y_poz_prev=atoi(dst);

	iio_channel_attr_read(volt3, ATTR_NAME, dst, sizeof(dst));
	y_neg_prev=atoi(dst);

	iio_channel_attr_read(volt4, ATTR_NAME, dst, sizeof(dst));
	z_poz_prev=atoi(dst);

	iio_channel_attr_read(volt5, ATTR_NAME, dst, sizeof(dst));
	z_neg_prev=atoi(dst);

    //for (int i=0; i<10; i++)
	while(true)
    {
        iio_channel_attr_read(volt0, ATTR_NAME, dst, sizeof(dst));
		x_poz = atoi(dst);

		iio_channel_attr_read(volt1, ATTR_NAME, dst, sizeof(dst));
		x_neg = atoi(dst);
       
		iio_channel_attr_read(volt2, ATTR_NAME, dst, sizeof(dst));
		y_poz = atoi(dst);

		iio_channel_attr_read(volt3, ATTR_NAME, dst, sizeof(dst));
		y_neg = atoi(dst);

		iio_channel_attr_read(volt4, ATTR_NAME, dst, sizeof(dst));
		z_poz = atoi(dst);

		iio_channel_attr_read(volt5, ATTR_NAME, dst, sizeof(dst));
		z_neg = atoi(dst);

	//x
		if(abs(x_poz_prev - x_poz) > THRESHOLD)
		{
			printf("x: shock %.2f\n", abs(x_poz_prev - x_poz)/2000.0);
			sleep(1);
			// y_poz_prev = y_poz;
			// y_neg_prev = y_neg;
			// z_poz_prev = z_poz;
			// z_neg_prev = z_neg;
		}
		else if(abs(x_neg_prev - x_neg) > THRESHOLD)
			{
				printf("x: shock %.2f\n", abs(x_neg_prev - x_neg)/2000.0);
				sleep(1);
				// y_poz_prev = y_poz;
				// y_neg_prev = y_neg;
				// z_poz_prev = z_poz;
				// z_neg_prev = z_neg;
			}
			else
			{
				printf("x: OK\n");
				// x_poz_prev = x_poz;
				// x_neg_prev = x_neg;
			}
	//y
		if(abs(y_poz_prev - y_poz) > THRESHOLD)
		{
			printf("y: shock %.2f\n", abs(y_poz_prev - y_poz)/2000.0);
			sleep(1);
			// z_poz_prev = z_poz;
			// z_neg_prev = z_neg;
			// x_poz_prev = x_poz;
			// x_neg_prev = x_neg;
		}
		else if(abs(y_neg_prev - y_neg) > THRESHOLD)
			{
				printf("y: shock %.2f\n", abs(y_neg_prev - y_neg)/2000.0);
				sleep(1);
				// z_poz_prev = z_poz;
				// z_neg_prev = z_neg;
				// x_poz_prev = x_poz;
				// x_neg_prev = x_neg;
			}
			else
			{
				printf("y: OK\n");
				// y_poz_prev = y_poz;
				// y_neg_prev = y_neg;
			}
	//z
		if(abs(z_poz_prev - z_poz) > THRESHOLD)
		{
			printf("z: shock %.2f\n", abs(z_poz_prev - z_poz)/2000.0);
			sleep(1);
			// y_poz_prev = y_poz;
			// y_neg_prev = y_neg;
			// x_poz_prev = x_poz;
			// x_neg_prev = x_neg;
		}
		else if(abs(z_neg_prev - z_neg) > THRESHOLD)
			{
				printf("z: shock %.2f\n", abs(z_neg_prev - z_neg)/2000.0);
				sleep(1);
				// y_poz_prev = y_poz;
				// y_neg_prev = y_neg;
				// x_poz_prev = x_poz;
				// x_neg_prev = x_neg;
			}
			else
			{
				printf("z: OK\n");
				// z_poz_prev = z_poz;
				// z_neg_prev = z_neg;
			}
			y_poz_prev = y_poz;
				y_neg_prev = y_neg;
				x_poz_prev = x_poz;
				x_neg_prev = x_neg;
				z_poz_prev = z_poz;
				z_neg_prev = z_neg;

	// //x
	// 	if(x_poz > THRESHOLD)
	// 		printf("x: turn left");
	// 	else if(x_neg > THRESHOLD)
	// 			printf("x: turn right");
	// 		else
	// 			printf("x: OK");
	// //y
	// 	if(y_poz > THRESHOLD)
	// 		printf("	y: turn right");
	// 	else if(y_neg > THRESHOLD)
	// 			printf("	y: turn left");
	// 		else
	// 			printf("	y: OK");
	// //z
	// 	if(z_poz > THRESHOLD)
	// 		printf("	z: turn right\n");
	// 	else if(z_neg > THRESHOLD)
	// 			printf("	z: turn left\n");
	// 		else
	// 			printf("	z: OK\n");

        
    }
    
    iio_device_attr_write(cora, EN, "0");
    iio_context_destroy(ctx);

    return 0;
}
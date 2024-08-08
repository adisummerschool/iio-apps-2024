#include <stdio.h>
#include <stdlib.h>
#include <iio.h>
#include <string.h>
#include <unistd.h>
#define URI "ip:10.76.84.104"


int compName(const char* i, char* j, int num) {
	for (int c = 0; c < num; c++) {
		if (i[c] != j[c]) {
			return 0;
		}
	}

	return 1;
}

int main() {
	int xadc_index = 0;
	int leds_index = 0;
	int ch_index = 0;
	
	struct iio_context *ctx;
	ctx = iio_create_context_from_uri(URI);
	
	if(!ctx) {
		printf("%s\n", "Cannot get ctx");
	}
	
	
	int ctx_attr = iio_context_get_devices_count(ctx);
	printf("Count of devices: %d\n", ctx_attr);

	struct iio_device *device;
	struct iio_device *deviceLeds;
	struct iio_channel *ch;
	struct iio_channel *led0;
	struct iio_channel *led1;
	struct iio_channel *led2;
	
	for (int i = 0; i < ctx_attr; i++) {
		device = iio_context_get_device(ctx, i);
		printf("Device %d has name: %s\n", i+1, iio_device_get_name(device));
		if (iio_device_get_name(device) != NULL) {
			if (compName(iio_device_get_name(device), "xadc", 4)) {
				xadc_index = i;
			}
			if (compName(iio_device_get_name(device), "axi-pwm-led", 11)) {
				leds_index = i;
			}
		}
	}
	
	printf("XADC is device number %d\n", xadc_index);
	printf("The leds are device number %d\n", leds_index);
	printf("\n");
	
	for (int i = 0; i < ctx_attr; i++) {
		device = iio_context_get_device(ctx, i);
		int c = iio_device_get_channels_count(device);
		printf("Device %d has %d channels:\n", i+1, c);
		
		for (int j = 0; j < c; j++) {
			ch = iio_device_get_channel(device, j);
			if (i == xadc_index) {
				if (iio_channel_get_id(ch) != NULL) {
					if (compName(iio_channel_get_id(ch), "temp0", 5)) {
						ch_index = j;
					} 
				}
			}
			
			if (iio_channel_is_output(ch)) 
				printf("   Channel %s is output with attributes:\n", iio_channel_get_id(ch));
			else 
				printf("   Channel %s is input with attributes:\n", iio_channel_get_id(ch));
				
			for (int k = 0; k < iio_channel_get_attrs_count(ch); k++) {
				printf("      %s\n", iio_channel_get_attr(ch, k));
			
			}
		}
	}
		
	printf("temp0 is channel: %d\n", ch_index);
	device = iio_context_get_device(ctx, xadc_index);
	deviceLeds = iio_context_get_device(ctx, leds_index);
	//getting the temp0 channel
	ch = iio_device_get_channel(device, ch_index);
	//getting random led
	led0 = iio_device_get_channel(deviceLeds, 2);
	led1 = iio_device_get_channel(deviceLeds, 4);
	led2 = iio_device_get_channel(deviceLeds, 0);
	
	printf("\nWriting values from XADC temp0 channel to one of the axi-pwm-leds...\n");
	
	
	while(1) {
		char value[10];
		char scale[100];
		char offset[10];	
		printf("Reading the value from the XADC...\n");
		iio_channel_attr_read(ch, "raw", value, 8);
		iio_channel_attr_read(ch, "scale", scale, 20);
		iio_channel_attr_read(ch, "offset", offset, 8);
		
		int v1 = atoi(value);
		float v2 = atof(scale);
		int v3 = atoi(offset);
				
		printf("Raw val = %d\n", v1);
		printf("Scale val = %f\n", v2);
		printf("Offset val = %d\n", v3);
		
		int final = (v1+v3)*v2;
		
		printf("Temp value: %d\n", final);
		
		int v = ((final-42000)*255)/8000;
		
		int b = v;
		int g = b + 31;
		int r = 255 - g;
		
		char valr[10];
		sprintf(valr, "%d", r);
		printf("Writing the value %d to the red led\n", r);
		
		char valg[10];
		sprintf(valg, "%d", g);
		printf("Writing the value %d to the green led\n", g);
		
		char valb[10];
		sprintf(valb, "%d", b);
		printf("Writing the value %d to the blue led\n", b);
		
		iio_channel_attr_write(led0, "raw", valr);
		iio_channel_attr_write(led1, "raw", valg);
		iio_channel_attr_write(led2, "raw", valb);
		sleep(1);
	}
	
	iio_context_destroy(ctx);
	return 0;
}

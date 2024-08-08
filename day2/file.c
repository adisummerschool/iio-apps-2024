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
	int adc_index = 0;
	int ch_index = 0;
	
	struct iio_context *ctx;
	ctx = iio_create_context_from_uri(URI);
	
	if(!ctx) {
		printf("%s\n", "Cannot get ctx");
	}
	
	
	int ctx_attr = iio_context_get_devices_count(ctx);
	printf("Count of devices: %d\n", ctx_attr);

	struct iio_device *device;
	struct iio_channel *ch;
	
	for (int i = 0; i < ctx_attr; i++) {
		device = iio_context_get_device(ctx, i);
		printf("Device %d has name: %s\n", i+1, iio_device_get_name(device));
		if (iio_device_get_name(device) != NULL) {
			if (compName(iio_device_get_name(device), "ad5592r_s", 9)) {
				adc_index = i;
			}
		}
	}
	
	printf("AD5592R_S is device number %d\n", adc_index);
	printf("\n");
	
	for (int i = 0; i < ctx_attr; i++) {
		device = iio_context_get_device(ctx, i);
		int c = iio_device_get_channels_count(device);
		printf("Device %d has %d channels:\n", i+1, c);
		
		for (int j = 0; j < c; j++) {
			ch = iio_device_get_channel(device, j);
			if (i == 0) {
				if (iio_channel_get_id(ch) != NULL) {
					if (compName(iio_channel_get_id(ch), "voltage0", 8)) {
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
	
	printf("\nThe voltage0 channel of AD5592R_S is number %d\n", ch_index);	
	
	device = iio_context_get_device(ctx, adc_index);
	ch = iio_device_get_channel(device, ch_index);
	printf("Enabling the ADC...\n");
	
	iio_device_attr_write(device, "en", "1");
	
	printf("\nWriting raw values from 1 to 10 on the voltage0 channel of the AD5592R_S...\n");
	
	
	for(int v = 1; v <= 10; v++) {
		printf("Writing %d...\n", v);
		char val[10];
		sprintf(val, "%d", v);
		iio_channel_attr_write(ch, "raw", val);
		printf("Reading the value from AD5592R_S:");
		char value[10];
		iio_channel_attr_read(ch, "raw", value, 4);
		printf(" %s\n", value);
		sleep(1);
	}
	
	iio_context_destroy(ctx);
	return 0;
}

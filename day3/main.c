#include <iio.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#define URI "ip:10.76.84.104"
#define EN "en"
#define CHAN_ATTR "raw"
#define THRESHOLD 25
#define SHOCK 500
#define G 2000


int main() {
 	struct iio_context *ctx;
        ctx = iio_create_context_from_uri(URI);

        if(!ctx) {
                printf("%s\n", "Cannot get ctx");
        }


    	struct iio_device* dev = iio_context_find_device(ctx, "ad5592r_s");
    	if(!dev) {
    		printf("%s\n", "Cannot get ADC device");    		
    	}
    	
    	//enabling the ADC
    	iio_device_attr_write(dev, EN, "1");
    	
    	//running averages 
    	long sum[6] = {0, 0, 0, 0, 0, 0};
    	int numReads = 0;
    	
    	
    	//continuously reading the channel values
    	while(true) {
    		printf("Calibrating...\n\n");
    	
	    	char value0[10] = "0";
	    	char value1[10] = "0";
	    	char value2[10] = "0";
	    	char value3[10] = "0";
	    	char value4[10] = "0";
	    	char value5[10] = "0";
	    	
	    	//x+
	    	struct iio_channel* voltage0 = iio_device_find_channel(dev, "voltage0", NULL);
	    	iio_channel_attr_read(voltage0, CHAN_ATTR, value0, 10);   	
	    	//x-
	    	struct iio_channel* voltage1 = iio_device_find_channel(dev, "voltage1", NULL);
	    	iio_channel_attr_read(voltage1, CHAN_ATTR, value1, 10);
	    	//y+
	    	struct iio_channel* voltage2 = iio_device_find_channel(dev, "voltage2", NULL);
	    	iio_channel_attr_read(voltage2, CHAN_ATTR, value2, 10);
	    	//y-
	    	struct iio_channel* voltage3 = iio_device_find_channel(dev, "voltage3", NULL);
	    	iio_channel_attr_read(voltage3, CHAN_ATTR, value3, 10);
	    	//z+
	    	struct iio_channel* voltage4 = iio_device_find_channel(dev, "voltage4", NULL);
	    	iio_channel_attr_read(voltage4, CHAN_ATTR, value4, 10);
	    	//z-
	    	struct iio_channel* voltage5 = iio_device_find_channel(dev, "voltage5", NULL);
	    	iio_channel_attr_read(voltage5, CHAN_ATTR, value5, 10);
	    	
	    	printf("Voltage0 channel raw value = %s\n", value0);
	    	printf("Voltage1 channel raw value = %s\n", value1);
	    	printf("Voltage2 channel raw value = %s\n", value2);
	    	printf("Voltage3 channel raw value = %s\n", value3);
	    	printf("Voltage4 channel raw value = %s\n", value4);
	    	printf("Voltage5 channel raw value = %s\n", value5);
	    	
	    	printf("\n\n");
	    	
	    	int val0 = atoi(value0);
	    	int val1 = atoi(value1);
	    	int val2 = atoi(value2);
	    	int val3 = atoi(value3);
	    	int val4 = atoi(value4);
	    	int val5 = atoi(value5);
	    	
	    	/*
	    	if (val0 < THRESHOLD && val1 < THRESHOLD) {
	    		printf("X axis OK\n");
	    	}
	    	if (val0 > THRESHOLD) {
			printf("Turn right X potentiometer\n");
		}
	    	if (val1 > THRESHOLD) {
	    		printf("Turn left X potentiometer\n");
	    	}
	    	
	    	if (val2 < THRESHOLD && val3 < THRESHOLD) {
	    		printf("Y axis OK\n");
	    	}
	    	if (val2 > THRESHOLD) {
			printf("Turn right Y potentiometer\n");
		}
	    	if (val3 > THRESHOLD) {
	    		printf("Turn left Y potentiometer\n");
	    	}
	    	
	    	if (val4 < THRESHOLD && val5 < THRESHOLD) {
	    		printf("Z axis OK\n");
	    	}
	    	if (val4 > THRESHOLD) {
			printf("Turn right Z potentiometer\n");
		}
	    	if (val5 > THRESHOLD) {
	    		printf("Turn left Z potentiometer\n");
	    	}
	    	*/
	    	
	    	sum[0] += val0;
	    	sum[1] += val1;
	    	sum[2] += val2;
	    	sum[3] += val3;
	    	sum[4] += val4;
	    	sum[5] += val5;
	    	numReads++;
	    	
	    	int dif;
	    	
	    	if ((val0 > sum[0]/numReads + SHOCK)) {
	    		dif = val0 - (sum[0]/numReads + SHOCK);
	    		printf("Shock on X+ axis = %.2fg\n", (float)dif/G);
	    		sum[0] -= val0;
	    		sum[0] += sum[0]/numReads;
	    	}
	    	else if ((val0 < sum[0]/numReads - SHOCK)) {
	    		dif = (sum[0]/numReads - SHOCK) - val0;
	    		printf("Shock on X+ axis = %.2fg\n", (float)dif/G);
	    		sum[0] -= val0;
	    		sum[0] += sum[0]/numReads;
	    	}
	    	else if ((val1 > sum[1]/numReads + SHOCK)) {
	    		dif = val1 - (sum[1]/numReads + SHOCK);
	    		printf("Shock on X- axis = %.2fg\n", (float)dif/G);
	    		sum[1] -= val1;
	    		sum[1] += sum[1]/numReads;
	    	}
	    	else if ((val1 < sum[1]/numReads - SHOCK)) {
	    		dif = (sum[1]/numReads - SHOCK) - val1;
	    		printf("Shock on X- axis = %.2fg\n", (float)dif/G);
	    		sum[1] -= val1;
	    		sum[1] += sum[1]/numReads;
	    	}
	    	else {
	    		printf("X: no shock\n");
	    	}
	    	
	    	
	    	if ((val2 > sum[2]/numReads + SHOCK)) {
	    		dif = val2 - (sum[2]/numReads + SHOCK);
	    		printf("Shock on Y+ axis = %.2fg\n", (float)dif/G);
	    		sum[2] -= val2;
	    		sum[2] += sum[2]/numReads;
	    	}
		else if ((val2 < sum[2]/numReads - SHOCK)) {
			dif = (sum[2]/numReads - SHOCK) - val2;
	    		printf("Shock on Y+ axis = %.2fg\n", (float)dif/G);
	    		sum[2] -= val2;
	    		sum[2] += sum[2]/numReads;
	    	}
	    	else if ((val3 > sum[3]/numReads + SHOCK)) {
	    		dif = val3 - (sum[3]/numReads + SHOCK);
	    		printf("Shock on Y- axis = %.2fg\n", (float)dif/G);
	    		sum[3] -= val3;
	    		sum[3] += sum[3]/numReads;
	    	}
	    	else if ((val3 < sum[3]/numReads - SHOCK)) {
	    		dif = (sum[3]/numReads - SHOCK) - val3;
	    		printf("Shock on Y- axis = %.2fg\n", (float)dif/G);
	    		sum[3] -= val3;
	    		sum[3] += sum[3]/numReads;
	    	}
	    	else {
	    		printf("Y: no shock\n");
	    	}
	    	
	
	    	if ((val4 > sum[4]/numReads + SHOCK) ) {
	    		dif = val4 - (sum[4]/numReads + SHOCK);
	    		printf("Shock on Z+ axis = %.2fg\n", (float)dif/G);
	    		sum[4] -= val4;
	    		sum[4] += sum[4]/numReads;
	    	}
	    	else if ((val4 < sum[4]/numReads - SHOCK)) {
	    		dif = (sum[4]/numReads - SHOCK) - val4;
	    		printf("Shock on Z- axis = %.2fg\n", (float)dif/G);
	    		sum[4] -= val4;
	    		sum[4] += sum[4]/numReads;
	    	}
	    	else if ((val5 > sum[5]/numReads + SHOCK)) {
	    		dif = val5 - (sum[5]/numReads + SHOCK);
	    		printf("Shock on Z- axis = %.2fg\n", (float)dif/G);
	    		sum[5] -= val5;
	    		sum[5] += sum[5]/numReads;
	    	}
	    	else if ((val5 < sum[5]/numReads - SHOCK)) {
	    		dif = (sum[5]/numReads - SHOCK) - val5;
	    		printf("Shock on Z- axis = %.2fg\n", (float)dif/G);
	    		sum[5] -= val5;
	    		sum[5] += sum[5]/numReads;
	    	}
	    	else {
	    		printf("Z: no shock\n");
	    	}
	    	
	    	printf("\n\n");	    	
	    	sleep(1);
    	}

        iio_context_destroy(ctx);



	return 0;
}

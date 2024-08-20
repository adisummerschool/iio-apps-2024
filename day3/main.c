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
#define DEVICE "ad5592r_s"


int main() {
    	//mode selection
    	int mode = 0;
    	printf("Choose mode:\n  1 -> board calibration\n  2 -> shock detection\n");
    	scanf("%d", &mode); 
    	
    	if (mode != 1 && mode != 2) {
	    printf("Invalid mode selected\nExiting...");
	    return -1;
	}
    	
    	struct iio_context *ctx;
        ctx = iio_create_context_from_uri(URI);

        if(!ctx) {
                printf("%s\n", "Cannot get ctx");
        }

    	struct iio_device* dev = iio_context_find_device(ctx, DEVICE);
    	if(!dev) {
    		printf("%s\n", "Cannot get ADC device");    		
    	}
    	
    	//enabling the ADC
    	iio_device_attr_write(dev, EN, "1");
    	
    	//running averages 
    	long avg[6] = {0, 0, 0, 0, 0, 0};
    	
    	//continuously reading the channel values
    	while(true) {
    		char *chan_name[6] = {"voltage0",
    					"voltage1", 
    					"voltage2",
    					"voltage3",
    					"voltage4",
    					"voltage5"};
    	
	    	char value[6][10] = {"0", "0", "0", "0", "0", "0"};
	    	struct iio_channel* voltage[6];
	    	int val[6];
	    	
	    	for (int i = 0; i < 6; i++) {
	    		voltage[i] = iio_device_find_channel(dev, chan_name[i], NULL);
	    		iio_channel_attr_read(voltage[i], CHAN_ATTR, value[i], 10);  
	    		val[i] = atoi(value[i]);
	    		printf("%s channel raw value = %s\n", chan_name[i], value[i]);
	    	}
	    	
	    	printf("\n\n");
	    	
	    	if (mode == 1) {
		    	printf("Calibrating...\n\n");
		    	
		     	if (val[0] < THRESHOLD && val[1] < THRESHOLD) {
		    		printf("X axis OK\n");
		    	}
		    	if (val[0] > THRESHOLD) {
				printf("Turn right X potentiometer\n");
			}
		    	if (val[1] > THRESHOLD) {
		    		printf("Turn left X potentiometer\n");
		    	}
		    	
		    	if (val[2] < THRESHOLD && val[3] < THRESHOLD) {
		    		printf("Y axis OK\n");
		    	}
		    	if (val[2] > THRESHOLD) {
				printf("Turn right Y potentiometer\n");
			}
		    	if (val[3] > THRESHOLD) {
		    		printf("Turn left Y potentiometer\n");
		    	}
		    	
		    	if (val[4] < THRESHOLD && val[5] < THRESHOLD) {
		    		printf("Z axis OK\n");
		    	}
		    	if (val[4] > THRESHOLD) {
				printf("Turn right Z potentiometer\n");
			}
		    	if (val[5] > THRESHOLD) {
		    		printf("Turn left Z potentiometer\n");
		    	}
	    	}
	    	
	    	if (mode == 2) {
	    		for (int j = 0; j < 6; j++) {
	    			avg[j] += val[j];
	    			avg[j] /= 2;
	    		}
		    	
		    	int dif;
		    	
		    	if ((val[0] > avg[0] + SHOCK)) {
		    		dif = val[0] - (avg[0] + SHOCK);
		    		printf("Shock on X+ axis = %.2fg\n", (float)dif/G);
		    	}
		    	else if ((val[0] < avg[0] - SHOCK)) {
		    		dif = (avg[0] - SHOCK) - val[0];
		    		printf("Shock on X+ axis = %.2fg\n", (float)dif/G);
		    	}
		    	else if ((val[1] > avg[1] + SHOCK)) {
		    		dif = val[1] - (avg[1] + SHOCK);
		    		printf("Shock on X- axis = %.2fg\n", (float)dif/G);
		    	}
		    	else if ((val[1] < avg[1] - SHOCK)) {
		    		dif = (avg[1] - SHOCK) - val[1];
		    		printf("Shock on X- axis = %.2fg\n", (float)dif/G);
		    	}
		    	else {
		    		printf("X: no shock\n");
		    	}
		    	
		    	
		    	if ((val[2] > avg[2] + SHOCK)) {
		    		dif = val[2] - (avg[2] + SHOCK);
		    		printf("Shock on Y+ axis = %.2fg\n", (float)dif/G);
		    	}
			else if ((val[2] < avg[2] - SHOCK)) {
				dif = (avg[2] - SHOCK) - val[2];
		    		printf("Shock on Y+ axis = %.2fg\n", (float)dif/G);
		    	}
		    	else if ((val[3] > avg[3] + SHOCK)) {
		    		dif = val[3] - (avg[3] + SHOCK);
		    		printf("Shock on Y- axis = %.2fg\n", (float)dif/G);
		    	}
		    	else if ((val[3] < avg[3] - SHOCK)) {
		    		dif = (avg[3] - SHOCK) - val[3];
		    		printf("Shock on Y- axis = %.2fg\n", (float)dif/G);
		    	}
		    	else {
		    		printf("Y: no shock\n");
		    	}
		    	
		
		    	if ((val[4] > avg[4] + SHOCK) ) {
		    		dif = val[4] - (avg[4] + SHOCK);
		    		printf("Shock on Z+ axis = %.2fg\n", (float)dif/G);
		    	}
		    	else if ((val[4] < avg[4] - SHOCK)) {
		    		dif = (avg[4] - SHOCK) - val[4];
		    		printf("Shock on Z+ axis = %.2fg\n", (float)dif/G);
		    	}
		    	else if ((val[5] > avg[5] + SHOCK)) {
		    		dif = val[5] - (avg[5] + SHOCK);
		    		printf("Shock on Z- axis = %.2fg\n", (float)dif/G);
		    	}
		    	else if ((val[5] < avg[5] - SHOCK)) {
		    		dif = (avg[5] - SHOCK) - val[5];
		    		printf("Shock on Z- axis = %.2fg\n", (float)dif/G);
		    	}
		    	else {
		    		printf("Z: no shock\n");
		    	}
	    	}
	    	
	    	printf("\n\n");	    	
	    	sleep(1);
    	}

        iio_context_destroy(ctx);
	return 0;
}

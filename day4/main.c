#include <iio.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#define URI "ip:10.76.84.100"
#define EN "en"
#define CHAN_ATTR "raw"
#define THRESHOLD 30
#define SHOCK 400
#define G 4095
#define DEVICE "ad5592r_s"


const int max_adc = 4095;
const double volt_per_lsb = 2.4/4095;
struct iio_context *ctx;

void sig_handler(int s) {
	signal(SIGINT, sig_handler);
	(void) s;
	iio_context_destroy(ctx);
	printf("\nDestroying context and exiting...\n");
	exit(0);
}


int main() {
	signal(SIGINT, sig_handler);

    	//mode selection
    	int mode = 0;
    	printf("Choose mode:\n  1 -> board calibration\n  2 -> shock detection\n");
    	scanf("%d", &mode); 
    	
    	if (mode != 1 && mode != 2) {
	    printf("Invalid mode selected\nExiting...");
	    return -1;
	}
    	
    	//the context has to be a global variable in order to be destroyed from the SIGINT handler
    	//struct iio_context *ctx;
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
    	
    	//initial values
    	char *chan_name[6] = {"voltage0",
    					"voltage1", 
    					"voltage2",
    					"voltage3",
    					"voltage4",
    					"voltage5"};
    	struct iio_channel* voltage[6];
    	char init_raw[6][10];
    	int init_raw_int[6];
    	for (int i = 0; i < 6; i++) {
	    	voltage[i] = iio_device_find_channel(dev, chan_name[i], NULL);
	    	iio_channel_attr_read(voltage[i], CHAN_ATTR, init_raw[i], 10);  
		init_raw_int[i] = atoi(init_raw[i]);
 		printf("%s channel initial raw value = %s\n", chan_name[i], init_raw[i]);
	}
	printf("\n\n");
    	
    	//continuously reading the channel values
    	while(true) {
    		double chan_voltage[6];
    		double chan_g[6];
    	
	    	char value[6][10] = {"0", "0", "0", "0", "0", "0"};
	    	int val[6];
	    	
	    	for (int i = 0; i < 6; i++) {
	    		iio_channel_attr_read(voltage[i], CHAN_ATTR, value[i], 10);  
	    		val[i] = atoi(value[i]);
	    		printf("%s channel raw value = %s\n", chan_name[i], value[i]);
	    		
	    		chan_voltage[i] = val[i] * volt_per_lsb;
	    		printf("%s channel voltage value = %f [V]\n", chan_name[i], chan_voltage[i]);
	    		
	    		chan_g[i] = (float)val[i] / max_adc;
	    		printf("%s channel acceleration value = %f [g]\n", chan_name[i], chan_g[i]);
	    	}
	    	
	    	printf("\n");
	    	
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
	    		char* axis_names[6] = {"X+",
	    					"X-",
	    					"Y+",
	    					"Y-",
	    					"Z+",
	    					"Z-"};
	    	
	    		int dif;
	    		for (int j = 0; j < 6; j++) {    	
		    		if (val[j] - init_raw_int[j] >= SHOCK) {
		    			dif = val[j] - init_raw_int[j];
		    			printf("Shock on %s axis = %.2fg\n", axis_names[j], (float)dif/G);
		    			printf("Recalibrate!\n");
		    			sleep(5);
		    			for (int i = 0; i < 6; i++) {
	    					iio_channel_attr_read(voltage[i], CHAN_ATTR, init_raw[i], 10);  
						init_raw_int[i] = atoi(init_raw[i]);
					}
		    		}
	    		} 	
	    		
    		}
    	
    		
	    	printf("\n");	  
    		sleep(1);
    	}

        //iio_context_destroy(ctx);
	return 0;
}

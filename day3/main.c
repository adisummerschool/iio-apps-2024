#include <iio.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#define URI "ip:10.76.84.100"
#define EN "en"
#define CHAN_ATTR "raw"
#define DEVICE "ad5592r_s"
#define SHOCK 400
#define G 4095


const int max_adc = 4095;
const double volt_per_lsb = 2.4/4095;
struct iio_context *ctx;
struct iio_buffer* buf;
const int samples_count = 1024;

typedef struct _buf
{
	uint16_t xpoz;
	uint16_t xneg;
	uint16_t ypoz;
	uint16_t yneg;
	uint16_t zpoz;
	uint16_t zneg;

}buffer_element;


uint16_t* buf_average(buffer_element* buf) {
	uint16_t* avg = (uint16_t*)malloc(6*sizeof(uint16_t));
	double sum[6] = {0, 0, 0, 0, 0, 0};

	for(int i = 0; i < samples_count; i++) {
		sum[0] += buf[i].xpoz;
		sum[1] += buf[i].xneg;
		sum[2] += buf[i].ypoz;
		sum[3] += buf[i].yneg;
		sum[4] += buf[i].zpoz;
		sum[5] += buf[i].zneg;
	}
	
	for(int i = 0; i < 6; i++) {
		avg[i] = sum[i]/samples_count;
	}

	return avg;
}

void sig_handler(int s) {
	signal(SIGINT, sig_handler);
	(void) s;
	iio_buffer_destroy(buf);
	printf("\nDestroying buffer...\n");
	iio_context_destroy(ctx);
	printf("Destroying context and exiting...\n");
	exit(0);
}


int main() {
	signal(SIGINT, sig_handler);
    	
    	//the context has to be a global variable in order to be destroyed from the SIGINT handler
    	//struct iio_context *ctx;
        ctx = iio_create_context_from_uri(URI);

        if(!ctx) {
                printf("%s\n", "Cannot get ctx");
                return -1;
        }

    	struct iio_device* dev = iio_context_find_device(ctx, DEVICE);
    	if(!dev) {
    		printf("%s\n", "Cannot get ADC device");  
    		return -1;  		
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
    					
    	char* axis_names[6] = {"X+",
 				"X-",
    				"Y+",
	   			"Y-",
	    			"Z+",
	    			"Z-"};
	    					
    	bool cyclic = false;
    	struct iio_channel* voltage[6];
    	char init_raw[6][10];
    	int init_raw_int[6];
    	for (int i = 0; i < 6; i++) {
	    	voltage[i] = iio_device_find_channel(dev, chan_name[i], NULL);
	    	if (!voltage[i]) {
	    		printf("Cannot get device channels\n");
	    		return -1;
	    	}
	    	//enabling the ADC channels for the buffers
	    	iio_channel_enable(voltage[i]);
	}
	
	buf = iio_device_create_buffer(dev, samples_count, cyclic);

	
	int bytes_read = iio_buffer_refill(buf);
	int values[6];
	buffer_element elem[samples_count];	
	buffer_element detected[samples_count];
	int k = 0;
	
	printf("Sampling initial data...\n");
	for(void *ptr = iio_buffer_start(buf); ptr < iio_buffer_end(buf); ptr += iio_buffer_step(buf)) {
		for (int i = 0; i < 6; i++) {
			values[i] = *((uint16_t*)ptr + i * sizeof(uint16_t));
			//printf("Value at index %d = %d\n", i, values[i]);
		}
		
		elem[k].xpoz = values[0];
		elem[k].xneg = values[1];
		elem[k].ypoz = values[2];
		elem[k].yneg = values[3];
		elem[k].zpoz = values[4];
		elem[k].zneg = values[5];
		
		/*
		printf("Xpoz = %d\n", elem[k].xpoz);
		printf("Xneg = %d\n", elem[k].xneg);
		printf("Ypoz = %d\n", elem[k].ypoz);
		printf("Yneg = %d\n", elem[k].yneg);
		printf("Zpoz = %d\n", elem[k].zpoz);
		printf("Zneg = %d\n", elem[k].zneg);
		*/
		
		k++;	
	}
	
	uint16_t* avg = buf_average(elem);
	printf("The average on X+ = %d\n", avg[0]);
	printf("The average on X- = %d\n", avg[1]);
	printf("The average on Y+ = %d\n", avg[2]);
	printf("The average on Y- = %d\n", avg[3]);
	printf("The average on Z+ = %d\n", avg[4]);
	printf("The average on Z- = %d\n", avg[5]);
	
	printf("\n\n");
	
	while(true) {
		bytes_read = iio_buffer_refill(buf);
		k = 0;
		for(void *ptr = iio_buffer_start(buf); ptr < iio_buffer_end(buf); ptr += iio_buffer_step(buf)) {
			for (int i = 0; i < 6; i++) {
				values[i] = *((uint16_t*)ptr + i * sizeof(uint16_t));
			}
			
			detected[k].xpoz = values[0];
			detected[k].xneg = values[1];
			detected[k].ypoz = values[2];
			detected[k].yneg = values[3];
			detected[k].zpoz = values[4];
			detected[k].zneg = values[5];

			k++;	
		}
		
		uint16_t* prev  = buf_average(elem);
		uint16_t* avg = buf_average(detected);
		
		for (int j = 0; j < 6; j++) {
			if (avg[j] - prev[j] >= SHOCK) {
				int dif = avg[j] - prev[j];
				printf("Shock on %s axis = %.2fg\n", axis_names[j], (float)dif/G);
		    		printf("Recalibrate!\n");
		    		sleep(5);
			
				k = 0;
				bytes_read = iio_buffer_refill(buf);
				for(void *ptr = iio_buffer_start(buf); ptr < iio_buffer_end(buf); ptr += iio_buffer_step(buf)) {
					for (int i = 0; i < 6; i++) {
						values[i] = *((uint16_t*)ptr + i * sizeof(uint16_t));
						//printf("Value at index %d = %d\n", i, values[i]);
					}
					
					elem[k].xpoz = values[0];
					elem[k].xneg = values[1];
					elem[k].ypoz = values[2];
					elem[k].yneg = values[3];
					elem[k].zpoz = values[4];
					elem[k].zneg = values[5];
						
					k++;	
				}
			}
		
		}
		
		printf("The average on X+ = %d\n", avg[0]);
		printf("The average on X- = %d\n", avg[1]);
		printf("The average on Y+ = %d\n", avg[2]);
		printf("The average on Y- = %d\n", avg[3]);
		printf("The average on Z+ = %d\n", avg[4]);
		printf("The average on Z- = %d\n", avg[5]);
		
		sleep(1);
		
		printf("\n\n");
	}
	

	iio_buffer_destroy(buf);
        iio_context_destroy(ctx);
	return 0;
}

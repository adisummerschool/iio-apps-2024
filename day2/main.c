#include <iio.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "findDev.h"
#include <unistd.h>
#include "temp.h"
#include "ledBridge.h"
#include "adcBridge.h"
#include "adcToLedBridge.h"
#include <unistd.h>
#include <sys/select.h>
#include <fcntl.h>

#define THRESHOLD 500
struct valList
{
	int val;
	struct valList *next;
};
struct valList* newLast(struct valList* last,int val)
{
	struct valList *aux = (struct valList*) malloc(sizeof(struct valList));
	aux->next = NULL;
	aux->val = val;
	//last->next = (struct valList*) malloc(sizeof(struct valList));
	last->next = aux;
	return aux;
}
struct valList* delFirst(struct valList *first)
{
	struct valList *aux = first;
	first = first->next;
	free(aux);
	return first;
}
int max(int a ,int b)
{
	return (a<=b) ? b : a;
}
void prettyDisplay(int tabs,int offset)
{
	char* toPrint = calloc((tabs*9-offset),sizeof(char));
	memset(toPrint,' ',(tabs*9-offset));
	printf("%s",toPrint);
	free(toPrint);
}
void displayList(struct valList *first)
{
	struct valList *aux = first;
	printf("list is : ");
	if(aux != NULL)
	{
		while(aux->next != NULL)
		{
			printf("%d ",aux->val);
			aux = aux->next;
		}
		printf("%d ",aux->val);
	}
}
int getAvgRaw(struct iio_channel *chann,struct valList *first,struct valList **last)
{
	printf("Getting the avg of the channel %s the list is ...\n",iio_channel_get_id(chann));
	int sum = 0;
	char * valOfChann;
	struct valList* newList = first;
	newList->next = (struct valList*) malloc(sizeof(struct valList));
	sum += atoi(valOfChann);
	newList->val = atoi(valOfChann);
	printf("%d ",newList->val);
	for(int i = 0 ;i < 19; i++)
	{
		valOfChann = readChannAttr("raw",chann);
		if(valOfChann != NULL)
		{
			newList = newList->next;
			sum += atoi(valOfChann);
			newList->val = atoi(valOfChann);
			printf("%d ",newList->val);
			newList->next = (struct valList*) malloc(sizeof(struct valList));
		}
		else return -1;
		sleep(.1);
	}
	*last = newList;
	if(newList->next == NULL)
		printf("we have null last %d",(*last)->val);
	else
		printf("we dont have null last %d",(*last)->val);
	printf("\n");
	if(newList == last)
		printf("we have last\n");
	else
		printf("we dont have last\n");
	(*last)->next = NULL;
	return sum;
}
int main()
{
	
	fd_set readfds;
	struct timeval tv;
	int retval;

	
	
	
	struct iio_device* ad5592rs = findMyDevice("ad5592r_s");
	struct iio_device* xadc = findMyDevice("xadc");
	struct iio_device* axi_pwm_led = findMyDevice("axi-pwm-led");
	struct iio_channel *adcInput0,*adcInput1,*adcInput2,*adcInput3,*adcInput4,*adcInput5,*pwmLed0,*pwmLed1,*pwmLed2,*temp;
	//struct iio_context *ctx,*ctx2 = iio_device_get_context(ad5592rs);
	//ctx = iio_create_context_from_uri(URI);
	//printf("pas 0\n");
	//printf("reusult of compare %d\n",checkCtx(ctx,ctx2));dasd
	if(!ad5592rs || !axi_pwm_led)
	{
		return -1;
	}
		
	adcInput0 = findChannel("voltage0",ad5592rs);
	adcInput1 = findChannel("voltage1",ad5592rs);
	adcInput2 = findChannel("voltage2",ad5592rs);
	adcInput3 = findChannel("voltage3",ad5592rs);
	adcInput4 = findChannel("voltage4",ad5592rs);
	adcInput5 = findChannel("voltage5",ad5592rs);
	
	pwmLed0 = findChannel("count0",axi_pwm_led);
	pwmLed1 = findChannel("count1",axi_pwm_led);
	pwmLed2 = findChannel("count2",axi_pwm_led);
	
	temp = findChannel("temp0",xadc);
	
	printf("GOT ALL CHANNELS\n");
	
	writeDevAttr("en",ad5592rs,"1");
	//writeChannAttr("raw",adcInput0,"255");
	
	/*char* offset = readChannAttr("offset",temp);
	printf("offset %s\n",offset);
	getTemp(temp);*/
	
	/*struct ledChannel* leds = initLedBridge();
	printf("size of ledchannel %d\n",getNmbOfLedChannels());
	//findChannel("count0",findMyDevice("axi-pwm-led"));	
	leds = addLedChannel(pwmLed0,"raw");
	leds = addLedChannel(pwmLed1,"raw");
	leds = addLedChannel(pwmLed2,"raw");
	
	printf("GOT ALL THE PWM LEDS TO BRIDGE\n");	
	
	writeValueToLed(&leds[0],"255");
	writeValueToLed(&leds[1],"255");
	writeValueToLed(&leds[2],"255");
	
	struct adcChannel *adcs = initAdcBridge();
	adcs = addAdcChannel(adcInput0,"raw");
	adcs = addAdcChannel(adcInput1,"raw");
	adcs = addAdcChannel(adcInput2,"raw");
	
	printf("GOT ALL THE ADC INPUTS TO BRIDGE\n");	

	struct adcToLedBridge *bridges;
	bridges = initAdcToLedBridge();
	bridges = addBridge(&leds[0],&adcs[0]);
	bridges = addBridge(&leds[1],&adcs[1]);
	bridges = addBridge(&leds[2],&adcs[2]);
	*/
	/*
	while(true)
	if(getTemp(temp) >= 45.f)	
		{
			readWriteChannAttr("raw",pwmLed0,1,"0");
			readWriteChannAttr("raw",pwmLed1,1,"255");
			sleep(1);
		}
	else
		{
			readWriteChannAttr("raw",pwmLed0,1,"0");
			readWriteChannAttr("raw",pwmLed1,1,"255");
			sleep(1);
		}
		*/
	printf("!!!!!!!!!!!!!!CALIBRATING DONT MOVE YOUR BOARD IN 2 SECS!!!!!!!!!!!!!!!!\n");
	sleep(1);
	printf("CALIBRATING DONT MOVE YOUR BOARD IN 1 SECS\n");
	sleep(1);
	struct valList *listXpFirst = (struct valList*) malloc(sizeof(struct valList));
	struct valList *listXnFirst = (struct valList*) malloc(sizeof(struct valList));
	struct valList *listYpFirst = (struct valList*) malloc(sizeof(struct valList));
	struct valList *listYnFirst = (struct valList*) malloc(sizeof(struct valList));
	struct valList *listZpFirst = (struct valList*) malloc(sizeof(struct valList));
	struct valList *listZnFirst = (struct valList*) malloc(sizeof(struct valList));
	struct valList *listXpLast = (struct valList*) malloc(sizeof(struct valList));
	struct valList *listXnLast = (struct valList*) malloc(sizeof(struct valList));
	struct valList *listYpLast = (struct valList*) malloc(sizeof(struct valList));
	struct valList *listYnLast = (struct valList*) malloc(sizeof(struct valList));
	struct valList *listZpLast = (struct valList*) malloc(sizeof(struct valList));
	struct valList *listZnLast = (struct valList*) malloc(sizeof(struct valList));
	int sumX_p = getAvgRaw(adcInput0,listXpFirst,&listXpLast);
	printf("X_p ");
	displayList(listXpFirst);
	printf("avg is %d\n",sumX_p/20);
	int sumX_n = getAvgRaw(adcInput1,listXnFirst,&listXnLast);
	printf("X_n ");
	displayList(listXnFirst);
	printf("avg is %d\n",sumX_n/20);
	int sumY_p = getAvgRaw(adcInput2,listYpFirst,&listYpLast);
	printf("Y_p ");
	displayList(listYpFirst);
	printf("avg is %d\n",sumY_p/20);
	int sumY_n = getAvgRaw(adcInput3,listYnFirst,&listYnLast);
	printf("Y_n ");
	displayList(listYnFirst);
	printf("avg is %d\n",sumY_n/20);
	int sumZ_p = getAvgRaw(adcInput4,listZpFirst,&listZpLast);
	printf("Z_p ");
	displayList(listZpFirst);
	printf("avg is %d\n",sumZ_p/20);
	int sumZ_n = getAvgRaw(adcInput5,listZnFirst,&listZnLast);
	printf("Z_n ");
	displayList(listZnFirst);
	printf("avg is %d\n",sumZ_n/20);
	int index = 0;
	int avgX_p = sumX_p/20;
	int avgX_n = sumX_n/20;
	int avgY_p = sumY_p/20;
	int avgY_n = sumY_n/20;
	int avgZ_p = sumZ_p/20;
	int avgZ_n = sumZ_n/20;
	/*
	if(listXpLast->next == NULL)
	{
		listXpLast->next = (struct valList*) malloc(sizeof(struct valList));
		printf("changed value for Xp\n");
	}
	if(listXnLast->next == NULL)
	{
		listXnLast->next = (struct valList*) malloc(sizeof(struct valList));
		printf("changed value for Xn\n");
	}
	if(listYpLast->next == NULL)
	{
		listYpLast->next = (struct valList*) malloc(sizeof(struct valList));
		printf("changed value for Yp\n");		
	}
	if(listYnLast->next == NULL)
	{
		listYnLast->next = (struct valList*) malloc(sizeof(struct valList));
		printf("changed value for Yn\n");		
	}
	if(listZpLast->next == NULL)
	{
		listZpLast->next = (struct valList*) malloc(sizeof(struct valList));
		printf("changed value for Zp\n");	
	}
	if(listZnLast->next == NULL)
	{
		listZnLast->next = (struct valList*) malloc(sizeof(struct valList));
		printf("changed value for Zn\n");	
	}
	*/
	bool stop = true;
	while(stop)
	{
		char * valOfChann0 = readChannAttr("raw",adcInput0);
		char * valOfChann1 = readChannAttr("raw",adcInput1);
		char * valOfChann2 = readChannAttr("raw",adcInput2);
		char * valOfChann3 = readChannAttr("raw",adcInput3);
		char * valOfChann4 = readChannAttr("raw",adcInput4);
		char * valOfChann5 = readChannAttr("raw",adcInput5);
		printf("x+=%s x-=%s y+=%s y-=%s z+=%s z-=%s\n",valOfChann0,valOfChann1,valOfChann2,valOfChann3,valOfChann4,valOfChann5);
		if(valOfChann0 && valOfChann1 && valOfChann2 && valOfChann3  && valOfChann4 && valOfChann5)
		{
			int x_p = atoi(valOfChann0);
			int x_n = atoi(valOfChann1);
			int y_p = atoi(valOfChann2);
			int y_n = atoi(valOfChann3);
			int z_p = atoi(valOfChann4);
			int z_n = atoi(valOfChann5);
			
			if(x_p - avgX_p > THRESHOLD || x_n - avgX_n > THRESHOLD)
			{	
				sumX_p += x_p;
				listXpLast = newLast(listXpLast,avgX_p);
				sumX_p -= listXpFirst->val;
				listXpFirst = delFirst(listXpFirst);
				sumX_n += x_n;
				listXnLast = newLast(listXnLast,avgX_n);
				sumX_n -= listXnFirst->val;
				listXnFirst = delFirst(listXnFirst);
				float avg = (float)(max(x_p-avgX_p,x_n-avgX_n)/2000.0);
				printf("SHOCK FOR X g = %.3f\n",(float)(avg)); 
			}
			else
			{
				sumX_p += x_p;
				listXpLast = newLast(listXpLast,x_p);
				sumX_p -= listXpFirst->val;
				listXpFirst = delFirst(listXpFirst);
				sumX_n += x_n;
				listXnLast = newLast(listXnLast,x_n);
				sumX_n -= listXnFirst->val;
				listXnFirst = delFirst(listXnFirst);
			}
			
			if(y_p - avgY_p > THRESHOLD || y_n - avgY_n > THRESHOLD)
			{	
				sumY_p += y_p;
				listYpLast = newLast(listYpLast,avgY_p);
				sumY_p -= listYpFirst->val;
				listYpFirst = delFirst(listYpFirst);
				sumY_n += y_n;
				listYnLast = newLast(listYnLast,avgY_n);
				sumY_n -= listYnFirst->val;
				listYnFirst = delFirst(listYnFirst);
				float avg = (float)(max(y_p-avgY_p,y_n-avgY_n)/2000.0);
				printf("SHOCK FOR Y g = %.3f\n",(float)(avg)); 
			}
			else
			{
				sumY_p += y_p;
				listYpLast = newLast(listYpLast,y_p);
				sumY_p -= listYpFirst->val;
				listYpFirst = delFirst(listYpFirst);
				sumY_n += y_n;
				listYnLast = newLast(listYnLast,y_n);
				sumY_n -= listYnFirst->val;
				listYnFirst = delFirst(listYnFirst);
			}
			
			if(z_p - avgZ_p > THRESHOLD || z_n - avgZ_n > THRESHOLD)
			{	
				sumZ_p += z_p;
				listZpLast = newLast(listZpLast,avgZ_p);
				sumZ_p -= listZpFirst->val;
				listZpFirst = delFirst(listZpFirst);
				sumZ_n += z_n;
				listZnLast = newLast(listZnLast,avgZ_n);
				sumZ_n -= listZnFirst->val;
				listZnFirst = delFirst(listZnFirst);
				float avg = (float)(max(z_p-avgZ_p,z_n-avgZ_n)/2000.0);
				printf("SHOCK FOR Z g = %.3f\n",(float)(avg)); 
			}
			else
			{
				sumZ_p += z_p;
				listZpLast = newLast(listZpLast,z_p);
				sumZ_p -= listZpFirst->val;
				listZpFirst = delFirst(listZpFirst);
				sumZ_n += z_n;
				listZnLast = newLast(listZnLast,z_n);
				sumZ_n -= listZnFirst->val;
				listZnFirst = delFirst(listZnFirst);
			}
			
			avgX_p = sumX_p/20;
			avgX_n = sumX_n/20;
			avgY_p = sumY_p/20;
			avgY_n = sumY_n/20;
			avgZ_p = sumZ_p/20;
			avgZ_n = sumZ_n/20;
			
			printf("List X_p: "); 
			displayList(listXpFirst);
			printf("avg is %d\n",avgX_p);
			printf("List X_n: "); 
			displayList(listXnFirst);
			printf("avg is %d\n",avgX_n);
			printf("List Y_p: "); 
			displayList(listYpFirst);
			printf("avg is %d\n",avgY_p);
			printf("List Y_n: "); 
			displayList(listYnFirst);
			printf("avg is %d\n",avgY_n);			
			printf("List Z_p: "); 
			displayList(listZpFirst);
			printf("avg is %d\n",avgZ_p);			
			printf("List Z_n: "); 
			displayList(listZnFirst);
			printf("avg is %d\n",avgZ_n);	
			
			// Watch stdin (fd 0) to see when it has input
			/*
	FD_ZERO(&readfds);
	FD_SET(STDIN_FILENO, &readfds);

	// Wait up to five seconds
	tv.tv_sec = 5;
	tv.tv_usec = 0;	
			
			retval = select(1, &readfds, NULL, NULL, &tv);
			if (retval == -1) {
				perror("select()");
		   	} 
		  	else if (retval) {
			printf("Data is available now.\n");
			char buffer[100];
			 ssize_t len = read(STDIN_FILENO, buffer, sizeof(buffer) - 1); // -1 to leave space for null-terminator
		    if (len > 0) {
			buffer[len] = '\0'; // Null-terminate the string

			// Remove the newline character if it exists
			if (buffer[len - 1] == '\n') {
			    buffer[len - 1] = '\0';
			}

			if (strcmp(buffer, "exit") == 0) {
			    stop = 0;
			    printf("Stopping...\n");
			} else {
			    printf("You entered: %s\n", buffer);
			}
		    }
		} else {
		    printf("No data within five seconds.\n");
		}
		*/
			
			/*
			if(x_p > 30)
				printf("turn right pot x\n");
			else if(x_n > 30)
				printf("turn left pot x\n");
			else if (x_n < 30 && x_p < 30)
				printf("OK for x\n");
			if(y_p > 30)
				printf("turn right pot y\n");
			else if(y_n > 30)
				printf("turn left pot y\n");
			else if (y_n < 30 && y_p < 30)
				printf("OK for y\n");
			if(z_p > 30)
				printf("turn right pot z\n");
			else if(z_n > 30)
				printf("turn left pot z\n");
			else if (z_n < 30 && z_p < 30)
				printf("OK for z\n");
				*/
		}
		else
			printf("ERROR CANT READ DEV\n");
		sleep(1);
	}
	
	
	return 0;
}

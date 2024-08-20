#include <iio.h>
#include "temp.h"
#include "findDev.h"
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>


float getTemp(struct iio_channel* tempChannel)
{
	char* scale = readWriteChannAttr("scale",tempChannel,0,"0");
	char* raw = readWriteChannAttr("raw",tempChannel,0,"0");
	//findChannelAttr("offset",tempChannel);
	char* offset = readWriteChannAttr("offset",tempChannel,0,"0");
	
	printf("converting values..");
	int rawInt = atoi(raw);
	int offsetInt = atoi(offset);
	float scaleFloat = atof(scale);
	
	printf("Values converted to math var are %d, %d, %.3f\n",rawInt,offsetInt,scaleFloat);
	
	float CPUtemp = (float)((rawInt + offsetInt)*scaleFloat/1000);
	printf("FPGA CPU temp is %.3f\n",CPUtemp);
	return CPUtemp;
}

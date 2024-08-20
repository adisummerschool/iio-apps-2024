#ifndef ADCBRIDGE_H_
#define ADCBRIDGE_H_
#include "findDev.h"
#include <iio.h>

struct adcChannel
{
	struct iio_channel *chann;
	char *channAttr;
};
struct adcChannel* initAdcBridge();
int getNmbOfAdcChannels();
struct adcChannel* addAdcChannel(struct iio_channel* channel,char* channelAttr);
int findChannInAdcs(struct iio_channel* channel);
int findChannAttrInAdcs(char* channelName);
//int writeValueToAdc(struct adcChannel *adc,char* src);
char* readValueFromAdc(struct adcChannel *adc);
int findLedBridge(struct ledChannel *led);

#endif

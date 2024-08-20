#ifndef ADCTOLEDBRIDGE_H_
#define ADCTOLEDBRIDGE_H_
#include "findDev.h"
#include "ledBridge.h"
#include "adcBridge.h"
#include <iio.h>

struct adcToLedBridge
{
	struct ledChannel *led;
	struct adcChannel *adc;
};
struct adcToLedBridge* initAdcToLedBridge();
int getNmbOfBridges();
struct adcToLedBridge* addBridge(struct ledChannel *led,struct adcChannel *adc);
int findAdcChannInBridges(struct adcChannel *adc);
int findLedChannInBridges(struct ledChannel *led);
//int writeValueToAdc(struct adcChannel *adc,char* src);
char* readValueFromAdcInBridge(struct adcChannel *adc);
char* readValueFromLedInBridge(struct ledChannel *led);

#endif

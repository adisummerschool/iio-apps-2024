#ifndef LEDBRIDGE_H_
#define LEDBRIDGE_H_
#include "findDev.h"
#include <iio.h>

struct ledChannel
{
	struct iio_channel *chann;
	char *channAttr;
};
struct ledChannel* initLedBridge();
int getNmbOfLedChannels();
struct ledChannel* addLedChannel(struct iio_channel* channel,char* channelAttr);
int findLedChann(struct iio_channel* channel);
int findLedChannAttr(char* channelName);
int writeValueToLed(struct ledChannel *led,char* src);
char* readValueFromLed(struct ledChannel *led);
int findLedBridge(struct ledChannel *led);

#endif

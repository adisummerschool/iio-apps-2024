#include <iio.h>
#include <string.h>
#include <stdio.h>
#include "ledBridge.h"
#include "findDev.h"

struct ledChannel *leds;
int nmbOfChannelsLed = 0;
void copyBridge(struct ledChannel *destBridge, struct ledChannel *srcBridge) {
    destBridge->chann  = srcBridge->chann;
    destBridge->channAttr = srcBridge->channAttr;
}
void memcopy(struct ledChannel *dest,struct ledChannel *src,size_t size)
{
	struct ledChannel *auxDest = calloc(size,sizeof(struct ledChannel));
	if(dest != NULL && src != NULL && size >= 0)
		for(int i = 0; i < size;i++)
		{
			printf("am intrat in memcopy %d\n",i);
			printf("dest char %s\n",auxDest[i].channAttr);
			printf("src char %s\n",src[i].channAttr);
			char *aux = calloc(strlen(src[i].channAttr),sizeof(char));
			strcpy(aux,src[i].channAttr);
			auxDest[i].channAttr = aux;
			printf("dest char %s\n",auxDest[i].channAttr);
		}
	dest = auxDest;
}

struct ledChannel* addLedChannel(struct iio_channel* channel,char* channelAttr)
{
	struct iio_channel* ret = findChannelAttr(channelAttr,channel);
	if(ret)
	{
		struct ledChannel *auxLeds;
		auxLeds = calloc(nmbOfChannelsLed+1,sizeof(struct ledChannel));
		if(nmbOfChannelsLed > 0)
			printf("!!!!!DEBUG %d %s\n",nmbOfChannelsLed,leds[nmbOfChannelsLed-1].channAttr);
		if (leds && nmbOfChannelsLed > 1) 
		{
			printf("Resizing ledChannel pointer..\n");
			//memcpy(auxLeds, leds, nmbOfChannelsLed * sizeof(struct ledChannel)*2);
			for (int i = 0; i < nmbOfChannelsLed; i++) {
			    copyBridge(&auxLeds[i], &leds[i]); // in case the memcopy fails
			}
			printf("aux leds first item %s\n",auxLeds[0].channAttr);
			free(leds);			
		}
		leds[nmbOfChannelsLed].chann = channel;
		leds[nmbOfChannelsLed].channAttr = channelAttr;
		printf("DEBUG for add error code for find chan attr ");
			printf("%dCODE\n",findChannelAttr(leds[nmbOfChannelsLed].channAttr,leds[nmbOfChannelsLed].chann));
		if(leds[nmbOfChannelsLed].chann == channel)
			{
				printf("CORRECT\n");
			}
			else printf("FALSE\n");
		nmbOfChannelsLed++;
		return leds;
	}
	else return NULL;
}
int deleteChannel(struct iio_channel* channel)
{
	if(findLedChann(channel) != -1)
	{
		struct ledChannel *auxLeds;
		auxLeds = calloc(nmbOfChannelsLed-1,sizeof(struct ledChannel));
		
		for(int i = findLedChann(channel); i<nmbOfChannelsLed; i++)
		{
			leds[i] = leds[i+1];	
		}
		
	}
	else return -1;
}
int findLedChann(struct iio_channel* channel)
{
	for(int i = 0;i < nmbOfChannelsLed;i++)
	{
		//printf("chan attr name %s\n",leds[i].channAttr);
		if(leds[i].chann == channel)
			return i;
	}
	return -1;
}
int findLedChannAttr(char* channelName)
{
	//printf("Looking for channel name in out list\n");
	for(int i = 0;i < nmbOfChannelsLed;i++)
	{
		if(strcmp(channelName,leds[i].channAttr) == 0)
			return i;
	}
	return -1;
}
int writeValueToLed(struct ledChannel *led,char* src)
{
	if(findLedChann((*led).chann) != -1)
	{
		return writeChannAttr((*led).channAttr,(*led).chann,src);
	}
	else return -1;
}
char* readValueFromLed(struct ledChannel *led)
{
	if(findLedChann((*led).chann) != -1)
	{
		return readChannAttr((*led).channAttr,(*led).chann);
	}
	else return NULL;
}
int findLedBridge(struct ledChannel *led)
{
	for(int i = 0;i < nmbOfChannelsLed;i++)
	{
		if(led == &leds[i])
			return i;
	}
	return -1;
}
struct ledChannel* initLedBridge()
{
	leds = calloc(0,sizeof(struct ledChannel));
	return leds;
}
int getNmbOfLedChannels()
{
	return nmbOfChannelsLed;
}

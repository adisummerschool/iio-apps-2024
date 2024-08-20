#include <iio.h>
#include <string.h>
#include <stdio.h>
#include "findDev.h"
#include "adcBridge.h"

struct adcChannel *adcs;
int nmbOfChannelsAdc = 0;
void memcopy(int* dest,const int *src,size_t size)
{
	if(dest != NULL && src != NULL && size >= 0)
		for(int i = 0; i <= size;i++)
		{
			dest[i] = src[i];
		}
}
struct adcChannel* addAdcChannel(struct iio_channel* channel,char* channelAttr)
{
	struct iio_channel* ret = findChannelAttr(channelAttr,channel);
	if(ret)
	{
		struct adcChannel *auxAdcs;
		auxAdcs = calloc(nmbOfChannelsAdc+1,sizeof(struct adcChannel));
		memcpy(auxAdcs,adcs,nmbOfChannelsAdc);
		free(adcs);
		adcs = auxAdcs;
		adcs[nmbOfChannelsAdc].chann = channel;
		adcs[nmbOfChannelsAdc].channAttr = channelAttr;
		printf("DEBUG for add error code for find chan attr ");
		printf("%d CODE\n",findChannelAttr(adcs[nmbOfChannelsAdc].channAttr,adcs[nmbOfChannelsAdc].chann));
		if (adcs && nmbOfChannelsAdc > 1) 
		{
			printf("Resizing adcChannel pointer..\n");
			memcpy(auxAdcs, adcs, nmbOfChannelsAdc * sizeof(struct adcChannel));
			free(adcs);			
		}
		if(adcs[nmbOfChannelsAdc].chann == channel)
			{
				printf("CORRECT\n");
			}
			else printf("FALSE\n");
		nmbOfChannelsAdc++;
		return adcs;
	}
	else return NULL;
}
int deleteChannel(struct iio_channel* channel)
{
	if(findChannInAdcs(channel) != -1)
	{
		struct adcChannel *auxAdcs;
		auxAdcs = calloc(nmbOfChannelsAdc-1,sizeof(struct adcChannel));
		
		for(int i = findChannInAdcs(channel); i<nmbOfChannelsAdc; i++)
		{
			adcs[i] = adcs[i+1];	
		}
		
	}
	else return -1;
}
int findChannInAdcs(struct iio_channel* channel)
{
	for(int i = 0;i < nmbOfChannelsAdc;i++)
	{
		if(adcs[i].chann == channel)
			return i;
	}
	return -1;
}
int findChannAttrInAdcs(char* channelName)
{
	//printf("Looking for channel name in out list\n");
	for(int i = 0;i < nmbOfChannelsAdc;i++)
	{
		if(strcmp(channelName,adcs[i].channAttr) == 0)
			return i;
	}
	return -1;
}
int writeValueToAdc(struct adcChannel *adc,char* src)
{
	if(findChannInAdcs((*adc).chann) != -1)
	{
		return writeChannAttr((*adc).channAttr,(*adc).chann,src);
	}
}
char* readValueFromAdc(struct adcChannel *adc)
{
	if(findChannInAdcs((*adc).chann) != -1)
	{
		printf("got here\n");
		return readChannAttr((*adc).channAttr,(*adc).chann);
	}
	return NULL;
}
int findAdcBridge(struct adcChannel *adc)
{
	for(int i = 0;i < nmbOfChannelsAdc;i++)
	{
		if(adc == &adcs[i])
			return i;
	}
	return -1;
}
struct adcChannel* initAdcBridge()
{
	adcs = calloc(0,sizeof(struct adcChannel));
	return adcs;
}
int getNmbOfAdcChannels()
{
	return nmbOfChannelsAdc;
}

#include "adcToLedBridge.h"


struct adcToLedBridge *bridges;
int nmbOfBridges = 0;
void copyBridge(struct adcToLedBridge *destBridge, struct adcToLedBridge *srcBridge) {
    destBridge->led  = srcBridge->led;
    destBridge->adc = srcBridge->adc;
}
struct adcToLedBridge* addBridge(struct ledChannel *led, struct adcChannel *adc) {
    if (findLedBridge(led) < 0 && findAdcBridge(adc) < 0) {
        struct adcToLedBridge *auxBridge;
        auxBridge = calloc(nmbOfBridges + 1, sizeof(struct adcToLedBridge));
        if (!auxBridge) {
            printf("Memory allocation failed\n");
            return NULL;
        }
        /*
        for (int i = 0; i < nmbOfBridges; i++) {
            copyBridge(&auxBridge[i], &bridges[i]); // in case the memcopy fails
        }
        */
        if (bridges && nmbOfBridges > 1) {
            memcpy(auxBridge, bridges, nmbOfBridges * sizeof(struct adcToLedBridge));
            free(bridges);
        }
        auxBridge[nmbOfBridges].led = led;
        auxBridge[nmbOfBridges].adc = adc;
        bridges = auxBridge;
        nmbOfBridges++;

        return bridges;
    } else {
        printf("ERROR: LED or ADC channel already in a bridge\n");
        return NULL;
    }
}
int findAdcBridgeInBridges(struct adcChannel *adc)
{
	if(findAdcBridge(adc) >= 0)
	{
		for(int i = 0; i <= nmbOfBridges; i++)
		{
			if(bridges[i].adc == adc)
				return i;
		}
		return -1;
	}
	return -2;
}
int findLedBridgeInBridges(struct ledChannel *led)
{
	if(findLedBridge(led) >= 0)
	{
		for(int i = 0; i <= nmbOfBridges; i++)
		{
			if(bridges[i].led == led)
				return i;
		}
		return -1;
	}
	return -2;	
}
char* readValueFromAdcInBridge(struct adcChannel *adc)
{
	int ret = findAdcBridgeInBridges(adc);
	printf("location of adc : %d\n",ret);
	if(ret >= 0)
	{
		printf("got here\n");
		return readValueFromAdc(bridges[ret].adc); 
	}
	else	return NULL;
}
char* readValueFromLedInBridge(struct ledChannel *led)
{
	int ret = findLedBridgeInBridges(led);
	printf("location of led : %d\n",ret);
	if(ret >= 0)
	{
		printf("got here\n");
		return readValueFromLed(bridges[ret].led); 
	}
	else	return NULL;
}
int getNmbOfBridges()
{
	return nmbOfBridges;
}
struct adcToLedBridge* initAdcToLedBridge()
{
	bridges = calloc(0,sizeof(struct adcToLedBridge));
	return bridges;
}


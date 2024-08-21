#include "findDev.h"
#include <iio.h>
#include <string.h>
struct iio_context *ctx,*FPGActx;
char* intToChar(int x)
{
	int nr=0,aux=x,*vec = calloc(100,sizeof(int)),neg=0;
	printf("Getting numbers...Converting to string\n");
	if(x<0)
		{neg=1;aux=-x;}
	while(aux!=0)
	{
		vec[nr] = aux%10;
		aux=aux/10;
		//printf("%d ",vec[nr]);
		nr++;
		
	}
	//printf("\n");
	char* result = calloc(nr+neg,sizeof(char));
	//printf("Converting to string...\n");
	result[0] = '-';
	for(int i = neg;i < (nr + neg) ; i++)
	{
		result[i] = vec[i] + '0';
	}
	//printf("Resulting string...%s\nFreeing pointers...quiting",result);
	free(vec);
	return result;
}	
int checkCtx(struct iio_context *ctx1,struct iio_context *ctx2)
{
	if(!ctx1 || !ctx2)
		return 0;
	int ctx_attr1 = iio_context_get_attrs_count(ctx1);
	char* name1;
	char* value1;
	int ctx_attr2 = iio_context_get_attrs_count(ctx2);
	char* name2;
	char* value2;
	if(ctx_attr1 == ctx_attr2)
	{
		for(int i=0;i < ctx_attr1; i++)
		{
			iio_context_get_attr(ctx1, i, &name1, &value1);
			iio_context_get_attr(ctx2, i, &name2, &value2);
			//printf("for name %s vs %s\n",name1,name2);		
			if((strcmp(name1,"ip,ip-addr") && strcmp(name2,"ip,ip-addr")) 
				&& strcmp(value1,value2))
				return 0;
		}
		return 1;
	}
	else
		return 0;
}
struct iio_device* findMyDevice(char *nameOfDevice)
{

	if(!ctx)
	{
		printf("%s\n","Cannot get context");
		return NULL;
	}

	int ctx_dev = iio_context_get_devices_count(ctx);
	printf("Getting all devices...\n");
	
	struct iio_device* device;
	printf("Looking for devices...\n");
	
	for(int i = 0; i < ctx_dev-1; i++)
	{
		device = iio_context_get_device(ctx, i);
		const char *name = iio_device_get_name(device);
		const char *id = iio_device_get_id(device);
		if(strcmp(nameOfDevice,name) == 0)
		{
			printf("%s was found! The id of the device is : %s\n",name,id);
			
			return device;
		}
		//else printf("Still looking...\n");
	}
	printf("Did not find device\n");

	return NULL;
}
struct iio_channel* findChannel(char *nameOfChann,struct iio_device* device)
{
	
	if(!ctx)
	{
		printf("Invalid device!\n");
		return NULL;
	}
	else printf("Got contxt\n");
	printf("Getting all the channels of the %s\n",iio_device_get_name(device));
	int ctx_chann = iio_device_get_channels_count(device);
	printf("%d channels found:\n",ctx_chann);
	struct iio_channel* chan;
	for(int i = 0; i < ctx_chann-1; i++)
	{
		chan = iio_device_get_channel(device, i);
		const char *name = iio_channel_get_name(chan);
		const char *id = iio_channel_get_id(chan);
		//printf("Found new channel %s %s\n",name,id);
		if(name && strcmp(nameOfChann,name) == 0)
		{
			printf("%s was found! The id of the channel is : %s\n",name,id);
			return chan;
		}
		else if(id && strcmp(nameOfChann,id)==0)
		{
			printf("Channel was found! The id of the channel is : %s\n",id);
			return chan;
		}
		//else printf("Still looking...\n");
	}
	printf("Did not find channel\n");
	return NULL;
}

int findChannelAttr(char *nameOfChannAttr,struct iio_channel* chan)
{
	ctx = iio_device_get_context(iio_channel_get_device(chan));
	FPGActx = iio_create_context_from_uri(URI);
	//printf("Looking for context...\n");
	if(!checkCtx(ctx,FPGActx))
	{
		printf("Invalid device!\n");
		return -1;
	}
	//else printf("Got contxt\n");
	//printf("Getting all the channel attributes of the %s\n",iio_channel_get_name(chan));
	int ctx_chann_attr = iio_channel_get_attrs_count(chan);
	//printf("%d channel attributes found:\n",ctx_chann_attr);
	int errorCode;
	for(int i = 0; i < ctx_chann_attr; i++)
	{
		const char *name = iio_channel_get_attr(chan,i);
		//printf("Found new channel %s \n",name);
		if(name && strcmp(nameOfChannAttr,name) == 0)
		{
			
			//printf("%s was found!  \n",name);
			return 1;
		}
		
		//else printf("Still looking...\n");
	}
	printf("Did not find attribute\n");
	return 0;
}

char* readChannAttr(char *nameOfChannAttr,struct iio_channel* chan)
{
	return readWriteChannAttr(nameOfChannAttr,chan,0,"0");
}

char* writeChannAttr(char *nameOfChannAttr,struct iio_channel* chan,char* src)
{
	return readWriteChannAttr(nameOfChannAttr,chan,1,src);
}

char* readWriteChannAttr(char *nameOfChannAttr,struct iio_channel* chan,int mode,char* src)
{

	char* value = calloc(100, sizeof(char));
	/*if(findChannelAttr(nameOfChannAttr,chan) <= 0 )
	{
		return NULL;
	}*/

	if(mode)
	{
		int errorCode = iio_channel_attr_write(chan,nameOfChannAttr,src);
		if(errorCode < 0)
		{	
			printf("ERROR when writting to %s : %s (%d)\n",nameOfChannAttr,
			strerror(-errorCode),-errorCode);
			return intToChar(-errorCode);
		}
		else
		{
			//printf("wrote to device attr %s with the value %s\n",nameOfChannAttr,src);
			return intToChar(errorCode);
		}
	
	}
	else
	{
		int errorCode = iio_channel_attr_read(chan,nameOfChannAttr,value,100);
		
		if(errorCode > 0)
		{
			//printf("%s was found! The value of the attribute is : %s\n",nameOfChannAttr,value);
			//printf("got here %s\n",value);
			return value;
		}
		else
			printf("Error when reading %s : %s (%d)\n",nameOfChannAttr,
			strerror(-errorCode),-errorCode);
		return NULL;
	}
}
struct iio_context* initCtx()
{
	ctx = iio_create_context_from_uri(URI);
	
	if(!ctx)
	{
		printf("%s\n","Cannot get context");
		return NULL;
	}
	return ctx;
}
char* readDevAttr(char *nameOfDevAttr,struct iio_device* device)
{
	return readWriteDevAttr(nameOfDevAttr,device,0,"0");
}

char* writeDevAttr(char *nameOfDevAttr,struct iio_device* device,char* src)
{
	return readWriteDevAttr(nameOfDevAttr,device,1,src); 
}

char* readWriteDevAttr(char *nameOfDevAttr,struct iio_device* device,int mode,char* src)
{
	
	if(!ctx)
	{
		printf("Invalid device!\n");
		return NULL;
	}
	else printf("Got contxt\n");
	//printf("Getting all the attributes of the %s\n",iio_device_get_name(device));
	int ctx_dev_attr = iio_device_get_attrs_count(device);
	char* value = calloc(100, sizeof(char));
	//printf("%d device-specific attributes found:\n",ctx_dev_attr);
	for(int i = 0; i < ctx_dev_attr-1; i++)
	{
		const char *name = iio_device_get_attr(device,i);
		int errorCode = iio_device_attr_read(device,name,value,100);
		//printf("Found new channel %s %s\n",name,id);
		if(name && strcmp(nameOfDevAttr,name) == 0)
		{
			//printf("%s was found! The value of the attribute is : %s\n",name,value);
			if(mode)
			{
				int errorCodeForWrite = iio_device_attr_write(device,name,src);
				if(errorCodeForWrite < 0)
				{	
					printf("attr %d : %s : ERROR when writting : %s (%d)\n",i,name,
					strerror(-errorCodeForWrite),-errorCodeForWrite);
					return intToChar(-errorCodeForWrite);
				}
				else
				{
					//printf("wrote to device attr %s with the value %s\n",name,src);
					return intToChar(errorCodeForWrite);
				}
			}
			else
				return value;
		}
		//else printf("Still looking...\n");
	}
	printf("Did not find attribute\n");
	return value;
}


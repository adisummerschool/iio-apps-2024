#include <stdio.h>
#include <iio.h>
#include <errno.h>
#include <string.h> 
#define URI "ip:10.76.84.100"
void prettyDisplay(int tabs,int offset)
{
	char* toPrint = calloc((tabs*9-offset),sizeof(char));
	memset(toPrint,' ',(tabs*9-offset));
	printf("%s",toPrint);
	free(toPrint);
}
int main()
{
	unsigned int major;
	unsigned int minor;
	char git_tag[8];
	
	iio_library_get_version(&major,&minor,git_tag);
	printf("%d, %d, %s \n",major,minor,git_tag);
	
	struct iio_context *ctx;
	ctx= iio_create_context_from_uri(URI);
	
	if(!ctx)
	{
		printf("%s\n","Cannot get context");
	}
	
	int ctx_attr = iio_context_get_attrs_count(ctx);
	prettyDisplay(0,0);
	printf("Number of attributes: %d\n",ctx_attr);
	
	char* name = calloc(40, sizeof(char));
	char* value = calloc(100, sizeof(char));
	
	for(int i = 0;i < ctx_attr; i++)
	{
		memset(name, 0, 40);
		memset(value, 0, 100);
		iio_context_get_attr(ctx, i, &name, &value);
		prettyDisplay(1,0);
		printf("Name: %s,Value: %s\n",name, value);
	}
	
	int ctx_dev = iio_context_get_devices_count(ctx);
	printf("Number of devices: %d\n",ctx_dev);
	
	struct iio_device* device;
	
	for(int i = 0;i < ctx_dev; i++)
	{
		device = iio_context_get_device(ctx, i);
		const char *name = iio_device_get_name(device);
		const char *id = iio_device_get_id(device);
		prettyDisplay(1,0);
		printf("%s: %s\n",name, id);
		int ctx_chann = iio_device_get_channels_count(device);
		prettyDisplay(2,4);
		printf("%d channels found:\n",ctx_chann);
		struct iio_channel* chan;
		for(int i = 0;i < ctx_chann; i++)
		{
			chan = iio_device_get_channel(device,i);
			const char *name = iio_channel_get_name(chan);
			const char *id = iio_channel_get_id(chan);
			int ctx_chann_attr = iio_channel_get_attrs_count(chan);
			prettyDisplay(3,5);
			printf("%s: ", id);
			if(name)
			printf("%s",name);
			if(iio_channel_is_output(chan))
			printf(" (output)");
			else
			printf(" (input)");
			printf("\n");
			prettyDisplay(3,2);
			printf("%d attributes found:\n",ctx_chann_attr);
			for(int i = 0;i < ctx_chann_attr; i++)
			{
				const char *name = iio_channel_get_attr(chan,i);
				char* value = calloc(100, sizeof(char));
				memset(value, 0, 100);
				prettyDisplay(3,0);
				int errorCode = iio_channel_attr_read(chan,name,value,100);
				if(errorCode < 0)	
				printf("attr %d : %s : ERROR : %s (%d)\n",i,name,strerror(-errorCode),-errorCode);
				else
				printf("attr %d : %s value : %s \n",i,name,value);
				//strerror(errorCode 
			}	
		}
		int ctx_dev_attr = iio_device_get_attrs_count(device);
		prettyDisplay(2,4);
		printf("%d device-specific attributes found:\n",ctx_dev_attr);
		for(int i = 0 ;i < ctx_dev_attr; i++)
		{
			const char *name = iio_device_get_attr(device,i);
			prettyDisplay(3,0);
			char* value = calloc(100, sizeof(char));
			memset(value, 0, 100);
			int errorCode = iio_device_attr_read(device,name,value,100);
			/*
			if(strcmp("en",name) == 0)
			{
				const char *src = "1";
				int errorCodeForWrite = iio_device_attr_write(device,name,src);
				printf("\n");
				prettyDisplay(3,0);
				printf("Error code for write %d\n",errorCodeForWrite); 
				iio_device_attr_read(device,name,value,100);
				prettyDisplay(3,0);
			}
			*/
			if(errorCode < 0)	
				printf("attr %d : %s : ERROR : %s (%d)\n",i,name,strerror(-errorCode),-errorCode);
				else
				printf("attr %d : %s value : %s \n",i,name,value);
		}
	}
	
	return 0;
	
}

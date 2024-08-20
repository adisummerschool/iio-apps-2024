#ifndef FIND_DEV_H_
#define FIND_DEV_H_
#define URI "ip:10.76.84.100"


struct iio_device* findMyDevice(char *name);
struct iio_channel* findChannel(char *nameOfChann,struct iio_device* device);
int findChannelAttr(char *nameOfChannAttr,struct iio_channel* chan);
char* readWriteChannAttr(char *nameOfChannAttr,struct iio_channel* chan,int mode,char* src);
char* readChannAttr(char *nameOfChannAttr,struct iio_channel* chan);
char* writeChannAttr(char *nameOfChannAttr,struct iio_channel* chan,char* src);
char* readWriteDevAttr(char *nameOfDevAttr,struct iio_device* device,int mode,char* src);
char* readDevAttr(char *nameOfDevAttr,struct iio_device* device);
char* writeDevAttr(char *nameOfDevAttr,struct iio_device* device,char* src);
char* intToChar(int x);

#endif

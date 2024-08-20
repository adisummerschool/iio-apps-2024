#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iio.h>
#include <unistd.h>


#define URI "ip:10.76.84.252"
#define THRESH  10
const int sample_count = 1024;

typedef struct 
{
  uint16_t xpoz;
  uint16_t xneg;
  uint16_t ypoz;
  uint16_t yneg;
  uint16_t zpoz;
  uint16_t zneg;
}buffer_element;

void remove_line(int lines)
{
  for(int i=0; i<lines; i++)
  {
    printf("\033[A");
    printf("\033[K");
  }
}

int main(int argc, char *argv[])
{
  struct iio_context *ctx;
  struct iio_device *dev;
  struct iio_buffer *buf;
  struct iio_channel *chan[5];

  ctx = iio_create_context_from_uri(URI);
  if(!ctx)
  {
      printf("Context unavailable\n");
      return -1;
  }
  printf("Conection established: %s\n", URI);

  dev = iio_context_find_device(ctx, "iio-ad5592r_s");
  if(!dev)
  {
      printf("Device iio-ad5592r_s not found\n");
      return -1;
  }
  iio_device_attr_write(dev, "en", "1");

  char dst[6][100];
  for(int i=0; i<6; i++)
  {
      chan[i] = iio_device_get_channel(dev, i);
      if(!chan[i])
      {
        printf("Channel %d not found\n", i);
        return -1;
      }
      iio_channel_enable(chan[i]);
      //ssize_t dev_attr = iio_channel_attr_read(chan[i], "raw" , dst[i], sizeof(dst));
      //printf("%s : %s\n", channel_name, dst[i]); 
  }

  buf=iio_device_create_buffer(dev, sample_count, false);
  if(!buf)
  {
    printf("Cannot create buffer\n");
    return -1;
  }

 
  uint16_t values[6];
  uint32_t sum[6]={0};

  while(true)
  {
    int bytes_read = iio_buffer_refill(buf);
    for(int i=0; i<6; i++)
    {
      sum[i]=0;
      for(void *ptr = iio_buffer_start(buf); ptr < iio_buffer_end(buf); ptr+=iio_buffer_step(buf))
      {
        values[i] = *(uint16_t*) (ptr + i*sizeof(uint16_t));
        sum[i]+=values[i];
        //printf("%d ", values[i]);
      }
      printf("%f\n", (float)sum[i]/(float)sample_count);
    } 
    remove_line(6);
  }
  return 0;
}


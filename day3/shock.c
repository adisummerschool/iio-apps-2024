#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iio.h>
#include <unistd.h>


#define URI "ip:10.76.84.100"
#define THRESH  10

int main(int argc, char *argv[])
{
  struct iio_context* ctx = iio_create_context_from_uri(URI);

  if (!ctx){
      printf("\n no context\n");
    }
  struct iio_device *dev = iio_context_find_device(ctx, "iio:device0");

  if (!dev){
      printf("\n no device \n");
    }

  iio_device_attr_write(dev, "en", "1");

  struct iio_channel *ch0 = iio_device_find_channel(dev, "voltage0", false);
  struct iio_channel *ch1 = iio_device_find_channel(dev, "voltage1", false);
  struct iio_channel *ch2 = iio_device_find_channel(dev, "voltage2", false);
  struct iio_channel *ch3 = iio_device_find_channel(dev, "voltage3", false);
  struct iio_channel *ch4 = iio_device_find_channel(dev, "voltage4", false);
  struct iio_channel *ch5 = iio_device_find_channel(dev, "voltage5", false);

  char *ch_name[6] = {"voltage0",
                     "voltage1",
                     "voltage2",
                     "voltage3",
                     "voltage4",
                     "voltage5"};


  char initial_str[6][100];

  for (int i = 0; i < 6; i++) {
      iio_channel_attr_read(iio_device_find_channel(dev,ch_name[i], false), "raw", initial_str[i], sizeof(initial_str[i]));
    }

  int init_raw_values[6];

  char detected_str[6][100];
  int detected_raw_values[6];

  for(int i =0 ; i < 6 ; i++) {

      init_raw_values[i] = atoi(initial_str[i]);
      printf("RAW VALUE %d \n", init_raw_values[i]);
    }

  while(true) {

      for (int i = 0; i < 6; i++) {
          iio_channel_attr_read(iio_device_find_channel(dev,ch_name[i], false), "raw", detected_str[i], sizeof(detected_str[i]));

          detected_raw_values[i] = atoi(detected_str[i]);
          printf("DETECTED RAW VALUE %d \n", detected_raw_values[i]);

          if(abs(detected_raw_values[i] - init_raw_values[i]) >= THRESH) {
              printf("\n !!!SHOCK!!!! \n");
              printf("\n RECALIBRATE\n ");
              sleep(5);
              iio_channel_attr_read(iio_device_find_channel(dev,ch_name[i], false), "raw", initial_str[i], sizeof(initial_str[i]));
              init_raw_values[i] = atoi(initial_str[i]);
            }

        }

    }




  return 0;
}



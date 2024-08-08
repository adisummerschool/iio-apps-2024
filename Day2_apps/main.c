#include <iio.h>
#include <stdio.h>
#include <stdlib.h>

#define URI "ip:10.76.84.242"

int main()
{
    unsigned int major;
    unsigned int minor;
    char git_tag[8];
   
    //const char *name;
    //const char *value;

    iio_library_get_version(&major, &minor, git_tag);
    printf("%d,%d, %s\n", major, minor, git_tag);

    struct iio_context * ctx;
    ctx=iio_create_context_from_uri(URI);
    
    if(!ctx){
        printf("%s","cannot get ctx");

    }

    struct iio_device *dev; //Am definit device-ul
    struct iio_channel *ch; //Am facut o strucura pentru channel
    
    int count=iio_context_get_devices_count(ctx); //device ii corespunde contextului
    dev = iio_context_find_device(ctx, "ad5592r_s"); //Am gasit device-ul in functie de context
    ch=iio_device_get_channel(dev, 0); //Am obtinut canale de la device 

    char *at;// Mi-am creat o strucura pentru atribute
    at=iio_channel_get_attr(ch,0);//Am obtinut de la canale atributul 
     iio_device_attr_write(dev, "en", "1");
    for(int i=1;i<=10; i++){
        char val[10];
        char stoc[10];
        sprintf(val, "%d", i);

        iio_channel_attr_write(ch, at, val);
        printf("S-a scris valoarea:%s\n",val);
        iio_channel_attr_read(ch, at,stoc,4);
        printf("Valoarea citita este: %s,\n",stoc);
        sleep(1);

    }

    /*int attributes=iio_context_get_attrs_count(ctx);
    printf("nr of attr: %d\n", attributes);
    for(int i=0; i<attributes;i++){
        iio_context_get_attr(ctx,i,&name,&value);
        printf("nume:%s , val: %s\n\n", name, value);
    }*/
}
#include <iio.h>
#include <stdio.h>

#define URI "ip:10.76.84.242"

int main()
{
    unsigned int major;
    unsigned int minor;
    char git_tag[8];
   
    const char *name;
    const char *value;

    iio_library_get_version(&major, &minor, git_tag);
    printf("%d,%d, %s\n", major, minor, git_tag);

    struct iio_context * ctx;
    ctx=iio_create_context_from_uri(URI);
    
    if(!ctx){
        perror("cannot get ctx");

    }
    int attributes=iio_context_get_attrs_count(ctx);
    printf("nr of attr: %d\n", attributes);

    
    for(int i=0; i<attributes;i++){
        iio_context_get_attr(ctx,i,&name,&value);
        printf("nume:%s , val: %s\n\n", name, value);
    }
}
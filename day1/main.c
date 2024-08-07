#include <iio.h>
#include <stdio.h>

#define URI "ip:10.76.84.247"

int main()
{
    unsigned int major;
    unsigned int minor;
    char git_tag[8]; 

    iio_library_get_version(&major, &minor, git_tag);
    printf("%d, %d, %s\n", major, minor, git_tag);

    struct iio_context *ctx;
    ctx = iio_create_context_from_uri(URI);

    if(!ctx){
        perror("Cannot get context");
    }

    int nr_attr = iio_context_get_attrs_count(ctx);

    printf("Number of attributes: %d\n", nr_attr);

    const char *name;
    const char *value;

    for(int i = 0; i < nr_attr; i++){
        if(iio_context_get_attr(ctx, i,  &name, &value) == 0)
            printf("Name: %s     Value: %s\n\n", name, value);
    }
}
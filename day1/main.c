#include <iio.h>
#include <stdio.h>

#define URI "ip:10.76.84.237"

int main(){
    int i;

    unsigned int major;
    unsigned int minor;
    char git_tag[8];
    
    iio_library_get_version(&major, &minor, git_tag);

    printf("%d, %d, %s\n", major, minor, git_tag);

    struct iio_context * ctx;
    ctx = iio_create_context_from_uri(URI);

    if(!ctx){
        printf("%s", "cannot get ctx");
    }

    int num_attrs = iio_context_get_attrs_count(ctx);
    printf("Number of attributes: %d\n", num_attrs);

    for (int i = 0; i < num_attrs; i++) {
        const char *value;
        const char *name;
        int attr = iio_context_get_attr(ctx, i, &name, &value);
        printf("Attribute %s: %s\n", name, value);
    }

    return 0;
}


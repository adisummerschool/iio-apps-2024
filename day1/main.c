#include <stdio.h>
// #include "lib.h"
#include <iio.h>

#define URI "ip:10.76.84.236"

int main()
{
    unsigned int major;
    unsigned int minor;
    char git_tag[8];
    iio_library_get_version(&major, &minor, git_tag);

    printf("version: %d, %d, %s\n", major, minor, git_tag);

    struct iio_context *ctx = iio_create_context_from_uri(URI);
    if (!ctx)
    {
        printf("cannot get ctx");
    }

    int nr = iio_context_get_attrs_count(ctx);
    printf("nr atribute: %d\n", nr);

    for (int i = 0; i < nr; i++)
    {
        const char *name;
        const char *value;
        int atr = iio_context_get_attr(ctx, i, &name, &value);
        printf("%s = %s\n", name, value);
    }

    return 0;
}
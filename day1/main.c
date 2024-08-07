#define URI "ip:10.76.84.245"

#include <iio.h>
#include <stdio.h>

int main()
{
    unsigned int major, minor;
    char git_tag[8];
    iio_library_get_version(&major, &minor, git_tag);

    printf("major: %d; minor: %d; git_tag: %s\n", major, minor, git_tag);

    struct iio_context *ctx = iio_create_context_from_uri(URI);

    if (!ctx)
    {
        printf("Context unavailable!!!");
        return -1;
    }

    unsigned int attrCount = iio_context_get_attrs_count(ctx);

    for (int i = 0; i < attrCount; i++)
    {
        const char *name, *value;
        iio_context_get_attr(ctx, i, &name, &value);
        printf("name: %s; value: %s\n", name, value);
    }

    return 0;
}
#include <iio.h>
#define URI "ip:10.76.84.253"
int main()
{
    unsigned int major;
    unsigned int minor;
    char git_tag[8];

    iio_library_get_version(&major, &minor, git_tag);
    printf("Library get version: %d, %d, %s\n", major, minor, git_tag);

    struct iio_context *ctx = iio_create_context_from_uri(URI);

    if (!ctx)   {
        printf("%s\n", "Cannot get ctx");
    }
    else
    {
        printf ("%s\n", "Connection established");
    }

    //int iio_context_get_attrs_count(const struct iio_context *ctx);
    //int iio_context_get_attr(const struct iio_context *ctx, unsigned int index, const char **name, const char **value)
    
    int no_attr = iio_context_get_attrs_count(ctx);
    printf("No. of attributes: %d\n", no_attr);

    const char *value, *name;

    for (int i=0; i<no_attr; i++)
    {
        int attr = iio_context_get_attr(ctx, i, &name, &value);
        printf("Attribute %s: %s\n", name, value);
    }

    
    return 0;
}
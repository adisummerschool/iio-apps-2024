#include <iio.h>

#define URI "ip:10.76.84.102"

int main()
{
    unsigned int major;
    unsigned int minor;
    char git_tag[8];

    iio_library_get_version(&major, &minor, git_tag);

    printf("%d, %d, %s\n\n", major, minor, git_tag);

    struct iio_context* ctx;
    ctx = iio_create_context_from_uri(URI);

    if (!ctx)
    {
        perror("cannot get ctx!\n");
    }

    int n = iio_context_get_attrs_count(ctx);

    for (int i = 0; i < n; ++i)
    {
        char* name;
        char* value;

        iio_context_get_attr(ctx, i, &name, &value);
        printf("%d) %s, %s\n", i, name, value);
    }

    return 0;
}
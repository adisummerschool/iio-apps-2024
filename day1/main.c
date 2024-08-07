#include <iio.h>
#include <stdio.h>

#define URI "ip:fdf10.76.84.100"

int main()
{
	unsigned int major;
	unsigned int minor;
	char git_tag[8];
	
	iio_library_get_version(&major, &minor, git_tag);
	printf("%d, %d, %s\n", major, minor, git_tag);
	
	// struct iio_context * iio_create_context_from_uri(const char *uri);
	
	struct iio_context * ctx;
	ctx = iio_create_context_from_uri(URI);
	
	if (!ctx) {
		printf("%s", "cannot get ctx");
	}
	
	// int iio_context_get_attrs_count(const struct iio_context *ctx);
	
	//  int iio_context_get_attr(const struct iio_context *ctx, unsigned int index, const char **name, const char **value);
}

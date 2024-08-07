#include <stdio.h>
#include <iio.h>
#include <string.h>

#define URI "ip:10.76.84.239"


int main()
{
	unsigned int major, minor;
	char git_tag[8];
	iio_library_get_version(&major, &minor, git_tag);

	printf("IIO library version: %d.%d-%s\n", major, minor, git_tag);

	struct iio_context *ctx = iio_create_context_from_uri(URI);

	if (!ctx) {
		perror("No context");
		return -1;
	}

	// iio_context_get_attrs_count(ctx);

	int n = iio_context_get_attrs_count(ctx);

	printf("Number of attributes: %d\n", n);

	for (int i = 0; i < n; i++) {
		const char *value;
		const char *name;
		
		int attr = iio_context_get_attr(ctx, i, &name, &value);
		printf("Attribute %s: %s\n", name, value);
	}

}

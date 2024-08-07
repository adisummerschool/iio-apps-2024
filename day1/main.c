#include <iio.h>
#define URI "ip:10.76.84.104"

int main() {
	unsigned int major;
	unsigned int minor;
	char git_tag[8];

	iio_library_get_version(&major, &minor, git_tag);
	printf("%d\n%d\n%s\n", major, minor, git_tag);
	
	struct iio_context *ctx;
	ctx = iio_create_context_from_uri(URI);
	
	if(!ctx) {
		printf("%s\n", "Cannot get ctx");
	}
	
	
	int ctx_attr = iio_context_get_attrs_count(ctx);
	printf("Count of context attributes: %d\n", ctx_attr);
	
	char* name = calloc(40, sizeof(char));
	char* value = calloc(100, sizeof(char));
	
	for (int i = 0; i < ctx_attr; i++) {
		memset(name, 0, 40);
		memset(value, 0, 100);
		iio_context_get_attr(ctx, i, &name, &value);
		printf("Name: %s,\nValue: %s\n", name, value);
	}
	
	return 0;
}

// https://github.com/rfjakob/fuse/blob/master/example/hello.c

// apt install fuse3 libfuse3-3 libfuse3-dev

// gcc -Wall MyFuse.c `pkg-config fuse3 --cflags --libs` -o myfuse

// ./myfuse mnt/
// cat mnt/hello
// fusermount -u mnt

#define FUSE_USE_VERSION 35

#include <fuse3/fuse.h>
#include <string.h>
#include <errno.h>

static const char *hello_str = "Hello World! leonardo soares de souza \n";
static const char *hello_path = "/hello";

int my_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {

	size_t len;

	if (strcmp(path, hello_path) != 0)
		return -ENOENT;

	len = strlen(hello_str);
	if (offset < len) {
		if (offset + size > len)
			size = len - offset;
		memcpy(buf, hello_str + offset, size);
	} else
		size = 0;

	return size;
}

int my_open(const char *path, struct fuse_file_info *fi) {
	if (strcmp(path, hello_path) != 0)
		return -ENOENT;

	if ((fi->flags & 3) != O_RDONLY)
		return -EACCES;

	return 0;
}

int my_getattr(const char *path, struct stat *stbuf, struct fuse_file_info *fi) {

	int res = 0;

	memset(stbuf, 0, sizeof(struct stat));
	if (strcmp(path, "/") == 0) {
		stbuf->st_mode = S_IFDIR | 0755;
		stbuf->st_nlink = 2;
	} else if (strcmp(path, hello_path) == 0) {
		stbuf->st_mode = S_IFREG | 0444;
		stbuf->st_nlink = 1;
		stbuf->st_size = strlen(hello_str);
	} else
		res = -ENOENT;

	return res;
}

int my_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi, enum fuse_readdir_flags flags) {

	if (strcmp(path, "/") != 0)
		return -ENOENT;

	filler(buf, ".", NULL, 0, 0);
	filler(buf, "..", NULL, 0, 0);
	filler(buf, hello_path + 1, NULL, 0, 0);

	return 0;
}

int main(int argc, char *argv[]) {

	struct fuse_operations hello_oper = {
			.getattr = my_getattr,
			.readdir = my_readdir,
			.open = my_open,
			.read = my_read };

	return fuse_main(argc, argv, &hello_oper, (void* ) 0);
}

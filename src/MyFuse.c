// https://github.com/rfjakob/fuse/blob/master/example/hello.c

// apt install fuse3 libfuse3-dev

// gcc -Wall MyFuse.c `pkg-config fuse3 --cflags --libs` -o myfuse

// fusermount -u mnt; gcc -Wall MyFuse.c `pkg-config fuse3 --cflags --libs` -o myfuse && ./myfuse mnt/

// ./myfuse mnt/
// cat mnt/hello
// fusermount -u mnt

#define FUSE_USE_VERSION 35

#include <fuse3/fuse.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

char hello_str[ 65535 ];
char *hello_path = "/hello";

int my_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {

	memcpy( &hello_str[ offset ], buf, size );

	return size;
}

int my_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {

	if (strcmp(path, hello_path) != 0)
		return -ENOENT;

	memcpy(buf, &hello_str + offset, size);

	return size;
}

int my_truncate(const char *path, off_t offset, struct fuse_file_info *fi) {

	memset( &hello_str[ offset ], 0, strlen(hello_str) - offset );

	return 0;
}

int my_open(const char *path, struct fuse_file_info *fi) {

	if (strcmp(path, hello_path) != 0)
		return -ENOENT;

	return 0;
}

int my_getattr(const char *path, struct stat *stbuf, struct fuse_file_info *fi) {

	stbuf->st_uid = getuid();
	stbuf->st_gid = getgid();
	stbuf->st_atime = stbuf->st_mtime = time(NULL);

	memset(stbuf, 0, sizeof(struct stat));
	if (strcmp(path, "/") == 0) {
		stbuf->st_mode = S_IFDIR | 0755;
		stbuf->st_nlink = 2;
	} else if (strcmp(path, hello_path) == 0) {
		stbuf->st_mode = S_IFREG | 0666;
		stbuf->st_nlink = 1;
		stbuf->st_size = strlen(hello_str);
	} else
		return -ENOENT;

	return 0;
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
			.getattr 	= my_getattr,
			.readdir 	= my_readdir,
			.open 		= my_open,
			.write 		= my_write,
			.truncate	= my_truncate,
			.read 		= my_read };

	return fuse_main(argc, argv, &hello_oper, (void* ) 0);
}

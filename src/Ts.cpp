#include <iostream>
#include <string.h>

#include <fstream>

#include <unistd.h>
#include <linux/sysctl.h>

int main() {

	std::ifstream tt("/proc/sys/vm/dirty_expire_centisecs");

	char buff[16];
	tt.read(buff, sizeof(buff));

	std::cout << strlen(buff);

	std::cout << "end" << std::endl;

	return 0;
}

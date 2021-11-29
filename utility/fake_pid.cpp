#include <cstring>
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>


int main(const int argc, const char* argv[])
{
	uint64_t fake_pid;
	const char* fake_pid_str = "87654321";

	memcpy((char*)&fake_pid, fake_pid_str, sizeof(uint64_t));
	std::cout << fake_pid << std::endl;

	int fp = open("/dev/simple_driver", O_RDWR);
	write(fp, fake_pid_str, sizeof(uint64_t));
	close(fp);

	return 0;
}

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

// helper function to handle the open-write-close POSIX routine
// writes a value into the linux sysfs file or any other file as the arguements are path and value
void write_sysfs(const char *path, const char *value){
	
	// fd is file descriptor integer that kernel gives to the opened file, it is a non negative nos
	// and O_WRONLY is 'Open File with Write Only'
	int fd = open(path, O_WRONLY);
	if (fd < 0){
		perror("open");	// perror prints the err like open: No such file or directory
		return;
	}

	// ssize_t is a signed integer type suitable for this return value because
	// write() returns number of bytes written or -1 if an err occurs
	ssize_t bytes_written = write(fd, value, strlen(value));
	if(bytes_written < 0){
		perror("write");
	}

	// close file descriptor because opening a file consumes a kernel managed resources
	close(fd); 
}

int main() {
	printf("stripping kernel control (setting trigger to none)..\n");
	write_sysfs("/sys/class/leds/work/trigger","none");

	printf("driving led high..\n");
	write_sysfs("/sys/class/leds/work/brightness", "255");

	sleep(2);

	printf("drivin led low..\n");
	write_sysfs("/sys/class/leds/work/brightness", "0");

	return 0;
}

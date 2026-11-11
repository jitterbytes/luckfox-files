#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/epoll.h>

#define GPIO_PIN "55"
#define MAX_EVENTS 1

// helper fnc to write value to sys path
void write_sysfs(const char *path, const char *value) {
    
    int fd = open(fd,O_WRONLY);
    if(fd < 0) {
        perror("Err opening sysfs path");
        exit(1);
    }

    ssize_t bytes_written = write(fd, value, strlen(value));
    if(bytes_written < 0) {
        perror("Err writing sysfs path");
    }

    close(fd);
}

int main() {
    printf("Configuring GPIO %s as Input with Interrupts...\n", GPIO_PIN);

    // In linux kernel all the gpios are not available for userspace we have to export it and then unexport 

    // 1. Export the pin and set the direction to input
    // The input of the function will be 55 and kernel will automatically create sub directories
    write_sysfs("/sys/class/gpio/export", GPIO_PIN);    
    write_sysfs("/sys/class/gpio/gpio55/direction", "in");

    // 2. Configure the edge trigger (interrupt on both rising and falling edges)
    write_sysfs()
}
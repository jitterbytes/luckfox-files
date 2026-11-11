#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#define GPIO_PIN "55"

void write_sysfs(const char *path, const char *value) {

    int fd = open(path, O_WRONLY);
    if (fd < 0) {
        perror("Error opening sysfs path");
        exit(1);
    }

    ssize_t bytes_written = write(fd, value, strlen(value));
    if (bytes_written < 0) {
        perror("Error writing sysfs path");
        close(fd);
        exit(1);
    }

    close(fd);
}

int main() {

    // 1. Export GPIO
    write_sysfs("/sys/class/gpio/export", GPIO_PIN);

    // 2. Set direction
    write_sysfs("/sys/class/gpio/gpio55/direction", "out");

    // 3. Toggle 10 times
    for (int i = 0; i < 10; i++) {

        write_sysfs("/sys/class/gpio/gpio55/value", "1");
        sleep(1);

        write_sysfs("/sys/class/gpio/gpio55/value", "0");
        sleep(1);
    }

    // 4. Release the hardware lock back to the kernel
    write_sysfs("/sys/class/gpio/unexport", GPIO_PIN);

    return 0;
}

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
    
    int fd = open(path,O_WRONLY);
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
    write_sysfs("/sys/class/gpio/gpio55/edge", "both");

    // 3. Open the value file descriptor to monitor
    int fd = open("/sys/class/gpio/gpio55/value", O_RDONLY | O_NONBLOCK);
    if (fd < 0) {
        perror("Err opening gpio file");
        return 1;
    }

    // 4. Set up the epoll instance
    int epfd = epoll_create1(0);
    struct epoll_event ev, events[MAX_EVENTS];

    // Tell epoll to monitor our file descriptor for priotized data (EPOLLPRI) and edge triggered (EPOLLET)
    ev.events = EPOLLPRI | EPOLLET;
    ev.data.fd = fd;
    epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev);

    // We must do a dummy read to clear the initial state before waiting
    char buffer[2];
    read(fd, buffer, sizeof(buffer));

    printf("Application sleeping. Waiting for hardware interrupts...\n");

    // 5. The blocking event loop
    while (1) {
        // epoll_wait blocks execution completely (0% CPU) until the file changes
        int n = epoll_wait(epfd, events, MAX_EVENTS, -1);

        if (n > 0) {
            // Rewind the file pointer to the beginning to read the new value
            lseek(fd, 0, SEEK_SET);
            read(fd, buffer, sizeof(buffer));

            // Trim the newline character and print
            buffer[1] = '\0';
            printf("Hardware Interrupt Received! Pin State: %s\n", buffer);
        }
    }
    
    close(fd);
    return 0;
}
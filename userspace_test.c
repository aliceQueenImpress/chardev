/**
 * @file main.c
 * @brief Testing in usersace the driver fan_chardev
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>
#include <errno.h>

#include "chardev.h"
#include "ioctl.h"

void print_config(const char *title, struct fan_chardev_struct *cfg) {
    printf("--- %s ---\n", title);
    printf("Speed     : %d%%\n", cfg->speed);
    printf("Auto Mode : %s\n", cfg->auto_mode ? "(ON)" : "(OFF)");
    printf("Profile   : '%s'\n\n", cfg->profile);
}

int main(int argc, char *argv[]) {
    int fd;
    struct fan_chardev_struct config;

    // Open the device caracter file under /dev/fan
    fd = open("/dev/fan", O_RDWR);
    if (fd < 0) {
        fprintf(stderr, "Error : Cannot open /dev/fan (errno: %d - %s)\n", errno, strerror(errno));
        fprintf(stderr, "do you create the node with 'mknod' and with root rights (sudo) ?\n");
        return EXIT_FAILURE;
    }

    printf("[+] device /dev/fan successfull open.\n\n");

    // FAN_IOCTL_GET_CONFIG  testing with openning
    if (ioctl(fd, FAN_IOCTL_GET_CONFIG, &config) < 0) {
        perror("[-] Failed with FAN_IOCTL_GET_CONFIG");
        close(fd);
        return EXIT_FAILURE;
    }
    print_config("Initial Config (KERNEL)", &config);

    // FAN_IOCTL_SET_CONFIG testing with modification of the configuration
    config.speed = 75;
    config.auto_mode = 0; // Mannaul mode
    strncpy(config.profile, "Performance", sizeof(config.profile) - 1);
    config.profile[sizeof(config.profile) - 1] = '\0'; // security

    printf("[*] Envoi de la nouvelle configuration...\n");
    if (ioctl(fd, FAN_IOCTL_SET_CONFIG, &config) < 0) {
        perror("[-] Failed with FAN_IOCTL_SET_CONFIG");
        close(fd);
        return EXIT_FAILURE;
    }

    printf("[+] Setting new configuration.\n\n");

    if (ioctl(fd, FAN_IOCTL_GET_CONFIG, &config) < 0) {
        perror("[-] Failled on  GET_CONFIG checking");
        close(fd);
        return EXIT_FAILURE;
    }
    print_config("Check after modification", &config);

    // FAN_IOCTL_RESET testing
    printf("[*] send the RESET...\n");
    if (ioctl(fd, FAN_IOCTL_RESET, NULL) < 0) {
        perror("[-] Failled with FAN_IOCTL_RESET");
        close(fd);
        return EXIT_FAILURE;
    }
    printf("[+] Reset done.\n\n");

    if (ioctl(fd, FAN_IOCTL_GET_CONFIG, &config) < 0) {
        perror("[-] Failled with GET_CONFIG after the  reset");
        close(fd);
        return EXIT_FAILURE;
    }
    print_config("Configuration after the RESET", &config);

    close(fd);
    printf("[+] close device. Test finished with success !\n");

    return EXIT_SUCCESS;
}

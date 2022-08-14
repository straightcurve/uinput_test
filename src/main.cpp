#include <cstdio>
#include <cstdlib>
#include <iostream>

#include <linux/input-event-codes.h>
#include <linux/uinput.h>
#include <fcntl.h>
#include <cstring>

#include <csignal>

__sig_atomic_t fd;
__sig_atomic_t application_should_exit = false;
__sig_atomic_t exit_code = 0;

void handle_sigterm(int signal_code) {
    application_should_exit = true;
    fd = 0;
}

int main (int argc, char *argv[])
{
    fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
    if(fd < 0) exit(EXIT_FAILURE);

    signal (SIGINT, handle_sigterm);
    signal (SIGTERM, handle_sigterm);

    ioctl(fd, UI_SET_EVBIT, EV_KEY);
    ioctl(fd, UI_SET_EVBIT, EV_SYN);
    ioctl(fd, UI_SET_KEYBIT, KEY_D);

    ioctl(fd, UI_SET_EVBIT, EV_ABS);
    ioctl(fd, UI_SET_ABSBIT, ABS_X);
    ioctl(fd, UI_SET_ABSBIT, ABS_Y);

    struct uinput_user_dev uidev;
    memset(&uidev, 0, sizeof(uidev));

    snprintf(uidev.name, UINPUT_MAX_NAME_SIZE, "Test Virtual Gamepad");
    uidev.id.bustype = BUS_USB;
    uidev.id.vendor  = 0x1234;
    uidev.id.product = 0xfedc;
    uidev.id.version = 1;
    uidev.absmin[ABS_X] = 0;
    uidev.absmax[ABS_X] = 1023;

    write(fd, &uidev, sizeof(uidev));

    if (ioctl(fd, UI_DEV_CREATE) < 0)
        application_should_exit = true;

    char c = '\0';
    int i = 0;
    while (!application_should_exit) {
        i++;
        std::cin >> c;

        application_should_exit = application_should_exit || c == 'q';
        if (application_should_exit) goto cleanup;

        struct input_event ev[3];

        memset(ev, 0, sizeof(ev));

        ev[0].type = EV_ABS;
        ev[0].code = ABS_X;
        ev[0].value = (1023 + i) & 1023;
        ev[1].type = EV_ABS;
        ev[1].code = ABS_Y;
        ev[1].value = (767 + i) & 1023;
        ev[2].type = EV_SYN;
        ev[2].code = SYN_REPORT;
        ev[2].value = 0;

        if (write(fd, ev, sizeof(ev)) < 0)
            goto cleanup;
    }

cleanup:
    if (fd > 0) {
        ioctl(fd, UI_DEV_DESTROY);
        close(fd);
    }

    if (exit_code != EXIT_SUCCESS)
        std::cerr << '\n' << strerror(exit_code) << '\n';

    return exit_code;
}

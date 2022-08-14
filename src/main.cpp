#include <virtual-device.hpp>

void handle_sigterm(int signal_code) {
    //  fds are closed by kernel on SIGINT/SIGTERM
    exit(errno);
}

void handle_exit() {
    if (errno != EXIT_SUCCESS)
        std::cerr << '\n' << strerror(errno) << '\n';

    exit(errno);
}

int main (int argc, char *argv[])
{
    std::atexit(handle_exit);
    signal (SIGINT, handle_sigterm);
    signal (SIGTERM, handle_sigterm);

    auto vd = VirtualDevice::make_ps5_gamepad();

    char c = '\0';
    while (1) {
        std::cin >> c;

        if (c == 'q') exit(0);

        struct input_event ev[3];
        memset(ev, 0, sizeof(ev));

        if (c == 'w') {
            ev[0].type = EV_ABS;
            ev[0].code = ABS_Y;
            ev[0].value = 255;
        }

        else if (c == 's') {
            ev[0].type = EV_ABS;
            ev[0].code = ABS_Y;
            ev[0].value = 0;
        }

        else if (c == 'a') {
            ev[0].type = EV_ABS;
            ev[0].code = ABS_X;
            ev[0].value = 0;
        }

        else if (c == 'd') {
            ev[0].type = EV_ABS;
            ev[0].code = ABS_X;
            ev[0].value = 255;
        }

        ev[1].type = EV_SYN;
        ev[1].code = SYN_REPORT;
        ev[1].value = 0;

        if (write(vd.get()->fd, ev, sizeof(ev)) < 0)
            exit(errno);
    }
}

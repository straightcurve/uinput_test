#include <virtual-device.hpp>

void handle_sigterm(int signal_code) {
    //  fds are closed by kernel on SIGINT/SIGTERM
    exit(errno);
}

void handle_exit() {
    if (errno != EXIT_SUCCESS)
        std::cerr << "\nERROR: " << strerror(errno) << '\n';

    exit(errno);
}

int main (int argc, const char *argv[])
{
    std::atexit(handle_exit);
    signal (SIGINT, handle_sigterm);
    signal (SIGTERM, handle_sigterm);

    if (argc < 2) {
        std::cout << "pass device fd\n";
        errno = ENODEV;
        exit(errno);
    }

    std::string rd_path = argv[1];

    int fd = open(rd_path.c_str(), O_RDONLY | O_NONBLOCK);
    if (fd < 0) exit(errno);

    struct libevdev *dev;
    int rc = libevdev_new_from_fd(fd, &dev);
    if (rc < 0) {
        errno = -rc;
        exit(errno);
    }

    printf("Device: %s\n", libevdev_get_name(dev));
    printf("Vendor: %x Product: %x\n",
           libevdev_get_id_vendor(dev),
           libevdev_get_id_product(dev));

    struct input_event read_ev;

    auto wd = VirtualDevice::make_ps5_gamepad();

    while (1) {
        rc = libevdev_next_event(dev, LIBEVDEV_READ_FLAG_NORMAL, &read_ev);
        if (rc < 0 && rc != -EAGAIN) {
            errno = -rc;
            exit(errno);
        }

        struct input_event ev[3];
        memset(ev, 0, sizeof(ev));

        int i = 0;

        __u16 abs_x = 127;
        if (libevdev_has_event_code(dev, EV_KEY, KEY_A) || libevdev_has_event_code(dev, EV_KEY, KEY_D)) {
            abs_x += libevdev_get_event_value(dev, EV_KEY, KEY_A) * -127;
            abs_x += libevdev_get_event_value(dev, EV_KEY, KEY_D) * 128;
        }

        ev[i].type = EV_ABS;
        ev[i].code = ABS_X;
        ev[i].value = abs_x;
        ++i;

        __u16 abs_y = 127;
        if (libevdev_has_event_code(dev, EV_KEY, KEY_S) || libevdev_has_event_code(dev, EV_KEY, KEY_W)) {
            abs_y += libevdev_get_event_value(dev, EV_KEY, KEY_S) * -127;
            abs_y += libevdev_get_event_value(dev, EV_KEY, KEY_W) * 128;
        }

        ev[i].type = EV_ABS;
        ev[i].code = ABS_Y;
        ev[i].value = abs_y;
        ++i;

        ev[i].type = EV_SYN;
        ev[i].code = SYN_REPORT;
        ev[i].value = 0;

        if (write(wd.get()->fd, ev, sizeof(ev)) < 0)
            exit(errno);
    }
}

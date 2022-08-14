#include <pch.hpp>
#include <virtual-device.hpp>

Scope<VirtualDevice> VirtualDevice::make(const std::function<void(VirtualDevice* vd)>& setup) {
    auto vd = makeScope<VirtualDevice>();
    auto vd_ptr = vd.get();

    if (vd_ptr->fd == 0) {
        vd_ptr->fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
        if(vd_ptr->fd < 0) exit(EXIT_FAILURE);

        ioctl(vd_ptr->fd, UI_SET_EVBIT, EV_SYN);
    }

    setup(vd_ptr);

    if (ioctl(vd_ptr->fd, UI_DEV_CREATE) < 0) {
        std::cerr << "[FAILED] device creation!";
        exit(EXIT_FAILURE);
    }

    return vd;
}

void VirtualDevice::enable_sticks(const std::vector<int> &sticks) {
    ioctl(fd, UI_SET_EVBIT, EV_ABS);
    for (const auto &s : sticks)
        ioctl(fd, UI_SET_ABSBIT, s);
}

void VirtualDevice::enable_keys(const std::vector<int> &keys) {
    ioctl(fd, UI_SET_EVBIT, EV_KEY);
    for (const auto &k : keys)
        ioctl(fd, UI_SET_KEYBIT, k);
}

Scope<VirtualDevice> VirtualDevice::make_ps5_gamepad() {
    return make([](VirtualDevice* vd_ptr) {
        vd_ptr->enable_keys({
            BTN_SOUTH,
            BTN_WEST,
            BTN_NORTH,
            BTN_EAST,
            BTN_TL,
            BTN_TR,
            BTN_TL2,
            BTN_TR2,
            BTN_MODE,
            BTN_SELECT,
            BTN_START,
            BTN_THUMBL,
            BTN_THUMBR,
        });

        vd_ptr->enable_sticks({
            ABS_X,
            ABS_Y,
            ABS_RX,
            ABS_RY,
            ABS_Z,
            ABS_RZ,
            ABS_HAT0X,
            ABS_HAT0Y,
        });

        struct uinput_user_dev device;
        std::memset(&device, 0, sizeof(device));

        std::snprintf(device.name, UINPUT_MAX_NAME_SIZE, "Sony Interactive Entertainment Wireless Controller");
        device.id.bustype = BUS_USB;
        device.id.vendor  = 0x54c;
        device.id.product = 0xce6;
        device.id.version = 0x8111;
        device.absmin[ABS_X] = 0;
        device.absmax[ABS_X] = 255;
        device.absmin[ABS_Y] = 0;
        device.absmax[ABS_Y] = 255;
        device.absmin[ABS_Z] = 0;
        device.absmax[ABS_Z] = 255;
        device.absmin[ABS_RX] = 0;
        device.absmax[ABS_RX] = 255;
        device.absmin[ABS_RY] = 0;
        device.absmax[ABS_RY] = 255;
        device.absmin[ABS_RZ] = 0;
        device.absmax[ABS_RZ] = 255;
        device.absmin[ABS_HAT0X] = -1;
        device.absmax[ABS_HAT0X] = 1;
        device.absmin[ABS_HAT0Y] = -1;
        device.absmax[ABS_HAT0Y] = 1;

        write(vd_ptr->fd, &device, sizeof(device));
    });
}

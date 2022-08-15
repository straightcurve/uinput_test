#include <pch.hpp>
#include <core.hpp>

class VirtualDevice {
private:

    explicit VirtualDevice(int fd): fd(fd) {}

public:
    int fd;
    VirtualDevice(): VirtualDevice(0) {}

    virtual ~VirtualDevice() {
        if (fd > 0) {
            ioctl(fd, UI_DEV_DESTROY);
            close(fd);
        }
    }

    static Scope<VirtualDevice> make(const std::function<void(VirtualDevice* vd)>& setup);
    static Scope<VirtualDevice> make_ps5_gamepad();

    void enable_keys(const std::vector<int> &keys);
    void enable_sticks(const std::vector<int> &sticks);
};

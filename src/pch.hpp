#include <cerrno>
#include <csignal>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <functional>
#include <iostream>
#include <vector>

#include <fcntl.h>
#include <linux/input-event-codes.h>
#include <linux/uinput.h>
#include <unistd.h>

#include <libevdev-1.0/libevdev/libevdev.h>

#define KENGINE_LINUX
#define KENGINE_IMPLEMENTATION
#include "kengine_linux.h"

int main(int argc, char *argv[])
{
    PlatformConsoleOut("Before %d after\n", 42);

    return 0;
}
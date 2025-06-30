#include "pico/stdlib.h"
#include "ushell_core.h"

int main()
{
    stdio_init_all();

    Microshell::getShellPtr(pluginEntry(), "root")->Run();
    return 0;
}

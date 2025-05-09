#include "gui_handler.hpp"

int main(int, char**)
{
    GuiHandler gui;
    gui.init();
    gui.start_main_loop();
    return 0;
}

#include "gui_handler.hpp"

int main(int, char**) {
  GuiHandler gui;
  if (!gui.init()) {
    return -1;
  }
  gui.start_main_loop();
  return 0;
}

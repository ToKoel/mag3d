#include <exception>
#include <iostream>
#include <ostream>

#include "gui_handler.hpp"

int main(int, char**) {
  GuiHandler gui;
  try {
    gui.init();
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return -1;
  }
  gui.start_main_loop();
  return 0;
}

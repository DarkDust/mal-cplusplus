#include <iostream>
#include <string>
import mal;

std::string read(const std::string &line) { return line; }

std::string eval(const std::string &line) { return line; }

std::string print(const std::string &line) { return line; }

bool rep() {
  auto line = mal::read_line();
  if (!line) {
    return false;
  }

  auto intermediate_read = read(*line);
  auto intermediate_eval = eval(intermediate_read);
  auto intermediate_print = print(intermediate_eval);
  std::cout << intermediate_print << std::endl;
  return true;
}

int main(int argc, char **argv) {
  while (rep())
    ;
  return 0;
}
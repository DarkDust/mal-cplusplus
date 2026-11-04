#include <iostream>
#include <string>
import mal;

std::optional<mal::ConstNodePtr>
read(const mal::NonNullSharedPtr<mal::Location::File> &file, int line_number,
     const std::string &line) {
  auto reader = mal::Reader(file, line_number, line);
  auto result = reader.read_form();
  return result;
}

std::optional<mal::ConstNodePtr>
eval(const std::optional<mal::ConstNodePtr> &node) {
  return node;
}

std::string print(const std::optional<mal::ConstNodePtr> &node) {
  if (!node) {
    return "";
  }
  return (*node)->print();
}

bool rep(const mal::NonNullSharedPtr<mal::Location::File> &file,
         int line_number) {
  auto line = mal::read_line();
  if (!line) {
    return false;
  }

  try {
    auto intermediate_read = read(file, line_number, *line);
    auto intermediate_eval = eval(intermediate_read);
    auto intermediate_print = print(intermediate_eval);
    std::cout << intermediate_print << std::endl;
  } catch (mal::ParseError error) {
    std::cerr << error.what() << std::endl;
  }
  return true;
}

int main(int argc, char **argv) {
  try {

    auto file = mal::make_nonnull_shared<mal::Location::File>("<stdin>");
    int line_number = 1;

    while (rep(file, line_number++)) {
    }
    return EXIT_SUCCESS;
  } catch (const std::exception &error) {
    std::cerr << "Fatal error: " << error.what() << std::endl;
  } catch (...) {
    std::cerr << "Fatal error: unknown exception" << std::endl;
  }

  return EXIT_FAILURE;
}
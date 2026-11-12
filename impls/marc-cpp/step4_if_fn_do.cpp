#include <iostream>
#include <optional>
#include <string>
import mal;

std::optional<mal::ConstNodePtr>
read(const mal::NonNullSharedPtr<mal::Location::File> &file, int line_number,
     const std::string &line) {
  auto reader = mal::Reader(file, line_number, line);
  auto result = reader.read_form();
  return result;
}

std::string print(const mal::ConstNodePtr &node) { return node->print(); }

bool rep_line(const mal::NonNullSharedPtr<mal::Location::File> &file,
              int line_number, const mal::EnvironmentPtr &env,
              const mal::Evaluator &evaluator, const std::string &line) {
  try {
    auto intermediate_read = read(file, line_number, line);
    if (intermediate_read) {
      auto intermediate_eval = evaluator.eval(*intermediate_read, env);
      auto intermediate_print = print(intermediate_eval);
      std::cout << intermediate_print << std::endl;
    }
  } catch (mal::ParseError error) {
    std::cerr << error.print() << std::endl;
  } catch (mal::RuntimeError error) {
    std::cerr << error.print() << std::endl;
  }
  return true;
}

bool rep(const mal::NonNullSharedPtr<mal::Location::File> &file,
         int line_number, const mal::EnvironmentPtr &env,
         const mal::Evaluator &evaluator) {
  auto line = mal::read_line();
  if (!line) {
    return false;
  }

  return rep_line(file, line_number, env, evaluator, *line);
}

int main(int argc, char **argv) {
  try {
    auto env = mal::make_nonnull_shared<mal::Environment>();
    env->populate_with_default();

    auto evaluator = mal::Evaluator();

    auto builtin = mal::make_nonnull_shared<mal::Location::File>("<builtin>");
    rep_line(builtin, 1, env, evaluator,
             "(def! not (fn* (a) (if a false true)))");

    auto file = mal::make_nonnull_shared<mal::Location::File>("<stdin>");
    int line_number = 1;

    while (rep(file, line_number++, env, evaluator)) {
    }
    return EXIT_SUCCESS;
  } catch (const std::exception &error) {
    std::cerr << "Fatal error: " << error.what() << std::endl;
  } catch (...) {
    std::cerr << "Fatal error: unknown exception" << std::endl;
  }

  return EXIT_FAILURE;
}
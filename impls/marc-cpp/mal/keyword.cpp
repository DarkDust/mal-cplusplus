module;

#include <string>

module mal;

using namespace mal;

Keyword::Keyword(const std::string &str) {
  if (str.starts_with(':')) {
    this->keyword = str.substr(1);
  } else {
    this->keyword = str;
  }
}

std::string Keyword::print() const { return ":" + this->keyword; }
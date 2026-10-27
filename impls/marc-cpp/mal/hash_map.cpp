module;

#include <string>

module mal;

using namespace mal;

HashMap::HashMap() : map{} {}

void HashMap::set(std::shared_ptr<const Hashable> key,
                  std::shared_ptr<Node> val) {
  this->map.emplace(key, val);
}

std::string HashMap::print() const {
  std::string output = "{";

  bool is_first = true;
  for (const auto &[key, val] : map) {
    if (is_first) {
      is_first = false;
    } else {
      output += " ";
    }

    output += key->print();
    output += " ";
    output += val->print();
  }

  output += "}";
  return output;
}
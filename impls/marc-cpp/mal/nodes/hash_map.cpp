module;

#include <string>

module mal;

using namespace mal;

HashMap::HashMap(const ConstLocationRangePtr &location)
    : Node(location), map{} {}

void HashMap::set(const NonNullSharedPtr<const Hashable> &key,
                  const ConstNodePtr &val) {
  this->map.emplace(key, val);
}

bool HashMap::equals(const NonNullSharedPtr<const Node> &maybe_other) const {
  // TODO: Implement me.
  return false;
};

std::string HashMap::print() const {
  std::string output = "{";

  bool is_first = true;
  for (const auto &[key, val] : map) {
    if (is_first) {
      is_first = false;
    } else {
      output += ' ';
    }

    output += key->print();
    output += ' ';
    output += val->print();
  }

  output += '}';
  return output;
}
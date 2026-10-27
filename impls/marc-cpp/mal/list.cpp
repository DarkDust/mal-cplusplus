module;

#include <memory>
#include <string>
#include <vector>

module mal;

using namespace mal;

List::List(Kind kind) : nodes{}, kind(kind) {}

std::string List::print() const {
  std::string opening = kind == Kind::List ? "(" : "[";
  std::string closing = kind == Kind::List ? ")" : "]";

  std::string result = opening;

  bool is_first = true;
  for (auto it : nodes) {
    if (is_first) {
      is_first = false;
    } else {
      result += " ";
    }
    result += it->print();
  }

  result += closing;
  return result;
}

void List::append(std::shared_ptr<Node> node) { this->nodes.push_back(node); }
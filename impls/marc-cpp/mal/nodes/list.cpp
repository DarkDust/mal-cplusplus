module;

#include <algorithm>
#include <string>
#include <vector>

module mal;

using namespace mal;

List::List(const ConstLocationRangePtr &location, Kind kind)
    : Node(location), nodes{}, kind(kind) {}

List::List(const ConstLocationRangePtr &location, Kind kind,
           const std::vector<ConstNodePtr> &nodes)
    : Node(location), nodes(nodes), kind(kind) {}

bool List::equals(const NonNullSharedPtr<const Node> &maybe_other) const {
  const auto &other = maybe_other.cast<const List>();
  if (!other) {
    return false;
  }

  return std::ranges::equal(
      this->nodes, (*other)->nodes,
      [](const auto &my, const auto &other) { return my->equals(other); });
};

std::string List::print() const {
  std::string opening = kind == Kind::List ? "(" : "[";
  std::string closing = kind == Kind::List ? ")" : "]";

  std::string result = opening;

  bool is_first = true;
  for (auto &it : nodes) {
    if (is_first) {
      is_first = false;
    } else {
      result += ' ';
    }
    result += it->print();
  }

  result += closing;
  return result;
}

void List::append(const ConstNodePtr &node) { this->nodes.push_back(node); }

const List::Kind List::get_kind() const { return this->kind; }

const std::vector<ConstNodePtr> &List::get_nodes() const { return this->nodes; }
module mal;

using namespace mal;

Node::Node(const ConstLocationRangePtr &location) : location(location) {}
Node::~Node() = default;

const ConstLocationRangePtr &Node::get_location() const {
  return this->location;
}

void Node::set_location(const ConstLocationRangePtr &location) {
  this->location = location;
}

bool Node::is_trueish() const { return true; }
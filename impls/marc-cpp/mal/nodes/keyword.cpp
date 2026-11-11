module;

#include <string>

module mal;

using namespace mal;

Keyword::Keyword(const ConstLocationRangePtr &location, const std::string &str)
    : Hashable(location) {
  if (str.starts_with(':')) {
    this->keyword = str.substr(1);
  } else {
    this->keyword = str;
  }
}

bool Keyword::equals(const NonNullSharedPtr<const Node> &maybe_other) const {
  auto other = maybe_other.cast<const Keyword>();
  if (!other) {
    return false;
  }

  return (*other)->keyword == this->keyword;
};

std::string Keyword::print() const { return ":" + this->keyword; }
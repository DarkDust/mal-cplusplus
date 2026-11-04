module;

#include "linenoise.hpp"
#include <optional>
#include <regex>
#include <string>
#include <vector>

module mal;

using namespace mal;

std::optional<std::string> mal::read_line() {
  std::string line;
  if (linenoise::Readline("user> ", line)) {
    return std::nullopt;
  }
  return line;
}

struct Token {
  std::string value;
  ConstLocationRangePtr location;
};

struct Reader::Impl {
  NonNullSharedPtr<Location::File> file;
  int line_number;
  std::vector<Token> tokens;
  std::vector<Token>::iterator pos;
  NonNullSharedPtr<const LocationRange> last_location;

  Impl(const NonNullSharedPtr<Location::File> &file, int line_number,
       const std::string &line)
      : file(file), line_number(line_number),
        last_location(LocationRange::create_virtual()) {
    std::regex pattern(
        "[\\s,]*(~@|[\\[\\]{}()'`~^@]|\"(?:\\\\.|[^\\\\\"])*\"?|;."
        "*|[^\\s\\[\\]{}('\"`,;)]*)");

    auto tokens_begin =
        std::sregex_token_iterator(line.begin(), line.end(), pattern, 1);
    auto tokens_end = std::sregex_token_iterator();

    for (auto it = tokens_begin; it != tokens_end; ++it) {
      auto str = it->str();
      if (str.length() == 0) {
        continue;
      }

      auto start = std::distance(line.cbegin(), it->first) + 1;
      auto end = std::distance(line.cbegin(), it->second);
      auto start_loc =
          make_nonnull_shared<Location>(this->file, this->line_number, start);
      auto end_loc =
          make_nonnull_shared<Location>(this->file, this->line_number, end);

      Token token = {
          .value = it->str(),
          .location = make_nonnull_shared<LocationRange>(start_loc, end_loc)};
      this->tokens.push_back(token);
    }

    this->pos = this->tokens.begin();
  }

  std::optional<Token> next() {
    if (this->pos == this->tokens.end()) {
      return std::nullopt;
    }

    auto result = *this->pos;
    this->pos++;
    this->last_location = result.location;
    return result;
  }

  std::optional<Token> peek() {
    if (this->pos == this->tokens.end()) {
      return std::nullopt;
    }

    return *this->pos;
  }

  NonNullSharedPtr<const List> read_list() {
    auto token = this->next();
    if (!token) {
      throw ParseError(this->last_location, "not a list");
    }

    auto kind = List::Kind::List;
    std::string closing;
    if (token->value == "(") {
      closing = ")";
    } else if (token->value == "[") {
      kind = List::Kind::Vector;
      closing = "]";
    } else {
      throw ParseError(token->location, "not a list");
    }

    auto list = make_nonnull_shared<List>(token->location, kind);

    while (true) {
      auto token = this->peek();
      if (!token) {
        throw ParseError(this->last_location, "unbalanced list");
      }
      if (token->value == closing) {
        // Need to consume the token.
        auto end = this->next();
        if (end) {
          auto extended = list->get_location()->extend(end->location->get_to());
          list->set_location(extended);
        }
        break;
      }

      auto node = this->read_form();
      if (!node) {
        throw ParseError(this->last_location, "unexpected unbalanced list");
      }

      list->append(*node);
    }

    return list;
  }

  NonNullSharedPtr<const HashMap> read_hashmap() {
    auto token = this->next();
    if (!token) {
      throw ParseError(this->last_location, "not a hash map");
    }

    auto map = make_nonnull_shared<HashMap>(token->location);

    while (true) {
      auto token = this->peek();
      if (!token) {
        throw ParseError(this->last_location, "unbalanced hash map");
      }
      if (token->value == "}") {
        // Need to consume the token.
        auto end = this->next();
        if (end) {
          auto extended = map->get_location()->extend(end->location->get_to());
          map->set_location(extended);
        }
        break;
      }

      auto raw_key = this->read_form();
      if (!raw_key) {
        throw ParseError(this->last_location, "unexpected unbalanced hash map");
      }
      auto key = raw_key->cast<const Hashable>();
      if (!key) {
        throw ParseError((*raw_key)->get_location(), "invalid key type");
      }

      auto val = this->read_form();
      if (!val) {
        throw ParseError(this->last_location,
                         "unbalanced hash map, missing value");
      }

      map->set(key->shared(), *val);
    }

    return map;
  }

  ConstNodePtr read_atom() {
    auto token = this->next();
    if (!token || token->value.length() == 0) {
      throw ParseError(this->last_location, "Missing atom");
    }

    char first_character = token->value.at(0);
    if (first_character >= '0' && first_character <= '9') {
      return make_nonnull_shared<Number>(token->location, token->value);

    } else if (first_character == '-') {
      if (token->value.length() == 1) {
        return make_nonnull_shared<Symbol>(token->location, token->value);
      }

      char second_character = token->value.at(1);
      if (second_character >= '0' && second_character <= '9') {
        return make_nonnull_shared<Number>(token->location, token->value);
      } else {
        return make_nonnull_shared<Symbol>(token->location, token->value);
      }

    } else if (first_character == ':' && token->value.length() > 1) {
      return make_nonnull_shared<Keyword>(token->location, token->value);

    } else if (first_character == '"') {
      return make_nonnull_shared<String>(token->location, token->value);

    } else {
      return make_nonnull_shared<Symbol>(token->location, token->value);
    }
  }

  // Stuff that is going to be implemented at a later time, but that is already
  // covered by tests.
  ConstNodePtr read_future() {
    const auto token = this->next();
    if (!token || token->value.length() == 0) {
      throw ParseError(this->last_location, "missing token");
    }

    const char c = token->value.at(0);
    switch (c) {
    case '\'': {
      auto contained = this->read_form();
      if (!contained) {
        throw ParseError(token->location, "missing form");
      }
      auto extended =
          token->location->extend((*contained)->get_location()->get_to());
      return make_nonnull_shared<Future>(extended, "(quote ", ")", *contained);
    }

    case '`': {
      auto contained = this->read_form();
      if (!contained) {
        throw ParseError(token->location, "missing form");
      }
      auto extended =
          token->location->extend((*contained)->get_location()->get_to());
      return make_nonnull_shared<Future>(extended, "(quasiquote ", ")",
                                         *contained);
    }

    case '~': {
      auto contained = this->read_form();
      if (!contained) {
        throw ParseError(token->location, "missing form");
      }
      auto extended =
          token->location->extend((*contained)->get_location()->get_to());
      if (token->value.length() > 1 && token->value.at(1) == '@') {
        return make_nonnull_shared<Future>(extended, "(splice-unquote ", ")",
                                           *contained);
      }
      return make_nonnull_shared<Future>(extended, "(unquote ", ")",
                                         *contained);
    }

    case '@': {
      auto contained = this->read_form();
      if (!contained) {
        throw ParseError(token->location, "missing form");
      }
      auto extended =
          token->location->extend((*contained)->get_location()->get_to());
      return make_nonnull_shared<Future>(extended, "(deref ", ")", *contained);
    }

    default:
      throw ParseError(token->location, "unknown future");
    }
  }

  std::optional<ConstNodePtr> read_form() {
    auto token = this->peek();
    if (!token || token->value.length() == 0) {
      return std::nullopt;
    }

    char first_char = token->value.at(0);
    switch (first_char) {
    case '(':
    case '[':
      return this->read_list();

    case '{':
      return this->read_hashmap();

    case '\'':
    case '`':
    case '~':
    case '@':
      return this->read_future();

    default:
      return this->read_atom();
    }
  }
};

Reader::Reader(const NonNullSharedPtr<Location::File> &file, int line_number,
               const std::string &line)
    : impl(std::make_unique<Impl>(file, line_number, line)) {}

Reader::~Reader() = default;

std::optional<ConstNodePtr> Reader::read_form() { return impl->read_form(); }

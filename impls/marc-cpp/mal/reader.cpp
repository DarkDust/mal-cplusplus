module;

#include <memory>
#include <optional>
#include <regex>
#include <string>
#include <vector>

module mal;

using namespace mal;

struct Reader::Impl {
  std::vector<std::string> tokens;
  std::vector<std::string>::iterator pos;

  Impl(const std::string &line) {
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
      this->tokens.push_back(it->str());
    }

    this->pos = this->tokens.begin();
  }

  std::optional<std::string> next() {
    if (this->pos == this->tokens.end()) {
      return std::nullopt;
    }

    auto result = *this->pos;
    this->pos++;
    return result;
  }

  std::optional<std::string> peek() {
    if (this->pos == this->tokens.end()) {
      return std::nullopt;
    }

    return *this->pos;
  }

  std::optional<std::shared_ptr<Node>> read_form() {
    auto token = this->peek();
    if (!token || token->length() == 0) {
      return std::nullopt;
    }

    char first_char = token->at(0);
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

  std::optional<std::shared_ptr<List>> read_list() {
    auto token = this->next();
    if (!token) {
      throw ParseError("not a list");
    }

    List::Kind kind;
    std::string closing;
    if (token == "(") {
      kind = List::Kind::List;
      closing = ")";
    } else if (token == "[") {
      kind = List::Kind::Vector;
      closing = "]";
    } else {
      throw ParseError("not a list");
    }

    auto list = std::make_shared<List>(kind);

    while (true) {
      auto token = this->peek();
      if (!token) {
        throw ParseError("unbalanced list");
      }
      if (token == closing) {
        // Need to consume the token.
        this->next();
        break;
      }

      auto node = this->read_form();
      if (!node) {
        throw ParseError("unexpected unbalanced list");
      }

      list->append(*node);
    }

    return list;
  }

  std::optional<std::shared_ptr<HashMap>> read_hashmap() {
    auto token = this->next();
    if (!token) {
      throw ParseError("not a hash map");
    }

    auto map = std::make_shared<HashMap>();

    while (true) {
      auto token = this->peek();
      if (!token) {
        throw ParseError("unbalanced hash map");
      }
      if (token == "}") {
        // Need to consume the token.
        this->next();
        break;
      }

      auto raw_key = this->read_form();
      if (!raw_key) {
        throw ParseError("unexpected unbalanced hash map");
      }
      auto key = dynamic_pointer_cast<Hashable>(*raw_key);
      if (!key) {
        throw ParseError("invalid key type");
      }

      auto val = this->read_form();
      if (!val) {
        throw ParseError("unbalanced hash map, missing value");
      }

      map->set(key, *val);
    }

    return map;
  }

  std::optional<std::shared_ptr<Node>> read_atom() {
    auto token = this->next();
    if (!token || token->length() == 0) {
      throw ParseError("Missing atom");
    }

    char first_character = token->at(0);
    if (first_character >= '0' && first_character <= '9') {
      return std::make_shared<Number>(*token);

    } else if (first_character == '-') {
      if (token->length() == 1) {
        return std::make_shared<Symbol>(*token);
      }

      char second_character = token->at(1);
      if (second_character >= '0' && second_character <= '9') {
        return std::make_shared<Number>(*token);
      } else {
        return std::make_shared<Symbol>(*token);
      }

    } else if (first_character == ':' && token->length() > 1) {
      return std::make_shared<Keyword>(*token);

    } else if (first_character == '"') {
      return std::make_shared<String>(*token);

    } else {
      return std::make_shared<Symbol>(*token);
    }
  }

  // Stuff that is going to be implemented at a later time, but that is already
  // covered by tests.
  std::optional<std::shared_ptr<Node>> read_future() {
    const auto token = this->next();
    if (!token || token->length() == 0) {
      throw ParseError("missing token");
    }

    const char c = token->at(0);
    switch (c) {
    case '\'': {
      auto contained = this->read_form();
      if (!contained) {
        throw ParseError("missing form");
      }
      return std::make_shared<Future>("(quote ", ")", *contained);
    }

    case '`': {
      auto contained = this->read_form();
      if (!contained) {
        throw ParseError("missing form");
      }
      return std::make_shared<Future>("(quasiquote ", ")", *contained);
    }

    case '~': {
      auto contained = this->read_form();
      if (!contained) {
        throw ParseError("missing form");
      }
      if (token->length() > 1 && token->at(1) == '@') {
        return std::make_shared<Future>("(splice-unquote ", ")", *contained);
      }
      return std::make_shared<Future>("(unquote ", ")", *contained);
    }

    case '@': {
      auto contained = this->read_form();
      if (!contained) {
        throw ParseError("missing form");
      }
      return std::make_shared<Future>("(deref ", ")", *contained);
    }

    default:
      throw ParseError("unknown future");
    }
  }
};

Reader::Reader(const std::string &line) : impl(std::make_unique<Impl>(line)) {}

Reader::~Reader() = default;

std::optional<std::shared_ptr<Node>> Reader::read_form() {
  return impl->read_form();
}

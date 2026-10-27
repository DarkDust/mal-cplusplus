module;

#include <string>

module mal;

using namespace mal;

static std::string escape(const std::string &str) {
  std::string output;

  for (auto c : str) {
    switch (c) {
    case '\n':
      output += "\\n";
      break;
    case '\r':
      output += "\\r";
      break;
    case '\t':
      output += "\\t";
      break;
    case '\\':
      output += "\\\\";
      break;
    case '"':
      output += "\\\"";
      break;
    default:
      output += c;
      break;
    }
  }

  return output;
}

static std::string unescape(const std::string &str) {
  std::string output;

  bool expecting_code = false;
  for (auto c : str) {
    if (expecting_code) {
      switch (c) {
      case '\\':
        output += "\\";
        break;
      case '"':
        output += "\"";
        break;
      case 'n':
        output += "\n";
        break;
      case 'r':
        output += "\r";
        break;
      case 't':
        output += "\t";
        break;
      default:
        throw ParseError("unsupport escape sequence");
      }
      expecting_code = false;

    } else if (c == '\\') {
      expecting_code = true;

    } else {
      output += c;
    }
  }

  if (expecting_code) {
    throw ParseError("unbalanced escape character in string");
  }

  return output;
}

String::String(const std::string &str) {
  if (str.starts_with("\"")) {
    if (!str.ends_with("\"") || str.length() == 1) {
      throw ParseError("unbalanced string");
    }
    this->str = unescape(str.substr(1, str.length() - 2));
  } else {
    // Assume raw, unescaped string.
    this->str = str;
  }
}

std::string String::print() const { return "\"" + escape(this->str) + "\""; }
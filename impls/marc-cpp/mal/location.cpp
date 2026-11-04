module;

#include <format>
#include <string>

module mal;

using namespace mal;

Location::File::File(std::string path) : path(std::move(path)) {};

const std::string &Location::File::get_path() const { return this->path; }

Location::Location(const NonNullSharedPtr<const File> &file, int line,
                   int character)
    : file(file), line(line), character(character) {}

const NonNullSharedPtr<const Location::File> &Location::get_file() const {
  return this->file;
}

int Location::get_line() const { return this->line; }

int Location::get_character() const { return this->character; }

std::string Location::print() const {
  return std::format("{}:{}:{}", this->file->get_path(), this->line,
                     this->character);
}

LocationRange::LocationRange(const NonNullSharedPtr<const Location> &from,
                             const NonNullSharedPtr<const Location> &to)
    : from(from), to(to) {}

const NonNullSharedPtr<const Location> &LocationRange::get_from() const {
  return this->from;
}

const NonNullSharedPtr<const Location> &LocationRange::get_to() const {
  return this->to;
}

std::string LocationRange::print() const {
  if (this->from->get_file()->get_path() == this->to->get_file()->get_path()) {
    if (this->from->get_line() == this->to->get_line()) {
      if (this->from->get_character() == this->to->get_character()) {
        return this->from->print();
      } else {
        return std::format("{}:{}:{}-{}", this->from->get_file()->get_path(),
                           this->from->get_line(), this->from->get_character(),
                           this->to->get_character());
      }

    } else {
      return std::format("{}:{}:{} - {}:{}:{}",
                         this->from->get_file()->get_path(),
                         this->from->get_line(), this->from->get_character(),
                         this->to->get_file()->get_path(), this->to->get_line(),
                         this->to->get_character());
    }

  } else {
    return std::format("{}:{}:{} - {}:{}:{}",
                       this->from->get_file()->get_path(),
                       this->from->get_line(), this->from->get_character(),
                       this->to->get_file()->get_path(), this->to->get_line(),
                       this->to->get_character());
  }
}

NonNullSharedPtr<LocationRange>
LocationRange::extend(const NonNullSharedPtr<const Location> &to) const {
  return make_nonnull_shared<LocationRange>(this->from, to);
}

// NOLINTNEXTLINE(bugprone-throwing-static-initialization)
static const NonNullSharedPtr<Location::File> gVirtualFile =
    make_nonnull_shared<Location::File>("<virtual>");

NonNullSharedPtr<const Location> Location::create_virtual() {
  return make_nonnull_shared<Location>(gVirtualFile, -1, -1);
}

NonNullSharedPtr<const LocationRange> LocationRange::create_virtual() {
  auto location = Location::create_virtual();
  return make_nonnull_shared<LocationRange>(location, location);
}
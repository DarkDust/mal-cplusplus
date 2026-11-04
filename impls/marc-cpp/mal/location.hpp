#pragma once

#include "nonnull_shared_ptr.hpp"
#include <string>

namespace mal {
  /** A location in a source file. */
  class Location {
  public:
    /** File path to a source. */
    class File {
    public:
      explicit File(std::string path);

      /** Get the source file path. */
      [[nodiscard]] const std::string &get_path() const;

    private:
      std::string path;
    };

    Location(const NonNullSharedPtr<const File> &file, int line, int character);

    [[nodiscard]] const NonNullSharedPtr<const File> &get_file() const;
    [[nodiscard]] int get_line() const;
    [[nodiscard]] int get_character() const;

    [[nodiscard]] std::string print() const;

    /** Create a new virtual location. */
    static NonNullSharedPtr<const Location> create_virtual();

  private:
    NonNullSharedPtr<const File> file;
    int line;
    int character;
  };

  /** Range of source locations. */
  class LocationRange {
  public:
    LocationRange(const NonNullSharedPtr<const Location> &from,
                  const NonNullSharedPtr<const Location> &to);

    [[nodiscard]] const NonNullSharedPtr<const Location> &get_from() const;
    [[nodiscard]] const NonNullSharedPtr<const Location> &get_to() const;

    [[nodiscard]] std::string print() const;

    /** Derive a new range with a new end location. */
    [[nodiscard]] NonNullSharedPtr<LocationRange>
    extend(const NonNullSharedPtr<const Location> &to) const;

    /** Create a new virtual location range. */
    static NonNullSharedPtr<const LocationRange> create_virtual();

  private:
    NonNullSharedPtr<const Location> from;
    NonNullSharedPtr<const Location> to;
  };

  using ConstLocationPtr = NonNullSharedPtr<const Location>;
  using ConstLocationRangePtr = NonNullSharedPtr<const LocationRange>;

} // namespace mal
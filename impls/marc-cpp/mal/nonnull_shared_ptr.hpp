#pragma once

#include <memory>
#include <optional>
#include <stdexcept>

namespace mal {
  // Forward declarations
  template <class T> class NonNullSharedPtr;

  template <class T, class... Args>
  NonNullSharedPtr<T> make_nonnull_shared(Args &&...args);

  /** Represents a shared non-null pointer. */
  template <class T> class NonNullSharedPtr {
  public:
    /** Conversion initializer. */
    template <class U>
      requires std::convertible_to<std::shared_ptr<U>, std::shared_ptr<T>>
    explicit(false) NonNullSharedPtr(const NonNullSharedPtr<U> &other)
        : ptr(other.shared()) {}

    ~NonNullSharedPtr() = default;

    // Allow copy.
    NonNullSharedPtr(const NonNullSharedPtr &) = default;
    NonNullSharedPtr &operator=(const NonNullSharedPtr &) = default;
    // Prevent move.
    // NonNullSharedPtr(NonNullSharedPtr &&) = delete;
    // NonNullSharedPtr &operator=(NonNullSharedPtr &&) = delete;

    T &operator*() const { return *ptr; }
    T *operator->() const { return ptr.get(); }

    // Return a copy, so callers cannot reset our stored pointer.
    [[nodiscard]] std::shared_ptr<T> shared() const { return ptr; }

    /** Try to cast the shared pointer to a target type. */
    template <class Target>
    std::optional<NonNullSharedPtr<Target>> cast() const {
      auto target = std::dynamic_pointer_cast<Target>(ptr);
      if (target) {
        return std::optional<NonNullSharedPtr<Target>>{
            NonNullSharedPtr<Target>(target)};
      } else {
        return std::nullopt;
      }
    };

  private:
    /** Initialize with an existing shared pointer. */
    explicit NonNullSharedPtr(std::shared_ptr<T> ptr) : ptr(std::move(ptr)) {
      if (!this->ptr) {
        throw std::invalid_argument("null pointer");
      }
    }

    std::shared_ptr<T> ptr;

    template <class U, class... Args>
    friend NonNullSharedPtr<U> make_nonnull_shared(Args &&...args);

    template <class> friend class NonNullSharedPtr;
  };

  /** Generate a non-null shared pointer. */
  template <class T, class... Args>
  NonNullSharedPtr<T> make_nonnull_shared(Args &&...args) {
    auto intermediate = std::make_shared<T>(std::forward<Args>(args)...);
    return NonNullSharedPtr(intermediate);
  }

} // namespace mal

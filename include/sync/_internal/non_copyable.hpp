#pragma once

namespace sync::internal {

/// Move-only type
class NonCopyable {
  public:
  NonCopyable() = default;
  virtual ~NonCopyable() = default;

  NonCopyable(const NonCopyable&) = delete;
  NonCopyable& operator=(const NonCopyable&) = delete;
  NonCopyable(NonCopyable&&) noexcept = default;
  NonCopyable& operator=(NonCopyable&&) noexcept = default;

  // NonCopyable(NonCopyable const&) = delete;
  // void operator=(NonCopyable const &x) = delete;

  // NonCopyable(NonCopyable&&) = default;
  // NonCopyable& operator=(NonCopyable&&) = default;
};

}

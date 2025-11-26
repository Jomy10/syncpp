#pragma once

#if defined(SYNC_HAVE_MUTEX) && defined(SYNC_HAVE_OPTIONAL)

#include <mutex>
#include <optional>

#include <sync/_internal/non_copyable.hpp>
#include <sync/once.hpp>

namespace sync {

template<class T>
class Lazy: public internal::NonCopyable {
  private:
  Once<T> inner;
  std::function<T(void)> initializer;

  public:
  Lazy(std::function<T(void)> fn):
    inner(),
    initializer(fn) {}

  T& get() {
    return this->inner.get_or_init(initializer);
  }
};

} // namespace

#endif

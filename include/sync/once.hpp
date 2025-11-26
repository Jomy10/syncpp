#pragma once

#if defined(SYNC_HAVE_MUTEX) && defined(SYNC_HAVE_OPTIONAL)

#include <mutex>
#include <optional>

#include <sync/_internal/non_copyable.hpp>

namespace sync {

/// low-level synchronization primitive for one-time global execution
template<class T>
class Once: public internal::NonCopyable {
  private:
  std::once_flag init_flag;
  std::optional<T> value;

  public:
  Once() = default;

  template<typename InitFunc, typename ...Args>
  void init_once(InitFunc&& fn, Args&& ...args) {
    std::call_once(this->init_flag, [&]() {
      this->value = std::make_optional<T>(std::forward<InitFunc>(fn)(std::forward<Args>(args)...));
    });
  }

  std::optional<T>& get() {
    return this->value;
  }

  T* operator->() { return &this->get(); }
  T& operator*() { return this->get(); }
};

} // namespace

#endif // HAVE_MUTEX

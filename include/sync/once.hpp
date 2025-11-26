#pragma once

#if defined(SYNC_HAVE_MUTEX) && defined(SYNC_HAVE_OPTIONAL)

#include <mutex>
#include <optional>

#include <sync/_internal/non_copyable.hpp>

namespace sync {

/// Initializes a variable once
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

  std::optional<T>* operator->() { return &this->get(); }
  std::optional<T>& operator*() { return this->get(); }

  template<typename InitFunc, typename ...Args>
  T& get_or_init(InitFunc&& fn, Args&& ...args) {
    this->init_once(std::forward<InitFunc>(fn), std::forward<Args>(args)...);
    return this->get().value();
  }
};

} // namespace

#endif // HAVE_MUTEX

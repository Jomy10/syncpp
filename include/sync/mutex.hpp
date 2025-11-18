#pragma once

#include <mutex>
#if __cplusplus >= 201703L
#include <optional>
#endif

namespace sync {

template<class T>
class Mutex;

template<class T>
class MutexGuard {
  friend class Mutex<T>;

  private:
  T& val;
  std::mutex& m;

  MutexGuard(T& val, std::mutex& m)
    : val(val),
      m(m) {}

  public:
  operator T&() {
    return this->val;
  }

  T& operator*() {
    return this->val;
  }
  const T& operator*() const {
    return this->val;
  }

  T* operator->() {
    return &this->val;
  }
  const T* operator->() const {
    return &this->val;
  }

  T& value() {
    return this->val;
  }

  ~MutexGuard() {
    m.unlock();
  }
};

template<class T>
class Mutex {
  private:
  T val;
  std::mutex m;

  public:
  Mutex(T v) : val(v) {}

  MutexGuard<T> acquire() {
    this->m.lock();
    return MutexGuard<T>(this->val, this->m);
  }

  #if __cplusplus >= 201703L
  std::optional<MutexGuard<T>> try_acquire() {
    if (this->m.try_lock()) {
      return MutexGuard<T>(this->val, this->m);
    } else {
      return std::nullopt;
    }
  }
  #endif
};

} // namespace

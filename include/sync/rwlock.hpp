#pragma once

#include <sync/_internal/guard.hpp>
#include <sync/_internal/non_copyable.hpp>
#include <sync/_internal/syserr.hpp>

namespace sync {

template<class T>
class RwLock;

#if defined(SYNC_RWLOCK_IS_SHARED_MUTEX)

#include <shared_mutex>
#ifdef SYNC_HAVE_OPTIONAL
#include <optional>
#endif

template<class T>
class RwLockReadGuard: public internal::NonCopyable {
  friend class RwLock<T>;

  private:
  T& val;
  std::shared_mutex& m;

  RwLockReadGuard(T& val, std::shared_mutex& m)
    : val(val),
      m(m) {}

  public:
  GUARD_GUARD_FNS_IMMUTABLE

  ~RwLockReadGuard() {
    m.unlock_shared();
  }
};

template<class T>
class RwLockWriteGuard: public internal::NonCopyable {
  friend class RwLock<T>;

  private:
  T& val;
  std::shared_mutex& m;

  RwLockWriteGuard(T& val, std::shared_mutex& m)
    : val(val),
      m(m) {}

  public:
  GUARD_FNS

  ~RwLockWriteGuard() {
    m.unlock();
  }
};

template<class T>
class RwLock {
  private:
  T val;

  std::shared_mutex m;

  public:
  RwLock(T val) : val(val) {}

  RwLockReadGuard<T> read() {
    this->m.lock_shared();
    return RwLockReadGuard<T>(this->val, this->m);
  }

  RwLockWriteGuard<T> write() {
    this->m.lock();
    return RwLockWriteGuard<T>(this->val, this->m);
  }

  #ifdef SYNC_HAVE_OPTIONAL
  std::optional<RwLockReadGuard<T>> try_read() {
    if (this->m.try_lock_shared()) {
      return std:make_optional<RwLockReadGuard<T>>(this->val, this->m);
    } else {
      return std::nullopt;
    }
  }

  std::optional<RwLockWriteGuard<T>> try_write() {
    if (this->m.try_lock()) {
      return std::make_optional<RwLockWriteGuard<T>>(this->val, this->m);
    } else {
      return std::nullopt;
    }
  }
  #endif
};

#elif defined(SYNC_RWLOCK_IS_PTHREAD)

#include <pthread.h>
#include <memory>

template<class T>
class RwLockReadGuard: public internal::NonCopyable {
  friend class RwLock<T>;

  private:
  T& val;
  pthread_rwlock_t* l;

  RwLockReadGuard(T& val, pthread_rwlock_t* l): val(val), l(l) {}

  public:
  GUARD_FNS_IMMUTABLE

  ~RwLockReadGuard() {
    pthread_rwlock_unlock(l);
  }
};

template<class T>
class RwLockWriteGuard: public internal::NonCopyable {
  friend class RwLock<T>;

  private:
  T& val;
  pthread_rwlock_t* l;

  RwLockWriteGuard(T& val, pthread_rwlock_t* l): val(val), l(l) {}

  public:
  GUARD_FNS

  ~RwLockWriteGuard() {
    pthread_rwlock_unlock(l);
  }
};

template<class T>
class RwLock {
  private:
  T val;
  pthread_rwlock_t lock;

  public:
  RwLock(T v) : val(v) {
    pthread_rwlock_init(&this->lock, NULL);
  }

  RwLockReadGuard<T> read() {
    int ret = pthread_rwlock_rdlock(&this->lock);
    if (ret != 0) internal::throw_syserr(ret);

    return RwLockReadGuard<T>(this->val, &this->lock);
  }

  std::unique_ptr<RwLockReadGuard<T>> try_read() {
    int ret = pthread_rwlock_tryrdlock(&this->lock);
    if (ret != 0) return nullptr;

    // opt = std::move(RwLockReadGuard<T>(this->val, &this->lock));
    return std::unique_ptr<RwLockReadGuard<T>>(new RwLockReadGuard<T>(this->val, &this->lock));
  }

  #ifdef SYNC_RWLOCK_EXTENSION
  std::unique_ptr<RwLockReadGuard<T>> try_read(int* return_code) {
    int ret = pthread_rwlock_tryrdlock(&this->lock);
    if (ret != 0) {
      *return_code = ret;
      return std::nullopt;
    }

    return std::unique_ptr<RwLockReadGuard<T>>(new RwLockReadGuard<T>(this->val, &this->lock));
  }
  #endif

  RwLockWriteGuard<T> write() {
    int ret = pthread_rwlock_wrlock(&this->lock);
    if (ret != 0) internal::throw_syserr(ret);

    return RwLockWriteGuard<T>(this->val, &this->lock);
  }

  std::unique_ptr<RwLockWriteGuard<T>> try_write() {
    int ret = pthread_rwlock_trywrlock(&this->lock);
    if (ret != 0) return nullptr;

    return std::unique_ptr<RwLockWriteGuard<T>>(new RwLockWriteGuard<T>(this->val, &this->lock));
  }

  #ifdef SYNC_RWLOCK_EXTENSION
  std::unique_ptr<RwLockWriteGuard<T>> try_write(int* return_code) {
    int ret = pthread_rwlock_trywrlock(&this->lock);
    if (ret != 0) {
      *return_code = ret;
      return std::nullopt;
    }

    return std::unique_ptr<RwLockWriteGuard<T>>(new RwLockWriteGuard<T>(this->val, &this->lock));
  }
  #endif

  ~RwLock() {
    pthread_rwlock_destroy(&this->lock);
  }
};

#else

#error "unimplemented"

#endif // Version check

} // namespace

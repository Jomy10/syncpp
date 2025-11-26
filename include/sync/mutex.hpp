#pragma once

#if defined(SYNC_HAVE_MUTEX) || defined(SYNC_MUTEX_IS_PTHREAD)

#include <memory>

#include <sync/_internal/guard.hpp>
#include <sync/_internal/non_copyable.hpp>
#include <sync/_internal/syserr.hpp>

#if defined(SYNC_HAVE_MUTEX) && !defined(SYNC_MUTEX_IS_PTHREAD)
  #include <mutex>
#endif

#if defined(SYNC_MUTEX_IS_PTHREAD)
  #include <pthread.h>
#endif

namespace sync {

#if defined(SYNC_HAVE_MUTEX) && !defined(SYNC_MUTEX_IS_PTHREAD)
  namespace internal {
    typedef std::mutex mutex_t;
    typedef mutex_t& mutex_ref_t;
  }
#endif

#if defined(SYNC_MUTEX_IS_PTHREAD)
  namespace internal {
    typedef pthread_mutex_t mutex_t;
    typedef mutex_t* mutex_ref_t;
  }
#endif

template<class T>
class Mutex;

template<class T>
class MutexGuard: public internal::NonCopyable {
  friend class Mutex<T>;

  private:
  T& val;
  internal::mutex_ref_t m;

  MutexGuard(T& val, internal::mutex_ref_t m)
    : val(val),
      m(m) {}

  public:
  GUARD_FNS

  ~MutexGuard() {
    #if defined(SYNC_MUTEX_IS_PTHREAD)
      int ret = pthread_mutex_unlock(this->m);
      if (ret != 0) internal::throw_syserr(ret);
    #else
      this->m.unlock();
    #endif
  }
};

template<class T>
class Mutex {
  private:
  T val;
  internal::mutex_t m;

  public:
  Mutex(T v) : val(v) {
    #if defined(SYNC_MUTEX_IS_PTHREAD)
    int ret = pthread_mutex_init(&this->m, NULL);
    if (ret != 0) internal::throw_syserr(ret);
    #endif
  }

  ~Mutex() {
    #if defined(SYNC_MUTEX_IS_PTHREAD)
    int ret = pthread_mutex_destroy(&this->m);
    if (ret != 0) internal::throw_syserr(ret);
    #endif
  }

  MutexGuard<T> acquire() {
    #if defined(SYNC_MUTEX_IS_PTHREAD)
      int ret = pthread_mutex_lock(&this->m);
      if (ret != 0) internal::throw_syserr(ret);
      return MutexGuard<T>(this->val, &this->m);
    #else
      this->m.lock();
      return MutexGuard<T>(this->val, this->m);
    #endif
  }

  std::unique_ptr<MutexGuard<T>> try_acquire() {
    #if defined(SYNC_MUTEX_IS_PTHREAD)
      if (pthread_mutex_trylock(&this->m) == 0) {
        return std::unique_ptr<MutexGuard<T>>(new MutexGuard<T>(this->val, &this->m));
      } else {
        return nullptr;
      }
    #else
      if (this->m.try_lock()) {
        return std::unique_ptr<MutexGuard<T>>(new MutexGuard<T>(this->val, this->m));
      } else {
        return nullptr;
      }
    #endif
  }

  #if defined(SYNC_MUTEX_EXTENSION)
    #if defined(SYNC_MUTEX_IS_PTHREAD)
      std::unique_ptr<MutexGuard<T>> try_acquire(int* return_code) {
        *return_code = pthread_mutex_trylock(&this->m);
        if (*return_code == 0) {
          return std::unique_ptr<MutexGuard<T>>(new MutexGuard<T>(this->val, &this->m));
        } else {
          return nullptr;
        }
      }
    #endif
  #endif
};

} // namespace

#endif // SYNC_HAVE_MUTEX

#pragma once

#include <sync/_internal/guard.hpp>
#include <sync/_internal/non_copyable.hpp>
#include <sync/_internal/syserr.hpp>

#include <memory>

#if defined(SYNC_RWLOCK_IS_SHARED_MUTEX)
  #include <shared_mutex>
  #ifdef SYNC_HAVE_OPTIONAL
  #include <optional>
  #endif
#elif defined(SYNC_RWLOCK_IS_PTHREAD)
  #include <pthread.h>
#else
  #error "unimplemented"
  // for example windows-specific: https://github.com/mqudsi/RWLock/blob/master/RWLock/RWLock.h
#endif

namespace sync {

template<class T>
class RwLock;

#if defined(SYNC_RWLOCK_IS_SHARED_MUTEX)
  namespace internal {
    typedef std::shared_mutex rwlock_lock_t;
    typedef rwlock_lock_t& rwlock_lock_ref_t;
  }
#elif defined(SYNC_RWLOCK_IS_PTHREAD)
  namespace internal {
    typedef pthread_rwlock_t rwlock_lock_t;
    typedef rwlock_lock_t* rwlock_lock_ref_t;
  }
#else

#error "unimplemented"

#endif

template<class T>
class RwLockReadGuard: public internal::NonCopyable {
  friend class RwLock<T>;

  private:
  T& val;
  internal::rwlock_lock_ref_t m;

  RwLockReadGuard(T& val, internal::rwlock_lock_ref_t m)
    : val(val),
      m(m) {}

  public:
  GUARD_FNS_IMMUTABLE

  #if defined(SYNC_RWLOCK_IS_SHARED_MUTEX)
  ~RwLockReadGuard() {
    m.unlock_shared();
  }
  #elif defined(SYNC_RWLOCK_IS_PTHREAD)
  ~RwLockReadGuard() {
    int ret = pthread_rwlock_unlock(m);
    if (ret != 0) internal::throw_syserr(ret);
  }
  #else

  #endif
};

template<class T>
class RwLockWriteGuard: public internal::NonCopyable {
  friend class RwLock<T>;

  private:
  T& val;
  internal::rwlock_lock_ref_t m;

  RwLockWriteGuard(T& val, internal::rwlock_lock_ref_t m)
    : val(val),
      m(m) {}

  public:
  GUARD_FNS

  #if defined(SYNC_RWLOCK_IS_SHARED_MUTEX)
  ~RwLockWriteGuard() {
    m.unlock();
  }
  #elif defined(SYNC_RWLOCK_IS_PTHREAD)
  ~RwLockWriteGuard() {
    int ret = pthread_rwlock_unlock(m);
    if (ret != 0) internal::throw_syserr(ret);
  }
  #else

  #endif
};

template<class T>
class RwLock {
  private:
  T val;
  internal::rwlock_lock_t m;

  public:
  RwLock(T val) : val(val) {
    #if defined(SYNC_RWLOCK_IS_PTHREAD)
    int ret = pthread_rwlock_init(&this->m, NULL);
    if (ret != 0) internal::throw_syserr(ret);
    #endif
  }

  ~RwLock() {
    #if defined(SYNC_RWLOCK_IS_PTHREAD)
    int ret = pthread_rwlock_destroy(&this->m);
    if (ret != 0) internal::throw_syserr(ret);
    #endif
  }

  RwLockReadGuard<T> read() {
    #if defined(SYNC_RWLOCK_IS_SHARED_MUTEX)
      this->m.lock_shared();
      return RwLockReadGuard(this->val, this->m);
    #elif defined(SYNC_RWLOCK_IS_PTHREAD)
      int ret = pthread_rwlock_rdlock(&this->m);
      if (ret != 0) internal::throw_syserr(ret);

      return RwLockReadGuard<T>(this->val, &this->m);
    #else
    #endif
  }

  RwLockWriteGuard<T> write() {
    #if defined(SYNC_RWLOCK_IS_SHARED_MUTEX)
      this->m.lock();
      return RwLockWriteGuard(this->val, this->m);
    #elif defined(SYNC_RWLOCK_IS_PTHREAD)
      int ret = pthread_rwlock_wrlock(&this->m);
      if (ret != 0) internal::throw_syserr(ret);

      return RwLockWriteGuard<T>(this->val, &this->m);
    #else
    #endif
  }

  std::unique_ptr<RwLockReadGuard<T>> try_read() {
    #if defined(SYNC_RWLOCK_IS_SHARED_MUTEX)
      if (this->m.try_lock_shared()) {
        return std::unique_ptr<RwLockReadGuard<T>>(new RwLockReadGuard<T>(this->val, this->m));
      } else {
        return nullptr;
      }
    #elif defined(SYNC_RWLOCK_IS_PTHREAD)
      int ret = pthread_rwlock_tryrdlock(&this->m);
      if (ret != 0) return nullptr;

      return std::unique_ptr<RwLockReadGuard<T>>(new RwLockReadGuard<T>(this->val, &this->m));
    #else
    #endif
  }

  std::unique_ptr<RwLockWriteGuard<T>> try_write() {
    #if defined(SYNC_RWLOCK_IS_SHARED_MUTEX)
      if (this->m.try_lock()) {
        return std::unique_ptr<RwLockWriteGuard<T>>(new RwLockWriteGuard<T>(this->val, this->m));
      } else {
        return nullptr;
      }
    #elif defined(SYNC_RWLOCK_IS_PTHREAD)
      int ret = pthread_rwlock_trywrlock(&this->m);
      if (ret != 0) return nullptr;

      return std::unique_ptr<RwLockWriteGuard<T>>(new RwLockWriteGuard<T>(this->val, &this->m));
    #else
    #endif
  }

  #ifdef SYNC_RWLOCK_EXTENSION
    #ifdef SYNC_RWLOCK_IS_PTHREAD
      std::unique_ptr<RwLockReadGuard<T>> try_read(int* return_code) {
        int ret = pthread_rwlock_tryrdlock(&this->m);
        if (ret != 0) {
          *return_code = ret;
          return nullptr;
        }

        return std::unique_ptr<RwLockReadGuard<T>>(new RwLockReadGuard<T>(this->val, &this->m));
      }

      std::unique_ptr<RwLockWriteGuard<T>> try_write(int* return_code) {
        int ret = pthread_rwlock_trywrlock(&this->m);
        if (ret != 0) {
          return nullptr;
        }

        return std::unique_ptr<RwLockWriteGuard<T>>(new RwLockWriteGuard<T>(this->val, &this->m));
      }
    #endif
  #endif
};

} // namespace

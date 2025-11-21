#pragma once

#define GUARD_FNS \
  operator T&() { \
    return this->val; \
  } \
  \
  T& operator*() { \
    return this->val; \
  } \
  \
  T* operator->() { \
    return &this->val; \
  } \
  \
  T& value() { \
    return this->val; \
  } \
  \
  GUARD_FNS_IMMUTABLE

#define GUARD_FNS_IMMUTABLE \
  operator const T&() const { \
    return this->val; \
  } \
  \
  const T& operator*() const { \
    return this->val; \
  } \
  \
  const T* operator->() const { \
    return &this->val; \
  } \
  \
  const T& value() const { \
    return this->val; \
  }

#include <sync/_internal/syserr.hpp>

#ifdef SYNC_HAVE_SYSTEM_ERROR
#include <system_error>
#endif

namespace sync::internal {

void throw_syserr(int code) {
  #if SYNC_HAVE_SYSTEM_ERROR
  throw std::system_error((int)code, (const std::error_category&)std::generic_category);
  #else
  throw code;
  #endif
}

}

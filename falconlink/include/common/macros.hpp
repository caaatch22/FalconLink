#pragma once

#include <cassert>
#include <stdexcept>

namespace falconlink {

#define ASSERT(expr, message) assert((expr) && (message))


// Macros to disable copying and moving
#define NON_COPYABLE(cname)                                    \
  cname(const cname &) = delete;                   /* NOLINT */ \
  auto operator=(const cname &)->cname & = delete; /* NOLINT */

#define NON_MOVEABLE(cname)                               \
  cname(cname &&) = delete;                   /* NOLINT */ \
  auto operator=(cname &&)->cname & = delete; /* NOLINT */

#define NON_COPYABLE_AND_NON_MOVEABLE(cname) \
  NON_COPYABLE(cname);                       \
  NON_MOVEABLE(cname);

}  // namespace falconlink

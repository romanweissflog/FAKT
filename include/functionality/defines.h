#ifndef DEFINES_H
#define DEFINES_H

#include <cstdint>

enum class ReturnValue : uint8_t
{
  ReturnSuccess = 0,
  ReturnAbort = 1,
  ReturnFailure = 2
};

#endif

#pragma once
#include <stdint.h>
#include "value.h"

/// @brief Ruby virtual machine.
struct Rvm {
  /// @brief Instruction Pointer.
  uint16_t ip;
  /// @brief Globals
  RObjectPtr globals;
  /// @brief Self
  RObjectPtr self;
};

extern struct Rvm hibernate;
/**
 * @file globalman.h
 * @author Go Suzuki (puyogo.suzuki@gmail.com)
 * @brief An implementation of the Global Manager of mruby/copro. This manages global objects and upvalues outside of Copro primitives.
 * @version 0.1
 * @date 2025-04-26
 * 
 * @copyright Copyright (c) 2025 Go Suzuki and Programming Systems Group in Institute of Science Tokyo.
 * 
 */

#pragma once
#include <stdint.h>
#include "vector_t.h"
#include "vm.h"
#include "../../src/value.h"
#include "value.copro.h"
#include "objman.h"

typedef struct mrbcopro_globalman_t {
  RObjectPtrCopro * current;
  RObjectPtrCopro * current_buffer_last;
  mrbcopro_vector_t dic;
} mrbcopro_globalman_t;

/// @brief Initializes `mrbcopro_globalman_t`.
/// @param vm The Virtual Machine
/// @param gman  The Global Manager
/// @return 1 if Out-of-Memory error. 0 if Success.
int mrbcopro_globalman_new(struct VM * vm, struct mrbcopro_globalman_t * gman);

/// @brief Gets an location on the coprocessor for the given value `key_addr`.
/// @param vm The Virtual Machine
/// @param gman The Global Manager
/// @param key_addr The Key Address
/// @return NULL if Out-of-Memory error. otherwise success.
RObjectPtrCopro * mrbcopro_globalman_get(struct VM * vm, struct mrbcopro_globalman_t * gman, mrbc_value * key_addr);

/// @brief Cleans the global manager, however it does not free objects allocated in the coprocessor.
/// @param vm The Virtual Machine
/// @param gman The Global Manager
void mrbcopro_globalman_clear(struct VM * vm, struct mrbcopro_globalman_t * gman);

/// @brief Send values allocated in global variables.
/// @param vm The Virtual Machine
/// @param gman The Global Manager
/// @return 1 if Error, 0 if Success.
int mrbcopro_globalman_send(struct VM * vm, struct mrbcopro_globalman_t * gman, mrbcopro_objman_t * objman);

/// @brief Receive values allocated in global variables.
/// @param vm The Virtual Machine
/// @param gman The Global Manager
/// @param objman The Object Manager
/// @return 1 if Error, 0 if Success.
int mrbcopro_globalman_receive(struct VM * vm, struct mrbcopro_globalman_t * gman, mrbcopro_objman_t * objman);
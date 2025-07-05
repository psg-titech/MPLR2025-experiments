#pragma once
#include <stdint.h>
#include "../../src/vm.h"

extern const uint8_t inst_skipper_inst_len[106];

typedef int (*inst_analyzer)(const uint8_t * inst_buffer, void * payload);
void inst_analyze(const mrbc_irep * irep, inst_analyzer analyzer, void * payload);
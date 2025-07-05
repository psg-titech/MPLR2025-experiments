#include "inst_skip.h"
const uint8_t inst_skipper_inst_len[] = {
    0, 2, 2, 2, 2, 
    1, 1, 1, 1, 1, 
    1, 1, 1, 1, 3,
    5, 2, 1, 1, 1, 
    1, 2, 2, 2, 2, 
    2, 2, 2, 2, 2, 
    2, 2, 2, 3, 3,
    1, 1, 2, 3, 3,
    3, 2, 1, 2, 1,
    3, 3, 3, 3, 0,
    2, 3, 3, 2, 0,
    2, 1, 1, 1, 3,
    1, 2, 1, 2, 1,
    1, 1, 1, 1, 1,
    1, 2, 3, 1, 2,
    1, 3, 3, 3, 1,
    2, 2, 1, 2, 2,
    1, 2, 2, 2, 1,
    1, 1, 2, 2, 2,
    2, 2, 1, 1, 1,
    3, 1, 0, 0, 0, 0
};

void inst_analyze(const mrbc_irep * irep, inst_analyzer analyzer, void * payload) {
    for(const uint8_t * inst = irep->inst, * end = &(irep->inst[irep->ilen]);
        inst < end;
        inst = (uint8_t *)((size_t)inst + inst_skipper_inst_len[*inst] + sizeof(uint8_t)))
        if(analyzer(inst, payload)) break;
}
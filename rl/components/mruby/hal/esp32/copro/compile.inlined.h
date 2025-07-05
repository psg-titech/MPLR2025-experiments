#pragma once
struct profiler;
struct RMethod;
struct RObject;
typedef int (*inline_code_gen_t)(struct profiler * prof, struct RMethod * method, int a, struct RObject *recv, int narg);
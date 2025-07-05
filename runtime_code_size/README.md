# Introduction
This directory contains results for Table 3 and 4.

# Structure
* For Table 3
 * COLLECTED.ods : This is a working file for Table 3.
 * ulp_main.map : This is raw data for Table 3.
* For Table 4
 * mrubyc-size-components.txt : RM
 * mrubycopro-size-components.txt : RL

# About Table 3
## Reproduction
Please build [`../cl`](../cl).  
Make sure disable all debug outputs:
`hal/esp32/copro/compile.merged.c:78`
```c
#define DBG_ALLOCATION_PROFILE 0
```

`hal/esp32/copro/dbg.h:2`
```c
#define MRBC_PROF_DBG_ENABLE 0
```

Then, you can get `ulp_main.map` from [`../rl/build/esp-idf/main/ulp_main`](../rl).  

To get `COLLECTED.ods`, please do by oneself.

# About Table 4
## Reproduction
Please build [`../rl`](../rl) and [`../rm`](../rm).
Then, execute the following command:  
```sh
$ idf.py size-components
```

We note that the content of `main.c` does not affect the results.

## How to get numbers in the Table
See `libmruby.a` in `*-size-components.txt`.
# Introduction
This directory contains results for Table 5.

# Structure
 * Ruby on LP coprocessor (rl-${APPNAME}.txt) : See `PROF allocated = ` (Metadata) and `GC allocated = ` (Generated Code)
 * C on LP coprocessor : Code maps for the LP coprocessor.
    * `cl-temp.ulp_core_main.map` : sum up `app_main` and `read_sensor`.
    * `cl-acc.ulp_core_main.map` : sum up `app_main`, `read_for_1sec`, `sensor_off`, `sensor_on`, `sensor_read`, and `conv`.
    * `cl-breathingled.ulp_core_main.map` : `app_main`.
    * `cl-tof.ulp_core_main.map` : sum up `app_main`, `sort`.

# Reproduction
## Ruby on LP coprocessor
Enable all debug outputs as following:  
`hal/esp32/copro/compile.c:78`
```c
#define DBG_ALLOCATION_PROFILE 1
```

`hal/esp32/copro/dbg.h:2`
```c
#define MRBC_PROF_DBG_ENABLE 1
```

We used `gather_sht30_fast.c`, `gps_acc.c`, `tofsense.c`, and `breathingled.c`.  
The microcontroller outputs a log via UART.  
We saved log outputs as `rl-${APPNAME}.txt`.

## C on LP coprocessor
Please build `cl/{gather_sht30, gps_acc, tof, breathingled}`.  
ESP-IDF creates `cl/{gather_sht30, gps_acc}/build/esp-idf/main/ulp_core_main/ulp_core_main.map`.
`cl-${APPNAME}.ulp_core_main.map` are copies of `ulp_core_main.map`.
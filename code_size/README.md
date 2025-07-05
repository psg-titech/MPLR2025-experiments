# Introduction
This directory contains results for Table 5.

# Structure
 * Ruby on LP coprocessor : See `PROF allocated = ` (Metadata) and `GC allocated = ` (Generated Code)
    * `gather_sht30.txt` : TEMP
    * `gps_acc.txt` : LOC
 * C on LP coprocessor : Code maps for the LP coprocessor.
    * `gather_sht30.ulp_core_main.map` : sum up `app_main` and `read_sensor`.
    * `gps_acc.ulp_core_main.map` : sum up `app_main`, `read_for_1sec`, `sensor_off`, `sensor_on`, `sensor_read`, and `conv`.

# Reproduction
## Ruby on LP coprocessor
Enable all debug outputs as following:  
`hal/esp32/copro/compile.merged.c:78`
```c
#define DBG_ALLOCATION_PROFILE 1
```

`hal/esp32/copro/dbg.h:2`
```c
#define MRBC_PROF_DBG_ENABLE 1
```

We used `gather_sht30_fast.c` and `gps_acc.c`.  
The microcontroller outputs a log via UART.  
We saved log outputs as `gather_sht30.txt` and `gps_acc.txt`.

## C on LP coprocessor
Please build `cl/{gather_sht30, gps_acc}`.  
ESP-IDF creates `cl/{gather_sht30, gps_acc}/build/esp-idf/main/ulp_core_main/ulp_core_main.map`.
`gather_sht30.ulp_core_main.map` and `gps_acc.ulp_core_main.map` are copies of `ulp_core_main.map`.
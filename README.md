# MPLR2025-experiments
Resources for the experiments in MPLR 2025.

# Difference from the paper
## Naming
In Paper: `Copro#run`  
Actually: `Copro#sleep_and_run`  

# Directory Structure
```
cl : C programs executed on the LP coprocessor
rm : mRuby programs executed entirely on the Main processor using the original mruby/c interpreter
rl : mRuby programs executed on the LP coprocessor using our modified interpreter

sensor_sim : Sensor simulator programs running on the sensor simulator (ESP32).

runtime_code_size : runtime code size results on the main processor (Table 3, 4)
code_size         : log files to check the generated code size for the LP coprocessor (Table 5)
power_consumption : power consumption measurement data (Table 6, Figure 4)
```

To reproduce the experimental results, see `README.md` on each directory.

# Compilation
## Prerequisites
You need: ESP-IDF SDK v5.5 alpha with the commit hash `28ac0243bb65f2d3ad6fc4e90c54365b10d435b9`  
[https://github.com/espressif/esp-idf/tree/28ac0243bb65f2d3ad6fc4e90c54365b10d435b9](https://github.com/espressif/esp-idf/tree/28ac0243bb65f2d3ad6fc4e90c54365b10d435b9)  

We need a small modification to ESP-IDF SDK.  
Please modify `components/ulp/esp32ulp_mapgen.py`:  
```python
@@ -59,7 +59,7 @@ def gen_ld_h_from_sym(f_sym: typing.TextIO, f_ld: typing.TextIO, f_h: typing.Tex
         ))

     # Format the regular expression to match the readelf output
-    expr = re.compile(r'^.*(?P<address>[a-f0-9]{8})\s+(?P<size>\d+) (OBJECT|NOTYPE)\s+GLOBAL\s+DEFAULT\s+[^ ]+ (?P<name>.*)$')
+    expr = re.compile(r'^.*(?P<address>[a-f0-9]{8})\s+(?P<size>\d+) (OBJECT|NOTYPE|FUNC)\s+GLOBAL\s+DEFAULT\s+[^ ]+ (?P<name>.*)$')
     for line in f_sym:
         # readelf format output has the following structure:
         #   Num:    Value  Size Type    Bind   Vis      Ndx Name
```


`rl` and `rm` are based on mruby/c v3.4 alpha with the commit hash `58ec64d56d144dd1ca0a9bba85e4c4f5c832e473`.  
[https://github.com/mrubyc/mrubyc/tree/58ec64d56d144dd1ca0a9bba85e4c4f5c832e473](https://github.com/mrubyc/mrubyc/tree/58ec64d56d144dd1ca0a9bba85e4c4f5c832e473)  
The software license are under the Revised BSD License (a.k.a. 3-caluse license). See [`rm/components/mruby/LICENSE`](rm/components/mruby/LICENSE) and [`rl/components/mruby/LICENSE`](rl/components/mruby/LICENSE).

## How to compile
Please execute `idf.py build` under the following directories:
* `rl`
* `rm`
* `cl/gather_sht30` : CL TEMP
* `cl/gps_acc` : CL LOC
* `sensor_sim/sht30` : For TEMP
* `sensor_sim/adxl367` : For LOC

Because compiler options are stored in `sdkconfig`, you do not need to specify the compiler options.

## How to switch the executing program
`cl` programs are split into `TEMP` and `LOC`. However, `rl` and `rm` are not.  
You can switch the executing program by modifying `main.c`.  
 * `TEMP` : Please `#include "gather_sht30.c"` or `#include "main_gather_sht30.c"`.
 * `LOC` : Please `#include "gps_acc.c"` or `#include "main_gps_acc.c"`.

`gather_sht30_fast.c` is a variant of `gather_sht30.c`. We modified the sleep duration.

## How to translate ruby scripts to mruby bytecodes
### Prerequisites
You can get mruby/c compiler by installing mruby toolkit.  
We used `mruby 3.2.0 (2023-02-24)` from DragonflyBSD Ports.

### Where the mruby scripts are
The mruby scripts are available on `rl` and `rm`.
You can compile with:  
```sh
$ mrbc -Bmrbbuf $FILENAME
```

## Debug Options
### Generated Code Output
`hal/esp32/copro/dbg.h:2`
```c
#define MRBC_PROF_DBG_ENABLE 1
```

### Allocations for Metadata and Generated Code
`hal/esp32/copro/compile.merged.c:78`
```c
#define DBG_ALLOCATION_PROFILE 1
```
# include
This directory contains results for Table 6 and Figure 4.

# Structure
* Raw data (not available)
 * main_gather_sht30.ppk2 : RM TEMP
 * main_gps_acc.ppk2 : RM LOC
 * c_gather_sht30.ppk2 : CL TEMP
 * c_gps_acc.ppk2 : CL LOC
 * gather_sht30.ppk2 : RL TEMP
 * gps_acc.ppk2 : RL LOC
* For Figure 4
 * result_main_gather_sht30.txt : RM TEMP
 * result_main_gps_acc.txt : RM LOC
 * result_c_gather_sht30.txt : CL TEMP
 * result_c_gps_acc.txt : CL LOC
 * result_gather_sht30.txt : RL TEMP
 * result_gps_acc.txt : RL LOC
* Figure 4
 * power_consumption.ods : LOC and TEMP
 * TEMP_fig.pdf : TEMP
 * LOC_fig.pdf : LOC

# Reproduction
## PPK2 files
The sizes of the raw data are very large.  
If you need, please notify us.  

### Environments
 - Evaluation Board: ESP32-C6-DevKitC-1 N8, v1.3
 - Ammeter: Nordic Power Profiler Kit 2 (PPK2)
 - Sensor Simulator: ESP32_Core_Board_V2
 - (Actual Temperature and Humidity Sensor): SENSIRION SHT30
 
### Setup
#### Circuit
We connected an ammeter, an evaluation board, and a sensor simulator.  
The ammeter (PPK2) provides 3.3 [V] power to the evaluation board.  
The power source of the sensor simulator is another power soruce (USB).  
We captured the input current of 3V3 of the evaluation board.
![figure.svg](figure.svg)

#### PPK2 configuration
 - Mode: source meter mode
 - Supply voltage: 3.3 V
 - Sampling paramter: 10^5 samples per second for 31 minutes
 - Digital channels: 0 and 1

### GPIO Outputs
GPIO4 of the evaluation board becomes high before/after `Copro#run`.
GPIO23 of the sensor simulator flips its value when particular write commands are coming.
 * For TEMP: 0x24, 0x16 (sensor value reading)
 * For LOC: 0x2D, 0x02 (sensing on)

### Compilation and Execution
#### CL (C programs with the Lp coprocessor)
Build and execute the projects on `cl/gather_sht30` and `cl/gps_acc`.

#### RM (Ruby programs with the Main processor)
Build and execute the projects on `rm`.  
Make sure that `main.c` includes correct C files.
`rm/main.c:18-23`:  
```c
///// CHANGE HERE!

//#include "main_gather_sht30.c"
#include "main_gps_acc.c"

/////
```

 * For TEMP: include `main_gather_sht30.c`
 * For LOC: include `main_gps_acc.c`

#### RL (Ruby programs with theLp coprocessor)
Build and execute the projects on `rl`.  
Make sure that `main.c` includes correct C files.
`rl/main.c:18-22`:  
```c
///// CHANGE HERE!
//#include "gather_sht30_fast.c"
//#include "gather_sht30.c"
#include "gps_acc.c"
/////
```

 * For TEMP: include `gather_sht30.c`
 * For LOC: include `gps_acc.c`

### Measurement
1. Disable the power output of PPK2.
2. Reset the sensor simulator by the reset button.
3. Start the measurement by PPK2.
4. Enable the power output of PPK2.
5. Wait until the recording stops.
6. Disable the power output of PPK2.
7. Save the recording.
 
## txt Files (Figure 4)
Convert the PPK2 files to CSV with PPK2 (Power Profiler Kit 2).  
Execute `read_csv.rb $CSVFILENAME`, which produces txt files.  
The format of txt files is:  
```
power consumption [mJ], duration [s]
```

## ODS File and figures (Figure 4)
We extracted the values from txt files.

### Methodology
For TEMP, we used the first 29 iterations whose duration is approx. 60 s (59.5 - 60.5 s).  
For LOC, we used the first 225 iterations whose duration is approx. 8 s (7.5 - 8.5 s).

### Reason
The PPK2 begins to capture before the evaluation board is powered up.
Thus, noises are observed due to the initialization of the evaluation board.


## Table 6
We calculated the average power consumption for 30 min from the rise of the GPIO4 of ESP32-C6.

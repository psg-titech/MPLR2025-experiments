#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "../../../misc.c"

const int RES_LEN = 30;

void sort(int length, int * ary) {
  for(int swapped = 1; swapped;) {
    swapped = 0;
    for(int i = 0; i < length-1; ++i) {
      int aryi = ary[i];
      int aryi1 = ary[i+1];
      if(aryi > aryi1) {
        ary[i] = aryi1;
        ary[i+1] = aryi;
        swapped = 1;
      }
    }
  }
}

int buf[20];
int result[30]; // RES_LEN is not constexpr...

int app_main(void) {
  for(int idx = 0; idx < RES_LEN; ++idx) {
    for(int i = 0; i < 20; ++i) {
      ulp_lp_core_gpio_set_level(6, 1);
      delayUs(10);
      ulp_lp_core_gpio_set_level(6, 0);
      buf[i] = pulseIn(0, 1, 200000);
      delayMs(2999);
    }
    sort(20, buf);
    int res = 0;
    for(int i = 2; i < 18; ++i) res += buf[i];
    result[idx] = res / 16;
  }
  return 0;
}

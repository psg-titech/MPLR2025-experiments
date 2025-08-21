#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "../../../misc.c"

const int MAX_ELAPSED = 500;
int app_main(void) {
    int prevPress = 0, press = 0;
    while(!prevPress || press) {
      for(int j = -MAX_ELAPSED; j <= MAX_ELAPSED; ++j) {
        int elapsed = abs(j);
        for(int i = 0; i < 2; ++i) {
          ulp_lp_core_gpio_set_level(6, 0);
          delayUs(elapsed);
          ulp_lp_core_gpio_set_level(6, 1);
          delayUs(MAX_ELAPSED - elapsed);
        }
      }
      prevPress = press;
      press = ulp_lp_core_gpio_get_level(0);
    }
    return 0;
}

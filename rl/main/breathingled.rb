Copro.gpio_output 1
Copro.gpio(1, true)
Copro.gpio_input(0, Copro.gpio_pulldown)
MAX_ELAPSED = 500
Copro.gpio_output 6
Copro.gpio(1,false)
Copro.sleep_and_run do
  prevPress = false
  press = false
  while !prevPress || press do
    j = -MAX_ELAPSED
    while j <= MAX_ELAPSED do
      elapsed = j.abs
      i = 0
      while i < 2 do
        Copro.gpio(6, false)
        Copro.delayUs(elapsed)
        Copro.gpio(6, true)
        Copro.delayUs((MAX_ELAPSED - elapsed))
        i = i + 1
      end
      j += 1
    end
    prevPress = press
    press = Copro.gpio? 0
  end
end
Copro.gpio(1,true)

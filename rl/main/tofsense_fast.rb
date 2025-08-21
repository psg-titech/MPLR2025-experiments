def sort!(ary)
  # bubble sort
  swapped = true
  while (swapped) do
    swapped = false
    i = 0
    while i < ary.length-1 do
      aryi = ary[i]
      aryi1 = ary[i+1]
      if aryi > aryi1 then
        ary[i] = aryi1
        ary[i+1] = aryi
        swapped = true
      end
      i += 1
    end
  end
end

Copro.gpio_output 1
Copro.gpio(1, true)
Copro.gpio_output 6
Copro.gpio_input(0, Copro.gpio_pullnone)

RES_LEN = 3
buf = Array.new(20)
result = Array.new(RES_LEN)
Copro.gpio(1,false)
Copro.sleep_and_run do
  idx = 0
  while idx < RES_LEN do
    i = 0
    while i < 20 do
      Copro.gpio(6, true)
      Copro.delayUs(10)
      Copro.gpio(6, false)
      buf[i] = Copro.pulseIn(0, true, 200000)
      Copro.delayMs(2999)
      i += 1
    end
    sort!(buf)
    i = 2
    res = 0
    while i < 18 do
      res += buf[i]
      i += 1
    end
    result[idx] = res / 16
    idx += 1
  end
end
Copro.gpio(1,true)
p buf
p result

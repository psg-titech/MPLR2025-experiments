#
# mrbc -Bmrbbuf test_copro_bytecode.rb
#

# puts "Hello"
# aa = Copro.sleep_and_run do
#   a = 0
#   b = 0
#   while a < 100 do
#     if a > 50 then
#       b = b + a
#     end
#     a = a + 1
#   end
#   b
# end
# puts "World"
# puts aa

def hoge(v)
  if v >= 5 then
    (10-v)
  else
    v
  end
end

def fuga(v)
  i = v
  j = 0
  while i < 10 do
    j = hoge(i) * 100
    Copro.gpio(4, true)
    Copro.delayMs(j)
    Copro.gpio(4, false)
    Copro.delayMs(j)
    i = i + 1
  end
  j
end
Copro.gpio_output 4

ii = Copro.sleep_and_run do
  fuga(1)
end
puts ii
puts "OK"

# l = Lcd.init(12, 11, 0, 800000)
# l.put_line(0, Lcd.kagakudaicchi)
# l.put_line(1, Lcd.gaumareta)

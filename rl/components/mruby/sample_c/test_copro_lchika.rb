#
# mrbc -Bmrbbuf test_copro_lchika.rb
#

class Foo
  attr_accessor :count
  def initialize
    @count = 0
  end
  def inc
    @count = @count + 1
  end
  def test

  end
end

Copro.gpio_input(5, Copro.gpio_pulldown)
Copro.gpio_output 4
counting = Foo.new
ret = Copro.sleep_and_run do
    prevPress = false
    press = false
    counting2 = 0
    while (!prevPress || press) do
        Copro.gpio(4, true)
        Copro.delayMs(30)
        Copro.gpio(4, false)
        Copro.delayMs(30)
        prevPress = press
        press = Copro.gpio? 5
        counting.inc()
        counting2 = counting2 + 1
    end
    counting2
end
puts counting.count
puts ret

#
# mrbc -Bmrbbuf test_copro_simple_lchika.rb
#

Copro.gpio_input(5, Copro.gpio_pulldown)
Copro.gpio_output 4
Copro.sleep_and_run do
    prevPress = false
    press = false
    while (!prevPress || press) do
        Copro.gpio(4, true)
        Copro.delayMs(30)
        Copro.gpio(4, false)
        Copro.delayMs(30)
        prevPress = press
        press = Copro.gpio? 5
    end
end
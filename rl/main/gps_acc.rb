class I2C
  def initialize()
    Copro.i2cinit()
  end
  def read(addr, len)
    Copro.i2cread(addr, len)
  end
  def write(addr, data)
    Copro.i2cwrite(addr, data)
  end
end
class ADXLResult
  attr_accessor :x, :y, :z
  def initialize(x, y, z)
    @x = x; @y = y; @z = z
  end
end
class ADXL
  DEV_ADDR = 0x53
  CTRL_REG = 0x2D
  CMD_MEASURE = [CTRL_REG, 2]
  CMD_STANDBY = [CTRL_REG, 0]
  CMD_FIFO = [0x18]
  def initialize(i2c)
    @i2c = i2c
  end
  def on()
    @i2c.write(DEV_ADDR, CMD_MEASURE)
  end
  def off()
    @i2c.write(DEV_ADDR, CMD_STANDBY)
  end
  def conv(ary, base)
    ((ary.getbyte(base) << 26) >> 18) + ary.getbyte(base+1)
  end
  def read()
    @i2c.write(DEV_ADDR, CMD_FIFO)
    Copro.delayMs(5)
    val = @i2c.read(DEV_ADDR, 6)
    if val.size == 0 then
      return nil
    end
    ADXLResult.new(conv(val, 0), conv(val, 2), conv(val, 4))
  end
end

# mock
class GPS
  def initialize(i2c)
  end
end

# class CircularBuffer
#   def initialize(size, val)
#     @ary = Array.new(size, val)
#     @idx = 0
#   end
#   def [](i)
#     @ary[i]
#   end
#   def next=(val)
#     @ary[@idx] = val
#     @idx = (@idx+1) % @ary.size
#   end
#   def avg()
#     i=0; s=@ary.size; sum=0
#     while i < s then
#       sum = @ary[i]
#     end
#     sum /= s
#   end
# end

Copro.gpio_output 1
Copro.gpio(1, true)
i2c = I2C.new()
acc = ADXL.new(i2c)
gps = GPS.new(i2c)
buf = Array.new(12)
#cb = CircularBuffer.new(5, ADXLResult.new(0, 0, 100)) # only Z is 1G.
THRESHOLD = 2000
GRAVITY = 4000 # 1G / 0.25MG/LSB = 4000

def read_for_1sec(acc, buffer)
  i = 0
  res = true
  acc.on()
  Copro.delayMs(20)
  while i < 12 do
    Copro.delayMs(80) # 1000/12
    v = acc.read()
    if v.nil? then
      res = false
    end
    buffer[i] = v
    i += 1
  end
  acc.off()
  res
end

Copro.gpio(1,false)
Copro.sleep_and_run do
  while true do
    Copro.delayMs(7000-100) # 7 sec
    break unless read_for_1sec(acc, buf)
    x = 0; y = 0; z = 0; i = 0
    while i < 12 do
      v = buf[i]
      x += v.x
      y += v.y
      z += v.z
      i += 1
    end
    x = (x.abs() + y.abs() + z.abs())/12
    break if (x - GRAVITY) > THRESHOLD
  end
end
Copro.gpio(1,true)

# do something with gps.

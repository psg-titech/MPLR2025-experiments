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

class SHT30Result
  attr_reader :temp, :rh
  def initialize(temp, rh)
    @temp = temp
    @rh = rh
  end
end

class SHT30
  DEV_ADDR = 0x44
  CMD_MID_REPEAT_READ = [0x24, 0x16]
  def initialize(i2c)
    @i2c = i2c
  end
  def read()
    @i2c.write(DEV_ADDR, CMD_MID_REPEAT_READ)
    Copro.delayMs(6)
    data = @i2c.read(DEV_ADDR, 6)
    if data.size == 0 then
      return nil
    end
    t_ticks = (data.getbyte(0) << 8) + data.getbyte(1)
    # checksum_t = data[2]
    rh_ticks = (data.getbyte(3) << 8) + data.getbyte(4)
    # checksum_rh = data[5]
    t_degC = ((t_ticks * 175)/0xFFFF) - 45
    rh_pRH = ((rh_ticks * 100)/0xFFFF)
    if (rh_pRH > 100) then
       rh_pRH = 100
    end
    if (rh_pRH < 0) then
       rh_pRH = 0
    end
    SHT30Result.new(t_degC, rh_pRH)
  end
end

COUNT=30

Copro.gpio_output 4
Copro.gpio(4, true)
i2c = I2C.new()
sht30 = SHT30.new(i2c)
result = Array.new(COUNT)
Copro.gpio(4,false)
#Copro.sleep_and_run do
  i = 0
  while i < COUNT do
    v = sht30.read()
    break if v.nil?
    result[i] = v
    i += 1
    Copro.delayMs(60*1000)
  end
#end
Copro.gpio(4,true)
result.each do |a|
  puts "tmp: #{a.temp} rh: #{a.rh}"
end

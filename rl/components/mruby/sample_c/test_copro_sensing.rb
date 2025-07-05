#
# mrbc -Bmrbbuf test_copro_sensing.rb
#

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

class MPU6886Result
    attr_reader :x, :y, :z
    def initialize(x, y, z)
        @x = x
        @y = y
        @z = z
    end
end
class MPU6886
    DEV_ADDR = 0x68
    CMD_GYRO = 0x3B

    def initialize(i2c)
        @i2c = i2c
    end
    def read()
        @i2c.write(DEV_ADDR, CMD_GYRO)
        data = @i2c.read(DEV_ADDR, 6)
        MPU6886Result.new(
          data.getbyte(0) << 8 + data.getbyte(1),
          data.getbyte(2) << 8 + data.getbyte(3),
          data.getbyte(4) << 8 + data.getbyte(5))
    end
end

class SHT40Result
    attr_reader :temp, :rh
    def initialize(temp, rh)
        @temp = temp
        @rh = rh
    end
end

class SHT40
    DEV_ADDR = 0x44
    CMD_LOWEST = [0xE0]

    def initialize(i2c)
        @i2c = i2c
    end
    def read()
        @i2c.write(DEV_ADDR, CMD_LOWEST)
        Copro.delayMs(10)
        data = @i2c.read(DEV_ADDR, 6)
        t_ticks = (data.getbyte(0) << 8) + data.getbyte(1)
        # checksum_t = data[2]
        rh_ticks = (data.getbyte(3) << 8) + data.getbyte(4)
        # checksum_rh = data[5]
        t_degC = ((t_ticks * 175)/0xFFFF) - 45
        rh_pRH = ((rh_ticks * 125)/0xFFFF) - 6
        if (rh_pRH > 100) then
            rh_pRH = 100
        end
        if (rh_pRH < 0) then
            rh_pRH = 0
        end
        SHT40Result.new(t_degC, rh_pRH)
    end
end


i2c = I2C.new()
result = Array.new(60)
sht40 = SHT40.new(i2c)
ret = Copro.sleep_and_run do
    i = 0
    while i < 60 do
        result[i] = sht40.read()
        i += 1
        Copro.delayMs(1000)
    end
end
result.each do |a|
  # p a
  puts "tmp: #{a.temp} rh: #{a.rh}"
end
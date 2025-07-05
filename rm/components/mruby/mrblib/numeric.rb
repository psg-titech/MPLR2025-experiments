#
# Integer, mrubyc class library
#
#  Copyright (C) 2015- Kyushu Institute of Technology.
#  Copyright (C) 2015- Shimane IT Open-Innovation Center.
#
#  This file is distributed under BSD 3-Clause License.
#

class Integer

  # times
  def times
    i = 0
    while i < self
      yield i
      i += 1
    end
    self
  end

  # upto
  def upto(max, &block)
    i = self
    while i <= max
      block.call i
      i += 1
    end
    return self
  end

  # downto
  def downto(min, &block)
    i = self
    while min <= i
      block.call i
      i -= 1
    end
    return self
  end

end

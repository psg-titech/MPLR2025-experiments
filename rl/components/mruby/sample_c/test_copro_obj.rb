class MyList
  attr_accessor :ne, :val, :vv, :vvv, :hoge
  VV = 1234
  VVV = "hogehoge"
  def initialize(v, n)
    @val = v
    @ne = n
    @vv = VV
    @vvv = VVV
  end
end

a = [1,2,3,4,5]
sum = 0
ret = Copro.sleep_and_run do
  li = nil
  i = 0
  while i < 10 do
    li = MyList.new(i, li)
    i += 1
  end
  i = 0
  while i < a.size do
    sum += a[i]
    i += 1
  end
  li
end

while ret != nil do
  puts ret.val, ret.vv, ret.hoge, ret.vvv
  ret = ret.ne
end
puts sum

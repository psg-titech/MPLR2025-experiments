#
# mrbc -Bmrbbuf test_copro_sort.rb
#

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

ary = [89,40,67,28,38,92]

Copro.sleep_and_run do
  sort!(ary)
end

p ary

class ArrayTest < Picotest::Test

  description "sort Integer"
  def test_sort_integer
    assert_equal [1, 2, 5], [2, 5, 1].sort
    assert_equal [31, 2000], [2000, 31].sort
  end

  description "sort Symbol"
  def test_sort_symbol
    assert_equal [:a, :b], [:b, :a].sort
    assert_equal [:ab, :abc, :b], [:ab, :b, :abc].sort
    assert_equal [:"2000", :"31"], [:"31", :"2000"].sort
  end

  description "operator +"
  def test_operator
    assert_equal [1,2,3,4], [1,2] + [3,4]
    a = [1,2,3]
    b = a + [4,5]
    assert_equal [1,2,3], a
    assert_equal [1,2,3,4,5], b
    a += b
    assert_equal [1,2,3,1,2,3,4,5], a
    assert_equal [1,2,3,4,5], b
  end

  description "size, length, empty, clear"
  def test_size
    a = [0,1,2,3,4]
    e = []
    assert_equal 5, a.size
    assert_equal 5, a.length
    assert_equal 5, a.count
    assert_equal 0, e.size

    assert_equal a, a
    assert_equal [0,1,2,3,4], a
    assert_equal e, e
    assert_equal [], e

    assert_equal false, a != a
    assert_equal true, a != e

    assert_equal true, e.empty?
    assert_equal false, a.empty?

    assert_equal [], a.clear
    assert_equal [], a
    assert_equal true, a.empty?
  end

  description "constructor"
  def test_constructor
    assert_equal [], Array.new
    assert_equal [nil, nil, nil, nil, nil], Array.new(5)
    assert_equal [0, 0, 0, 0, 0], Array.new(5,0)
    assert_equal ["AB","CD","E"], %w(AB CD E)
    assert_equal [:AB, :CD, :E], %i(AB CD E)
  end

  description "array.[]=(idx, replace)"
  def test_send_aset
    a = [0, 1, 2, 3, 4]
    assert_equal 9, a.[]=(1, 9)
    assert_equal [0, 9, 2, 3, 4], a

    b = [0, 1, 2, 3, 4]
    assert_equal( 9, b.[]=(1, 3, 9))
    assert_equal [0, 9, 4], b
  end

  description "Array#[idx] = replace (GCed value)"
  def test_replace_with_GCed_value
    a = [0,1,2]
    str = "string"
    assert_equal str, a[0] = str
    assert_equal ["string", 1, 2], a

    b = [0,1,2]
    obj = Object.new
    assert_equal obj, b[0, 2] = obj
    assert_equal [obj, 2], b
  end

  description "setter"
  def test_setter
    a = Array.new
    assert_equal 0, a[0] = 0
    assert_equal [0], a
    a[1] = 1
    assert_equal [0,1], a
    a[3] = 3
    assert_equal [0,1,nil,3], a
    a[0] = 99
    assert_equal [99,1,nil,3], a
    a[1] = 88
    assert_equal [99,88,nil,3], a

    a[-1] = 77
    assert_equal [99,88,nil,77], a
    a[-2] = 66
    assert_equal [99,88,66,77], a

    assert_equal 44, a[6,50] = 44
    assert_equal [99,88,66,77,nil,nil,44], a
    a[2,1] = 77
    assert_equal [99,88,77,77,nil,nil,44], a
    a[3,0] = 66
    assert_equal [99,88,77,66,77,nil,nil,44], a
    a[4,3] = 55
    assert_equal [99,88,77,66,55,44], a
    a[4,10] = 44
    assert_equal [99,88,77,66,44], a
  end

  description "getter"
  def test_getter
    a = [1,2,3,4]
    assert_equal 1, a[0]
    assert_equal 1, a.at(0)
    assert_equal 2, a[1]
    assert_equal 3, a[2]
    assert_equal 4, a[3]
    assert_equal nil, a[4]

    assert_equal 4, a[-1]
    assert_equal 3, a[-2]
    assert_equal 2, a[-3]
    assert_equal 1, a[-4]
    assert_equal nil, a[-5]

    assert_equal [2,3], a[1,2]
    assert_equal [2,3,4], a[1,10000]
    assert_equal [4], a[3,1]
    assert_equal [4], a[3,2]
    assert_equal nil, a[10000,2]
    assert_equal nil, a[10000,10000]

    assert_equal [2,3], a[-3,2]
    assert_equal [2,3,4], a[-3,10000]
    assert_equal [1], a[-4,1]
    assert_equal nil, a[-5,1]
    assert_equal nil, a[-10000,1]
    assert_equal nil, a[-10000,10000]
    assert_equal nil, a[-10000,-10000]
  end

  description "index / first / last"
  def test_index
    a = [1,2,3,4]
    e = []
    assert_equal 1, a.index(2)
    assert_equal nil, a.index(0)
    assert_equal nil, e.index(9)

    assert_equal 1, a.first
    assert_equal 4, a.last
    assert_equal nil, e.first
    assert_equal nil, e.last
  end

  description "delete_at"
  def test_delete_at
    a = [1,2,3,4]
    assert_equal 1, a.delete_at(0)
    assert_equal [2,3,4], a
    assert_equal 3, a.delete_at(1)
    assert_equal [2,4], a
    assert_equal 4, a.delete_at(-1)
    assert_equal [2], a
  end

  description "push / pop"
  def test_push
    a = []
    assert_equal [1], a.push(1)
    assert_equal [1,2], a.push(2)
    assert_equal [1,2], a

    assert_equal 2, a.pop()
    assert_equal [1], a
    assert_equal 1, a.pop()
    assert_equal [], a
    assert_equal nil, a.pop()
    assert_equal [], a

    a = [1,2,3,4]
    assert_equal [2,3,4], a.pop(3)
    assert_equal [1], a
    assert_equal [1], a.pop(2)
    assert_equal [], a

    a = []
    assert_equal [1], a << 1
    assert_equal [1,2], a << 2
    assert_equal [1,2], a
  end

  description "unshift / shift"
  def test_shift
    a = []
    assert_equal [1], a.unshift(1)
    assert_equal [2,1], a.unshift(2)
    assert_equal [2,1], a

    assert_equal 2, a.shift()
    assert_equal [1], a
    assert_equal 1, a.shift()
    assert_equal [], a
    assert_equal nil, a.shift()
    assert_equal [], a

    a = [1,2,3,4]
    assert_equal [1,2,3], a.shift(3)
    assert_equal [4], a
    assert_equal [4], a.shift(2)
    assert_equal [], a
  end

  description "dup"
  def test_dup
    a = [1,2,3]
    b = a
    a[0] = 11
    assert_equal a, b
    assert_equal [11,2,3], b

    a = [1,2,3]
    b = a.dup
    a[0] = 11
    assert_not_equal a, b
    assert_equal [11,2,3], a
    assert_equal [1,2,3], b
  end

  description "min, max, minmax"
  def test_minmax
    a = %w(albatross dog horse)
    assert_equal "albatross", a.min
    assert_equal "horse", a.max
    assert_equal ["albatross","horse"], a.minmax

    a = ["AAA"]
    assert_equal "AAA", a.min
    assert_equal "AAA", a.max
    assert_equal ["AAA","AAA"], a.minmax

    a = []
    assert_equal nil, a.min
    assert_equal nil, a.max
    assert_equal [nil,nil], a.minmax
  end

  description "inspect, to_s, join"
  def test_inspect
    a = [1,2,3]
    assert_equal "[1, 2, 3]",       a.inspect
    assert_equal "[1, 2, 3]",       a.to_s
    assert_equal "123",             a.join
    assert_equal "1,2,3",           a.join(",")
    assert_equal "1, 2, 3",         a.join(", ")

    array = [1,"AA",:sym]
    hash = {1=>1, :k2=>:v2, "k3"=>"v3"}
    range = 1..3
    a = [nil, false, true, 123, 2.718, :symbol, array, "string", range, hash]
    assert_equal %q![nil, false, true, 123, 2.718, :symbol, [1, "AA", :sym], "string", 1..3, {1 => 1, k2: :v2, "k3" => "v3"}]!, a.inspect
    assert_equal %q![nil, false, true, 123, 2.718, :symbol, [1, "AA", :sym], "string", 1..3, {1 => 1, k2: :v2, "k3" => "v3"}]!, a.to_s
    assert_equal %q!,false,true,123,2.718,symbol,1,AA,sym,string,1..3,{1 => 1, k2: :v2, "k3" => "v3"}!, a.join(",")
  end

  description "each"
  def test_each

    a = [1,2,3]
    $cnt = 0
    a.each {|a1|
      $cnt += 1
      assert_equal a1, $cnt
    }
    assert_equal 3, $cnt

    $cnt = 0
    a.each_index {|i|
      assert_equal i, $cnt
      $cnt += 1
      assert_equal a[i], $cnt
    }
    assert_equal 3, $cnt

    $cnt = 0
    a.each_with_index {|a1,i|
      $cnt += 1
      assert_equal a1, $cnt
      assert_equal a[i], $cnt
    }
    assert_equal 3, $cnt
  end

  description "collect"
  def test_collect
    a = [1,2,3]
    assert_equal [2,4,6], a.collect {|a1| a1 * 2}
    assert_equal [1,2,3], a

    a.collect! {|a1| a1 * 2}
    assert_equal [2,4,6], a
  end

  description "include?"
  def test_include
    assert_true [1,2,3].include?(1)
    assert_true [[1],2,3].include?([1])
    assert_false [1,2,3].include?(4)
    assert_false [1,2,3].include?("3")
    assert_false [].include?(true)
  end

  description "& (and) operation"
  def test_and_operation
    assert_equal [1, 3], [1, 1, 2, 3] & [3, 1, 4]
    assert_raise(TypeError, "no implicit conversion into Array") do
      [1] & 1
    end
  end

  description "| (or) operation"
  def test_or_operation
    assert_equal [1, 4, 2, 3, 5], [1, 1, 4, 2, 3] | [5, 4, 5]
    assert_raise(TypeError, "no implicit conversion into Array") do
      [1] | "1"
    end
  end

  description "uniq test"
  def test_uniq
    a = %W(A B C B)
    assert_equal %W(A B C), a.uniq
    assert_equal %W(A B C B), a

    assert_equal %W(A B C), a.uniq!
    assert_equal %W(A B C), a

    assert_equal nil, a.uniq!
    assert_equal %W(A B C), a
  end

end

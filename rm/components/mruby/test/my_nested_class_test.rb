
class MyNestedClassTest < Picotest::Test

  # Can't use setup
  class A
    def h; 'A' ;end
    class B
      def h; 'B' ;end
      class C
        def h; 'C' ;end
        class D
          def h; 'D' ;end
        end
      end
    end
  end

  description 'nested class'
  def test_nested
    assert_equal "A", A.new.h
    assert_equal "B", A::B.new.h
    assert_equal "C", A::B::C.new.h
    assert_equal "D", A::B::C::D.new.h
  end
  
end

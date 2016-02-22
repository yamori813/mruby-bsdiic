##
## BsdIic Test
##

assert("BsdIic#hello") do
  t = BsdIic.new "hello"
  assert_equal("hello", t.hello)
end

assert("BsdIic#bye") do
  t = BsdIic.new "hello"
  assert_equal("hello bye", t.bye)
end

assert("BsdIic.hi") do
  assert_equal("hi!!", BsdIic.hi)
end

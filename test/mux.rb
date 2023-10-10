#!/usr/local/bin/mruby
# PCA9547D channel change script

MUXADDR = 0x70

ch = ARGV[0] .to_i

if ch >= 0 && ch <= 7
  t = BsdIic.new(0)

  arr = Array.new

  arr[0] = 8 | ch

  t.write(MUXADDR, arr)
end

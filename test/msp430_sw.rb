#!/usr/local/bin/mruby

MSPADDR = 0x48

t = BsdIic.new(0)

last = t.read(MSPADDR, 1, 1)
loop do
  cur = t.read(MSPADDR, 1, 1)
  if cur == 0 && last != cur
    p "push switch"
  end
  last = cur
  usleep 100_000
end


#!/usr/local/bin/mruby

MSPADDR = 0x48

t = BsdIic.new(0)

loop do
    t.write(MSPADDR, 0, 0)
    sleep 2
    t.write(MSPADDR, 0, 1)
    sleep 2
end


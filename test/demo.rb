#!/usr/local/bin/mruby

MSPADDR = 0x48
LCDADDR = 0x3e
PICADDR = 0x04

LCD_CLEARDISPLAY = 0x01
LCD_RETURNHOME = 0x02
LCD_ENTRYMODESET = 0x04
LCD_DISPLAYCONTROL = 0x08
LCD_CURSORSHIFT = 0x10
LCD_FUNCTIONSET = 0x20
LCD_SETCGRAMADDR = 0x40
LCD_SETDDRAMADDR = 0x80

def getvol(t)
  t.write(PICADDR, 0, 0)
  vol = t.read(PICADDR, 1, 0) << 8 |  t.read(PICADDR, 1, 1)
  t.read(PICADDR, 1, 2)
  vol
end

def dispvol(t, v, c)
  t.write(LCDADDR, [0x00, LCD_SETDDRAMADDR | 0x40])
  usleep 2_000
  sp = " " * (8 - v.to_s.length)
  arr = [0x40]
  arr += sp.to_s.bytes
  arr += v.to_s.bytes
  t.write(LCDADDR, arr)
  t.write(LCDADDR, [0x00, LCD_SETDDRAMADDR | 0x00 + c])
end

t = BsdIic.new(0)

count = 0

t.write(LCDADDR, [0x38, 0x39, 0x14, 0x70, 0x56, 0x6c])
usleep 200_000
t.write(LCDADDR, [0x38, 0x0d, LCD_CLEARDISPLAY])
usleep 2_000

vol = getvol(t)
dispvol(t, vol, count)
lastvol = vol

lastkey = 0
loop do
  key = t.read(MSPADDR, 1, 2)
  if key != 0 && key != lastkey
    if key == "#".ord
      t.write(LCDADDR, [0x38, 0x0d, LCD_CLEARDISPLAY])
      usleep 2_000
      count = 0
      dispvol(t, lastvol, count)
    else
      arr = [0x40]
      arr.push(key)
      t.write(LCDADDR, arr)
      count += 1
    end
  end
  vol = getvol(t)
  if lastvol != vol
    dispvol(t, vol, count)
    lastvol = vol
  end
  lastkey = key
  usleep 100_000
end


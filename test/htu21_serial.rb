#!/usr/local/bin/mruby

def htu21_crc_check value, crc
  
  polynom = 0x988000   # x^8 + x^5 + x^4 + 1
  msb     = 0x800000
  mask    = 0xFF8000
  result  = value << 8   # Pad with zeros as specified in spec 

  while msb != 0x80
    if (result & msb) != 0
      result = ((result ^ polynom) & mask) | ( result & ~mask)
    end
    # Shift by one
    msb >>= 1;
    mask >>= 1;
    polynom >>=1;
  end
  result == crc
end

begin

HTU21_ADDR = 0x40

HTU21_READ_SERIAL_FIRST_8BYTES_COMMAND = [0xFA, 0x0F]
HTU21_READ_SERIAL_LAST_6BYTES_COMMAND = [0xFC, 0xC9]

t = BsdIic.new(0)

rcv_data = t.read(HTU21_ADDR, 8, HTU21_READ_SERIAL_FIRST_8BYTES_COMMAND)
rcv_data += t.read(HTU21_ADDR, 6, HTU21_READ_SERIAL_LAST_6BYTES_COMMAND)

values = [0, 2, 4, 6]
values.each do |n|
  if !htu21_crc_check rcv_data[n], rcv_data[n+1]
    raise
  end
end

values = [8, 11]
values.each do |n|
  if !htu21_crc_check rcv_data[n] << 8 | rcv_data[n+1], rcv_data[n+2]
    raise
  end
end

serial_number = ""
values = [0, 2, 4, 6, 8, 9, 11, 12]
values.each do |n|
  serial_number += rcv_data[n].to_s(16).rjust(2, '0')
end

p serial_number

rescue
p "CRC Error"
end

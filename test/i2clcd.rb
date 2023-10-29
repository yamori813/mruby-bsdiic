#!/usr/local/bin/mruby
# SB0802G display script

LCDADDR = 0x3e

LCD_CLEARDISPLAY = 0x01
LCD_RETURNHOME = 0x02
LCD_ENTRYMODESET = 0x04
LCD_DISPLAYCONTROL = 0x08
LCD_CURSORSHIFT = 0x10
LCD_FUNCTIONSET = 0x20
LCD_SETCGRAMADDR = 0x40
LCD_SETDDRAMADDR = 0x80

t = BsdIic.new(0)

t.write(LCDADDR, [0x38, 0x39, 0x14, 0x70, 0x56, 0x6c])
usleep 200_000
t.write(LCDADDR, [0x38, 0x0d, 0x01])
usleep 2_000

t.write(LCDADDR, [0x00, LCD_CLEARDISPLAY])
usleep 2_000
arr = [0x40]
arr += "Hello".bytes
t.write(LCDADDR, arr)

t.write(LCDADDR, [0x00, LCD_SETDDRAMADDR | 0x40])
usleep 2_000
arr = [0x40]
arr += "mruby".bytes
t.write(LCDADDR, arr)

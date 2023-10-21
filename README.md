# mruby-bsdiic   [![Build Status](https://travis-ci.org/yamori813/mruby-bsdiic.svg?branch=master)](https://travis-ci.org/yamori813/mruby-bsdiic)
BsdIic class

This is FreeBSD iic support class

support one byte register address and one byte data access and array data
send.

read(device address, read size, [write data])
device address : int, read size : int, write data : array
result : array

write(device address, register address, data)
device address : int, register address : int, data : int

write(device address, write data)
device address : int, write data : array

## install by mrbgems
- add conf.gem line to `build_config.rb`

```ruby
MRuby::Build.new do |conf|

    # ... (snip) ...

    conf.gem :github => 'yamori813/mruby-bsdiic'
end
```
## example
```ruby
t = BsdIic.new(0)
p t.read(0x50, 1, [2]).first
#=> 32
t.write(0x50, 4, 7)
p t.read(0x50, 1, [4]).first
#=> 7
t.write(4, [0x33, 0x4d, 0x10])
```

## License
under the BSD License:
- see LICENSE file

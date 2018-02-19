# mruby-bsdiic   [![Build Status](https://travis-ci.org/yamori813/mruby-bsdiic.svg?branch=master)](https://travis-ci.org/yamori813/mruby-bsdiic)
BsdIic class

only support one byte register address and one byte data format.

read(device address, register address)

write(device address, register address, data)
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
p t.read(0x50,2)
#=> 32
t.write(0x50,4,7)
p t.read(0x50,4)
#=> 7
t.writes(4, [0x33, 0x4d, 0x10])
```

## License
under the BSD License:
- see LICENSE file

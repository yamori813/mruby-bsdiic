# mruby-bsdiic   [![Build Status](https://travis-ci.org/yamori813/mruby-bsdiic.svg?branch=master)](https://travis-ci.org/yamori813/mruby-bsdiic)
BsdIic class
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
```

## License
under the BSD License:
- see LICENSE file

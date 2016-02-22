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
p BsdIic.hi
#=> "hi!!"
t = BsdIic.new "hello"
p t.hello
#=> "hello"
p t.bye
#=> "hello bye"
```

## License
under the BSD License:
- see LICENSE file

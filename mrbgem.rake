MRuby::Gem::Specification.new('mruby-bsdiic') do |spec|
  spec.license = 'BSD'
  spec.authors = 'Hiroki Mori'
  cc.flags = "-DUSE_RDWR"
end

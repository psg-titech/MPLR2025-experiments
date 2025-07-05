# mruby/c

[![Ruby](https://github.com/mrubyc/mrubyc/actions/workflows/test.yml/badge.svg)](https://github.com/mrubyc/mrubyc/actions/workflows/test.yml)

[![Join the chat at https://gitter.im/mrubyc/mrubyc](https://badges.gitter.im/mrubyc/mrubyc.svg)](https://gitter.im/mrubyc/mrubyc?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

mruby/c is another implementation of mruby.

- Small memory consumption
- Limited class libraries
- Small size rather than execution speed

### Comparison between mruby and mruby/c

||mruby/c|mruby|
|----|----|----|
|memory size| < 40KB | < 400KB |
|main target| one-chip microprocessors | general embedded software|


## Documents

[How to compile?](doc/compile.md)

[How to run tests?](doc/test.md)


## Developer team

- [Shimane IT Open-Innovation Center](http://www.s-itoc.jp/)
- [Kyushu Institute of Technology](http://www.kyutech.ac.jp/)

## License

mruby/c is released under the Revised BSD License(aka 3-clause license).

## Related work

- Device classes for mruby/c (https://github.com/mrubyc/dev)
- Some sample programs that mainly control sensors. (https://github.com/mrubyc/devkit02/tree/main/samples)

# mC Compiler

## About

This project was a university team project, completed in collaboration with Raoul Shikora. 
We wrote a compiler from "mini-C" to assembly, the specification (which was provided by the university) can be found in 
`docs/specification.md`.

## Prerequisites

- [Meson](http://mesonbuild.com/) in version `0.47.0` or higher
  (you may want to install it via `pip3 install --user meson`)
- [Ninja](https://ninja-build.org/)
- `time`, typically located at `/usr/bin/time`, do not confuse this with the Bash built-in
- `flex` for generating the lexer
- `bison` for generating the parser
- A compiler supporting C11 — typically GCC or Clang
- glibc version 2.20 or higher
- For execution of `mcc`, gcc multi-lib support must be available for the compiler backend.

## Build Instructions

First, generate the build directory.

    $ meson builddir
    $ cd builddir

Meson creates Ninja build files.
Let's build.

    $ ninja

Unit tests can be run directly with Ninja (or Meson).

    $ ninja test

For integration testing we try to compile mC programs and compare their output for a given input.

    $ ../scripts/run_integration_tests

For simple integration testing of the generated assembly code, we compile mC programs and check if their return values
match the expected values:

    $ ../scripts/run_asm_test.sh

## Usage

Use the generated `mcc` app to compile mC-programs as follows:

    $ ./mcc test.mc

On successful compilation `mcc` will generate an executable from test.mc named `a.out`, execute it like this:

    $ ./a.out

See `mcc -h` for more info about command line arguments.
For intermediate stages of the compilation pipeline printers are provided, see [development
notes](docs/development_notes.md) for more details.

## Known Issues

### Compiler warning 

Since the unit test matches any grammar rule, there are 5 reduce/reduce-conflicts. These will however not arise, when 
the testing mode isn't specified, because the parser can only match the unit-test grammar rule when a dedicated variable
is set (which can only happen when the function is called via the library API, not when parsing input from `mcc`).


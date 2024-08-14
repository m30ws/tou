# tou.h

Collection of various help(er|ful) functions, structures, data structures, preprocessor definitions, systems and pretty much anything else you may want to use in your C/C++ projects to make your (my) life easier. Continuously upgraded over time as I feel the repetitive need for things while working on other projects.

Might consider a many-headers version sometime in the future. However, packaged all in one file, it should be a very simple process to include it wherever you would want.
> *If it no work, it my fault for write crappy code. Not some build system you may or may not be able to install and run without losing a chunk of your sanity.*

## Usage
You must use `#define TOU_IMPLEMENTATION` before including this file (and do it only in ONE place/file!) otherwise linker will complain.

### Test
*Not really a test, more like I just threw a whole bunch of imaginary scenarios at the main() function and called it a day (days), but should still test some things and show some usage examples in the meantime. Enable some and build.*

Made a switch to using `justfile`s for building and running so if just (https://github.com/casey/just) is installed use
```sh
just rebuild
just list # to see all available commands
```
If not, look into `justfile` to see the commands it runs.
Alternatively, use a simple
```sh
gcc tou_test.c -o tou_test -std=c11 && ./tou_test
```
which should suffice. 
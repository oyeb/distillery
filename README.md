[![Project Status: Active – The project has reached a stable, usable state and is being actively developed.](http://www.repostatus.org/badges/latest/active.svg)](http://www.repostatus.org/#active)
[![license](https://img.shields.io/badge/license-Unlicense-blue.svg)](https://github.com/oyeb/distillery/blob/master/LICENSE)
[![Build Status](https://travis-ci.com/oyeb/distillery.svg?branch=master)](https://travis-ci.com/oyeb/distillery)
[![codecov](https://codecov.io/gh/oyeb/distillery/branch/master/graph/badge.svg)](https://codecov.io/gh/oyeb/distillery)
[![Documentation Status](https://readthedocs.org/projects/distillery/badge/?version=latest)](https://distillery.readthedocs.io/en/latest/?badge=latest)

<!--
[![Lines of Code](https://tokei.rs/b1/github/oyeb/distillery)](https://github.com/Aaronepower/tokei)
[![Build status](https://ci.appveyor.com/api/projects/status/g9bh9kjl6ocvsvse/branch/master?svg=true)](https://ci.appveyor.com/project/bsamseth/cpp-project/branch/master)
[![Coverage Status](https://coveralls.io/repos/github/bsamseth/cpp-project/badge.svg?branch=master)](https://coveralls.io/github/bsamseth/cpp-project?branch=master)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/eb004322b0d146239a57eb242078e179)](https://www.codacy.com/app/bsamseth/cpp-project?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=bsamseth/cpp-project&amp;utm_campaign=Badge_Grade)
[![Language grade: C/C++](https://img.shields.io/lgtm/grade/cpp/g/bsamseth/cpp-project.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/bsamseth/cpp-project/context:cpp)
[![Total alerts](https://img.shields.io/lgtm/alerts/g/bsamseth/cpp-project.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/bsamseth/cpp-project/alerts/)
[![Average time to resolve an issue](http://isitmaintained.com/badge/resolution/bsamseth/cpp-project.svg)](http://isitmaintained.com/project/bsamseth/cpp-project "Average time to resolve an issue")
[![Percentage of issues still open](http://isitmaintained.com/badge/open/bsamseth/cpp-project.svg)](http://isitmaintained.com/project/bsamseth/cpp-project "Percentage of issues still open")
-->

# Distillery

This is a bare-bones program analysis library that exposes simple interfaces to
control flow graphs and concrete analyses.

## Building

Build by making a build directory (i.e. `build/`), run `cmake` in that dir, and
then use `make` to build the desired target.

Example:

``` bash
> mkdir build && cd build
> cmake .. -DCMAKE_BUILD_TYPE=[Debug | Coverage | Release]
> make
> ./main
> make test                       # Makes and runs the tests.
> make coverage                   # Generate a coverage report.
> make Doxygen && make Sphinx     # Generate html documentation.
```

## Attribution
Many thanks to this [starter template](https://github.com/bsamseth/cpp-project)!

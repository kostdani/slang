# build.sh


# [[file:README.org::*build.sh][build.sh:1]]
#!/bin/sh
  mkdir -p build
  cd build
## build folder for cmake is build
  cmake ..
  make
# build.sh:1 ends here

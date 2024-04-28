# DPFPSI
A C++ implementation of labeled Private Set Intersection from Distributed Point Function.

## How to build

### [vcpkg](https://github.com/microsoft/vcpkg)

```
vcpkg install kuku
vcpkg install openssl
vcpkg install ms-gsl
vcpkg install boost-system 
vcpkg install boost-regex
```

### Build
```
git clone --recursive https://github.com/lqvir/DPFPSI.git
mkdir build 
cd build 
cmake .. -DCMAKE_TOOLCHAIN_FILE=/your/path
cmake --build . 
```

## Note 

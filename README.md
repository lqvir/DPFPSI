# DPFPSI
A C++ implementation of labeled Private Set Intersection from Distributed Point Function.

## How to build

### [vcpkg](https://github.com/microsoft/vcpkg)

Vcpkg is a helpful package manager for C and C++. If you've never used vcpkg before, or if you're trying to figure out how to use vcpkg, check out their [Getting Started section](https://github.com/microsoft/vcpkg?tab=readme-ov-file#getting-started) for how to start using vcpkg. Employing vcpkg, we install the dependencies for our DPFPSI by the following commands.
```
vcpkg install kuku
vcpkg install openssl
vcpkg install ms-gsl
vcpkg install boost-system 
vcpkg install boost-regex
vcpkg install boost-asio
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
We invoke the Garble Circuit based OPRF in [mobile_psi_cpp](https://github.com/contact-discovery/mobile_psi_cpp) including an old version librelic which may fail to build by the error in 'mobile_psi_cpp/droidCrypto/relic/src/md/blake.h'. We can handle this error as follows:
``` cpp
// Update the line 64 in obile_psi_cpp/droidCrypto/relic/src/md/blake.h 
// ALIGNME( 64 ) typedef struct __blake2s_state
typedef struct __blake2s_state

// Update the line 89 in obile_psi_cpp/droidCrypto/relic/src/md/blake.h 
// ALIGNME( 64 ) typedef struct __blake2b_state
typedef struct __blake2b_state
```
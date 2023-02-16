![Header](/imgs/banner.png)

We are building a vocoder, which is a vocal effect that combines your voice with a sound wave. The result is a unique sound used in popular songs like Daft Punk's "Harder, Better, Faster, Stronger" (the inspiration for our name), or ELO's "Mr. Blue Sky".

# Pre-requisites

To build just the vocoder itself you will need:

- Software
    - CMake 3.16+
    - C++11 compiler
    - Debian
- Hardware
    - Raspberry Pi 3 (or newer)
    - Codec Zero

Additionally, to run the unit tests and static analysis suite, LLVM 15 + Tools is required. To install this on debian the automatic script from [their website](https://apt.llvm.org/) can be used.

You will need to add clang and the appropriate tools to your PATH if you want the automated setup script to work. This can be done with:

```
sudo ln -s /usr/lib/llvm-15/bin/clang{,++,-format,-tidy} /usr/local/bin
```

# Getting started (using Clang)

Once you have installed the prerequisites and configured your hardware you should clone the repository and run the setup script.

```
git clone git@github.com:daft-engineers/vocoder.git
cd vocoder
chmod +x setup.sh
./setup.sh
```

The project is now ready to be built.

```
cd build
make
cd unit_tests  # run unit tests
ctest
cd ../src      # run vocoder
./vocoder
```

![Header](https://user-images.githubusercontent.com/10051310/219484888-cb6844f1-e377-42c5-ac6a-09ce9f30a023.png)

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

# Social Media
<a href="https://www.instagram.com/daftengineers/"><img src="https://user-images.githubusercontent.com/10051310/219481632-10430e66-73dc-400e-a046-dc9d7dd9f3a9.svg" height=80px></a>
<a href="https://twitter.com/DaftEngineers"><img src="https://user-images.githubusercontent.com/10051310/219482103-8422c45f-ca60-4918-b5b8-6de79d0add22.png" height=80px></a>
<a href="https://www.youtube.com/@daftengineers"><img src="https://user-images.githubusercontent.com/10051310/219484585-2eb87c95-5951-428b-b3a1-7ac846e40f65.png" height=80px>

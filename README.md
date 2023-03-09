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

Python 3 is required to automatically build and link unit tests. Python 3 comes preinstalled on most Linux distributions, and is available as a package on all others.

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

## Running our validation suite
As well as using the GoogleTest framework to provide unit and integration tests, we also use clang-tidy for static analysis and clang-format to ensure code meets style standards. To run the static analysis and style tests use:

```
chmod +x checks.sh
./checks.sh
```

This will run static analysis and return any errors found. It will also reformat the code in-place to meet our style guidelines, which are based on the LLVM guidelines. If you would like to check if the code meet style guideline _without_ it being updated in place, then the flag `--formatting` can be passed to the script.

# Social Media
<a href="https://www.instagram.com/daftengineers/"><img src="https://user-images.githubusercontent.com/10051310/219481632-10430e66-73dc-400e-a046-dc9d7dd9f3a9.svg" height=80px></a>
<a href="https://twitter.com/DaftEngineers"><img src="https://user-images.githubusercontent.com/10051310/219482103-8422c45f-ca60-4918-b5b8-6de79d0add22.png" height=80px></a>
<a href="https://www.youtube.com/@daftengineers"><img src="https://user-images.githubusercontent.com/10051310/219484585-2eb87c95-5951-428b-b3a1-7ac846e40f65.png" height=80px></a>
<a href="https://www.tiktok.com/@daftengineers"><img src="https://user-images.githubusercontent.com/10051310/220791551-141ee0cc-34ef-47b8-bbcb-701461f88851.png" height=80px></a>

# Program Design
## Sequence Diagram
![sequence diagram for vocoder](https://user-images.githubusercontent.com/10051310/220790624-48ad3c57-34fe-4f8b-b89b-a98e7c718f88.png)

Sequence diagram generated with TikZ-UML library, source is available upon request.

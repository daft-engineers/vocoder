![Header](https://user-images.githubusercontent.com/10051310/219484888-cb6844f1-e377-42c5-ac6a-09ce9f30a023.png)

We are building a vocoder, which is a vocal effect that combines your voice with a sound wave. The result is a unique sound used in popular songs like Daft Punk's "Harder, Better, Faster, Stronger" (the inspiration for our name), or ELO's "Mr. Blue Sky".

# Pre-requisites

To build just the vocoder itself you will need:

- Software
    - CMake 3.16+
    - C++11 compiler
    - Debian
    - ALSA
- Hardware
    - Raspberry Pi 3 (or newer) running a 64bit OS
    - Codec Zero

Additionally, to run the unit tests and static analysis suite, LLVM 15 + Tools is required. To install this on debian the automatic script from [their website](https://apt.llvm.org/) can be used.

To build the doxygen documentation, the `doxygen` and `graphviz` packages are required.

We recommend the following to get started from scratch (requires 1GB+ of storage):

```
sudo apt update
sudo apt install wget gpg build-essential cmake libasound2-dev
wget https://apt.llvm.org/llvm.sh
chmod +x llvm.sh
sudo ./llvm.sh 15 all
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

# Documentation

Documentation for main can be [found here](https://daft-engineers.github.io/vocoder/).

If you want to generate the documentation for yourself, the `doxygen` and `graphviz` packages are required. These can be installed with the following command on ubuntu:

```$
sudo apt install doxygen graphviz
```
With all the required dependences, from the root of the project, the command 
```$
doxygen Doxyfile
```
generates the documentation, and then the page can be opened by navigating to `docs/html/` and opening `index.html`

# Social Media
<a href="https://www.instagram.com/daftengineers/"><img src="https://user-images.githubusercontent.com/10051310/219481632-10430e66-73dc-400e-a046-dc9d7dd9f3a9.svg" height=80px></a>
<a href="https://twitter.com/DaftEngineers"><img src="https://user-images.githubusercontent.com/10051310/219482103-8422c45f-ca60-4918-b5b8-6de79d0add22.png" height=80px></a>
<a href="https://www.youtube.com/@daftengineers"><img src="https://user-images.githubusercontent.com/10051310/219484585-2eb87c95-5951-428b-b3a1-7ac846e40f65.png" height=80px></a>
<a href="https://www.tiktok.com/@daftengineers"><img src="https://user-images.githubusercontent.com/10051310/220791551-141ee0cc-34ef-47b8-bbcb-701461f88851.png" height=80px></a>

# Program Design
## Sequence Diagram
![sequence diagram for vocoder](https://user-images.githubusercontent.com/10051310/220790624-48ad3c57-34fe-4f8b-b89b-a98e7c718f88.png)

Sequence diagram generated with TikZ-UML library, source is available upon request.

# Project management

Regualar scrum meetings were held through the project, notes are available [here](https://github.com/daft-engineers/project/blob/main/README.md). 

The project uses github's issue system for management. The project board is located [here.](https://github.com/orgs/daft-engineers/projects/1/views/3) ![Image](https://user-images.githubusercontent.com/56744932/232234904-d9e0ba0a-6532-460d-966f-4a65a5f11cc9.png)

# Contributions 

Contributions must be made in the form of a pull request and must pass the unit tests and validation suite. Any contrbutions must be licenceable under the MPL 2.0.

# Licence

The project is licenced under the Mozilla Public License Version 2.0. More details can be found in the licence file, and [here](https://choosealicense.com/licenses/mpl-2.0/).

# Continuous integration

<<<<<<< HEAD
The project has a fleshed out continuous integration pipeline run on a Raspberry Pi. On every push and pull request, the project is built, the static analysis and formatting is run, and unit tests are run. Only on a successful merge to the main branch, the [hosted documentation](https://daft-engineers.github.io/vocoder/) is updated. The image below shows the workflow for a standard push.
=======
The project has a fleshed out continuous integration pipeline run on a Raspberry Pi. On every push and pull request, the project is built, the static analysis and formatting is run, and unit tests are run. Only on a successful merge to the main branch, the [hosted documentation](https://daft-engineers.github.io/vocoder/annotated.html) is updated. The image below shows the workflow for a standard push.
>>>>>>> added link to choosealicence

![Image](https://user-images.githubusercontent.com/56744932/232238323-5977fc16-2d5f-4533-b965-6143a109ffd9.png)

Nightly, a more extensive test suite is run. This suite takes too long to run on every commit, but is valuable for finding threading bugs in particular.

![Image](https://user-images.githubusercontent.com/56744932/232495456-cb85b482-2e24-48c8-92cf-c3656f83ca41.png)

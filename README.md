# cad-2023

## Index:
1. [Authors](https://github.com/rzimmerdev/cad-2023/tree/main#authors)
2. [Installation](https://github.com/rzimmerdev/cad-2023/tree/main#installation)
3. [Usage](https://github.com/rzimmerdev/cad-2023/tree/main#usage)
4. [Details](https://github.com/rzimmerdev/cad-2023/tree/main#details)

## Authors

* __Danielle Modesti__
* __Hélio Nogueira Cardoso__
* __Pedro Falcão Rocha__
* __Luis Eduardo de Brito Câmara__
* __Rafael Zimmer__

## Installation

Implementation used: [OpenMPI](https://www.open-mpi.org/software/ompi/v5.0/)

* On [Ubuntu](https://packages.ubuntu.com/search?keywords=openmpi):

```bash
sudo apt-get install openmpi-bin openmpi-doc libopenmpi-dev
```

* On [RedHat/Centos](https://linuxdesktops.soton.ac.uk/openmpi.html):

```bash
sudo yum install openmpi openmpi-devel
export PATH=$PATH:/usr/lib/openmpi/bin
```

* On [Fedora/Arch](https://archlinux.org/packages/extra/x86_64/openmpi/):

```bash
sudo dnf install openmpi openmpi-devel
source /etc/profile.d/modules.sh
module load mpi/openmpi-x86_64
```

* On [MacOS](https://formulae.brew.sh/formula/open-mpi)

```bash
brew install open-mpi
```

* On [Windows](https://learn.microsoft.com/en-us/message-passing-interface/microsoft-mpi):

Download the wizard and install from the official [Microsoft-MPI](https://learn.microsoft.com/en-us/message-passing-interface/microsoft-mpi) page, and add to the PATH environment variable if needed. 

## Usage

To compile and link all source code, and header files use:
```bash
make all
```

To run the main source file, use (where _<args>_ are arguments accepted by mpirun):
```bash
make N=3 S=42 T=4 run
```

Example:

```bash
> make all
> make N=3 S=42 T=4 run

Hello from MPI process 0 out of 4
Hello from MPI process 1 out of 4
...
Hello from thread 4 of MPI process 0
Hello from thread 4 of MPI process 2
```

## Details

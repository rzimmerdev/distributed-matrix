# distributed-matrix

> A point-wise matrix distances calculator, 
> implemented in C/MP+MPI using distributed memory and parallel processing paradigms.

## Index:
1. [Authors](https://github.com/rzimmerdev/cad-2023/tree/main#authors)
2. [Installation](https://github.com/rzimmerdev/cad-2023/tree/main#installation)
3. [Usage](https://github.com/rzimmerdev/cad-2023/tree/main#usage)
4. [Details](https://github.com/rzimmerdev/cad-2023/tree/main#details)

## Authors

* __Danielle Modesti__, nUSP ********
* __Hélio Nogueira Cardoso__, nUSP ********
* __Pedro Falcão Rocha__, nUSP ********
* __Luis Eduardo de Brito Câmara__, nUSP ********
* __Rafael Zimmer__, nUSP ********

## Installation

Implementation used: [OpenMPI](https://www.open-mpi.org/software/ompi/v5.0/)

* On [Ubuntu](https://packages.ubuntu.com/search?keywords=openmpi):

```bash
$ sudo apt-get install openmpi-bin openmpi-doc libopenmpi-dev
```

* On [RedHat/Centos](https://linuxdesktops.soton.ac.uk/openmpi.html):

```bash
$ sudo yum install openmpi openmpi-devel
$ export PATH=$PATH:/usr/lib/openmpi/bin
```

* On [Fedora/Arch](https://archlinux.org/packages/extra/x86_64/openmpi/):

```bash
$ sudo dnf install openmpi openmpi-devel
$ source /etc/profile.d/modules.sh
$ module load mpi/openmpi-x86_64
```

* On [MacOS](https://formulae.brew.sh/formula/open-mpi)

```bash
$ brew install open-mpi
```

* On [Windows](https://learn.microsoft.com/en-us/message-passing-interface/microsoft-mpi):

Download the wizard and install from the official [Microsoft-MPI](https://learn.microsoft.com/en-us/message-passing-interface/microsoft-mpi) page, and add to the PATH environment variable if needed. 

## Usage

To compile and link all source code, and header files use:
```bash
$ make all
```

To run the main source file, use (where _<args>_ are arguments accepted by mpirun):
```bash
$ make run <N> <S> <T> [P=10]
```

Example:

```bash
$ make run N=100 S=42 T=4 
Distância Manhattan mínima: 0 (soma min: 57015) e máxima: 287 (soma max: 2130946)
Distância Euclidiana mínima: 0.00 (soma min: 39607.81) e máxima: 165.71 (soma max: 1250695.85)
```

## Details

The Makefile also has a timing utility, which can be used to measure the execution time of the program. To use it, run:

```bash
$ make time <N> <S> <T> [P=10]
```

Example:

```bash
$ make time P=2 N=150 S=42 T=10
Calculating average time over 5 runs...
Average time: 5.778 seconds
```

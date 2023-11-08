# cad-2023

## Index:
- [Authors](https://github.com/rzimmerdev/cad-2023/tree/main#authors)
- [Installation](https://github.com/rzimmerdev/cad-2023/tree/main#installation)
- [Usage](https://github.com/rzimmerdev/cad-2023/tree/main#usage)
- [Details](https://github.com/rzimmerdev/cad-2023/tree/main#details)

## Authors

* Rafael Zimmer
* Danielle Modesti
* Hélio Nogueiro Cardoso

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

## Details

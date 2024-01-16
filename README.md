srsRAN Project
==============

[![Build Status](https://github.com/srsran/srsRAN_Project/actions/workflows/ccpp.yml/badge.svg?branch=main)](https://github.com/srsran/srsRAN_Project/actions/workflows/ccpp.yml)
[![OpenSSF Best Practices](https://www.bestpractices.dev/projects/7868/badge)](https://www.bestpractices.dev/projects/7868)

The srsRAN Project is a complete 5G RAN solution, featuring an ORAN-native CU/DU developed by [SRS](http://www.srs.io).

The solution includes a complete L1/2/3 implementation with minimal external dependencies. Portable across processor architectures, the software has been optimized for x86 and ARM. srsRAN follows the 3GPP 5G system architecture implementing the functional splits between distributed unit (DU) and centralized unit (CU). The CU is further disaggregated into control plane (CU-CP) and user-plane (CU-UP).

See the [srsRAN Project](https://www.srsran.com/) for information, guides and project news.

Build instructions and user guide(s) - [srsRAN Project documentation](https://docs.srsran.com/projects/project).

Community announcements and support - [Discussion board](https://www.github.com/srsran/srsran_project/discussions).

Features and roadmap - [Features](https://docs.srsran.com/projects/project/en/latest/general/source/2_features_and_roadmap.html).

Build Preparation
-----------------

* Build tools:
  * cmake:               <https://cmake.org/>
  
* Mandatory requirements:
  * libfftw:             <https://www.fftw.org/>
  * libsctp:             <https://github.com/sctp/lksctp-tools>
  * yaml-cpp:            <https://github.com/jbeder/yaml-cpp>
  * PolarSSL/mbedTLS:    <https://www.trustedfirmware.org/projects/mbed-tls/>
  * googletest:          <https://github.com/google/googletest/>

You can install the build tools and mandatory requirements for some example distributions with the commands below:

<details open>
<summary>Ubuntu 22.04</summary>

```bash
sudo apt-get install cmake make gcc g++ pkg-config libfftw3-dev libmbedtls-dev libsctp-dev libyaml-cpp-dev libgtest-dev
```

</details>
<details>
<summary>Fedora</summary>

```bash
sudo yum install cmake make gcc gcc-c++ fftw-devel lksctp-tools-devel yaml-cpp-devel mbedtls-devel gtest-devel
```

</details>
<details>
<summary>Arch Linux</summary>

```bash
sudo pacman -S cmake make base-devel fftw mbedtls yaml-cpp lksctp-tools gtest
```

</details>

The srsRAN Project supports split-8 and split-7.2 fronthaul. Split-8 fronthaul is supported via UHD for USRP devices:

* UHD:                 <https://github.com/EttusResearch/uhd>
  * See UHD documentation for installation instructions.

Build Instructions
------------------

Download and build srsRAN:

```bash
git clone https://github.com/Christ7777777/Weighted-Round-Robin-Implementation-In-SRSRAN.git
cd srsRAN_Project
mkdir build
cd build
cmake ..
make
make test
```

PHY layer tests use binary test vectors and are not built by default. To enable, see the [docs](https://docs.srsran.com/projects/project/en/latest/user_manuals/source/installation.html).

KEY MODIFICATIONS FOR RESOURCE ALLOCATION
------------------------------------------

In the round_robin_apply function, there is a special handling mechanism for User Equipments (UEs) with odd indices:

Identification of Odd-Indexed UEs: The function checks if the index of a UE is odd using the condition is_odd_index = u.ue_index % 2 != 0. If the result of this modulus operation is non-zero, the UE index is odd.

Dual Allocation Attempt for Odd-Indexed UEs: For UEs with an odd index, the function performs a unique allocation strategy. After a successful first allocation attempt (alloc_result_first == alloc_outcome::success), the function immediately attempts a second resource allocation for the same UE, regardless of whether other UEs are yet to be allocated resources in the current round.

Purpose of This Approach: This approach of double allocation for odd-indexed UEs(Prioritized UEs) suggests a potential strategy to balance or optimize resource allocation in a certain context

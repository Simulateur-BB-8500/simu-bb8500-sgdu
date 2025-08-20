# Description

The **Sound and Game Control Unit** (SGDU) is a C program executed on the Windows machine where the train simulation game (ORTS) is running.

The main functions of the SGDU are the following:

* **Receive and parse the commands** from the **Simulator Control Unit** (SCU).
* **Play the real locomotive sounds** according to the received commands.
* **Control ORTS** according to the received commands, by emulating the game keyboard shortcuts.
* **Read the data computed by ORTS** such as the train speed and the speed limit, by using the local API server.
* **Sending these data** to the **Simulator Control Unit** (SCU).

Unlike hardware related codes which have dedicated repositories for each simulator version (V0, V1, V2), the SGDU evolves in a single repository according to the following compatibility table:

| Simulator version | SGDU version |
|:---:|:---:|
| V0 | - |
| V1 | >= [sw0.0](https://github.com/Simulateur-BB-8500/simu-bb8500-sgdu/releases/tag/sw0.0) |
| V2 | - |

## SCU interface

### V0 and V1 projects

The interface between the SGDU and SCU can be considered as **two independent unidirectional links**: there is no master and there is no reply after the commands. Both the SCU and SGDU can initiate a transfer at anytime, and both must be able to receive interrupt-driven data.

The commands are listed in the [simu-bb8500-v1-commands](https://github.com/Simulateur-BB-8500/simu-bb8500-v1-commands) repository.

### V2 project

The **SCU is the master of the interface**: each transfer is always initiated by the SCU and is followed by a reply from the SGDU (acknowledge in case of writing operation, data in case of reading operation). Even if it is not connected to the same physical interface, **the SGDU can be functionally seen as a slave node of the SCU** in the same way as the other boards.

## Environment

This software is developed under **Eclipse IDE** version 2020-09 (4.17.0) and **MinGW** toolchain. The `script` folder contains the Eclipse run configuration file.

> [!WARNING]
> To compile any version under `sw2.0`, the `git_version.sh` script must be patched when `sscanf` function is called: the `SW` prefix must be replaced by `sw` since Git tags have been renamed in this way.

## Structure

The project is organized as follow:

* `audio` : real locomotive **sounds files**.
* `dll` : **DLL** files.
* `lib` : external **libraries** and **submodules**.
* `drivers` :
    * `peripherals` : OS **peripherals** drivers.
    * `utils` : **utility** functions.
* `middleware` :
    * `game` : **OpenRails Train Simulator** driver.
    * `interface` : **SCU commands** parser.
    * `simulator` : **simulator modules** drivers.
* `application` : Main **application**.

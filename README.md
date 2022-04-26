# printmgr

## Description

This program runs on an atmega (Arduino Mega 2560 in this case) to create a sort
of dummy terminal/teletype using a ps2 keyboard and parallel dot matrix printer
and interfacing with it over the Arduino's main builtin 5V TTL serial interface.

## Building/Uploading

### Examples

Compile and Upload a sketch
```
make all
```
Compile, Upload, and open serial monitor using `minicom`
```
make all monitor
```
Clean build files
```
make clean
```
Install all required libraries from `requirements.txt` using `arduino-cli lib install`
```
make requirements
```

### Configuring the Makefile

 - `FQBN` must be set to the Fully Qualified Board Name e.g.
    - Arduino Mega `arduino:avr:mega`
    - Arduino Nano `arduino:avr:nano`
    - Arduino Nano (old bootloader) `arduino:avr:nano:cpu=atmega328old`
 - `SERIAL_DEV` is optional and leaving blank will attempt to autodetect
 - `V` Enables/Disables verbose output for `arduino-cli`

### Valid Make targets

 - all
 - compile
 - upload
 - clean
 - monitor
 - requirements

## Pinouts

### Parallel connector

| Arduino   | Parallel  | Name   |
| ---       | ---       | ---    |
| 4         | 1         | Strobe |
| 5         | 2         | Data 0 |
| 6         | 3         | Data 1 |
| 7         | 4         | Data 2 |
| 8         | 5         | Data 3 |
| 9         | 6         | Data 4 |
| 10        | 7         | Data 5 |
| 11        | 8         | Data 6 |
| 12        | 9         | Data 7 |
| 13        | 10        | Ack    |
| 14        | 11        | Busy   |
| GND       | GND       | Ground |

### PS/2 Keyboard

| Arduino   | PS/2  | Name   |
| ---       | ---   | ---    |
| 3         | 1     | Data   |
| 2         | 5     | Clk    |
| 5V        | 4     | VCC    |
| GND       | 3     | Ground |

Notes: Pin 14 on an Arduino Mega is labelled D14, However on boards where there
are only 13 digital pins, it is labelled A0.
The DEBUG flag also only works on this Arduino as it has
extra serial interfaces

## Requirements

 - PS2Keyboard (Arduino Library)
 - arduino or arduino-cli
 - gnumake

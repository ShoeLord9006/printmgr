# printmgr

## Description

This program runs on an atmega (Arduino Mega 2560 in this case) to create a sort
of dummy terminal/teletype using a ps2 keyboard and parallel dot matrix printer
and interfacing with it over the Arduino's main builtin 5V TTL serial interface.

## Pinouts

### Parallel connector

| Arduino   | Parallel  | Name   |
| ---       | ---       | ---    |
| 4         | 2         | Strobe |
| 5         | 3         | Data 0 |
| 6         | 4         | Data 1 |
| 7         | 5         | Data 2 |
| 8         | 6         | Data 3 |
| 9         | 7         | Data 4 |
| 10        | 8         | Data 5 |
| 11        | 9         | Data 6 |
| 12        | 10        | Data 7 |
| 13        | 11        | Ack    |
| 14        | 12        | Busy   |
| GND       | GND       | Ground |

### PS/2 Keyboard

| Arduino   | PS/2  | Name   |
| ---       | ---   | ---    |
| 18        | 1     | Data   |
| 19        | 5     | Clk    |
| 5V        | 4     | VCC    |
| GND       | 3     | Ground |

Notes: Pin 14 on an Arduino Mega is labelled D14, However on boards where there
are only 13 digital pins, it is labelled A0.
The DEBUG flag also only works on this Arduino as it has
extra serial interfaces

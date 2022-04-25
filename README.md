# printmgr

## Description

This program runs on an atmega (Arduino Mega 2560 in this case) to create a sort
of dummy terminal/teletype using a ps2 keyboard and parallel dot matrix printer
and interfacing with it over the Arduino's main builtin 5V TTL serial interface.

## Pinouts

### Parallel connector

| Arduino   | Parallel  | Name   |
| ---       | ---       | ---    |
| 2         | 2         | Strobe |
| 3         | 3         | Data 0 |
| 4         | 4         | Data 1 |
| 5         | 5         | Data 2 |
| 6         | 6         | Data 3 |
| 7         | 7         | Data 4 |
| 8         | 8         | Data 5 |
| 9         | 9         | Data 6 |
| 10        | 10        | Data 7 |
| 11        | 11        | Ack    |
| 12        | 12        | Busy   |
| GND       | GND       | Ground |

### PS/2 Keyboard

| Arduino   | PS/2  | Name   |
| 18        | 1     | Data   |
| 19        | 5     | Clk    |
| 5V        | 4     | VCC    |
| GND       | 3     | Ground |

Note: These pinouts are only valid for an Arduino Mega 2560 where there is an
interrupt on pin 19. The DEBUG flag also only works on this Arduino as it has
extra serial interfaces

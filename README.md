# Compass2root
Simple program to convert list mode data from CoMPASS software to a root file.

## Installation requirements
This program requires root to be installed. It can be compiled using make and the executable will be created in the local directory.

## Usage
To run the program use
```
Compass2root <input file> <output file>
```
you might have to specify the path to the executable as well depending on the setup of your environment.

At the moment the program will read the input file completely into memory and then start parsing it. This might need to be changed if the files processed become very large.

The created tree has the following branches (not up to date!):

| data type | branch name | description |
|-----------|-------------|-------------|
| int                   | aggregateSize |  |
| bool                  | dualTraceEnabled | flag if dual trace has been enabled |
| bool                  | waveformEnabled | flag if waveform writing has been enabled |
| bool                  | energyEnabled | flag if writing of energy has been enabled |
| bool                  | timeEnabled | flag if writing of time has been enabled |
| uint8_t               | analogProbe1 | format of analog probe 1 (00 - Input, 01 - RC-CR, 10 - Fast Filter, 11 - Trapezoid) |
| uint8_t               | analogProbe2 | format of analog probe 2 (00 - Input, 01 - Threshold, 10 - Trapezoid-Baseline, 11 - Baseline) |
| uint8_t               | digitalProbe | format of digital probe (0000 - Peaking, 0001 - Armed, 0010 - Peak Run, 0011 - Pile-Up, 0100 - Peaking (again?), 0101 - Trigger Validation Window, 0110 - baseline freeze, 0111 - trigger holdoff, 1000 - trigger validation, 1001 - over range protection time, 1010 - trigger window, 1011 - external trigger, 1100 - busy, 1101 - peak ready) |
| uint64_t              | timestamp | timestamp including all roll-overs |
| std::vector<uint16_t> | waveform | waveform samples (14 LSB are analog waveform, bit 14 is the digital waveform, and bit 15 identifies the sample where the trigger occured) |
| uint8_t               | extras | bit 0 - dead time, 1 - roll over, 2 - time stamp reset, 3 - fake event, 4 - input saturation, 5 - N lost triggers, 6 - N total triggers, 7 - double saturation of input and trapezoid |
| bool                  | pileUp | flag indicating pile up (if enabled via bit 27 of register 0x1n80) |
| uint16_t              | energy | energy (zero indicates pile up unless bit 27 of register 0x1n80 has been set) |


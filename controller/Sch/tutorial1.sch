EESchema Schematic File Version 2
LIBS:power
LIBS:device
LIBS:switches
LIBS:relays
LIBS:motors
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:philips
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:valves
LIBS:nodemcu
EELAYER 25 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L FINDER-32.21-x000 K?
U 1 1 59F9DF4B
P 4150 2700
F 0 "K?" H 4600 2850 50  0000 L CNN
F 1 "FINDER-32.21-x000" H 4600 2750 50  0000 L CNN
F 2 "Relays_THT:Relay_SPST_Finder_32.21-x300" H 5420 2670 50  0001 C CNN
F 3 "" H 4150 2700 50  0001 C CNN
	1    4150 2700
	0    1    1    0   
$EndComp
$Comp
L SW_DIP_x01 SW?
U 1 1 59F9E026
P 3250 2900
F 0 "SW?" H 3250 3050 50  0000 C CNN
F 1 "SW_DIP_x01" H 3250 2750 50  0000 C CNN
F 2 "" H 3250 2900 50  0001 C CNN
F 3 "" H 3250 2900 50  0001 C CNN
	1    3250 2900
	1    0    0    -1  
$EndComp
$Comp
L NodeMCU_0.9 U?
U 1 1 59F9E2C4
P 2000 1550
F 0 "U?" H 2000 2350 50  0000 C CNN
F 1 "NodeMCU_0.9" H 2000 700 50  0000 C CNN
F 2 "" H 2250 1550 50  0000 C CNN
F 3 "" H 2250 1550 50  0000 C CNN
	1    2000 1550
	1    0    0    -1  
$EndComp
$Comp
L +5V #PWR?
U 1 1 59F9E495
P 4700 2300
F 0 "#PWR?" H 4700 2150 50  0001 C CNN
F 1 "+5V" H 4700 2440 50  0000 C CNN
F 2 "" H 4700 2300 50  0001 C CNN
F 3 "" H 4700 2300 50  0001 C CNN
	1    4700 2300
	1    0    0    -1  
$EndComp
Wire Wire Line
	2450 1550 3850 1550
Wire Wire Line
	3850 1550 3850 2500
Wire Wire Line
	4450 2500 4700 2500
Wire Wire Line
	4700 2500 4700 2300
Wire Wire Line
	3850 2900 3550 2900
$Comp
L SW_DPDT_x2 SW?
U 1 1 59F9E596
P 3750 1050
F 0 "SW?" H 3750 1220 50  0000 C CNN
F 1 "SW_DPDT_x2" H 3750 850 50  0000 C CNN
F 2 "" H 3750 1050 50  0001 C CNN
F 3 "" H 3750 1050 50  0001 C CNN
	1    3750 1050
	1    0    0    -1  
$EndComp
$EndSCHEMATC

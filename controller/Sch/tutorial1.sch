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
LIBS:tutorial1-cache
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
P 2900 2550
F 0 "K?" V 3250 2800 50  0000 L CNN
F 1 "5V_Relay" V 2450 2350 50  0000 L CNN
F 2 "Relays_THT:Relay_SPST_Finder_32.21-x300" H 4170 2520 50  0001 C CNN
F 3 "" H 2900 2550 50  0001 C CNN
	1    2900 2550
	0    1    1    0   
$EndComp
$Comp
L SW_DIP_x01 FuseBoard_AirPump
U 1 1 59F9E026
P 1350 1200
F 0 "FuseBoard_AirPump" H 1350 1350 50  0000 C CNN
F 1 "SW_DIP_x01" H 1350 1050 50  0000 C CNN
F 2 "" H 1350 1200 50  0001 C CNN
F 3 "" H 1350 1200 50  0001 C CNN
	1    1350 1200
	1    0    0    -1  
$EndComp
$Comp
L NodeMCU_0.9 U?
U 1 1 59F9E2C4
P 9800 1550
F 0 "U?" H 9800 2350 50  0000 C CNN
F 1 "NodeMCU_0.9" H 9800 700 50  0000 C CNN
F 2 "" H 10050 1550 50  0000 C CNN
F 3 "" H 10050 1550 50  0000 C CNN
	1    9800 1550
	1    0    0    -1  
$EndComp
$Comp
L Motor_DC AirPump
U 1 1 5A0CBA38
P 3600 1200
F 0 "AirPump" H 3700 1300 50  0000 L CNN
F 1 "Motor_DC" H 3700 1000 50  0000 L TNN
F 2 "" H 3600 1110 50  0001 C CNN
F 3 "" H 3600 1110 50  0001 C CNN
	1    3600 1200
	0    -1   -1   0   
$EndComp
$Comp
L SW_DIP_x01 FuseBoard_WaterPump
U 1 1 5A0CBB90
P 1350 3150
F 0 "FuseBoard_WaterPump" H 1350 3300 50  0000 C CNN
F 1 "SW_DIP_x01" H 1350 3000 50  0000 C CNN
F 2 "" H 1350 3150 50  0001 C CNN
F 3 "" H 1350 3150 50  0001 C CNN
	1    1350 3150
	1    0    0    -1  
$EndComp
$Comp
L Motor_DC SumpTankPump_AON
U 1 1 5A0CBC3D
P 3600 1850
F 0 "SumpTankPump_AON" V 3850 1350 50  0000 L CNN
F 1 "Motor_DC" V 3700 1300 50  0000 L TNN
F 2 "" H 3600 1760 50  0001 C CNN
F 3 "" H 3600 1760 50  0001 C CNN
	1    3600 1850
	0    -1   -1   0   
$EndComp
$Comp
L Motor_DC VerticalPump
U 1 1 5A0CBD10
P 3600 2850
F 0 "VerticalPump" V 3850 2600 50  0000 L CNN
F 1 "Motor_DC" V 3700 2300 50  0000 L TNN
F 2 "" H 3600 2760 50  0001 C CNN
F 3 "" H 3600 2760 50  0001 C CNN
	1    3600 2850
	0    -1   -1   0   
$EndComp
$Comp
L Motor_DC WaterfallPump
U 1 1 5A0CBDA1
P 3600 3750
F 0 "WaterfallPump" V 3850 3450 50  0000 L CNN
F 1 "Motor_DC" V 3700 3200 50  0000 L TNN
F 2 "" H 3600 3660 50  0001 C CNN
F 3 "" H 3600 3660 50  0001 C CNN
	1    3600 3750
	0    -1   -1   0   
$EndComp
$Comp
L Motor_DC ClonerPump
U 1 1 5A0CBE22
P 3600 4650
F 0 "ClonerPump" V 3850 4450 50  0000 L CNN
F 1 "Motor_DC" V 3700 4100 50  0000 L TNN
F 2 "" H 3600 4560 50  0001 C CNN
F 3 "" H 3600 4560 50  0001 C CNN
	1    3600 4650
	0    -1   -1   0   
$EndComp
$Comp
L FINDER-32.21-x000 K?
U 1 1 5A0CCE7A
P 2900 3450
F 0 "K?" V 3250 3700 50  0000 L CNN
F 1 "12V_Relay" V 2450 3250 50  0000 L CNN
F 2 "Relays_THT:Relay_SPST_Finder_32.21-x300" H 4170 3420 50  0001 C CNN
F 3 "" H 2900 3450 50  0001 C CNN
	1    2900 3450
	0    1    1    0   
$EndComp
$Comp
L FINDER-32.21-x000 K?
U 1 1 5A0CCF87
P 2900 4350
F 0 "K?" V 3250 4600 50  0000 L CNN
F 1 "12V_Relay" V 2450 4150 50  0000 L CNN
F 2 "Relays_THT:Relay_SPST_Finder_32.21-x300" H 4170 4320 50  0001 C CNN
F 3 "" H 2900 4350 50  0001 C CNN
	1    2900 4350
	0    1    1    0   
$EndComp
$Comp
L GND #PWR?
U 1 1 5A0CD4E5
P 4000 4800
F 0 "#PWR?" H 4000 4550 50  0001 C CNN
F 1 "GND" H 4000 4650 50  0000 C CNN
F 2 "" H 4000 4800 50  0001 C CNN
F 3 "" H 4000 4800 50  0001 C CNN
	1    4000 4800
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR?
U 1 1 5A0CD515
P 4000 3900
F 0 "#PWR?" H 4000 3650 50  0001 C CNN
F 1 "GND" H 4000 3750 50  0000 C CNN
F 2 "" H 4000 3900 50  0001 C CNN
F 3 "" H 4000 3900 50  0001 C CNN
	1    4000 3900
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR?
U 1 1 5A0CD545
P 4000 3000
F 0 "#PWR?" H 4000 2750 50  0001 C CNN
F 1 "GND" H 4000 2850 50  0000 C CNN
F 2 "" H 4000 3000 50  0001 C CNN
F 3 "" H 4000 3000 50  0001 C CNN
	1    4000 3000
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR?
U 1 1 5A0CD575
P 4000 2000
F 0 "#PWR?" H 4000 1750 50  0001 C CNN
F 1 "GND" H 4000 1850 50  0000 C CNN
F 2 "" H 4000 2000 50  0001 C CNN
F 3 "" H 4000 2000 50  0001 C CNN
	1    4000 2000
	1    0    0    -1  
$EndComp
Wire Wire Line
	1650 1200 3400 1200
Wire Wire Line
	1800 3150 1650 3150
Wire Wire Line
	1800 1850 1800 4550
Wire Wire Line
	3200 2850 3400 2850
Wire Wire Line
	3200 3750 3400 3750
Wire Wire Line
	3200 4650 3400 4650
Wire Wire Line
	1800 1850 3500 1850
Wire Wire Line
	4000 1850 4000 2000
Wire Wire Line
	3900 2850 4000 2850
Wire Wire Line
	4000 2850 4000 3000
Wire Wire Line
	3900 3750 4000 3750
Wire Wire Line
	4000 3750 4000 3900
Wire Wire Line
	3900 4650 4000 4650
Wire Wire Line
	4000 4650 4000 4800
Connection ~ 3400 1850
Wire Wire Line
	2600 2750 1800 2750
Connection ~ 1800 2750
Wire Wire Line
	1800 4550 2600 4550
Connection ~ 1800 3150
Wire Wire Line
	2600 3650 1800 3650
Connection ~ 1800 3650
$Comp
L SW_DIP_x01 FuseBoard_Fan
U 1 1 5A0CF773
P 5200 1250
F 0 "FuseBoard_Fan" H 5200 1400 50  0000 C CNN
F 1 "SW_DIP_x01" H 5200 1100 50  0000 C CNN
F 2 "" H 5200 1250 50  0001 C CNN
F 3 "" H 5200 1250 50  0001 C CNN
	1    5200 1250
	1    0    0    -1  
$EndComp
$Comp
L FINDER-32.21-x000 K?
U 1 1 5A0CF82E
P 6300 1050
F 0 "K?" H 6750 1200 50  0000 L CNN
F 1 "12V_Relay" V 5850 850 50  0000 L CNN
F 2 "Relays_THT:Relay_SPST_Finder_32.21-x300" H 7570 1020 50  0001 C CNN
F 3 "" H 6300 1050 50  0001 C CNN
	1    6300 1050
	0    1    1    0   
$EndComp
$Comp
L Fan M?
U 1 1 5A0CF8A7
P 7100 1350
F 0 "M?" H 7200 1550 50  0000 L CNN
F 1 "Fan" V 6850 1300 50  0000 L TNN
F 2 "" H 7100 1360 50  0001 C CNN
F 3 "" H 7100 1360 50  0001 C CNN
	1    7100 1350
	0    1    1    0   
$EndComp
$Comp
L GND #PWR?
U 1 1 5A0CF91C
P 7500 1500
F 0 "#PWR?" H 7500 1250 50  0001 C CNN
F 1 "GND" H 7500 1350 50  0000 C CNN
F 2 "" H 7500 1500 50  0001 C CNN
F 3 "" H 7500 1500 50  0001 C CNN
	1    7500 1500
	1    0    0    -1  
$EndComp
Wire Wire Line
	5500 1250 6000 1250
Wire Wire Line
	6600 1350 6900 1350
Wire Wire Line
	7400 1350 7500 1350
Wire Wire Line
	7500 1350 7500 1500
$Comp
L SW_DIP_x01 FuseBoard_Heater
U 1 1 5A0D01A7
P 5200 3250
F 0 "FuseBoard_Heater" H 5200 3400 50  0000 C CNN
F 1 "SW_DIP_x01" H 5200 3100 50  0000 C CNN
F 2 "" H 5200 3250 50  0001 C CNN
F 3 "" H 5200 3250 50  0001 C CNN
	1    5200 3250
	1    0    0    -1  
$EndComp
$Comp
L FINDER-32.21-x000 K?
U 1 1 5A0D0232
P 6300 2700
F 0 "K?" H 6750 2850 50  0000 L CNN
F 1 "12V_Relay" V 5850 2500 50  0000 L CNN
F 2 "Relays_THT:Relay_SPST_Finder_32.21-x300" H 7570 2670 50  0001 C CNN
F 3 "" H 6300 2700 50  0001 C CNN
	1    6300 2700
	0    1    1    0   
$EndComp
$Comp
L FINDER-32.21-x000 K?
U 1 1 5A0D02B1
P 6300 3800
F 0 "K?" H 6750 3950 50  0000 L CNN
F 1 "12V_Relay" V 5850 3600 50  0000 L CNN
F 2 "Relays_THT:Relay_SPST_Finder_32.21-x300" H 7570 3770 50  0001 C CNN
F 3 "" H 6300 3800 50  0001 C CNN
	1    6300 3800
	0    1    1    0   
$EndComp
$Comp
L Fan M?
U 1 1 5A0D0338
P 7150 3000
F 0 "M?" H 7250 3200 50  0000 L CNN
F 1 "Fish_Tank_Heater" V 6900 2950 50  0000 L TNN
F 2 "" H 7150 3010 50  0001 C CNN
F 3 "" H 7150 3010 50  0001 C CNN
	1    7150 3000
	0    1    1    0   
$EndComp
$Comp
L Fan M?
U 1 1 5A0D03AF
P 7200 4100
F 0 "M?" H 7300 4300 50  0000 L CNN
F 1 "Cloner_Heater" V 6950 4050 50  0000 L TNN
F 2 "" H 7200 4110 50  0001 C CNN
F 3 "" H 7200 4110 50  0001 C CNN
	1    7200 4100
	0    1    1    0   
$EndComp
$Comp
L GND #PWR?
U 1 1 5A0D043E
P 7550 3150
F 0 "#PWR?" H 7550 2900 50  0001 C CNN
F 1 "GND" H 7550 3000 50  0000 C CNN
F 2 "" H 7550 3150 50  0001 C CNN
F 3 "" H 7550 3150 50  0001 C CNN
	1    7550 3150
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR?
U 1 1 5A0D0478
P 7600 4250
F 0 "#PWR?" H 7600 4000 50  0001 C CNN
F 1 "GND" H 7600 4100 50  0000 C CNN
F 2 "" H 7600 4250 50  0001 C CNN
F 3 "" H 7600 4250 50  0001 C CNN
	1    7600 4250
	1    0    0    -1  
$EndComp
Wire Wire Line
	5500 2900 5500 4000
Wire Wire Line
	5500 2900 6000 2900
Wire Wire Line
	5500 4000 6000 4000
Connection ~ 5500 3250
Wire Wire Line
	6600 3000 6950 3000
Wire Wire Line
	6600 4100 7000 4100
Wire Wire Line
	7450 3000 7550 3000
Wire Wire Line
	7550 3000 7550 3150
Wire Wire Line
	7500 4100 7600 4100
Wire Wire Line
	7600 4100 7600 4250
$EndSCHEMATC

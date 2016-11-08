/*
 * Apple System Management Control (SMC) Tool
 * Copyright (C) 2006 devnull 
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef __SMC_H__
#define __SMC_H__
#endif

#define VERSION               "0.01"

#define KERNEL_INDEX_SMC      2

#define SMC_CMD_READ_BYTES    5
#define SMC_CMD_WRITE_BYTES   6
#define SMC_CMD_READ_INDEX    8
#define SMC_CMD_READ_KEYINFO  9
#define SMC_CMD_READ_PLIMIT   11
#define SMC_CMD_READ_VERS     12

#define DATATYPE_FPE2         "fpe2"
#define DATATYPE_UINT8        "ui8 "
#define DATATYPE_UINT16       "ui16"
#define DATATYPE_UINT32       "ui32"
#define DATATYPE_SP78         "sp78"

// key values
#define PECI_CPU						"TCXC"
#define PECI_CPU1						"TCXc"
#define CPU_1_PROXIMITY					"TC0P"
#define CPU_1_HEATSINK					"TC0H"
#define CPU_1_PACKAGE					"TC0D"
#define CPU_1							"TC0E"
#define CPU_11							"TC0F"
#define CPU_CORE_1						"TC1C"
#define CPU_CORE_2						"TC2C"
#define CPU_CORE_3						"TC3C"
#define CPU_CORE_4						"TC4C"
#define CPU_CORE_5						"TC5C"
#define CPU_CORE_6						"TC6C"
#define CPU_CORE_7						"TC7C"
#define CPU_CORE_8						"TC8C"
#define CPU_1_HEATSINK_ALT				"TCAH"
#define CPU_1_PACKAGE_ALT				"TCAD"
#define CPU_2_PROXIMITY					"TC1P"
#define CPU_2_HEATSINK					"TC1H"
#define CPU_2_PACKAGE					"TC1D"
#define CPU_2							"TC1E"
#define CPU_21							"TC1F"
#define CPU_2_HEATSINK_ALT				"TCBH"
#define CPU_2_PACKAGE_ALT				"TCBD"
#define PECI_SA							"TCSC"
#define PECI_SA1							"TCSc"
#define PECI_SA2							"TCSA"
#define PECI_GPU						"TCGC"
#define PECI_GPU1						"TCGc"
#define GPU_PROXIMITY					"TG0P"
#define GPU_DIE							"TG0D"
#define GPU_DIE1							"TG1D"
#define GPU_HEATSINK					"TG0H"
#define GPU_HEATSINK1					"TG1H"
#define MEMORY_PROXIMITY				"Ts0S"
#define MEM_BANK_A1						"TM0P"
#define MEM_BANK_A2						"TM1P"
#define MEM_BANK_B1						"TM8P"
#define MEM_BANK_B2						"TM9P"
#define MEM_MODULE_A1					"TM0S"
#define MEM_MODULE_A2					"TM1S"
#define MEM_MODULE_B1					"TM8S"
#define MEM_MODULE_B2					"TM9S"
#define NORTHBRIDGE_DIE					"TN0D"
#define NORTHBRIDGE_PROXIMITY_1			"TN0P"
#define NORTHBRIDGE_PROXIMITY_2			"TN1P"
#define MCH_DIE							"TN0C"
#define MCH_HEATSINK					"TN0H"
#define PCH_DIE							"TP0D"
#define PCH_DIE1							"TPCD"
#define PCH_PROXIMITY					"TP0P"
#define AIRFLOW_1						"TA0P"
#define AIRFLOW_2						"TA1P"
#define HEATPIPE_1						"Th0H"
#define HEATPIPE_2						"Th1H"
#define HEATPIPE_3						"Th2H"
#define MAINBOARD_PROXIMITY				"Tm0P"
#define POWERBOARD_PROXIMITY			"Tp0P"
#define PALM_REST						"Ts0P"
#define BLC_PROXIMITY					"Tb0P"
#define LCD_PROXIMITY					"TL0P"
#define AIRPORT_PROXIMITY				"TW0P"
#define HDD_BAY_1						"TH0P"
#define HDD_BAY_2						"TH1P"
#define HDD_BAY_3						"TH2P"
#define HDD_BAY_4						"TH3P"
#define OPTICAL_DRIVE					"TO0P"
#define BATTERY_TS_MAX					"TB0T"
#define BATTERY_1						"TB1T"
#define BATTERY_2						"TB2T"
#define BATTERY							"TB3T"
#define POWER_SUPPLY_1					"Tp0P"
#define POWER_SUPPLY_1_ALT				"Tp0C"
#define POWER_SUPPLY_2					"Tp1P"
#define POWER_SUPPLY_2_ALT				"Tp1C"
#define POWER_SUPPLY_3					"Tp2P"
#define POWER_SUPPLY_4					"Tp3P"
#define POWER_SUPPLY_5					"Tp4P"
#define POWER_SUPPLY_6					"Tp5P"
#define EXPANSION_SLOTS					"TS0C"
#define PCI_SLOT_1_POS_1				"TA0S"
#define PCI_SLOT_1_POS_2				"TA1S"
#define PCI_SLOT_2_POS_1				"TA2S"
#define PCI_SLOT_2_POS_2				"TA3S"


typedef struct {
    char                  major;
    char                  minor;
    char                  build;
    char                  reserved[1]; 
    UInt16                release;
} SMCKeyData_vers_t;

typedef struct {
    UInt16                version;
    UInt16                length;
    UInt32                cpuPLimit;
    UInt32                gpuPLimit;
    UInt32                memPLimit;
} SMCKeyData_pLimitData_t;

typedef struct {
    UInt32                dataSize;
    UInt32                dataType;
    char                  dataAttributes;
} SMCKeyData_keyInfo_t;

typedef char              SMCBytes_t[32]; 

typedef struct {
  UInt32                  key; 
  SMCKeyData_vers_t       vers; 
  SMCKeyData_pLimitData_t pLimitData;
  SMCKeyData_keyInfo_t    keyInfo;
  char                    result;
  char                    status;
  char                    data8;
  UInt32                  data32;
  SMCBytes_t              bytes;
} SMCKeyData_t;

typedef char              UInt32Char_t[5];

typedef struct {
  UInt32Char_t            key;
  UInt32                  dataSize;
  UInt32Char_t            dataType;
  SMCBytes_t              bytes;
} SMCVal_t;


// prototypes
double SMCGetTemperature(char *key);
kern_return_t SMCSetFanRpm(char *key, int rpm);
int SMCGetFanRpm(char *key);


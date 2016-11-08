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

#include <stdio.h>
#include <string.h>
#include <IOKit/IOKitLib.h>

#include "smc.h"

static io_connect_t conn;

UInt32 _strtoul(char *str, int size, int base)
{
    UInt32 total = 0;
    int i;

    for (i = 0; i < size; i++)
    {
        if (base == 16)
            total += str[i] << (size - 1 - i) * 8;
        else
           total += (unsigned char) (str[i] << (size - 1 - i) * 8);
    }
    return total;
}

void _ultostr(char *str, UInt32 val)
{
    str[0] = '\0';
    sprintf(str, "%c%c%c%c", 
            (unsigned int) val >> 24,
            (unsigned int) val >> 16,
            (unsigned int) val >> 8,
            (unsigned int) val);
}

kern_return_t SMCOpen(void)
{
    kern_return_t result;
    io_iterator_t iterator;
    io_object_t   device;

    CFMutableDictionaryRef matchingDictionary = IOServiceMatching("AppleSMC");
    result = IOServiceGetMatchingServices(kIOMasterPortDefault, matchingDictionary, &iterator);
    if (result != kIOReturnSuccess)
    {
        printf("Error: IOServiceGetMatchingServices() = %08x\n", result);
        return 1;
    }

    device = IOIteratorNext(iterator);
    IOObjectRelease(iterator);
    if (device == 0)
    {
        printf("Error: no SMC found\n");
        return 1;
    }

    result = IOServiceOpen(device, mach_task_self(), 0, &conn);
    IOObjectRelease(device);
    if (result != kIOReturnSuccess)
    {
        printf("Error: IOServiceOpen() = %08x\n", result);
        return 1;
    }

    return kIOReturnSuccess;
}

kern_return_t SMCClose()
{
    return IOServiceClose(conn);
}


kern_return_t SMCCall(int index, SMCKeyData_t *inputStructure, SMCKeyData_t *outputStructure)
{
    size_t   structureInputSize;
    size_t   structureOutputSize;

    structureInputSize = sizeof(SMCKeyData_t);
    structureOutputSize = sizeof(SMCKeyData_t);

    #if MAC_OS_X_VERSION_10_5
    return IOConnectCallStructMethod( conn, index,
                            // inputStructure
                            inputStructure, structureInputSize,
                            // ouputStructure
                            outputStructure, &structureOutputSize );
    #else
    return IOConnectMethodStructureIStructureO( conn, index,
                                                structureInputSize, /* structureInputSize */
                                                &structureOutputSize,   /* structureOutputSize */
                                                inputStructure,        /* inputStructure */
                                                outputStructure);       /* ouputStructure */
    #endif

}

kern_return_t SMCReadKey(UInt32Char_t key, SMCVal_t *val)
{
    kern_return_t result;
    SMCKeyData_t  inputStructure;
    SMCKeyData_t  outputStructure;

    memset(&inputStructure, 0, sizeof(SMCKeyData_t));
    memset(&outputStructure, 0, sizeof(SMCKeyData_t));
    memset(val, 0, sizeof(SMCVal_t));

    inputStructure.key = _strtoul(key, 4, 16);
    inputStructure.data8 = SMC_CMD_READ_KEYINFO;

    result = SMCCall(KERNEL_INDEX_SMC, &inputStructure, &outputStructure);
    if (result != kIOReturnSuccess)
        return result;

    val->dataSize = outputStructure.keyInfo.dataSize;
    _ultostr(val->dataType, outputStructure.keyInfo.dataType);
    inputStructure.keyInfo.dataSize = val->dataSize;
    inputStructure.data8 = SMC_CMD_READ_BYTES;

    result = SMCCall(KERNEL_INDEX_SMC, &inputStructure, &outputStructure);
    if (result != kIOReturnSuccess)
        return result;

    memcpy(val->bytes, outputStructure.bytes, sizeof(outputStructure.bytes));

    return kIOReturnSuccess;
}

double SMCGetTemperature(char *key)
{
    SMCVal_t val;
    kern_return_t result;

    result = SMCReadKey(key, &val);
    if (result == kIOReturnSuccess) {
        // read succeeded - check returned value
        if (val.dataSize > 0) {
            if (strcmp(val.dataType, DATATYPE_SP78) == 0) {
                // convert sp78 value to temperature
                int intValue = val.bytes[0] * 256 + (unsigned char)val.bytes[1];
                return intValue / 256.0;
            }
        }
    }
    // read failed
    return 0.0;
}

double convertToFahrenheit(double celsius) {
  return (celsius * (9.0 / 5.0)) + 32.0;
}

int main(int argc, char *argv[])
{
    char scale = 'C';

    int c;
    while ((c = getopt(argc, argv, "CF")) != -1) {
      switch (c) {
        case 'F':
        case 'C':
          scale = c;
          break;
      }
    }

    SMCOpen();
    double peci_cpu = SMCGetTemperature(PECI_CPU);
    double peci_cpu1 = SMCGetTemperature(PECI_CPU1);
    double cpu_1_proximity = SMCGetTemperature(CPU_1_PROXIMITY);
    double cpu_1_heatsink = SMCGetTemperature(CPU_1_HEATSINK);
    double cpu_1_package = SMCGetTemperature(CPU_1_PACKAGE);
    double cpu_1 = SMCGetTemperature(CPU_1);
    double cpu_11 = SMCGetTemperature(CPU_11);
    double cpu_core_1 = SMCGetTemperature(CPU_CORE_1);
    double cpu_core_2 = SMCGetTemperature(CPU_CORE_2);
    double cpu_core_3 = SMCGetTemperature(CPU_CORE_3);
    double cpu_core_4 = SMCGetTemperature(CPU_CORE_4);
    double cpu_core_5 = SMCGetTemperature(CPU_CORE_5);
    double cpu_core_6 = SMCGetTemperature(CPU_CORE_6);
    double cpu_core_7 = SMCGetTemperature(CPU_CORE_7);
    double cpu_core_8 = SMCGetTemperature(CPU_CORE_8);
    double cpu_1_heatsink_alt = SMCGetTemperature(CPU_1_HEATSINK_ALT);
    double cpu_1_package_alt = SMCGetTemperature(CPU_1_PACKAGE_ALT);
    double cpu_2_proximity = SMCGetTemperature(CPU_2_PROXIMITY);
    double cpu_2_heatsink = SMCGetTemperature(CPU_2_HEATSINK);
    double cpu_2_package = SMCGetTemperature(CPU_2_PACKAGE);
    double cpu_2 = SMCGetTemperature(CPU_2);
    double cpu_21 = SMCGetTemperature(CPU_21);
    double cpu_2_heatsink_alt = SMCGetTemperature(CPU_2_HEATSINK_ALT);
    double cpu_2_package_alt = SMCGetTemperature(CPU_2_PACKAGE_ALT);
    double peci_sa = SMCGetTemperature(PECI_SA);
    double peci_sa1 = SMCGetTemperature(PECI_SA1);
    double peci_sa2 = SMCGetTemperature(PECI_SA2);
    double peci_gpu = SMCGetTemperature(PECI_GPU);
    double peci_gpu1 = SMCGetTemperature(PECI_GPU1);
    double gpu_proximity = SMCGetTemperature(GPU_PROXIMITY);
    double gpu_die = SMCGetTemperature(GPU_DIE);
    double gpu_die1 = SMCGetTemperature(GPU_DIE1);
    double gpu_heatsink = SMCGetTemperature(GPU_HEATSINK);
    double gpu_heatsink1 = SMCGetTemperature(GPU_HEATSINK1);
    double memory_proximity = SMCGetTemperature(MEMORY_PROXIMITY);
    double mem_bank_a1 = SMCGetTemperature(MEM_BANK_A1);
    double mem_bank_a2 = SMCGetTemperature(MEM_BANK_A2);
    double mem_bank_b1 = SMCGetTemperature(MEM_BANK_B1);
    double mem_bank_b2 = SMCGetTemperature(MEM_BANK_B2);
    double mem_module_a1 = SMCGetTemperature(MEM_MODULE_A1);
    double mem_module_a2 = SMCGetTemperature(MEM_MODULE_A2);
    double mem_module_b1 = SMCGetTemperature(MEM_MODULE_B1);
    double mem_module_b2 = SMCGetTemperature(MEM_MODULE_B2);
    double northbridge_die = SMCGetTemperature(NORTHBRIDGE_DIE);
    double northbridge_proximity_1 = SMCGetTemperature(NORTHBRIDGE_PROXIMITY_1);
    double northbridge_proximity_2 = SMCGetTemperature(NORTHBRIDGE_PROXIMITY_2);
    double mch_die = SMCGetTemperature(MCH_DIE);
    double mch_heatsink = SMCGetTemperature(MCH_HEATSINK);
    double pch_die = SMCGetTemperature(PCH_DIE);
    double pch_die1 = SMCGetTemperature(PCH_DIE1);
    double pch_proximity = SMCGetTemperature(PCH_PROXIMITY);
    double airflow_1 = SMCGetTemperature(AIRFLOW_1);
    double airflow_2 = SMCGetTemperature(AIRFLOW_2);
    double heatpipe_1 = SMCGetTemperature(HEATPIPE_1);
    double heatpipe_2 = SMCGetTemperature(HEATPIPE_2);
    double heatpipe_3 = SMCGetTemperature(HEATPIPE_3);
    double mainboard_proximity = SMCGetTemperature(MAINBOARD_PROXIMITY);
    double powerboard_proximity = SMCGetTemperature(POWERBOARD_PROXIMITY);
    double palm_rest = SMCGetTemperature(PALM_REST);
    double blc_proximity = SMCGetTemperature(BLC_PROXIMITY);
    double lcd_proximity = SMCGetTemperature(LCD_PROXIMITY);
    double airport_proximity = SMCGetTemperature(AIRPORT_PROXIMITY);
    double hdd_bay_1 = SMCGetTemperature(HDD_BAY_1);
    double hdd_bay_2 = SMCGetTemperature(HDD_BAY_2);
    double hdd_bay_3 = SMCGetTemperature(HDD_BAY_3);
    double hdd_bay_4 = SMCGetTemperature(HDD_BAY_4);
    double optical_drive = SMCGetTemperature(OPTICAL_DRIVE);
    double battery_ts_max = SMCGetTemperature(BATTERY_TS_MAX);
    double battery_1 = SMCGetTemperature(BATTERY_1);
    double battery_2 = SMCGetTemperature(BATTERY_2);
    double battery = SMCGetTemperature(BATTERY);
    double power_supply_1 = SMCGetTemperature(POWER_SUPPLY_1);
    double power_supply_1_alt = SMCGetTemperature(POWER_SUPPLY_1_ALT);
    double power_supply_2 = SMCGetTemperature(POWER_SUPPLY_2);
    double power_supply_2_alt = SMCGetTemperature(POWER_SUPPLY_2_ALT);
    double power_supply_3 = SMCGetTemperature(POWER_SUPPLY_3);
    double power_supply_4 = SMCGetTemperature(POWER_SUPPLY_4);
    double power_supply_5 = SMCGetTemperature(POWER_SUPPLY_5);
    double power_supply_6 = SMCGetTemperature(POWER_SUPPLY_6);
    double expansion_slots = SMCGetTemperature(EXPANSION_SLOTS);
    double pci_slot_1_pos_1 = SMCGetTemperature(PCI_SLOT_1_POS_1);
    double pci_slot_1_pos_2 = SMCGetTemperature(PCI_SLOT_1_POS_2);
    double pci_slot_2_pos_1 = SMCGetTemperature(PCI_SLOT_2_POS_1);
    double pci_slot_2_pos_2 = SMCGetTemperature(PCI_SLOT_2_POS_2);
    SMCClose();

    if (scale == 'F') {
      peci_cpu = convertToFahrenheit(peci_cpu);
      peci_cpu1 = convertToFahrenheit(peci_cpu1);
      cpu_1_proximity = convertToFahrenheit(cpu_1_proximity);
      cpu_1_heatsink = convertToFahrenheit(cpu_1_heatsink);
      cpu_1_package = convertToFahrenheit(cpu_1_package);
      cpu_1 = convertToFahrenheit(cpu_1);
      cpu_11 = convertToFahrenheit(cpu_11);
      cpu_core_1 = convertToFahrenheit(cpu_core_1);
      cpu_core_2 = convertToFahrenheit(cpu_core_2);
      cpu_core_3 = convertToFahrenheit(cpu_core_3);
      cpu_core_4 = convertToFahrenheit(cpu_core_4);
      cpu_core_5 = convertToFahrenheit(cpu_core_5);
      cpu_core_6 = convertToFahrenheit(cpu_core_6);
      cpu_core_7 = convertToFahrenheit(cpu_core_7);
      cpu_core_8 = convertToFahrenheit(cpu_core_8);
      cpu_1_heatsink_alt = convertToFahrenheit(cpu_1_heatsink_alt);
      cpu_1_package_alt = convertToFahrenheit(cpu_1_package_alt);
      cpu_2_proximity = convertToFahrenheit(cpu_2_proximity);
      cpu_2_heatsink = convertToFahrenheit(cpu_2_heatsink);
      cpu_2_package = convertToFahrenheit(cpu_2_package);
      cpu_2 = convertToFahrenheit(cpu_2);
      cpu_21 = convertToFahrenheit(cpu_21);
      cpu_2_heatsink_alt = convertToFahrenheit(cpu_2_heatsink_alt);
      cpu_2_package_alt = convertToFahrenheit(cpu_2_package_alt);
      peci_sa = convertToFahrenheit(peci_sa);
      peci_sa1 = convertToFahrenheit(peci_sa1);
      peci_sa2 = convertToFahrenheit(peci_sa2);
      peci_gpu = convertToFahrenheit(peci_gpu);
      peci_gpu1 = convertToFahrenheit(peci_gpu1);
      gpu_proximity = convertToFahrenheit(gpu_proximity);
      gpu_die = convertToFahrenheit(gpu_die);
      gpu_die1 = convertToFahrenheit(gpu_die1);
      gpu_heatsink = convertToFahrenheit(gpu_heatsink);
      gpu_heatsink1 = convertToFahrenheit(gpu_heatsink1);
      memory_proximity = convertToFahrenheit(memory_proximity);
      mem_bank_a1 = convertToFahrenheit(mem_bank_a1);
      mem_bank_a2 = convertToFahrenheit(mem_bank_a2);
      mem_bank_b1 = convertToFahrenheit(mem_bank_b1);
      mem_bank_b2 = convertToFahrenheit(mem_bank_b2);
      mem_module_a1 = convertToFahrenheit(mem_module_a1);
      mem_module_a2 = convertToFahrenheit(mem_module_a2);
      mem_module_b1 = convertToFahrenheit(mem_module_b1);
      mem_module_b2 = convertToFahrenheit(mem_module_b2);
      northbridge_die = convertToFahrenheit(northbridge_die);
      northbridge_proximity_1 = convertToFahrenheit(northbridge_proximity_1);
      northbridge_proximity_2 = convertToFahrenheit(northbridge_proximity_2);
      mch_die = convertToFahrenheit(mch_die);
      mch_heatsink = convertToFahrenheit(mch_heatsink);
      pch_die = convertToFahrenheit(pch_die);
      pch_die1 = convertToFahrenheit(pch_die1);
      pch_proximity = convertToFahrenheit(pch_proximity);
      airflow_1 = convertToFahrenheit(airflow_1);
      airflow_2 = convertToFahrenheit(airflow_2);
      heatpipe_1 = convertToFahrenheit(heatpipe_1);
      heatpipe_2 = convertToFahrenheit(heatpipe_2);
      heatpipe_3 = convertToFahrenheit(heatpipe_3);
      mainboard_proximity = convertToFahrenheit(mainboard_proximity);
      powerboard_proximity = convertToFahrenheit(powerboard_proximity);
      palm_rest = convertToFahrenheit(palm_rest);
      blc_proximity = convertToFahrenheit(blc_proximity);
      lcd_proximity = convertToFahrenheit(lcd_proximity);
      airport_proximity = convertToFahrenheit(airport_proximity);
      hdd_bay_1 = convertToFahrenheit(hdd_bay_1);
      hdd_bay_2 = convertToFahrenheit(hdd_bay_2);
      hdd_bay_3 = convertToFahrenheit(hdd_bay_3);
      hdd_bay_4 = convertToFahrenheit(hdd_bay_4);
      optical_drive = convertToFahrenheit(optical_drive);
      battery_ts_max = convertToFahrenheit(battery_ts_max);
      battery_1 = convertToFahrenheit(battery_1);
      battery_2 = convertToFahrenheit(battery_2);
      battery = convertToFahrenheit(battery);
      power_supply_1 = convertToFahrenheit(power_supply_1);
      power_supply_1_alt = convertToFahrenheit(power_supply_1_alt);
      power_supply_2 = convertToFahrenheit(power_supply_2);
      power_supply_2_alt = convertToFahrenheit(power_supply_2_alt);
      power_supply_3 = convertToFahrenheit(power_supply_3);
      power_supply_4 = convertToFahrenheit(power_supply_4);
      power_supply_5 = convertToFahrenheit(power_supply_5);
      power_supply_6 = convertToFahrenheit(power_supply_6);
      expansion_slots = convertToFahrenheit(expansion_slots);
      pci_slot_1_pos_1 = convertToFahrenheit(pci_slot_1_pos_1);
      pci_slot_1_pos_2 = convertToFahrenheit(pci_slot_1_pos_2);
      pci_slot_2_pos_1 = convertToFahrenheit(pci_slot_2_pos_1);
      pci_slot_2_pos_2 = convertToFahrenheit(pci_slot_2_pos_2);
    }
    printf("PECI_CPU TCXC %0.1f°%c\n", peci_cpu, scale);
    printf("PECI_CPU1 TCXc %0.1f°%c\n", peci_cpu1, scale);
    printf("CPU_1_PROXIMITY TC0P %0.1f°%c\n", cpu_1_proximity, scale);
    printf("CPU_1_HEATSINK TC0H %0.1f°%c\n", cpu_1_heatsink, scale);
    printf("CPU_1_PACKAGE TC0D %0.1f°%c\n", cpu_1_package, scale);
    printf("CPU_1 TC0E %0.1f°%c\n", cpu_1, scale);
    printf("CPU_11 TC0F %0.1f°%c\n", cpu_11, scale);
    printf("CPU_CORE_1 TC1C %0.1f°%c\n", cpu_core_1, scale);
    printf("CPU_CORE_2 TC2C %0.1f°%c\n", cpu_core_2, scale);
    printf("CPU_CORE_3 TC3C %0.1f°%c\n", cpu_core_3, scale);
    printf("CPU_CORE_4 TC4C %0.1f°%c\n", cpu_core_4, scale);
    printf("CPU_CORE_5 TC5C %0.1f°%c\n", cpu_core_5, scale);
    printf("CPU_CORE_6 TC6C %0.1f°%c\n", cpu_core_6, scale);
    printf("CPU_CORE_7 TC7C %0.1f°%c\n", cpu_core_7, scale);
    printf("CPU_CORE_8 TC8C %0.1f°%c\n", cpu_core_8, scale);
    printf("CPU_1_HEATSINK_ALT TCAH %0.1f°%c\n", cpu_1_heatsink_alt, scale);
    printf("CPU_1_PACKAGE_ALT TCAD %0.1f°%c\n", cpu_1_package_alt, scale);
    printf("CPU_2_PROXIMITY TC1P %0.1f°%c\n", cpu_2_proximity, scale);
    printf("CPU_2_HEATSINK TC1H %0.1f°%c\n", cpu_2_heatsink, scale);
    printf("CPU_2_PACKAGE TC1D %0.1f°%c\n", cpu_2_package, scale);
    printf("CPU_2 TC1E %0.1f°%c\n", cpu_2, scale);
    printf("CPU_21 TC1F %0.1f°%c\n", cpu_21, scale);
    printf("CPU_2_HEATSINK_ALT TCBH %0.1f°%c\n", cpu_2_heatsink_alt, scale);
    printf("CPU_2_PACKAGE_ALT TCBD %0.1f°%c\n", cpu_2_package_alt, scale);
    printf("PECI_SA TCSC %0.1f°%c\n", peci_sa, scale);
    printf("PECI_SA1 TCSc %0.1f°%c\n", peci_sa1, scale);
    printf("PECI_SA2 TCSA %0.1f°%c\n", peci_sa2, scale);
    printf("PECI_GPU TCGC %0.1f°%c\n", peci_gpu, scale);
    printf("PECI_GPU1 TCGc %0.1f°%c\n", peci_gpu1, scale);
    printf("GPU_PROXIMITY TG0P %0.1f°%c\n", gpu_proximity, scale);
    printf("GPU_DIE TG0D %0.1f°%c\n", gpu_die, scale);
    printf("GPU_DIE1 TG1D %0.1f°%c\n", gpu_die1, scale);
    printf("GPU_HEATSINK TG0H %0.1f°%c\n", gpu_heatsink, scale);
    printf("GPU_HEATSINK1 TG1H %0.1f°%c\n", gpu_heatsink1, scale);
    printf("MEMORY_PROXIMITY Ts0S %0.1f°%c\n", memory_proximity, scale);
    printf("MEM_BANK_A1 TM0P %0.1f°%c\n", mem_bank_a1, scale);
    printf("MEM_BANK_A2 TM1P %0.1f°%c\n", mem_bank_a2, scale);
    printf("MEM_BANK_B1 TM8P %0.1f°%c\n", mem_bank_b1, scale);
    printf("MEM_BANK_B2 TM9P %0.1f°%c\n", mem_bank_b2, scale);
    printf("MEM_MODULE_A1 TM0S %0.1f°%c\n", mem_module_a1, scale);
    printf("MEM_MODULE_A2 TM1S %0.1f°%c\n", mem_module_a2, scale);
    printf("MEM_MODULE_B1 TM8S %0.1f°%c\n", mem_module_b1, scale);
    printf("MEM_MODULE_B2 TM9S %0.1f°%c\n", mem_module_b2, scale);
    printf("NORTHBRIDGE_DIE TN0D %0.1f°%c\n", northbridge_die, scale);
    printf("NORTHBRIDGE_PROXIMITY_1 TN0P %0.1f°%c\n", northbridge_proximity_1, scale);
    printf("NORTHBRIDGE_PROXIMITY_2 TN1P %0.1f°%c\n", northbridge_proximity_2, scale);
    printf("MCH_DIE TN0C %0.1f°%c\n", mch_die, scale);
    printf("MCH_HEATSINK TN0H %0.1f°%c\n", mch_heatsink, scale);
    printf("PCH_DIE TP0D %0.1f°%c\n", pch_die, scale);
    printf("PCH_DIE1 TPCD %0.1f°%c\n", pch_die1, scale);
    printf("PCH_PROXIMITY TP0P %0.1f°%c\n", pch_proximity, scale);
    printf("AIRFLOW_1 TA0P %0.1f°%c\n", airflow_1, scale);
    printf("AIRFLOW_2 TA1P %0.1f°%c\n", airflow_2, scale);
    printf("HEATPIPE_1 Th0H %0.1f°%c\n", heatpipe_1, scale);
    printf("HEATPIPE_2 Th1H %0.1f°%c\n", heatpipe_2, scale);
    printf("HEATPIPE_3 Th2H %0.1f°%c\n", heatpipe_3, scale);
    printf("MAINBOARD_PROXIMITY Tm0P %0.1f°%c\n", mainboard_proximity, scale);
    printf("POWERBOARD_PROXIMITY Tp0P %0.1f°%c\n", powerboard_proximity, scale);
    printf("PALM_REST Ts0P %0.1f°%c\n", palm_rest, scale);
    printf("BLC_PROXIMITY Tb0P %0.1f°%c\n", blc_proximity, scale);
    printf("LCD_PROXIMITY TL0P %0.1f°%c\n", lcd_proximity, scale);
    printf("AIRPORT_PROXIMITY TW0P %0.1f°%c\n", airport_proximity, scale);
    printf("HDD_BAY_1 TH0P %0.1f°%c\n", hdd_bay_1, scale);
    printf("HDD_BAY_2 TH1P %0.1f°%c\n", hdd_bay_2, scale);
    printf("HDD_BAY_3 TH2P %0.1f°%c\n", hdd_bay_3, scale);
    printf("HDD_BAY_4 TH3P %0.1f°%c\n", hdd_bay_4, scale);
    printf("OPTICAL_DRIVE TO0P %0.1f°%c\n", optical_drive, scale);
    printf("BATTERY_TS_MAX TB0T %0.1f°%c\n", battery_ts_max, scale);
    printf("BATTERY_1 TB1T %0.1f°%c\n", battery_1, scale);
    printf("BATTERY_2 TB2T %0.1f°%c\n", battery_2, scale);
    printf("BATTERY TB3T %0.1f°%c\n", battery, scale);
    printf("POWER_SUPPLY_1 Tp0P %0.1f°%c\n", power_supply_1, scale);
    printf("POWER_SUPPLY_1_ALT Tp0C %0.1f°%c\n", power_supply_1_alt, scale);
    printf("POWER_SUPPLY_2 Tp1P %0.1f°%c\n", power_supply_2, scale);
    printf("POWER_SUPPLY_2_ALT Tp1C %0.1f°%c\n", power_supply_2_alt, scale);
    printf("POWER_SUPPLY_3 Tp2P %0.1f°%c\n", power_supply_3, scale);
    printf("POWER_SUPPLY_4 Tp3P %0.1f°%c\n", power_supply_4, scale);
    printf("POWER_SUPPLY_5 Tp4P %0.1f°%c\n", power_supply_5, scale);
    printf("POWER_SUPPLY_6 Tp5P %0.1f°%c\n", power_supply_6, scale);
    printf("EXPANSION_SLOTS TS0C %0.1f°%c\n", expansion_slots, scale);
    printf("PCI_SLOT_1_POS_1 TA0S %0.1f°%c\n", pci_slot_1_pos_1, scale);
    printf("PCI_SLOT_1_POS_2 TA1S %0.1f°%c\n", pci_slot_1_pos_2, scale);
    printf("PCI_SLOT_2_POS_1 TA2S %0.1f°%c\n", pci_slot_2_pos_1, scale);
    printf("PCI_SLOT_2_POS_2 TA3S %0.1f°%c\n", pci_slot_2_pos_2, scale);
    return 0;
}

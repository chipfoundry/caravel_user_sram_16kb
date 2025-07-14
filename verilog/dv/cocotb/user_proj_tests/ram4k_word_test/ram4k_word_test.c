// SPDX-FileCopyrightText: 2023 Efabless Corporation

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//      http://www.apache.org/licenses/LICENSE-2.0

// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#define USER_ADDR_SPACE_C_HEADER_FILE  // TODO disable using the other file until tag is updated and https://github.com/efabless/caravel_mgmt_soc_litex/pull/137 is merged

#include <firmware_apis.h>
#include <custom_user_space.h>
#include <ram4k_info.h>

void main(){
    // Enable managment gpio as output to use as indicator for finishing configuration  
    GPIOs_configure(32,GPIO_MODE_MGMT_STD_OUTPUT);
    GPIOs_configure(33,GPIO_MODE_MGMT_STD_OUTPUT);
    GPIOs_loadConfigs();
    User_enableIF(1);
    ManagmentGpio_outputEnable();
    ManagmentGpio_write(0);
    GPIOs_writeHigh(0b10);

    volatile int shifting;
    volatile int data_used;
    
    // Test different regions of the 4096x32 SRAM
    // Test first 10% of memory
    int start_address[4] = {0, (RAM_NUM_WORDS*25/100), (RAM_NUM_WORDS*50/100), (RAM_NUM_WORDS*75/100)};
    int end_address[4] = {(RAM_NUM_WORDS*10/100), (RAM_NUM_WORDS*35/100), (RAM_NUM_WORDS*60/100), (RAM_NUM_WORDS*85/100)};
    
    for (int k = 0; k < 4; k++){
        for (int i = start_address[k]; i < end_address[k]; i++){
            shifting = 0xFFFFFFFF - (0x1 << i%32);
            data_used = 0x55555555 & shifting;
            USER_writeWord(data_used,i);
        }
        for (int i = start_address[k]; i < end_address[k]; i++){
            shifting = 0xFFFFFFFF - (0x1 << i%32);
            data_used = 0x55555555 & shifting;
            int data = USER_readWord(i);
            if (data_used != data) {
                GPIOs_writeHigh(0b01); // failure
                ManagmentGpio_write(1); // finish test
                return;
            }
        }
    }
    
    // Test boundary conditions and edge cases
    // Test the last few addresses
    int boundary_start = RAM_NUM_WORDS - 100;
    int boundary_end = RAM_NUM_WORDS;
    
    for (int i = boundary_start; i < boundary_end; i++){
        data_used = 0xAAAAAAAA + (i % 0x55555555);
        USER_writeWord(data_used, i);
    }
    
    for (int i = boundary_start; i < boundary_end; i++){
        data_used = 0xAAAAAAAA + (i % 0x55555555);
        int data = USER_readWord(i);
        if (data_used != data) {
            GPIOs_writeHigh(0b01); // failure
            ManagmentGpio_write(1); // finish test
            return;
        }
    }
    
    // Test SRAM bank boundaries (every 1024 words)
    int bank_boundaries[7] = {1023, 1024, 2047, 2048, 3071, 3072, 4095};
    int bank_values[7] = {0xDEADBEEF, 0xCAFEBABE, 0x12345678, 0x87654321, 0xAAAAAAAA, 0x55555555, 0xFFFFFFFF};
    
    for (int i = 0; i < 7; i++){
        USER_writeWord(bank_values[i], bank_boundaries[i]);
    }
    
    for (int i = 0; i < 7; i++){
        int data = USER_readWord(bank_boundaries[i]);
        if (bank_values[i] != data) {
            GPIOs_writeHigh(0b01); // failure
            ManagmentGpio_write(1); // finish test
            return;
        }
    }
    
    ManagmentGpio_write(1); // finish test

} 
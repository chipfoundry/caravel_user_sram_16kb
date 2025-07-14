# 4096x32 SRAM Test Benches

This directory contains test benches for the 4096x32 SRAM macro that was created using four 1024x32 SRAM instances.

## Test Structure

### 1. ram4k_smoke_test
A comprehensive smoke test that verifies basic functionality of the 4096x32 SRAM:

**Features:**
- Word-level read/write operations across different SRAM banks
- Half-word operations with byte enable testing
- Byte-level operations with selective byte writes
- Write-then-read operations for immediate verification
- Boundary condition testing (last few addresses)

**Test Coverage:**
- Tests addresses: 0, 1024, 2048, 3072, 100, 3950 (different banks)
- Tests boundary addresses: 4093, 4094, 4095
- Tests various data patterns: 0x8e277123, 0xdeadbeef, 0xcafebabe, etc.

### 2. ram4k_word_test
A comprehensive word-level test that exercises the entire memory space:

**Features:**
- Systematic testing across 4 different memory regions (0-10%, 25-35%, 50-60%, 75-85%)
- Boundary condition testing for the last 100 addresses
- SRAM bank boundary testing (addresses 1023, 1024, 2047, 2048, 3071, 3072, 4095)
- Pattern-based data generation for thorough testing

**Test Coverage:**
- Tests approximately 40% of the total memory space
- Uses pattern-based data: 0x55555555 with shifting patterns
- Tests bank boundaries to ensure proper address decoding

## Test Configuration

Both tests are configured in:
- `user_proj_tests.yaml` - Main test configuration
- `cocotb_tests.py` - Python test imports

## Running the Tests

To run the tests, use the following commands:

```bash
# Run RTL simulation
make cocotb-verify-rtl

# Run specific tests
make verify-ram4k_smoke_test-rtl
make verify-ram4k_word_test-rtl
```

## Test Dependencies

The tests require:
- `ram4k_info.h` - Defines RAM_NUM_WORDS as 4096
- `firmware_apis.h` - Caravel firmware APIs
- `custom_user_space.h` - User space definitions

## Expected Results

Both tests should:
1. Write test data to various memory locations
2. Read back the data and verify it matches
3. Set GPIO[33:32] to 0b10 on success or 0b01 on failure
4. Signal completion via management GPIO

## Architecture Considerations

The tests are designed to exercise the 4096x32 SRAM's internal structure:
- Address bits [11:10] select which of the four 1024x32 SRAM banks
- Address bits [9:0] select the word within each bank
- Tests verify proper bank selection and data multiplexing 
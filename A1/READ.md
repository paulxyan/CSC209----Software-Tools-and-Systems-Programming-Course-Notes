# CSC209 Assignment 1 — Parity Computation & RAID Block Recovery

## **About**
For this assignment, I implemented a simplified simulation of how RAID file systems use parity bits to improve data reliability and recover missing information. The goal was to write C programs that can compute parity blocks, restore missing data, and verify correctness using a set of custom test cases.

---

## **Project Overview**
RAID (Redundant Array of Independent Disks) stores parity information so that lost or corrupted data can be recovered. In this project, I:

- Wrote functions to compute parity values using XOR operations.
- Implemented a way to restore missing blocks when one “storage device” fails.
- Practiced working with 2D arrays, bitwise operations, and file input in C.
- Created a set of custom test cases to ensure my implementation works correctly.

## **Files in This Project**
| **File**            | **Purpose** |
|---------------------|-------------------------------------------|
| `parity.c`          | Where I implemented all required functions for parity computation and data restoration. |
| `compute_parity.c`  | A program that reads data, computes the parity block, and prints it in hexadecimal format. |
| `restore_block.c`   | A program that simulates data loss and restores missing blocks using parity. |
| `quotes.txt`        | A sample input file used for testing. |
| `testing.txt`       | My test descriptions, commands, and expected outputs. |
| Other `.txt` files  | Extra test inputs I created to validate my implementation. |

## **What I Implemented**

### **1. Computing Parity Blocks**
- Implemented in the `compute_parity_block()` function.
- Uses the XOR operator (`^`) to calculate parity for each column in a 2D data array.
- Stores the results in the `parity_block` array.
- Uses `print_parity_block()`** to print the values in hexadecimal format (`%02x`) for readability.

### **2. Restoring Missing Blocks**
- Implemented in `reconstruct_block()`.
- Uses the parity block and XOR operations to reconstruct missing data when a block is lost.
- Relies on `delete_data_block()` to simulate a failure by setting all bytes in a block to `\0`.

### **3. Testing My Code**
- I created nine test cases in `testing.txt` using:
  - `Test1.txt`
  - `Test2.txt`
  - `Test3.txt`
  - `Test4.txt`
- Tests cover:
  - 3 tests for `get_data`
  - 3 tests for `compute_parity_block`
  - 3 tests for `restore_block`
- Each test includes:
  1. A short description of the scenario.
  2. The command I used to run it.
  3. The expected output.

---

## **How I Compile and Run My Code**
I compiled my programs using gcc on the teach.cs lab machines at the university:

```bash
# Compile compute_parity program
gcc -Wall -g -o compute_parity compute_parity.c parity.c

# Compile restore_block program
gcc -Wall -g -o restore_block restore_block.c parity.c



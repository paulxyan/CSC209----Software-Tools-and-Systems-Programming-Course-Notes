# CSC209 — Assignment 3: RAID Simulator

This project implements a RAID Level 4 Simulator in C, designed to simulate a storage system where data blocks are striped across multiple disks with one disk dedicated to parity. The simulator demonstrates how RAID achieves fault tolerance and improves read performance at the controller level.

---

## 📂 Repository Structure


CSC209-A3-RAID-Simulator/
├── README.md                          # High-level project overview & usage guide
├── Makefile                           # Automates compilation & cleanup
├── docs/
│   └── CSC209_Assignment3_Instructions.pdf   # Assignment instructions
├── src/
│   ├── raid_sim.c                    # Main RAID simulator driver
│   ├── controller.c                 # Implements RAID operations (read/write/parity)
│   ├── disk_sim.c                  # Simulates disk processes
│   ├── raid.h                      # Header file with constants, structs, enums
├── tests/
│   ├── data1                       # Sample data file for testing
│   ├── data2                       # Sample data file for testing
│   ├── simpletrans                 # Example transaction file for simulator input
│   └── advanced_trans             # (Optional) Custom stress tests
└── logs/
    └── debug.log                   # (Optional) Debugging or output logs
🛠️ Building the Project
The project uses a Makefile to automate compilation.

bash
Copy code
make
This produces the following executable:

raid_sim → The RAID simulator driver

To clean up object files and executables:

bash
Copy code
make clean
▶️ Running the RAID Simulator
The RAID simulator can be run in two modes:

1. Interactive Shell Mode
Manually type RAID commands in the terminal:

bash
Copy code
./raid_sim -n 2 -b 16 -s 16
Where:

-n → Number of data disks (not including the parity disk)

-b → Block size (in bytes)

-s → Disk size (in blocks)

Then enter commands interactively:

bash
Copy code
wb 0 tests/data1       # Write block 0 from data1
rb 0                   # Read block 0
kill 1                 # Simulate disk 1 failure
exit                   # Save all disks & exit
2. Transaction File Mode
Supply a transaction file containing prewritten RAID commands:

bash
Copy code
./raid_sim -n 2 -t tests/simpletrans
Example tests/simpletrans:

txt
Copy code
wb 0 tests/data1
wb 1 tests/data2
rb 0
rb 1
exit
🧠 RAID Concepts in This Project
This simulator implements a simplified RAID Level 4 configuration:

Striping → Data blocks are distributed across disks for better performance.

Parity Disk → Stores XOR-computed parity data to allow recovery from disk failures.

Fault Tolerance → If a disk fails, its contents are reconstructed using the parity disk and remaining disks.

📜 Key Source Files
File	Description
raid_sim.c	Main driver program; parses transactions & handles user input.
controller.c	Implements core RAID logic: reads, writes, parity calculations, disk recovery.
disk_sim.c	Simulates disk processes as child processes; handles I/O requests.
raid.h	Shared constants, struct definitions, and enums used across the project.

🧪 Testing the Simulator
The tests/ folder includes sample input and transaction files.

Run with provided example:

bash
Copy code
./raid_sim -n 1 -t tests/simpletrans
Sample output:

plaintext
Copy code
[0] Waiting for command
wb
[1] Waiting for command
Block 0 written to RAID
Block 1 written to RAID
🧹 Memory & Error Checking
Use Valgrind to ensure no memory leaks:

bash
Copy code
valgrind ./raid_sim -n 1 -t tests/simpletrans
Confirms proper memory management.

Ensures disk processes are closed and data saved.

📌 Notes
Do not submit object (.o) files or executables.

Always test on the teach.cs servers before final submission.

Debugging logs can be directed to logs/debug.log for easier troubleshooting.

✍ Author
Paul Yan
CSC209 — University of Toronto
Assignment 3 — RAID Simulator


# CSC209 — Assignment 2: Mark-and-Sweep Garbage Collection

This project implements a mark-and-sweep garbage collector in C and demonstrates it on two dynamic data structures:

- A singly linked list (`list.c`)
- A file-system tree (`fstree.c`) that supports cycles via links

The goal of this assignment is to understand manual memory management in C and simulate the behavior of garbage-collected languages like Python and Java.

---

# 📂 Repository Structure



```plaintext
CSC209-A2-Garbage-Collector/
├── README.md                         # High-level project overview & instructions
├── Makefile                         # Automates building and running programs
├── docs/                            # Documentation and references
│   ├── CSC209_Assignment2_Instructions.pdf
│   └── design_notes.md             # (Optional) Design explanation for employers & graders
├── src/                             # Source code implementation
│   ├── do_list.c                   # Linked list driver
│   ├── do_fstree.c                 # Fstree driver
│   ├── mktrans.c                   # Transaction generator
│   ├── list.c                      # Linked list implementation
│   ├── list.h                      # Linked list header
│   ├── fstree.c                    # Fstree implementation
│   ├── fstree.h                    # Fstree header
│   ├── gc.c                        # Core garbage collector implementation
│   ├── gc.h                        # Garbage collector header
│   ├── gc_list.c                   # Marking function for linked list
│   └── gc_fstree.c                 # Marking function for fstree
├── include/                        # (Optional) Separate folder for headers
│   ├── gc.h
│   ├── list.h
│   └── fstree.h
├── tests/                          # Custom tests and transaction inputs
│   ├── testing.txt                # Descriptions of custom test cases
│   ├── transactions/             # Transaction files for do_list & do_fstree
│   │   ├── list_trans1
│   │   ├── list_trans2
│   │   ├── fs_trans1
│   │   └── fs_trans2
│   └── valgrind_logs/            # (Optional) Memory-leak reports from Valgrind
└── logs/                          # Output logs
    └── gc.log                    # Log of garbage collection activity

## 🛠️ Building the Project

The provided `Makefile` handles compilation for you.

# Build everything
make
This generates three executables:

do_list → Runs linked-list transactions

do_fstree → Runs file-system tree transactions

mktrans → Generates random linked-list transaction files

To clean up build artifacts:

Copy code
make clean
▶️ Running the Programs
1. Running Linked List Tests
bash
Copy code
./do_list transactions/list_trans1
Uses the linked list driver (do_list.c)

Reads transactions (add, remove, print, GC) from a .txt file

Example transaction file:

css
Copy code
a 10
a 20
r 10
p
g
a <val> → Add node

r <val> → Remove node

p → Print list

g → Trigger garbage collection

2. Running Fstree Tests
bash
Copy code
./do_fstree transactions/fs_trans1
Uses the file-system tree driver (do_fstree.c)

Supports directories, subdirectories, and links (cycles)

Demonstrates mark-and-sweep on a more complex structure

3. Generating Random Transactions
bash
Copy code
./mktrans > transactions/random_trans.txt
Produces a random sequence of linked-list operations

Useful for stress-testing your garbage collector

🧠 How It Works
Step 1 — Memory Tracking
All allocations go through gc_malloc() (in gc.c)

Every allocated pointer is stored in a memory reference list

Each memory block is tagged with an in_use flag

Step 2 — Mark Phase
Traverses data structures (lists, fstree) and marks reachable nodes

Implemented separately for each structure:

gc_list.c → marks reachable linked-list nodes

gc_fstree.c → marks reachable fstree nodes

Step 3 — Sweep Phase
Scans the memory reference list

Frees any node not marked as reachable

Removes freed blocks from the list

🧪 Testing
All tests are described in testing.txt.

At least 6 tests required:

3 linked-list cases

3 fstree cases

Example test descriptions:

Adding/removing multiple nodes

Triggering GC with unreachable nodes

Cyclic references in fstree


✍ Author
Paul Yan
CSC209 — University of Toronto
Assignment 2 — Garbage Collection

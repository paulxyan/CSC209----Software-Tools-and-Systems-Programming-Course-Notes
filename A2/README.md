
# CSC209 — Assignment 2: Mark-and-Sweep Garbage Collection

This project implements a mark-and-sweep garbage collector in C and demonstrates it on two dynamic data structures:

- A singly linked list (`list.c`)
- A file-system tree (`fstree.c`) that supports cycles via links

The goal of this assignment is to understand manual memory management in C and simulate the behavior of garbage-collected languages like Python and Java.

---

## 📂 Repository Structure

.
├── transactions/ # Your own transaction test files go here
├── Makefile # Builds and runs all programs
├── do_list.c # Driver for the linked-list program
├── do_fstree.c # Driver for the file-system tree program
├── mktrans.c # Generates random linked-list transactions
├── list.c / list.h # Linked list implementation & interface
├── fstree.c / fstree.h # File-system tree implementation & interface
├── gc.c / gc.h # Core garbage collector logic + definitions
├── gc_list.c # Linked-list-specific marking logic
├── gc_fstree.c # Fstree-specific marking logic
└── testing.txt # Contains descriptions of test cases

---

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

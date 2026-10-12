# Student points analyzer

Course work for **Systems Programming (PITa-25-NL-EN) – 2026 Autumn**, practical
tasks Nr. 1, Nr. 2 and Nr. 3 (assignment group GIA1).

The program reads student records, calculates the final points for every
student, sorts them, divides them into *failed* and *passed* groups, writes the
two groups into separate files, and measures how long each stage takes with
`std::vector`, `std::list` and `std::deque`.

---

## 1. Points calculation formula

The final result of a student is a weighted sum of the homework part and the
exam part:

```
final = 0.4 * homework + 0.6 * exam
```

where `homework` is either

* the **average** of all homework grades, or
* the **median** of all homework grades,

whichever the user selects in the menu. All grades are integers in the range
`1 … 10`, and the final result is printed with two decimal places.

A student whose final result is **below 5.00** is put into the *failed* group,
a student with **5.00 or more** into the *passed* group.

---

## 2. Releases

| Release | Task | What it adds |
|---|---|---|
| **v0.1** | Task Nr. 1 | `Person` class with the rule of three, overloaded `>>` / `<<`, average and median calculation, unknown number of homework results via `std::vector`, reading `Students.txt`, sorted and formatted output. |
| **v0.2** | Task Nr. 2 | Code split into `*.h` / `*.cpp` units, typed exceptions, random test-file generation (10 000 … 10 000 000 records), splitting into *failed* / *passed*, two output files, per-stage timing with `<chrono>`. `std::vector` only. |
| **v0.25** | Task Nr. 2 | The whole pipeline turned into a template, so the identical code runs with `std::vector`, `std::list` and `std::deque`; all three are measured on the same data files. |
| **v1.0** | Task Nr. 3 | Both splitting strategies selectable from the menu, a second implementation of every step written with the `<algorithm>` library, `Makefile` and `CMakeLists.txt`, measured results written into `benchmark_results.md`. |

### v0.1 — Task Nr. 1

Single file `main.cpp`. Class `Person` holds the first name, the surname, the
homework results and the exam result, and stores the final grade inside the
object. The rule of three (copy constructor, copy-assignment operator,
destructor) is written out explicitly, even though `Person` owns no raw memory
and the compiler-generated versions would behave identically — the task asks for
all three. Input uses the overloaded `operator>>`, where the user ends the
homework list by entering `0`, so the number of homework results does not need
to be known in advance.

### v0.2 — Task Nr. 2, first half

* `main.cpp` broken up into `person`, `fileio`, `processing`, `generator`,
  `timer` and `exceptions` units.
* Error handling through a small exception hierarchy
  (`AppError` → `FileError`, `ParseError`, `InvalidGradeError`, `EmptyDataError`).
  A missing file, a line without grades and a grade outside `1…10` all throw
  instead of silently producing wrong output.
* Random data files are generated with `<random>` (`std::mt19937` +
  `std::uniform_int_distribution`) using the default names
  `Name1 Surname1`, `Name2 Surname2`, …
* The four stages — read, sort, split, write — are timed separately.

### v0.25 — Task Nr. 2, second half

The container type became a template parameter, so `std::vector<Person>`,
`std::list<Person>` and `std::deque<Person>` all go through exactly the same
code. One detail had to be handled specially: `std::list` has no random access
iterators, so `std::sort` cannot be used on it and its own member `sort()` is
called instead (selected with `if constexpr`).

### v1.0 — Task Nr. 3

* **Strategy 1** — the base container keeps every student and two new
  containers are filled with copies.
* **Strategy 2** — failed students are taken out of the base container and the
  base container is shrunk, so only passed students remain in it.
* Every step exists twice: as a hand-written loop and written with the
  `<algorithm>` library (`std::stable_partition`, `std::remove_copy_if`,
  `std::copy`, `std::find_if`, `std::count_if`, `std::transform`,
  `std::list::splice`). The menu lets both be measured against each other.

---

## 3. Usage guide

### Build with make (Unix, WSL, MSYS2)

```bash
make                 # builds bin/students
make run             # builds and starts the interactive menu
make data            # generates students1000.txt, students10000.txt, students100000.txt
make bench           # runs the full benchmark on students1000.txt
make clean           # removes build/ and bin/
make distclean       # also removes generated data and result files
```

### Build with CMake (Windows, Linux, macOS)

```bash
cmake -S . -B build
cmake --build build --config Release
./build/students                  # Windows: build\Release\students.exe
```

### Running

Started without arguments the program shows a menu:

```
===== Student points analyzer  v1.0 =====
1. Generate random test files
2. Read a file and show the table
3. Enter students from the keyboard
4. Process one file (choose container / strategy / code path)
5. Full benchmark on one file (all combinations)
6. Change the final grade mode
0. Exit
```

A typical first session is: **1** (generate `students1000.txt`) → **5** (run the
full benchmark on it) → the table appears on the screen and is appended to
`benchmark_results.md`.

There is also a non-interactive interface, which is what the measurements below
were produced with:

```bash
./bin/students --generate 1000000          # creates students1000000.txt
./bin/students --show students1000.txt     # prints a formatted table
./bin/students --bench students1000.txt    # every container / strategy / code path
./bin/students --run students1000.txt vector 2 algo   # one single run
```

### Input file format

```
Name        Surname    HW1   HW2   HW3   HW4   HW5   Exam
Vardas1     Pavarde1     8     9    10     6    10      9
Vardas2     Pavarde2     7    10     8     5     4      6
```

The header line is optional and the number of `HW` columns is free — the last
number in a line is always taken as the exam result. A sample file is in
`data/Students.txt`.

### Output

`<prefix>_failed.txt` and `<prefix>_passed.txt`, both sorted by final points:

```
Name            Surname           Final (Avg.)
----------------------------------------------
Name332         Surname332                9.24
Name291         Surname291                9.20
```

---

## 4. Measured results and comments

Measurement environment: Ubuntu 24.04, g++ 13.3.0, `-O2 -std=c++17`, 2 cores,
8 GB RAM. Times in seconds, five homework results per student, final grade from
the homework **average**. Each file produced 38.5 % failed and 61.5 % passed
students, checked against an independent `awk` calculation.

### 1 000 records

| Container | Strategy | Implementation | Read | Sort | Split | Write | Total |
|---|---|---|---:|---:|---:|---:|---:|
| `std::vector` | 1 | loops | 0.0007 | 0.0005 | 0.0002 | 0.0041 | 0.0054 |
| `std::list` | 1 | loops | 0.0004 | 0.0001 | 0.0001 | 0.0039 | 0.0045 |
| `std::deque` | 1 | loops | 0.0005 | 0.0003 | 0.0000 | 0.0043 | 0.0052 |
| `std::vector` | 1 | algorithms | 0.0006 | 0.0004 | 0.0002 | 0.0011 | 0.0023 |
| `std::list` | 1 | algorithms | 0.0006 | 0.0001 | 0.0001 | 0.0011 | 0.0019 |
| `std::deque` | 1 | algorithms | 0.0006 | 0.0004 | 0.0000 | 0.0006 | 0.0017 |
| `std::vector` | 2 | loops | 0.0005 | 0.0004 | 0.0014 | 0.0009 | 0.0032 |
| `std::list` | 2 | loops | 0.0005 | 0.0001 | 0.0000 | 0.0008 | 0.0015 |
| `std::deque` | 2 | loops | 0.0003 | 0.0003 | 0.0015 | 0.0007 | 0.0028 |
| `std::vector` | 2 | algorithms | 0.0003 | 0.0004 | 0.0002 | 0.0007 | 0.0015 |
| `std::list` | 2 | algorithms | 0.0003 | 0.0001 | 0.0001 | 0.0007 | 0.0012 |
| `std::deque` | 2 | algorithms | 0.0003 | 0.0003 | 0.0001 | 0.0010 | 0.0018 |

At this size everything is noise — the whole run is under 6 ms and the
differences are smaller than the measurement error.

### 10 000 records

| Container | Strategy | Implementation | Read | Sort | Split | Write | Total |
|---|---|---|---:|---:|---:|---:|---:|
| `std::vector` | 1 | loops | 0.0050 | 0.0046 | 0.0023 | 0.0034 | 0.0154 |
| `std::list` | 1 | loops | 0.0038 | 0.0018 | 0.0016 | 0.0027 | 0.0100 |
| `std::deque` | 1 | loops | 0.0035 | 0.0049 | 0.0005 | 0.0032 | 0.0121 |
| `std::vector` | 1 | algorithms | 0.0047 | 0.0044 | 0.0014 | 0.0033 | 0.0138 |
| `std::list` | 1 | algorithms | 0.0042 | 0.0019 | 0.0014 | 0.0030 | 0.0106 |
| `std::deque` | 1 | algorithms | 0.0036 | 0.0046 | 0.0005 | 0.0027 | 0.0113 |
| `std::vector` | 2 | **loops** | 0.0040 | 0.0045 | **0.1867** | 0.0030 | **0.1983** |
| `std::list` | 2 | loops | 0.0036 | 0.0017 | 0.0004 | 0.0031 | 0.0089 |
| `std::deque` | 2 | **loops** | 0.0030 | 0.0046 | **0.1906** | 0.0033 | **0.2015** |
| `std::vector` | 2 | algorithms | 0.0039 | 0.0045 | 0.0032 | 0.0031 | 0.0148 |
| `std::list` | 2 | algorithms | 0.0032 | 0.0016 | 0.0013 | 0.0031 | 0.0092 |
| `std::deque` | 2 | algorithms | 0.0033 | 0.0051 | 0.0014 | 0.0026 | 0.0123 |

The first real result: strategy 2 written as an `erase()` inside a loop is
already **13 times slower** than everything else for `vector` and `deque`,
while `list` is unaffected.

### 100 000 records

| Container | Strategy | Implementation | Read | Sort | Split | Write | Total |
|---|---|---|---:|---:|---:|---:|---:|
| `std::vector` | 1 | loops | 0.0563 | 0.0705 | 0.0226 | 0.0300 | 0.1793 |
| `std::list` | 1 | loops | 0.0490 | 0.0685 | 0.0428 | 0.0268 | 0.1872 |
| `std::deque` | 1 | loops | 0.0473 | 0.0631 | 0.0051 | 0.0214 | 0.1369 |
| `std::vector` | 1 | algorithms | 0.0423 | 0.0578 | 0.0131 | 0.0209 | 0.1341 |
| `std::list` | 1 | algorithms | 0.0322 | 0.0484 | 0.0348 | 0.0218 | 0.1372 |
| `std::deque` | 1 | algorithms | 0.0468 | 0.0621 | 0.0060 | 0.0208 | 0.1358 |
| `std::vector` | 2 | **loops** | 0.0389 | 0.0636 | **17.3022** | 0.0241 | **17.4288** |
| `std::list` | 2 | loops | 0.0539 | 0.0770 | 0.0249 | 0.0615 | 0.2173 |
| `std::deque` | 2 | **loops** | 0.0614 | 0.1005 | **19.8059** | 0.0299 | **19.9977** |
| `std::vector` | 2 | algorithms | 0.0420 | 0.0867 | 0.0384 | 0.0400 | 0.2071 |
| `std::list` | 2 | algorithms | 0.0539 | 0.0577 | 0.0654 | 0.0401 | 0.2171 |
| `std::deque` | 2 | algorithms | 0.0339 | 0.0646 | 0.0204 | 0.0232 | 0.1422 |

Ten times more data turned a 0.19 s split into a 17 s split — a 93× jump for a
10× larger input. That is the signature of a quadratic algorithm: each
`vector::erase()` shifts every element behind it, so removing 38 500 students
from a 100 000 element vector moves billions of bytes.

### 1 000 000 records

| Container | Strategy | Implementation | Read | Sort | Split | Write | Total |
|---|---|---|---:|---:|---:|---:|---:|
| `std::vector` | 1 | loops | 0.4960 | 0.6743 | 0.2563 | 0.2526 | 1.6791 |
| `std::list` | 1 | loops | 0.4015 | 1.0323 | 0.3900 | 0.3711 | 2.1948 |
| `std::deque` | 1 | loops | 0.6056 | 0.8059 | 0.0477 | 0.1905 | 1.6498 |
| `std::vector` | 1 | algorithms | 0.4551 | 0.8548 | 0.2752 | 0.2514 | 1.8365 |
| `std::list` | 1 | algorithms | 0.3512 | 0.9756 | 0.4248 | 0.2016 | 1.9532 |
| `std::deque` | 1 | algorithms | 0.6282 | 0.8901 | 0.0590 | 0.2032 | 1.7805 |
| `std::vector` | 2 | loops | — | — | *skipped, ≈1900 s* | — | — |
| `std::list` | 2 | loops | 0.3690 | 1.1042 | 0.1694 | 0.3568 | 1.9995 |
| `std::deque` | 2 | loops | — | — | *skipped, ≈1900 s* | — | — |
| `std::vector` | 2 | **algorithms** | 0.4013 | 0.7530 | 0.2504 | 0.2558 | **1.6605** |
| `std::list` | 2 | algorithms | 0.3874 | 0.9740 | 0.5971 | 0.3885 | 2.3470 |
| `std::deque` | 2 | algorithms | 0.3558 | 0.8706 | 0.2646 | 0.2162 | 1.7073 |

The two quadratic combinations are skipped automatically above 200 000 records;
extrapolating the 100 000 record measurement they would need roughly half an
hour each. The program prints them as *skipped* instead of running them.

The 10 000 000 record file was not measured: the data file alone is ~590 MB and
`std::list` needs over 3 GB of RAM for it, which did not fit in the machine used
for the measurements. The code handles it — only the hardware did not.

### What the numbers say

**Containers.** For this workload the three containers are much closer than one
might expect, because reading and sorting dominate and all three do those at a
similar speed. The differences appear in the split stage:

* `std::deque` has the fastest split under strategy 1 (0.048 s vs 0.256 s for
  vector at 1 M). It grows in fixed-size blocks, so appending never reallocates
  and copies the whole container the way `vector` does.
* `std::list` is the only container that is indifferent to strategy 2 with the
  naive loop — `erase()` on a list just relinks two pointers, it never shifts
  anything. It is also the only one that can hand over a whole range of elements
  without copying a single object, via `splice()`.
* `std::list` pays for that everywhere else: it is consistently the slowest at
  sorting (1.03 s vs 0.67 s for vector at 1 M), because its elements are
  scattered across the heap and every comparison is a cache miss.

**Fastest overall combination at 1 M records: `std::vector` + strategy 2 +
`<algorithm>`, 1.66 s** — and `std::deque` + strategy 1 is within noise of it.

**Strategies.** Strategy 2 is only worth using if it is implemented properly.
Removing elements one by one from a `vector` or a `deque` is quadratic; one
`std::stable_partition` pass followed by a single `erase(first, middle)` is
linear and turned a 17.3 s split into a 0.038 s split at 100 000 records —
a **450× speed-up from changing the algorithm, not the container**. This is the
clearest result of the whole task: the choice of algorithm mattered far more
than the choice of container.

**Is strategy 1 really wasteful with memory?** The task says it is, and asks to
verify that. Measured peak resident memory at 1 000 000 records:

| Scenario | Peak RSS |
|---|---:|
| read 1 M students into a `std::vector`, no split (baseline) | 148 MB |
| strategy 1, `std::vector` | 367 MB |
| strategy 2, `std::vector` (with `shrink_to_fit`) | 304 MB |
| strategy 2, `std::vector` (without `shrink_to_fit`) | 278 MB |
| strategy 1, `std::list` | 342 MB |
| strategy 2, `std::list` | 309 MB |
| strategy 1, `std::deque` | 292 MB |
| strategy 2, `std::deque` | 284 MB |

So the claim is **true but much weaker than it sounds**. Strategy 1 does use
more memory, but only about 17–21 % more at peak, not twice as much. The reason
is that strategy 2 is not free either: it still needs a second container for the
failed students, and `shrink_to_fit()` has to allocate a whole new buffer for
the remaining students before it can release the old one. The measurement above
shows this directly — calling `shrink_to_fit()` *raised* the peak from 278 MB to
304 MB, even though it lowers the memory held afterwards. If the goal is the
lowest peak, strategy 2 without an explicit shrink wins; if the goal is the
lowest memory held after the operation, the shrink is worth its temporary cost.

**Reading and writing.** Across every configuration, reading the file takes
20–30 % of the total time and writing another 12–20 %, so at these sizes the
I/O is as expensive as everything else put together. Using a hand-written
parser (`strtol` over the line buffer) instead of `std::istringstream` per line,
and building output in a 1 MB string buffer instead of streaming field by field,
is what keeps the 1 M record file at roughly 0.4 s read / 0.25 s write.

---

## 5. Repository layout

```
.
├── CMakeLists.txt          cmake build, any OS
├── Makefile                make build, Unix
├── README.md
├── data/
│   └── Students.txt        small sample input
├── include/
│   ├── benchmark.h         timed pipeline, results table
│   ├── exceptions.h        AppError hierarchy
│   ├── fileio.h            templated reading / writing / printing
│   ├── generator.h         random test file generation
│   ├── person.h            the Person class
│   ├── processing.h        sorting and both splitting strategies
│   └── timer.h             <chrono> stopwatch
└── src/
    ├── benchmark.cpp
    ├── generator.cpp
    ├── main.cpp            menu and command line interface
    └── person.cpp
```

Branches `v0.1`, `v0.2` and `v0.25` hold the earlier releases; `main` holds
v1.0.

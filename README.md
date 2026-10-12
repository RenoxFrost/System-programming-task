# Student points analyzer

Course work for Systems Programming (PITa-25-NL-EN) - 2026 Autumn.

The application reads student records (first name, surname, homework results
and an exam result), calculates the final points of every student, sorts them
and divides them into two groups: "failed" and "passed".

## Points calculation formula

    final = 0.4 * homework + 0.6 * exam

where `homework` is either the average or the median of all homework grades,
chosen by the user. Grades are integers in the range 1..10 and the final
result is printed with two decimal places. A student with fewer than 5.00
points has failed the subject.

## Release v0.1 - Task Nr. 1

* class `Person`: first name, surname, homework results, exam result and the
  final grade stored inside the object;
* the rule of three written out explicitly - copy constructor,
  copy-assignment operator, destructor;
* data input through the overloaded `operator>>`, data output through the
  overloaded `operator<<`;
* the final grade can be calculated from the **average** or from the
  **median** of the homework results, and the chosen method is shown in the
  output;
* the number of homework results does not have to be known in advance: the
  user ends the list by entering `0`, and the grades are kept in a
  `std::vector`;
* reading records from `Students.txt`, output sorted by name and formatted
  into columns.

## Build and run

```bash
g++ -std=c++17 -O2 -Wall -Wextra -o students main.cpp
./students
```

Menu option **2** reads `Students.txt`, option **5** prints the table using one
chosen calculation method.

## Input file format

```
Name        Surname    HW1   HW2   HW3   HW4   HW5   Exam
Vardas1     Pavarde1     8     9    10     6    10      9
Vardas2     Pavarde2     7    10     8     5     4      6
```

The header line is optional and the number of `HW` columns is free - the last
number in a line is always taken as the exam result.

## Planned releases

* v0.2  - multi-file project, exceptions, random data files, failed / passed split, timing
* v0.25 - the same pipeline with std::vector, std::list and std::deque
* v1.0  - both splitting strategies, `<algorithm>` based implementation, make + cmake

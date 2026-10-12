# Student points analyzer

Course work for Systems Programming (PITa-25-NL-EN) - 2026 Autumn.

## Points calculation formula

    final = 0.4 * homework + 0.6 * exam

`homework` is the average or the median of all homework grades, chosen by the
user. A student with fewer than 5.00 points has failed the subject.

## Releases

### v0.1 - Task Nr. 1
Single `main.cpp`. Class `Person` with the rule of three, overloaded `>>` and
`<<`, average and median based final grade, unknown number of homework results
kept in a `std::vector`, reading `Students.txt`, sorted and formatted output.

### v0.2 - Task Nr. 2
* the single file was split into units: `person`, `fileio`, `processing`,
  `generator`, `timer`, `exceptions` - every class and new data type now lives
  in its own header with a matching `*.cpp`;
* error handling through an exception hierarchy
  (`AppError` -> `FileError`, `ParseError`, `InvalidGradeError`,
  `EmptyDataError`): a missing file, a line without grades or a grade outside
  `1..10` throws instead of silently producing wrong output;
* random student files can be generated with 10 000, 100 000, 1 000 000 and
  10 000 000 records, using `<random>` (`std::mt19937`) and the default names
  `Name1 Surname1`, `Name2 Surname2`, ...;
* the students are divided into **failed** (final points < 5.00) and
  **passed** (>= 5.00) and written into two separate files;
* every stage - reading, sorting, splitting, writing - is measured separately
  with `<chrono>`.

The container used in that release was `std::vector` only.

### v0.25 - Task Nr. 2 (this release)
* the whole pipeline - reading, sorting, splitting, writing - became a
  template over the container type, so exactly the same code runs with
  `std::vector<Person>`, `std::list<Person>` and `std::deque<Person>`;
* one detail needed special handling: `std::list` has no random access
  iterators, so `std::sort` cannot be used on it and its own member `sort()`
  is called instead, selected at compile time with `if constexpr`;
* the menu can run one chosen container, or all three one after another on the
  same file, and prints a comparison table of the measured times.

## Build and run

```bash
make          # builds bin/students
./bin/students
```

Menu option **1** generates the test files, option **3** runs one chosen
container, option **4** compares all three on the same file.

## Next release

v1.0 - both splitting strategies selectable from the menu, a second
implementation written with the `<algorithm>` library, `Makefile` and
`CMakeLists.txt`, and the measured results written up in the README.

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

### v0.2 - Task Nr. 2 (this release)
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

The container used in this release is `std::vector` only.

## Build and run

```bash
make          # builds bin/students
./bin/students
```

Menu option **1** generates the test files, option **4** runs the whole
pipeline on one file and prints the time of every stage.

## Next release

v0.25 - the same pipeline turned into a template so that `std::vector`,
`std::list` and `std::deque` can be compared on identical data.

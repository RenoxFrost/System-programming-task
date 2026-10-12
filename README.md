# Student points analyzer

Course work for Systems Programming (PITa-25-NL-EN) - 2026 Autumn.

The application reads student records (first name, surname, homework results
and an exam result), calculates the final points of every student, sorts them
and divides them into two groups: "failed" and "passed".

## Points calculation formula

    final = 0.4 * homework + 0.6 * exam

where homework is either the average or the median of all homework grades,
chosen by the user. Grades are integers in the range 1..10 and the final
result is printed with two decimal places. A student with fewer than 5.00
points has failed the subject.

## Planned releases

* v0.1  - Person class, keyboard and file input, average / median, sorted table
* v0.2  - multi-file project, exceptions, random data files, failed / passed split, timing
* v0.25 - the same pipeline with std::vector, std::list and std::deque
* v1.0  - both splitting strategies, algorithm library based implementation, make + cmake

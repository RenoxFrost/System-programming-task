#ifndef SP_FILEIO_H
#define SP_FILEIO_H

#include "person.h"

#include <string>
#include <vector>

namespace sp {

/// Reads "Name Surname hw1 ... hwN Exam" records from a text file.
/// The number of homework columns does not have to be known in advance,
/// and an optional header line is skipped. Throws FileError / ParseError /
/// EmptyDataError on trouble.
void readStudents(const std::string& path, std::vector<Person>& students);

/// Writes a formatted table of students to a text file.
void writeStudents(const std::string& path, const std::vector<Person>& students,
                   GradeMode mode);

/// Prints the first `maxRows` students as a table on the screen.
void printTable(const std::vector<Person>& students, std::size_t maxRows = 20);

} // namespace sp

#endif // SP_FILEIO_H

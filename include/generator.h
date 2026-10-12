#ifndef SP_GENERATOR_H
#define SP_GENERATOR_H

#include <cstddef>
#include <string>
#include <vector>

namespace sp {

/// Creates a text file with `records` randomly generated students.
/// Names use the "default values" form: Name1 Surname1, Name2 Surname2, ...
/// Grades are drawn from a uniform 1..10 distribution (<random>, mt19937).
/// Returns the time the generation took, in seconds.
double generateStudentFile(const std::string& path,
                           std::size_t records,
                           int homeworkCount = 5);

/// File name used for a given record count, e.g. 10000 -> "students10000.txt".
std::string defaultFileName(std::size_t records);

/// The five record counts the task asks to test with.
const std::vector<std::size_t>& standardRecordCounts();

} // namespace sp

#endif // SP_GENERATOR_H

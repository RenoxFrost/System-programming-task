#ifndef SP_PROCESSING_H
#define SP_PROCESSING_H

#include "person.h"

#include <vector>

namespace sp {

/// Sorts by final points, best first.
void sortByPoints(std::vector<Person>& students, GradeMode mode);

/// Sorts alphabetically by name, then surname.
void sortAlphabetically(std::vector<Person>& students);

/// Divides the students into "failed" (< 5.00 points) and "passed" (>= 5.00).
void splitStudents(const std::vector<Person>& all,
                   std::vector<Person>& failed,
                   std::vector<Person>& passed,
                   GradeMode mode);

} // namespace sp

#endif // SP_PROCESSING_H

#include "processing.h"

#include <algorithm>

namespace sp {

void sortByPoints(std::vector<Person>& students, GradeMode mode) {
    std::sort(students.begin(), students.end(),
              [mode](const Person& a, const Person& b) {
                  const double x = a.finalGrade(mode);
                  const double y = b.finalGrade(mode);
                  if (x != y) return x > y;
                  return bySurname(a, b);
              });
}

void sortAlphabetically(std::vector<Person>& students) {
    std::sort(students.begin(), students.end(), byName);
}

void splitStudents(const std::vector<Person>& all,
                   std::vector<Person>& failed,
                   std::vector<Person>& passed,
                   GradeMode mode) {
    for (const Person& person : all) {
        if (person.passed(mode)) passed.push_back(person);
        else                     failed.push_back(person);
    }
}

} // namespace sp

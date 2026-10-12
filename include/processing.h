#ifndef SP_PROCESSING_H
#define SP_PROCESSING_H

#include "person.h"

#include <algorithm>
#include <deque>
#include <list>
#include <type_traits>
#include <vector>

namespace sp {
namespace detail {

template <typename T>             struct IsList                  : std::false_type {};
template <typename T, typename A> struct IsList<std::list<T, A>> : std::true_type  {};

template <typename C>
inline constexpr bool kIsList = IsList<C>::value;

} // namespace detail

/// Sorts by final points, best first.
/// std::list has no random access iterators, so std::sort cannot be used on it
/// and its own member sort() is called instead.
template <typename Container>
void sortByPoints(Container& students, GradeMode mode) {
    const auto cmp = [mode](const Person& a, const Person& b) {
        const double x = a.finalGrade(mode);
        const double y = b.finalGrade(mode);
        if (x != y) return x > y;
        return bySurname(a, b);
    };
    if constexpr (detail::kIsList<Container>) students.sort(cmp);
    else std::sort(students.begin(), students.end(), cmp);
}

template <typename Container>
void sortAlphabetically(Container& students) {
    if constexpr (detail::kIsList<Container>) students.sort(byName);
    else std::sort(students.begin(), students.end(), byName);
}

/// Divides the students into "failed" (< 5.00) and "passed" (>= 5.00).
/// The base container keeps all of its data; two new containers are filled.
template <typename Container>
void splitStudents(const Container& all, Container& failed, Container& passed,
                   GradeMode mode) {
    for (const Person& person : all) {
        if (person.passed(mode)) passed.push_back(person);
        else                     failed.push_back(person);
    }
}

} // namespace sp

#endif // SP_PROCESSING_H

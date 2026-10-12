#ifndef SP_PROCESSING_H
#define SP_PROCESSING_H

#include "person.h"

#include <algorithm>
#include <deque>
#include <iterator>
#include <list>
#include <type_traits>
#include <vector>

namespace sp {

/// Which of the two splitting strategies from the task is used.
enum class Strategy {
    CopyToTwo,   ///< Strategy 1: base container is kept, two new ones are filled.
    MoveFailed   ///< Strategy 2: failed are taken out, base container shrinks.
};

/// Hand-written loops versus the <algorithm> library.
enum class Impl { Loops, Algorithms };

namespace detail {

template <typename T>            struct IsList                         : std::false_type {};
template <typename T, typename A> struct IsList<std::list<T, A>>        : std::true_type  {};

template <typename C>
inline constexpr bool kIsList = IsList<C>::value;

/// shrink_to_fit() exists for vector and deque but not for list.
template <typename C>
void shrinkIfPossible(C& container) {
    if constexpr (!kIsList<C>) container.shrink_to_fit();
    else (void)container;
}

} // namespace detail

// =============================================================================
//  Sorting
// =============================================================================
/// Sorts by final points (descending). std::list cannot be fed to std::sort
/// because its iterators are not random access, so its own member sort is used.
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

/// Sorts alphabetically, used for the on-screen table.
template <typename Container>
void sortAlphabetically(Container& students) {
    if constexpr (detail::kIsList<Container>) students.sort(byName);
    else std::sort(students.begin(), students.end(), byName);
}

// =============================================================================
//  Strategy 1 - copy into two new containers, keep the original intact
// =============================================================================
template <typename Container>
void splitCopyToTwo(const Container& all, Container& failed, Container& passed,
                    GradeMode mode, Impl impl) {
    const auto isPassed = [mode](const Person& p) { return p.passed(mode); };

    if (impl == Impl::Loops) {
        for (const Person& person : all) {
            if (person.passed(mode)) passed.push_back(person);
            else                     failed.push_back(person);
        }
        return;
    }

    // std::remove_copy_if copies the elements for which the predicate is false,
    // so one call fills each of the two containers.
    std::remove_copy_if(all.begin(), all.end(), std::back_inserter(failed), isPassed);
    std::remove_copy_if(all.begin(), all.end(), std::back_inserter(passed),
                        [&](const Person& p) { return !isPassed(p); });
}

// =============================================================================
//  Strategy 2 - move the failed students out, shrink the base container
// =============================================================================
template <typename Container>
void splitMoveFailed(Container& all, Container& failed, GradeMode mode, Impl impl) {
    const auto isFailed = [mode](const Person& p) { return !p.passed(mode); };

    if (impl == Impl::Loops) {
        // The straightforward way: walk the container and erase as we go.
        // For a vector or a deque every erase shifts the tail, which is exactly
        // the behaviour the task asks us to observe.
        for (auto it = all.begin(); it != all.end();) {
            if (isFailed(*it)) {
                failed.push_back(*it);
                it = all.erase(it);
            } else {
                ++it;
            }
        }
        detail::shrinkIfPossible(all);
        return;
    }

    // One stable_partition pass puts every failed student in front, then the
    // whole block is handed over in a single operation.
    auto middle = std::stable_partition(all.begin(), all.end(), isFailed);

    if constexpr (detail::kIsList<Container>) {
        // Relinking nodes: no element is copied at all.
        failed.splice(failed.end(), all, all.begin(), middle);
    } else {
        std::copy(all.begin(), middle, std::back_inserter(failed));
        all.erase(all.begin(), middle);
        detail::shrinkIfPossible(all);
    }
}

// =============================================================================
//  Small query helpers built on <algorithm>
// =============================================================================
template <typename Container>
std::size_t countFailed(const Container& students, GradeMode mode) {
    return static_cast<std::size_t>(
        std::count_if(students.begin(), students.end(),
                      [mode](const Person& p) { return !p.passed(mode); }));
}

/// Finds the first student whose surname matches, or end() when there is none.
template <typename Container>
auto findBySurname(Container& students, const std::string& surname) {
    return std::find_if(students.begin(), students.end(),
                        [&surname](const Person& p) { return p.surname() == surname; });
}

/// Collects the final points of every student (uses std::transform).
template <typename Container>
std::vector<double> collectPoints(const Container& students, GradeMode mode) {
    std::vector<double> points;
    points.reserve(students.size());
    std::transform(students.begin(), students.end(), std::back_inserter(points),
                   [mode](const Person& p) { return p.finalGrade(mode); });
    return points;
}

} // namespace sp

#endif // SP_PROCESSING_H

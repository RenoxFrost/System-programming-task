#ifndef SP_FILEIO_H
#define SP_FILEIO_H

#include "exceptions.h"
#include "person.h"

#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <string>
#include <vector>

namespace sp {
namespace detail {

inline const char* skipSpaces(const char* p) {
    while (*p == ' ' || *p == '\t' || *p == '\r') ++p;
    return p;
}

/// Reads one whitespace-delimited word starting at p into `out`.
/// Returns the position right after the word, or nullptr if the line ended.
inline const char* readWord(const char* p, std::string& out) {
    p = skipSpaces(p);
    if (*p == '\0') return nullptr;
    const char* begin = p;
    while (*p != '\0' && *p != ' ' && *p != '\t' && *p != '\r') ++p;
    out.assign(begin, static_cast<std::size_t>(p - begin));
    return p;
}

} // namespace detail

// =============================================================================
//  Reading
// =============================================================================
/// Reads "Name Surname hw1 hw2 ... hwN Exam" records into any container that
/// supports push_back (std::vector, std::list, std::deque).
/// An optional header line is detected and skipped. The number of homework
/// columns does not have to be known in advance.
template <typename Container>
void readStudents(const std::string& path, Container& students) {
    std::ifstream in(path);
    if (!in) throw FileError("cannot open " + path + " for reading");

    std::string line;
    std::string name;
    std::string surname;
    std::vector<int> grades;
    std::size_t lineNumber = 0;
    bool anyRecord = false;

    while (std::getline(in, line)) {
        ++lineNumber;
        const char* p = detail::skipSpaces(line.c_str());
        if (*p == '\0') continue;                       // blank line

        p = detail::readWord(p, name);
        if (p == nullptr) continue;
        p = detail::readWord(p, surname);
        if (p == nullptr) {
            if (!anyRecord) continue;                   // header-ish line
            throw ParseError(path + ":" + std::to_string(lineNumber) +
                             " - surname is missing");
        }

        grades.clear();
        while (true) {
            p = detail::skipSpaces(p);
            if (*p == '\0') break;
            char* end = nullptr;
            const long value = std::strtol(p, &end, 10);
            if (end == p) break;                        // not a number
            grades.push_back(static_cast<int>(value));
            p = end;
        }

        if (grades.empty()) {
            if (!anyRecord) continue;                   // this was the header
            throw ParseError(path + ":" + std::to_string(lineNumber) +
                             " - no grades found for " + name + " " + surname);
        }

        const int exam = grades.back();
        grades.pop_back();
        students.push_back(Person(name, surname, grades, exam));
        anyRecord = true;
    }

    if (!anyRecord) throw EmptyDataError(path + " holds no student records");
}

// =============================================================================
//  Writing
// =============================================================================
/// Writes a formatted table of students into a text file.
template <typename Container>
void writeStudents(const std::string& path, const Container& students, GradeMode mode) {
    std::ofstream out(path);
    if (!out) throw FileError("cannot open " + path + " for writing");

    const char* header = (mode == GradeMode::Average) ? "Final (Avg.)" : "Final (Med.)";
    char buf[256];
    std::snprintf(buf, sizeof(buf), "%-16s%-16s%14s\n", "Name", "Surname", header);
    std::string buffer(buf);
    buffer.append(46, '-');
    buffer.push_back('\n');
    buffer.reserve(1 << 20);

    for (const Person& person : students) {
        const int written = std::snprintf(buf, sizeof(buf), "%-16s%-16s%14.2f\n",
                                          person.name().c_str(),
                                          person.surname().c_str(),
                                          person.finalGrade(mode));
        buffer.append(buf, static_cast<std::size_t>(written));
        if (buffer.size() > (1 << 20)) {
            out.write(buffer.data(), static_cast<std::streamsize>(buffer.size()));
            buffer.clear();
        }
    }
    out.write(buffer.data(), static_cast<std::streamsize>(buffer.size()));
    out.flush();
    if (!out) throw FileError("writing " + path + " failed");
}

// =============================================================================
//  Screen output
// =============================================================================
template <typename Container>
void printTable(const Container& students, std::size_t maxRows = 20) {
    if (students.empty()) {
        std::cout << "\n(the container is empty)\n";
        return;
    }
    std::cout << '\n'
              << std::left  << std::setw(14) << "Name"
              << std::setw(16) << "Surname"
              << std::right << std::setw(12) << "Final (Avg.)" << " | "
              << std::setw(12) << "Final (Med.)" << '\n'
              << std::string(58, '-') << '\n';

    std::size_t shown = 0;
    for (const Person& person : students) {
        if (shown++ == maxRows) break;
        std::cout << person << '\n';
    }
    std::cout << std::string(58, '-') << '\n';
    if (students.size() > shown)
        std::cout << "... " << students.size() - shown << " more row(s) not shown\n";
    std::cout << students.size() << " student(s).\n";
}

} // namespace sp

#endif // SP_FILEIO_H

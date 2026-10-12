#include "person.h"

#include "exceptions.h"

#include <algorithm>
#include <iomanip>
#include <limits>
#include <numeric>

namespace sp {

Person::Person()
    : name_("-"), surname_("-"), exam_(0), finalAverage_(0.0), finalMedian_(0.0) {}

Person::Person(std::string name, std::string surname, std::vector<int> homework, int exam)
    : name_(std::move(name)), surname_(std::move(surname)),
      homework_(std::move(homework)), exam_(exam),
      finalAverage_(0.0), finalMedian_(0.0) {
    validateGrade(exam_);
    for (int grade : homework_) validateGrade(grade);
    recalculate();
}

// --- rule of three -----------------------------------------------------------
// Person owns no raw memory, so the compiler-generated versions would behave
// identically. They are written out because the task requires all three.

Person::Person(const Person& other)
    : name_(other.name_), surname_(other.surname_), homework_(other.homework_),
      exam_(other.exam_), finalAverage_(other.finalAverage_),
      finalMedian_(other.finalMedian_) {}

Person& Person::operator=(const Person& other) {
    if (this != &other) {
        name_         = other.name_;
        surname_      = other.surname_;
        homework_     = other.homework_;
        exam_         = other.exam_;
        finalAverage_ = other.finalAverage_;
        finalMedian_  = other.finalMedian_;
    }
    return *this;
}

Person::~Person() = default;

// --- modifiers ---------------------------------------------------------------

void Person::setExam(int grade) {
    validateGrade(grade);
    exam_ = grade;
    recalculate();
}

void Person::addHomework(int grade) {
    validateGrade(grade);
    homework_.push_back(grade);
    recalculate();
}

void Person::clearHomework() {
    homework_.clear();
    recalculate();
}

// --- calculations ------------------------------------------------------------

double Person::homeworkAverage() const {
    if (homework_.empty()) return 0.0;
    const double sum = std::accumulate(homework_.begin(), homework_.end(), 0.0);
    return sum / static_cast<double>(homework_.size());
}

double Person::homeworkMedian() const {
    if (homework_.empty()) return 0.0;
    std::vector<int> sorted(homework_);
    std::sort(sorted.begin(), sorted.end());
    const std::size_t n = sorted.size();
    if (n % 2 == 1) return static_cast<double>(sorted[n / 2]);
    return (sorted[n / 2 - 1] + sorted[n / 2]) / 2.0;
}

void Person::recalculate() {
    finalAverage_ = kHomeworkWeight * homeworkAverage() + kExamWeight * exam_;
    finalMedian_  = kHomeworkWeight * homeworkMedian()  + kExamWeight * exam_;
}

void Person::validateGrade(int grade) {
    if (grade < 1 || grade > 10)
        throw InvalidGradeError("grade " + std::to_string(grade) +
                                " is outside the allowed range 1..10");
}

// --- stream operators --------------------------------------------------------

/// Interactive input. The user decides when the homework list is finished by
/// entering 0, so the number of homework results does not have to be known.
std::istream& operator>>(std::istream& is, Person& person) {
    std::cout << "  First name : ";
    if (!(is >> person.name_)) throw ParseError("could not read the first name");

    std::cout << "  Surname    : ";
    if (!(is >> person.surname_)) throw ParseError("could not read the surname");

    person.homework_.clear();
    std::cout << "  Homework grades (1-10), enter 0 when finished:\n";
    while (true) {
        std::cout << "    hw[" << person.homework_.size() + 1 << "] = ";
        int grade = 0;
        if (!(is >> grade)) {
            is.clear();
            is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "    not a number, try again\n";
            continue;
        }
        if (grade == 0) break;
        try {
            Person::validateGrade(grade);
        } catch (const InvalidGradeError& e) {
            std::cout << "    " << e.what() << '\n';
            continue;
        }
        person.homework_.push_back(grade);
    }

    while (true) {
        std::cout << "  Exam grade (1-10): ";
        int grade = 0;
        if (!(is >> grade)) {
            is.clear();
            is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "    not a number, try again\n";
            continue;
        }
        try {
            Person::validateGrade(grade);
        } catch (const InvalidGradeError& e) {
            std::cout << "    " << e.what() << '\n';
            continue;
        }
        person.exam_ = grade;
        break;
    }

    person.recalculate();
    return is;
}

/// One formatted table row: name, surname, both final grades.
std::ostream& operator<<(std::ostream& os, const Person& person) {
    os << std::left  << std::setw(14) << person.name_
       << std::setw(16) << person.surname_
       << std::right << std::fixed << std::setprecision(2)
       << std::setw(12) << person.finalAverage_ << " | "
       << std::setw(12) << person.finalMedian_;
    return os;
}

// --- ordering helpers --------------------------------------------------------

bool byName(const Person& a, const Person& b) {
    if (a.name() != b.name()) return a.name() < b.name();
    return a.surname() < b.surname();
}

bool bySurname(const Person& a, const Person& b) {
    if (a.surname() != b.surname()) return a.surname() < b.surname();
    return a.name() < b.name();
}

} // namespace sp

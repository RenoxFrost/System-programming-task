// =============================================================================
//  Systems Programming - Task Nr. 1  (release v0.1)
//  Student points analyzer, single-file version.
//
//  Final points formula:   final = 0.4 * homework + 0.6 * exam
//  where "homework" is either the average or the median of the homework grades.
//
//  Build:  g++ -std=c++17 -O2 -Wall -Wextra -o students main.cpp
// =============================================================================

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <numeric>
#include <random>
#include <sstream>
#include <string>
#include <vector>

// -----------------------------------------------------------------------------
// How the final grade is produced from the homework results.
// -----------------------------------------------------------------------------
enum class GradeMode { Average, Median };

// =============================================================================
//  class Person
//  Holds one student: identity, homework results, exam result, final grade.
// =============================================================================
class Person {
public:
    // --- constructors ------------------------------------------------------
    Person() : name_("-"), surname_("-"), exam_(0), finalAvg_(0.0), finalMed_(0.0) {}

    Person(std::string name, std::string surname, std::vector<int> homework, int exam)
        : name_(std::move(name)), surname_(std::move(surname)),
          homework_(std::move(homework)), exam_(exam), finalAvg_(0.0), finalMed_(0.0) {
        recalculate();
    }

    // --- rule of three -----------------------------------------------------
    // The compiler-generated versions would be correct here (no raw pointers),
    // but the task explicitly asks for all three to be written out.
    Person(const Person& other)                      // copy constructor
        : name_(other.name_), surname_(other.surname_), homework_(other.homework_),
          exam_(other.exam_), finalAvg_(other.finalAvg_), finalMed_(other.finalMed_) {}

    Person& operator=(const Person& other) {         // copy-assignment operator
        if (this != &other) {
            name_      = other.name_;
            surname_   = other.surname_;
            homework_  = other.homework_;
            exam_      = other.exam_;
            finalAvg_  = other.finalAvg_;
            finalMed_  = other.finalMed_;
        }
        return *this;
    }

    ~Person() = default;                             // destructor

    // --- accessors ---------------------------------------------------------
    const std::string&      name()     const { return name_; }
    const std::string&      surname()  const { return surname_; }
    const std::vector<int>& homework() const { return homework_; }
    int                     exam()     const { return exam_; }

    void setName(const std::string& n)    { name_ = n; }
    void setSurname(const std::string& s) { surname_ = s; }
    void setExam(int e)                   { exam_ = e; recalculate(); }
    void addHomework(int g)               { homework_.push_back(g); recalculate(); }
    void clearHomework()                  { homework_.clear(); recalculate(); }

    // --- calculations ------------------------------------------------------
    double homeworkAverage() const {
        if (homework_.empty()) return 0.0;
        return std::accumulate(homework_.begin(), homework_.end(), 0.0) /
               static_cast<double>(homework_.size());
    }

    double homeworkMedian() const {
        if (homework_.empty()) return 0.0;
        std::vector<int> tmp(homework_);
        std::sort(tmp.begin(), tmp.end());
        const std::size_t n = tmp.size();
        return (n % 2 == 1) ? static_cast<double>(tmp[n / 2])
                            : (tmp[n / 2 - 1] + tmp[n / 2]) / 2.0;
    }

    // Final grade is stored in the object and refreshed by recalculate().
    double finalGrade(GradeMode mode) const {
        return (mode == GradeMode::Average) ? finalAvg_ : finalMed_;
    }

    void recalculate() {
        finalAvg_ = 0.4 * homeworkAverage() + 0.6 * exam_;
        finalMed_ = 0.4 * homeworkMedian()  + 0.6 * exam_;
    }

    // --- stream operators --------------------------------------------------
    friend std::istream& operator>>(std::istream& is, Person& p);
    friend std::ostream& operator<<(std::ostream& os, const Person& p);

private:
    std::string      name_;
    std::string      surname_;
    std::vector<int> homework_;
    int              exam_;
    double           finalAvg_;   // final grade, homework average based
    double           finalMed_;   // final grade, homework median based
};

// -----------------------------------------------------------------------------
// Data input method (overloaded >>). Interactive: the user decides when the
// homework results are finished by entering 0.
// -----------------------------------------------------------------------------
std::istream& operator>>(std::istream& is, Person& p) {
    std::cout << "  First name : ";
    is >> p.name_;
    std::cout << "  Surname    : ";
    is >> p.surname_;

    p.homework_.clear();
    std::cout << "  Homework grades (1-10), enter 0 when finished:\n";
    int grade = 0;
    while (true) {
        std::cout << "    hw[" << p.homework_.size() + 1 << "] = ";
        if (!(is >> grade)) {
            is.clear();
            is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "    (not a number, try again)\n";
            continue;
        }
        if (grade == 0) break;
        if (grade < 1 || grade > 10) {
            std::cout << "    (grade must be 1..10)\n";
            continue;
        }
        p.homework_.push_back(grade);
    }

    while (true) {
        std::cout << "  Exam grade (1-10): ";
        if (is >> p.exam_ && p.exam_ >= 1 && p.exam_ <= 10) break;
        is.clear();
        is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "    (grade must be 1..10)\n";
    }

    p.recalculate();
    return is;
}

// -----------------------------------------------------------------------------
// Data output method (overloaded <<). One formatted table row.
// -----------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& os, const Person& p) {
    os << std::left  << std::setw(12) << p.name_
       << std::setw(14) << p.surname_
       << std::right << std::fixed << std::setprecision(2)
       << std::setw(12) << p.finalAvg_ << " | "
       << std::setw(12) << p.finalMed_;
    return os;
}

// =============================================================================
//  Helpers
// =============================================================================
void printTable(const std::vector<Person>& students) {
    if (students.empty()) {
        std::cout << "\nNo student data loaded.\n";
        return;
    }
    std::cout << '\n'
              << std::left  << std::setw(12) << "Name"
              << std::setw(14) << "Surname"
              << std::right << std::setw(12) << "Final (Avg.)" << " | "
              << std::setw(12) << "Final (Med.)" << '\n'
              << std::string(54, '-') << '\n';
    for (const Person& p : students) std::cout << p << '\n';
    std::cout << std::string(54, '-') << '\n'
              << students.size() << " student(s).\n";
}

void printTableSingleMode(const std::vector<Person>& students, GradeMode mode) {
    const char* label = (mode == GradeMode::Average) ? "Final_Point(Aver.)"
                                                     : "Final_Point(Med.)";
    std::cout << '\n'
              << std::left  << std::setw(12) << "Name"
              << std::setw(14) << "Surname"
              << std::right << std::setw(20) << label << '\n'
              << std::string(46, '-') << '\n';
    for (const Person& p : students)
        std::cout << std::left  << std::setw(12) << p.name()
                  << std::setw(14) << p.surname()
                  << std::right << std::fixed << std::setprecision(2)
                  << std::setw(20) << p.finalGrade(mode) << '\n';
    std::cout << std::string(46, '-') << '\n';
}

void sortByName(std::vector<Person>& students) {
    std::sort(students.begin(), students.end(), [](const Person& a, const Person& b) {
        if (a.name() != b.name()) return a.name() < b.name();
        return a.surname() < b.surname();
    });
}

// Reads "Name Surname hw1 hw2 ... hwN Exam" records; an optional header line
// (the one that does not start with two words followed by numbers) is skipped.
std::vector<Person> readFromFile(const std::string& path) {
    std::ifstream in(path);
    if (!in) {
        std::cout << "Could not open file: " << path << '\n';
        return {};
    }

    std::vector<Person> students;
    std::string line;
    bool first = true;

    while (std::getline(in, line)) {
        if (line.find_first_not_of(" \t\r\n") == std::string::npos) continue;

        std::istringstream ss(line);
        std::string name, surname;
        if (!(ss >> name >> surname)) continue;

        std::vector<int> numbers;
        int value = 0;
        while (ss >> value) numbers.push_back(value);

        if (numbers.empty()) {          // header line such as "Name Surname HW1 ..."
            if (first) { first = false; continue; }
            continue;
        }
        first = false;

        const int exam = numbers.back();
        numbers.pop_back();
        students.emplace_back(name, surname, numbers, exam);
    }
    return students;
}

// Small random generator, handy for testing without typing data by hand.
std::vector<Person> generateRandom(std::size_t count, int homeworkCount) {
    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> grade(1, 10);

    std::vector<Person> students;
    students.reserve(count);
    for (std::size_t i = 1; i <= count; ++i) {
        std::vector<int> hw(homeworkCount);
        for (int& g : hw) g = grade(rng);
        students.emplace_back("Name" + std::to_string(i),
                              "Surname" + std::to_string(i), hw, grade(rng));
    }
    return students;
}

int readMenuChoice() {
    int choice = -1;
    if (!(std::cin >> choice)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return -1;
    }
    return choice;
}

// =============================================================================
//  main
// =============================================================================
int main() {
    std::vector<Person> students;

    while (true) {
        std::cout << "\n==== Student points analyzer  v0.1 ====\n"
                  << "1. Enter students from the keyboard\n"
                  << "2. Read students from a file\n"
                  << "3. Generate random students\n"
                  << "4. Show table (average and median)\n"
                  << "5. Show table with one chosen method\n"
                  << "0. Exit\n"
                  << "Your choice: ";

        switch (readMenuChoice()) {
        case 1: {
            std::cout << "How many students? ";
            int n = 0;
            if (!(std::cin >> n) || n <= 0) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "Invalid number.\n";
                break;
            }
            students.clear();
            for (int i = 0; i < n; ++i) {
                std::cout << "\nStudent " << i + 1 << ":\n";
                Person p;
                std::cin >> p;
                students.push_back(p);
            }
            sortByName(students);
            printTable(students);
            break;
        }
        case 2: {
            std::cout << "File name [Students.txt]: ";
            std::string path;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::getline(std::cin, path);
            if (path.empty()) path = "Students.txt";
            students = readFromFile(path);
            sortByName(students);
            printTable(students);
            break;
        }
        case 3: {
            std::cout << "How many students? ";
            int n = 0;
            std::cin >> n;
            std::cout << "How many homework results each? ";
            int hw = 0;
            std::cin >> hw;
            if (n <= 0 || hw < 0) { std::cout << "Invalid input.\n"; break; }
            students = generateRandom(static_cast<std::size_t>(n), hw);
            sortByName(students);
            printTable(students);
            break;
        }
        case 4:
            sortByName(students);
            printTable(students);
            break;
        case 5: {
            std::cout << "1 - average, 2 - median: ";
            int m = 0;
            std::cin >> m;
            sortByName(students);
            printTableSingleMode(students,
                                 (m == 2) ? GradeMode::Median : GradeMode::Average);
            break;
        }
        case 0:
            std::cout << "Bye.\n";
            return 0;
        default:
            std::cout << "Unknown choice.\n";
            break;
        }
    }
}

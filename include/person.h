#ifndef SP_PERSON_H
#define SP_PERSON_H

#include <iostream>
#include <string>
#include <vector>

namespace sp {

/// How the homework part of the final grade is produced.
enum class GradeMode { Average, Median };

/// Points below this value mean the student failed the subject.
inline constexpr double kPassThreshold = 5.0;

/// Weight of the homework part in the final grade.
inline constexpr double kHomeworkWeight = 0.4;

/// Weight of the exam part in the final grade.
inline constexpr double kExamWeight = 0.6;

// =============================================================================
//  class Person
//  One student record: identity, homework results, exam result, final grade.
//  final = 0.4 * homework(average|median) + 0.6 * exam
// =============================================================================
class Person {
public:
    Person();
    Person(std::string name, std::string surname, std::vector<int> homework, int exam);

    // --- rule of three -------------------------------------------------------
    Person(const Person& other);              ///< copy constructor
    Person& operator=(const Person& other);   ///< copy-assignment operator
    ~Person();                                ///< destructor

    // --- accessors -----------------------------------------------------------
    const std::string&      name()     const noexcept { return name_; }
    const std::string&      surname()  const noexcept { return surname_; }
    const std::vector<int>& homework() const noexcept { return homework_; }
    int                     exam()     const noexcept { return exam_; }

    void setName(std::string name)       { name_ = std::move(name); }
    void setSurname(std::string surname) { surname_ = std::move(surname); }
    void setExam(int grade);
    void addHomework(int grade);
    void clearHomework();

    // --- calculations --------------------------------------------------------
    double homeworkAverage() const;
    double homeworkMedian() const;

    /// Final grade, taken from the value stored inside the object.
    double finalGrade(GradeMode mode) const noexcept {
        return (mode == GradeMode::Average) ? finalAverage_ : finalMedian_;
    }

    bool passed(GradeMode mode) const noexcept {
        return finalGrade(mode) >= kPassThreshold;
    }

    /// Refreshes both stored final grades. Called after every data change.
    void recalculate();

    // --- stream operators ----------------------------------------------------
    friend std::istream& operator>>(std::istream& is, Person& person);
    friend std::ostream& operator<<(std::ostream& os, const Person& person);

private:
    static void validateGrade(int grade);

    std::string      name_;
    std::string      surname_;
    std::vector<int> homework_;
    int              exam_;
    double           finalAverage_;   ///< final grade, homework average based
    double           finalMedian_;    ///< final grade, homework median based
};

/// Ordering helpers used by the sorting stage.
bool byName(const Person& a, const Person& b);
bool bySurname(const Person& a, const Person& b);

} // namespace sp

#endif // SP_PERSON_H

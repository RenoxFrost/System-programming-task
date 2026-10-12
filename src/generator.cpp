#include "generator.h"

#include "exceptions.h"
#include "timer.h"

#include <cstdio>
#include <fstream>
#include <random>

namespace sp {

std::string defaultFileName(std::size_t records) {
    return "students" + std::to_string(records) + ".txt";
}

const std::vector<std::size_t>& standardRecordCounts() {
    static const std::vector<std::size_t> counts{1000, 10000, 100000, 1000000, 10000000};
    return counts;
}

double generateStudentFile(const std::string& path, std::size_t records, int homeworkCount) {
    if (records == 0) throw EmptyDataError("asked to generate a file with 0 records");
    if (homeworkCount < 0) throw InvalidGradeError("homework count cannot be negative");

    std::ofstream out(path);
    if (!out) throw FileError("cannot create " + path);

    Timer timer;

    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> grade(1, 10);

    // Header line, same shape as the Students.txt example in the task.
    out << "Name            Surname     ";
    for (int i = 1; i <= homeworkCount; ++i) {
        char head[16];
        std::snprintf(head, sizeof(head), "%5s", ("HW" + std::to_string(i)).c_str());
        out << head;
    }
    out << "  Exam\n";

    // Building the lines in a string buffer is far quicker than streaming
    // every field separately through the formatted output operators.
    std::string buffer;
    buffer.reserve(1 << 20);
    char line[512];

    for (std::size_t i = 1; i <= records; ++i) {
        int written = std::snprintf(line, sizeof(line), "%-16s%-12s",
                                    ("Name" + std::to_string(i)).c_str(),
                                    ("Surname" + std::to_string(i)).c_str());
        for (int h = 0; h < homeworkCount; ++h)
            written += std::snprintf(line + written, sizeof(line) - written,
                                     "%5d", grade(rng));
        written += std::snprintf(line + written, sizeof(line) - written,
                                 "%6d\n", grade(rng));
        buffer.append(line, static_cast<std::size_t>(written));

        if (buffer.size() > (1 << 20)) {
            out.write(buffer.data(), static_cast<std::streamsize>(buffer.size()));
            buffer.clear();
        }
    }
    out.write(buffer.data(), static_cast<std::streamsize>(buffer.size()));
    out.flush();

    if (!out) throw FileError("writing " + path + " failed");
    return timer.seconds();
}

} // namespace sp

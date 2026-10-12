#ifndef SP_EXCEPTIONS_H
#define SP_EXCEPTIONS_H

#include <stdexcept>
#include <string>

namespace sp {

/// Base class for every error this application throws on purpose.
class AppError : public std::runtime_error {
public:
    explicit AppError(const std::string& what) : std::runtime_error(what) {}
};

/// A file could not be opened, read or written.
class FileError : public AppError {
public:
    explicit FileError(const std::string& what) : AppError("File error: " + what) {}
};

/// A line of input did not have the expected shape.
class ParseError : public AppError {
public:
    explicit ParseError(const std::string& what) : AppError("Parse error: " + what) {}
};

/// A grade outside the allowed 1..10 range was supplied.
class InvalidGradeError : public AppError {
public:
    explicit InvalidGradeError(const std::string& what)
        : AppError("Invalid grade: " + what) {}
};

/// An operation was asked for, but there is nothing to work on.
class EmptyDataError : public AppError {
public:
    explicit EmptyDataError(const std::string& what)
        : AppError("No data: " + what) {}
};

} // namespace sp

#endif // SP_EXCEPTIONS_H

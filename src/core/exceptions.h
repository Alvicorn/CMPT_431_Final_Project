#include <stdexcept>


class IndexOutOfBoundsException : public std::exception {

private:
    std::string message;

public:
    IndexOutOfBoundsException(const std::string& msg) : message(msg) {}

    const char* what() const noexcept override {
        return message.c_str();
    }
};
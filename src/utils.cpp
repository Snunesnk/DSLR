#include "utils.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cctype>
#include <iomanip>
#include <limits>
#include <stdexcept>

// Function to check if a string represents a number, allowing for negative numbers and decimal points.
bool Utils::isNumber(const std::string& str) {
    bool hasDigit = false;
    bool hasDot = false;

    for (size_t i = 0; i < str.size(); ++i) {
        char c = str[i];

        if (i == 0 && c == '-') {
            continue;
        }

        if (std::isdigit(c)) {
            hasDigit = true;
        }
        else if (c == '.' && i > 0 && i < str.size() - 1 && std::isdigit(str[i - 1]) && std::isdigit(str[i + 1])) {
            hasDot = true;
        }
        else {
            return false;
        }
    }

    return hasDigit || hasDot;
}

// Function to load headers from the first line of a file.
void loadHeader(std::ifstream& file, std::vector<std::string>& headers) {
    std::string header;
    std::getline(file, header);

    std::istringstream headerStream(header);
    std::string headerElement;

    // Parsing comma-separated header elements and storing them in the vector.
    while (std::getline(headerStream, headerElement, ',')) {
        headers.push_back(headerElement);
    }
}

// Function to determine the starting index of features in each data line.
void determineFeaturesStartIndex(std::ifstream& file, size_t headersCount, size_t& featuresStartIndex) {
    std::vector<std::string> tempElementsLine(headersCount, "");

    size_t linesCount = 0;
    std::string line;

    // Iterating through lines to find the first non-empty numeric element in each column.
    while (std::getline(file, line)) {
        if (linesCount++ > 10000) {
            throw std::runtime_error("Error: Wrong file size");
        }

        std::istringstream lineStream(line);

        size_t currentIndex = 0;
        std::string element;

        while (std::getline(lineStream, element, ',')) {
            if (tempElementsLine[currentIndex].empty() && Utils::isNumber(element)) {
                tempElementsLine[currentIndex] = element;
            }

            if (currentIndex++ > 100) {
                throw std::runtime_error("Error: Wrong header");
            }
        }
    }

    // Setting the featuresStartIndex based on the first non-empty numeric element in each column.
    featuresStartIndex = 1;
    for (size_t i = 1; i < headersCount; ++i) {
        if (tempElementsLine[i].empty()) {
            featuresStartIndex++;
        }
    }
}



// Function to load data lines from the file and construct StudentInfo objects.
void loadDataLines(std::ifstream& file, std::vector<StudentInfo>& students, size_t headersCount, size_t featuresStartIndex) {
    std::string line;
    std::getline(file, line);

    // Iterating through lines to parse and validate index, labels, and features.
    while (std::getline(file, line)) {
        StudentInfo student;

        std::istringstream lineStream(line);
        std::string element;

        std::getline(lineStream, element, ',');
        if (!Utils::isNumber(element)) {
            throw std::runtime_error("Error: Wrong index in file : " + element + " at index " + std::to_string(student.index));
        }
        student.index = static_cast<size_t>(std::stoi(element));

        // Parsing labels for the student.
        for (size_t i = 1; i < featuresStartIndex; ++i) {
            std::getline(lineStream, element, ',');
            if (element.empty() || !Utils::isNumber(element)) {
                student.labels.push_back(element);
            }
            else {
                throw std::runtime_error("Error: Wrong label in file : " + element + " at index " + std::to_string(student.index));
            }
        }

        // Parsing features for the student.
        for (size_t i = featuresStartIndex; i < headersCount; ++i) {
            std::getline(lineStream, element, ',');
            if (element.empty()) {
                student.features.push_back(std::numeric_limits<double>::quiet_NaN());
            }
            else if (Utils::isNumber(element)) {
                student.features.push_back(std::stod(element));
            }
            else {
                throw std::runtime_error("Error: Wrong feature in file : " + element + " at index " + std::to_string(student.index));
            }
        }

        students.push_back(student);
    }
}

// Function to load data from a file, including headers, features start index, and student information.
void Utils::loadDataFile(const std::string& filename, std::vector<StudentInfo>& students, std::vector<std::string>& headers, size_t& featuresStartIndex) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Error: Opening file.");
    }

    // Loading headers from the file.
    loadHeader(file, headers);

    // Determining the starting index of features in each data line.
    determineFeaturesStartIndex(file, headers.size(), featuresStartIndex);

    // Resetting the file position to the beginning.
    file.clear();
    file.seekg(0);

    // Loading data lines and constructing StudentInfo objects.
    loadDataLines(file, students, headers.size(), featuresStartIndex);

    file.close();
}

// Function to execute a system command and print an error message if the execution fails.
void Utils::executeCommand(const std::string& command) {
    if (std::system(command.c_str()) != 0) {
        std::cerr << "Error: Executing command" << std::endl;
    }
}

// Function to print feature headers with a specified maximum width.
void Utils::printFeatureHeader(const size_t max) {
    const int fieldWidth = 14;

    std::cout << std::setw(fieldWidth) << std::left << "";

    // Printing feature headers with the specified field width.
    for (size_t i = 1; i <= max; i++) {
        std::cout << std::setw(fieldWidth) << std::right << "Feature " + std::to_string(i);
    }

    std::cout << std::endl;
}

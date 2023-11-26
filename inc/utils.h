#ifndef UTILS_H
#define UTILS_H

#pragma warning(push, 0)
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <limits>
#include <vector>
#include <functional>
#pragma warning(pop)


struct StudentInfo {
    std::vector<std::string> labels;
    std::vector<double> features;
    size_t index;
};

class Utils {
public:
    // Check if a string represents a number.
    static bool isNumber(const std::string& str);

    // Load data from a file, including headers, features start index, and student information.
    static void loadDataFile(const std::string& filename, std::vector<StudentInfo>& students, std::vector<std::string>& headers, size_t& featuresStartIndex);

    // Execute a system command and print an error message if the execution fails.
    static void executeCommand(const std::string& command);

    // Print feature headers with a specified maximum width.
    static void printFeatureHeader(const size_t max);

    // Display section name and computed features with fixed precision.
    template <typename Function>
    static void computeAndPrintFeatures(const std::string& sectionName, Function function, std::vector<std::vector<double>> featuresValues) {
        const int fieldWidth = 14; // Output field width.

        // Print section name and computed features.
        std::cout << std::setw(fieldWidth) << std::left << sectionName;
        for (const auto& value : featuresValues) {
            std::cout << std::setw(fieldWidth) << std::right << std::fixed << std::setprecision(6) << function(value);
        }
        std::cout << std::endl;
    }
};

#endif // UTILS_H

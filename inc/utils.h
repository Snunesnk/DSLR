#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <limits>
#include <vector>
#include <functional>

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
    static std::pair<std::vector<std::string>, size_t> LoadDataFile(const std::string& filename, std::vector<StudentInfo>& students);

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

    static void NormalizeData(std::vector<StudentInfo>& data, std::vector<double>& featureMeans, std::vector<double>& featureStdDevs);

    static void SaveWeightsAndNormalizationParameters(const std::vector<std::vector<double>>& weights,
        const std::vector<double>& featureMeans,
        const std::vector<double>& featureStdDevs,
        const std::string& filename);

    static void LoadWeightsAndNormalizationParameters(std::vector<std::vector<double>>& weights,
        std::vector<double>& featureMeans,
        std::vector<double>& featureStdDevs,
        const std::string& filename);
};

#endif // UTILS_H

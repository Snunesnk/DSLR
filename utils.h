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
    int index;
    std::string house;
    std::string firstName;
    std::string lastName;
    std::string birthday;
    std::string bestHand;
    std::vector<double> features;
};

class Utils {
public:
    static std::vector<StudentInfo> readCSV(const std::string& filename);
    static void printFeatureHeader(const size_t maxFeatures);

    template <typename Function>
    static void computeAndPrintFeatures(const std::string& sectionName, Function function, std::vector<std::vector<double>> featuresValues) {
        const int fieldWidth = 14;

        std::cout << std::setw(fieldWidth) << std::left << sectionName;

        for (const auto& value : featuresValues) {
            std::cout << std::setw(fieldWidth) << std::right << std::fixed << std::setprecision(6) << function(value);
        }

        std::cout << std::endl;
    }

    static void executeCommand(const std::string& command);
};

#endif // UTILS_H

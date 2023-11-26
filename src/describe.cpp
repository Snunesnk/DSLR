#include "utils.h"
#include "calculate.h"

int main(int argc, char* argv[])
{
    try {

#ifndef _MSC_VER

        if (argc != 2)
        {
            std::cerr << "Usage: " << argv[0] << " <dataset>.csv" << std::endl;
            return 1;
        }
        std::vector<StudentInfo> studentData;
        std::vector<std::string> headers;
        size_t featuresStartIndex;
        Utils::loadDataFile(argv[1], studentData, headers, featuresStartIndex);
        size_t featuresCount = headers.size() - featuresStartIndex;

#else

        std::vector<StudentInfo> studentData;
        std::vector<std::string> headers;
        size_t featuresStartIndex;
        Utils::loadDataFile("dataset_train.csv", studentData, headers, featuresStartIndex);
        size_t featuresCount = headers.size() - featuresStartIndex;

#endif // MVS

        std::vector<std::vector<double>> featuresValues(featuresCount);
        for (size_t i = 0; i < featuresCount; i++)
        {
            for (const auto& student : studentData)
            {
                featuresValues[i].push_back(student.features[i]);
            }
        }

        Utils::printFeatureHeader(featuresValues.size());
        Utils::computeAndPrintFeatures( "Count", [](const auto& data) { return static_cast<double>(data.size()); }, featuresValues);
        Utils::computeAndPrintFeatures("Mean", Calculate::Mean, featuresValues);
        Utils::computeAndPrintFeatures("Std", Calculate::StandardDeviation, featuresValues);
        Utils::computeAndPrintFeatures("Min", Calculate::Min, featuresValues);
        Utils::computeAndPrintFeatures("25%", std::bind(Calculate::Percentile, std::placeholders::_1, 25), featuresValues);
        Utils::computeAndPrintFeatures("50%", std::bind(Calculate::Percentile, std::placeholders::_1, 50), featuresValues);
        Utils::computeAndPrintFeatures("75%", std::bind(Calculate::Percentile, std::placeholders::_1, 75), featuresValues);
        Utils::computeAndPrintFeatures("Max", Calculate::Max, featuresValues);
    }
    catch (const std::exception& e) {
        // Gérer les exceptions et afficher les messages d'erreur
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}
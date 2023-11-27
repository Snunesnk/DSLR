#include "utils.h"
#include "calculate.h"

int main(int argc, char* argv[])
{
    try {
        std::vector<StudentInfo> students;
#ifndef _MSC_VER

        if (argc != 2)
        {
            std::cerr << "Usage: " << argv[0] << " <dataset>.csv" << std::endl;
            return 1;
        }
        Utils::LoadDataFile(argv[1], students);
#else       
        Utils::LoadDataFile("dataset_train.csv", students);
#endif // MVS
        size_t featuresCount = students[0].features.size();
        std::vector<std::vector<double>> featuresValues(featuresCount);
        for (size_t i = 0; i < featuresCount; i++)
        {
            for (const auto& student : students)
            {
                featuresValues[i].push_back(student.features[i]);
            }
        }

        Utils::printFeatureHeader(featuresValues.size());
        Utils::computeAndPrintFeatures("Count", [](const auto& data) { return static_cast<double>(data.size()); }, featuresValues);
        Utils::computeAndPrintFeatures("Mean", Calculate::Mean, featuresValues);
        Utils::computeAndPrintFeatures("Std", Calculate::StandardDeviation, featuresValues);
        Utils::computeAndPrintFeatures("Min", Calculate::Min, featuresValues);
        Utils::computeAndPrintFeatures("25%", std::bind(Calculate::Quartile, std::placeholders::_1, 25), featuresValues);
        Utils::computeAndPrintFeatures("50%", std::bind(Calculate::Quartile, std::placeholders::_1, 50), featuresValues);
        Utils::computeAndPrintFeatures("75%", std::bind(Calculate::Quartile, std::placeholders::_1, 75), featuresValues);
        Utils::computeAndPrintFeatures("Max", Calculate::Max, featuresValues);
    }
    catch (const std::exception& e) {
        // Gérer les exceptions et afficher les messages d'erreur
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}
#include <iostream>
#include <vector>
#include <cmath>
#include <numeric>
#include <fstream>
#include <unordered_map>
#include "utils.h"
#include "calculate.h"

// Function to handle missing values by replacing NaN with the mean
void handleMissingValues(std::vector<StudentInfo>& students)
{
    // Collect feature values
    std::vector<std::vector<double>> featureValues(13, std::vector<double>(students.size()));
    for (size_t i = 0; i < 13; ++i) {
        for (size_t j = 0; j < students.size(); ++j) {
            featureValues[i][j] = students[j].features[i];
        }
    }

    for (auto& student : students)
    {
        for (size_t j = 0; j < student.features.size(); ++j)
        {
            if (std::isnan(student.features[j]))
            {
                student.features[j] = Calculate::Mean(featureValues[j]);
            }
        }
    }
}

// Function to create input vectors
void createInputVectors(const std::vector<StudentInfo>& students,
    const std::vector<size_t>& featuresSelected,
    std::vector<std::vector<double>>& inputs)
{
    for (const auto& student : students) {
        std::vector<double> selection;
        for (auto feature : featuresSelected) {
            selection.push_back(student.features[feature - 1]);
        }
        inputs.push_back(selection);
    }
}

// Function to perform predictions and write results to a CSV file
void performPredictions(const std::vector<StudentInfo>& students,
    const std::vector<std::vector<double>>& weights,
    const std::vector<std::string>& headers,
    const std::vector<std::vector<double>>& inputs)
{
    std::ofstream outputFile("houses.csv");
    if (!outputFile.is_open()) {
        throw std::runtime_error("Impossible d'ouvrir le fichier de sortie.");
    }

    // Mapping of house indices
    std::unordered_map<size_t, std::string> housesIndex = {
        {0, "Slytherin"},
        {1, "Ravenclaw"},
        {2, "Gryffindor"},
        {3, "Hufflepuff"},
    };

    outputFile << headers[0] << "," << headers[1] <<std::endl;

    for (size_t i = 0; i < students.size(); ++i) {
        double maxProbability = 0;
        size_t predictedHouse = 0;

        for (size_t house = 0; house < weights.size(); ++house) {
            double probability = Calculate::LogisticRegressionHypothesis(weights[house], inputs[i]);
            if (probability > maxProbability) {
                maxProbability = probability;
                predictedHouse = house;
            }
        }
        outputFile << students[i].index << "," << housesIndex.at(predictedHouse) << std::endl;
    }

    outputFile.close();
}

int main(int argc, char* argv[]) {
    try {
        std::vector<StudentInfo> students;
#ifndef _MSC_VER

        if (argc != 2)
        {
            std::cerr << "Usage: " << argv[0] << " <dataset>.csv" << std::endl;
            return 1;
        }
        auto headers = Utils::LoadDataFile(argv[1], students).first;
#else       
        auto headers = Utils::LoadDataFile("dataset_train.csv", students).first;
#endif // MVS

        std::vector<size_t> featuresSelected = { 3, 4, 7 };
        const size_t featuresCount = featuresSelected.size();
        const size_t housesCount = 4;

        // Initialize parameters
        std::vector<std::vector<double>> weights(housesCount, std::vector<double>(featuresCount, 0.0));
        std::vector<double> featureMeans, featureStdDevs;
        Utils::LoadWeightsAndNormalizationParameters(weights, featureMeans, featureStdDevs, "models.save");

        // Handle missing values
        handleMissingValues(students);

        // Normalize data
        Utils::NormalizeData(students, featureMeans, featureStdDevs);

        // Create input vectors
        std::vector<std::vector<double>> inputs;
        createInputVectors(students, featuresSelected, inputs);

        // Perform predictions and write results
        performPredictions(students, weights, headers, inputs);
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return 0;
}

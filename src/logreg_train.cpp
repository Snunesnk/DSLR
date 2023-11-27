#include <iostream>
#include <vector>
#include <cmath>
#include <numeric>
#include "utils.h"
#include "calculate.h"

#include <random>

double lossFunction(const std::vector<std::vector<double>>& inputs, const std::vector<std::vector<double>>& weights,
    const std::vector<std::vector<double>>& target, const size_t house)
{
    const size_t size = inputs.size();
    double loss = 0;

    for (size_t i = 0; i < size; ++i)
    {
        double proba = Calculate::LogisticRegressionHypothesis(weights[house], inputs[i]);
        loss += target[i][house] * std::log(proba + 1e-15) +
            (1.0 - target[i][house]) * std::log(1.0 - proba + 1e-15);
    }
    return - (1.0 / size) * loss;
}

double lossFunctionPartialDerivative(const std::vector<std::vector<double>>& inputs, std::vector<std::vector<double>>& weights,
    const std::vector<std::vector<double>>& target, const size_t house, const size_t j)
{
    const size_t size = inputs.size();
    double derivative = 0;

    for (size_t i = 0; i < size; i++)
    {
        double proba = Calculate::LogisticRegressionHypothesis(weights[house], inputs[i]);
        derivative += (proba - target[i][house]) * inputs[i][j];
    }
    return (1.0 / size) * derivative;
}

void gradientDescent(const std::vector<std::vector<double>>& inputs, std::vector<std::vector<double>>& weights,
    const std::vector<std::vector<double>>& target, const size_t house)
{
    const double learningRate = 0.1;
    const size_t size = weights[0].size();

    std::vector<std::vector<double>> tmp_weights = weights;
    tmp_weights[house][0] = weights[house][0];

    for (size_t j = 0; j < size; j++)
    {       
        double derivative = lossFunctionPartialDerivative(inputs, weights, target, house, j);
        
        tmp_weights[house][j] -= learningRate * derivative;
    }

    // Mise à jour des poids après avoir calculé toutes les dérivées partielles
    weights[house] = tmp_weights[house];
}

void trainModels(std::vector<std::vector<double>>& weights, const std::vector<std::vector<double>>& inputs,
    const std::vector<std::vector<double>>& targets, const size_t epochs)
{
    const size_t housesCount = weights.size();

    std::cout << std::left << std::setw(std::to_string(epochs).length() + 8) << "Epochs"
        << std::setw(10) << "Loss 1"
        << std::setw(10) << "Loss 2"
        << std::setw(10) << "Loss 3"
        << std::setw(10) << "Loss 4"
        << std::setw(10) << "Accuracy" << std::endl;
    // Entraînement du modèle
    for (size_t epoch = 0; epoch < epochs; ++epoch)
    {
        for (size_t house = 0; house < housesCount; house++)
        {
            gradientDescent(inputs, weights, targets, house);
        }
        // Calculer la perte moyenne pour chaque maison après chaque époque (facultatif)
        std::cout << "Epoch " << std::left << std::setw(std::to_string(epochs).length() + 2) << epoch + 1;
        for (size_t house = 0; house < housesCount; house++)
        {
            double loss = lossFunction(inputs, weights, targets, house);
            std::cout << std::setw(10) << std::setprecision(6) << loss; 
        }
        double accuracy = Calculate::Accuracy(inputs, targets, weights);
        std::cout << std::setw(5) << std::fixed << std::setprecision(2) << accuracy << "%";
        std::cout << std::endl;
    }
}

// Function to handle missing values by replacing NaN with feature means
void handleMissingValues(std::vector<StudentInfo>& students)
{
    size_t studentCount = students.size();
    size_t featureCount = students[0].features.size();
    std::vector<std::vector<double>> featureValues(featureCount, std::vector<double>(students.size()));
    for (size_t i = 0; i < featureCount; ++i)
    {
        for (size_t j = 0; j < studentCount; ++j)
        {
            featureValues[i][j] = students[j].features[i];
        }
    }

    for (auto& student : students)
    {
        for (size_t j = 0; j < featureCount; ++j)
        {
            if (std::isnan(student.features[j]))
            {
                student.features[j] = Calculate::Mean(featureValues[j]);
            }
        }
    }
}

// Function to set up data for training
void setupTrainingData(const std::vector<StudentInfo>& students,
    const std::vector<size_t>& selectedFeatures,
    const std::unordered_map<size_t, std::string>& houseIndex,
    std::vector<std::vector<double>>& weights,
    std::vector<std::vector<double>>& trainingInputs,
    std::vector<std::vector<double>>& trainingLabels) {
    const size_t houseCount = houseIndex.size();

    // Initialize weights randomly
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> distribution(-0.5, 0.5);

    for (size_t i = 0; i < houseCount; ++i) {
        for (size_t j = 0; j < selectedFeatures.size(); ++j) {
            weights[i][j] = distribution(gen);
        }
    }

    // Populate training data
    for (size_t i = 0; i < students.size(); i++) {
        std::vector<double> selection;
        for (auto feature : selectedFeatures) {
            selection.push_back(students[i].features[feature - 1]);
        }
        trainingInputs.push_back(selection);

        std::vector<double> result(houseCount, 0.0);
        for (const auto& entry : houseIndex) {
            if (entry.second == students[i].labels[0]) {
                result[entry.first] = 1.0;
                break;
            }
        }
        trainingLabels.push_back(result);
    }
}

int main() {
    try {
        std::vector<StudentInfo> students;
#ifndef _MSC_VER

        if (argc != 2)
        {
            std::cerr << "Usage: " << argv[0] << " <dataset>.csv" << std::endl;
            return 1;
        }
        auto [headers, featuresStartIndex] = Utils::LoadDataFile(argv[1], students);
#else       
        auto [headers, featuresStartIndex] = Utils::LoadDataFile("dataset_train.csv", students);
#endif // MVS

        // Handle missing values
        handleMissingValues(students);

        // Normalize training data
        std::vector<double> featureMeans, featureStdDevs;
        Utils::NormalizeData(students, featureMeans, featureStdDevs);

        // Set up data for training
        std::vector<size_t> selectedFeatures = { 3, 4, 7 };
        std::unordered_map<size_t, std::string> houseIndex = {
            {0, "Slytherin"},
            {1, "Ravenclaw"},
            {2, "Gryffindor"},
            {3, "Hufflepuff"}
        };

        std::vector<std::vector<double>> weights(houseIndex.size(), std::vector<double>(selectedFeatures.size(), 0.0));
        std::vector<std::vector<double>> trainingInputs;
        std::vector<std::vector<double>> trainingLabels;

        setupTrainingData(students, selectedFeatures, houseIndex, weights, trainingInputs, trainingLabels);

        // Train the model
        trainModels(weights, trainingInputs, trainingLabels, 100);

        // Save weights and normalization parameters
        Utils::SaveWeightsAndNormalizationParameters(weights, featureMeans, featureStdDevs, "models.save");
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return 0;
}

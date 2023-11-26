#include <iostream>
#include <vector>
#include <cmath>
#include <numeric>
#include "utils.h"
#include "calculate.h"

double weightedSum(const std::vector<double>& weights, const std::vector<double>& inputs)
{
    const size_t elemsCount = weights.size();
    double weightedSum = 0;

    for (size_t i = 0; i < elemsCount; ++i) {
        weightedSum += weights[i] * inputs[i];
    }
    return weightedSum;
}

double sigmoid(double x)
{
    return 1.0 / (1.0 + exp(-x));
}

double activationFunction(const std::vector<double>& weights, const std::vector<double>& inputs)
{
    return sigmoid(weightedSum(weights, inputs));
}

double prediction(const std::vector<double>& weights, const std::vector<double>& inputs)
{
    return activationFunction(weights, inputs);
}

double lossFunction(const std::vector<std::vector<double>>& inputs, const std::vector<std::vector<double>>& weights,
    const std::vector<std::vector<double>>& target, const double house)
{
    const size_t size = inputs.size();
    double loss = 0;

    for (size_t i = 0; i < size; ++i)
    {
        double proba = prediction(weights[house], inputs[i]);
        loss += target[i][house] * std::log(proba + 1e-15) +
            (1.0 - target[i][house]) * std::log(1.0 - proba + 1e-15);
    }
    return - (1.0 / size) * loss;
}

double lossFunctionPartialDerivative(const std::vector<std::vector<double>>& inputs, std::vector<std::vector<double>>& weights,
    const std::vector<std::vector<double>>& target, const double house, size_t j)
{
    const size_t size = inputs.size();
    double derivative = 0;

    for (size_t i = 0; i < size; i++)
    {
        double proba = prediction(weights[house], inputs[i]);
        derivative += (proba - target[i][house]) * inputs[i][j];
    }
    return (1.0 / size) * derivative;
}

void gradientDescent(const std::vector<std::vector<double>>& inputs, std::vector<std::vector<double>>& weights,
    const std::vector<std::vector<double>>& target, const double house)
{
    const double learningRate = 0.1;
    const size_t size = weights[0].size();

    for (size_t j = 0; j < size; j++)
    {       
        double derivative = lossFunctionPartialDerivative(inputs, weights, target, house, j);
        // Mise à jour du poids
        weights[house][j] = weights[house][j] - learningRate * derivative;
    }
}

std::vector<double> training(std::vector<std::vector<double>>& weights, const std::vector<std::vector<double>>& inputs, const std::vector<std::vector<double>>& targets)
{
    const size_t housesCount = 4;

    // Vecteur pour stocker les probabilités de classe
    std::vector<double> probClass(housesCount);

    const size_t epochs = 100;
    // Entraînement du modèle
    for (size_t epoch = 0; epoch < epochs; ++epoch)
    {
        for (size_t house = 0; house < housesCount; house++)
        {
            gradientDescent(inputs, weights, targets, house);
        }

        // Calculer la perte moyenne pour chaque maison après chaque époque (facultatif)
        std::cout << "Epoch " << epoch;
        for (size_t house = 0; house < housesCount; house++)
        {
            double loss = lossFunction(inputs, weights, targets, house);
            std::cout << " " << loss;
        }
        std::cout << std::endl;
    }

    const size_t size = inputs.size();

    double valid = 0;
    for (size_t i = 0; i < size; ++i)
    {
        double probSave = 0;
        size_t houseIndex = 0;
        for (size_t house = 0; house < housesCount; house++)
        {
            double proba = prediction(weights[house], inputs[i]);
            if (proba > probSave)
            {
                probSave = proba;
                houseIndex = house;
            }
        }
        if (targets[i][houseIndex] == 1.0)
        {
            valid++;
        }
    }
    std::cout << "Prediction % : " << (valid / size) * 100.0 << std::endl;

    return probClass;
}

void normalizeData(std::vector<StudentInfo>& data) {
    // Calculer la moyenne et l'écart type des caractéristiques des données
    size_t numFeatures = data[0].features.size();
    // Ajout des membres pour stocker les moyennes et les écarts types
    std::vector<double> featureMeans; // Moyennes des caractéristiques
    std::vector<double> featureStdDevs; // Écarts types des caractéristiques
    for (size_t i = 0; i < numFeatures; ++i) {
        double mean = std::accumulate(data.begin(), data.end(), 0.0, [i](double sum, const StudentInfo& entry) {
            return sum + entry.features[i];
            }) / data.size();

            double stdDev = std::sqrt(std::accumulate(data.begin(), data.end(), 0.0, [i, mean](double sum, const StudentInfo& entry) {
                return sum + std::pow(entry.features[i] - mean, 2);
                }) / data.size());

            featureMeans.push_back(mean);
            featureStdDevs.push_back(stdDev);
    }

    // Normalise les données
    for (StudentInfo& entry : data) {
        size_t numFeatures = entry.features.size();
        for (size_t i = 0; i < numFeatures; ++i) {
            if (featureStdDevs[i] != 0.0) {
                entry.features[i] = (entry.features[i] - featureMeans[i]) / featureStdDevs[i];
            }
            else {
                std::cerr << "Warning: Feature " << i << " has a zero standard deviation. Normalization ignored.\n";
            }
        }
    }
}

int main() {

    std::vector<StudentInfo> studentData;
    std::vector<std::string> headers;
    size_t featuresStartIndex;
    Utils::loadDataFile("dataset_train.csv", studentData, headers, featuresStartIndex);

    std::vector<std::vector<double>> featuresValues(13);
    for (int i = 0; i < 13; i++)
    {
        for (const auto& student : studentData)
        {
            featuresValues[i].push_back(student.features[i]);
        }
    }
    for (auto i = 0; i < studentData.size(); i++)
    {
        for (auto j = 0; j < studentData[i].features.size(); j++)
        {
            if (std::isnan(studentData[i].features[j]))
            {
                studentData[i].features[j] = Calculate::Mean(featuresValues[j]);
            }
        }
    }
    normalizeData(studentData);

    std::vector<size_t> featuresSelected = {  3, 4, 7 };
    const size_t featuresCount = featuresSelected.size();
    const size_t housesCount = 4;
    std::vector<std::vector<double>> weights(housesCount, std::vector<double>(featuresCount, 0.0));
    std::vector<std::vector<double>> inputs;
    std::vector<std::vector<double>> labels;

    std::unordered_map<size_t, std::string> housesIndex;
    housesIndex[0] = "Slytherin";
    housesIndex[1] = "Ravenclaw";
    housesIndex[2] = "Gryffindor";
    housesIndex[3] = "Hufflepuff";

    for (auto i = 0; i < studentData.size(); i++)
    {
        std::vector<double> selection;
        for (auto feature : featuresSelected)
        {
            selection.push_back(studentData[i].features[feature - 1]);
        }

        inputs.push_back(selection);

        if (studentData[i].labels[0] == housesIndex[0])
        {
            labels.push_back({ 1.0, 0.0, 0.0, 0.0 });
        }
        else if (studentData[i].labels[0] == housesIndex[1])
        {
            labels.push_back({ 0.0, 1.0, 0.0, 0.0 });
        }
        else if (studentData[i].labels[0] == housesIndex[2])
        {
            labels.push_back({ 0.0, 0.0, 1.0, 0.0 });
        }
        else if (studentData[i].labels[0] == housesIndex[3])
        {
            labels.push_back({ 0.0, 0.0, 0.0, 1.0 });
        }
    }
    std::vector<double> probClass(housesCount);
    probClass = training(weights, inputs, labels);

    //std::vector<StudentInfo> testData;
    //Utils::loadDataFile("dataset_test.csv", testData, headers, featuresStartIndex);

    return 0;
}
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

void normalizeData(std::vector<StudentInfo>& data, std::vector<double>& featureMeans, std::vector<double>& featureStdDevs) {
    // Calculer la moyenne et l'écart type des caractéristiques des données
    size_t numFeatures = data[0].features.size();

    if (featureMeans.size() + featureStdDevs.size() == 0) {
        for (size_t i = 0; i < numFeatures; ++i) {
            // Extraction des valeurs de la caractéristique i
            std::vector<double> featureValues;
            for (const StudentInfo& entry : data) {
                featureValues.push_back(entry.features[i]);
            }

            // Calcul de la moyenne et de l'écart type en utilisant vos fonctions
            double mean = Calculate::Mean(featureValues);
            double stdDev = Calculate::StandardDeviation(featureValues);

            featureMeans.push_back(mean);
            featureStdDevs.push_back(stdDev);
        }
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

void loadWeightsAndNormalizationParameters(std::vector<std::vector<double>>& weights,
    std::vector<double>& featureMeans,
    std::vector<double>& featureStdDevs,
    const std::string& filename) {
    // Ouvrir le fichier en mode lecture
    std::ifstream inFile(filename);

    if (!inFile.is_open()) {
        std::cerr << "Error: Unable to open the file " << filename << " for reading." << std::endl;
        return;
    }

    std::string line;
    // Lire les moyennes des caractéristiques
    std::getline(inFile, line);
    std::istringstream meanStream(line);
    std::string meanLabel;
    meanStream >> meanLabel; // Ignorer le label "Feature Means:"
    double meanValue;
    while (meanStream >> meanValue) {
        featureMeans.push_back(meanValue);
    }

    // Lire les écarts types des caractéristiques
    std::getline(inFile, line);
    std::istringstream stdDevStream(line);
    std::string stdDevLabel;
    stdDevStream >> stdDevLabel; // Ignorer le label "Feature Standard Deviations:"
    double stdDevValue;
    while (stdDevStream >> stdDevValue) {
        featureStdDevs.push_back(stdDevValue);
    }

    // Lire les poids
    weights.clear(); // Assurez-vous de vider le vecteur avant de le remplir
    std::getline(inFile, line);
    while (std::getline(inFile, line)) {
        std::istringstream weightStream(line);
        double weight;
        std::vector<double> houseWeights;
        while (weightStream >> weight) {
            houseWeights.push_back(weight);
        }
        weights.push_back(houseWeights);
    }

    // Fermer le fichier
    inFile.close();
}

int main() {
    try {

        std::vector<StudentInfo> studentData;
        std::vector<std::string> headers;
        size_t featuresStartIndex;
        Utils::loadDataFile("dataset_test.csv", studentData, headers, featuresStartIndex);

        std::vector<size_t> featuresSelected = { 3, 4, 7 };
        const size_t featuresCount = featuresSelected.size();
        const size_t housesCount = 4;

        std::vector<std::vector<double>> weights(housesCount, std::vector<double>(featuresCount, 0.0));
        std::vector<double> featureMeans; // Moyennes des caractéristiques
        std::vector<double> featureStdDevs; // Écarts types des caractéristiques
        loadWeightsAndNormalizationParameters(weights, featureMeans, featureStdDevs, "weights.save");

        // Collecter les valeurs de caractéristiques pour chaque colonne
        std::vector<std::vector<double>> featureValues(13);
        for (int i = 0; i < 13; i++) {
            for (const auto& student : studentData) {
                featureValues[i].push_back(student.features[i]);
            }
        }

        // Remplacer les valeurs NaN par la moyenne de la caractéristique respective
        for (auto& student : studentData) {
            for (size_t j = 0; j < student.features.size(); j++) {
                if (std::isnan(student.features[j])) {
                    student.features[j] = Calculate::Mean(featureValues[j]);
                }
            }
        }
        normalizeData(studentData, featureMeans, featureStdDevs);

        std::vector<std::vector<double>> inputs;

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
        }

        const size_t dataSize = studentData.size();
        double correctPredictions = 0;

        for (size_t i = 0; i < dataSize; ++i)
        {
            double maxProbability = 0;
            size_t predictedHouse = 0;

            for (size_t house = 0; house < housesCount; ++house)
            {
                double probability = prediction(weights[house], inputs[i]);
                if (probability > maxProbability)
                {
                    maxProbability = probability;
                    predictedHouse = house;
                }
            }
            std::cout << studentData[i].index << "," << housesIndex[predictedHouse] << std::endl;
        }
    }
    catch (const std::exception& e) {
        // Handle exceptions and display error messages
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return 0;
}
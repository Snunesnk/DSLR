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
    const std::vector<std::vector<double>>& target, const size_t house)
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
    const std::vector<std::vector<double>>& target, const size_t house, const size_t j)
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
    const std::vector<std::vector<double>>& target, const size_t house)
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

// La fonction de prédiction, poids et autres fonctions nécessaires doivent être définies ailleurs dans votre code.
double calculateAccuracy(const std::vector<std::vector<double>>& inputs, const std::vector<std::vector<double>>& targets,
    const std::vector<std::vector<double>>& weights)
{
    const size_t dataSize = inputs.size();
    double correctPredictions = 0;

    for (size_t i = 0; i < dataSize; ++i)
    {
        double maxProbability = 0;
        size_t predictedHouse = 0;

        for (size_t house = 0; house < weights.size(); ++house)
        {
            double probability = prediction(weights[house], inputs[i]);
            if (probability > maxProbability)
            {
                maxProbability = probability;
                predictedHouse = house;
            }
        }

        if (targets[i][predictedHouse] == 1.0)
        {
            correctPredictions++;
        }
    }

    // Calculer le pourcentage de prédictions correctes
    double accuracy = (correctPredictions / dataSize) * 100.0;
    return accuracy;
}

void training(std::vector<std::vector<double>>& weights, const std::vector<std::vector<double>>& inputs, const std::vector<std::vector<double>>& targets)
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

void saveWeightsAndNormalizationParameters(const std::vector<std::vector<double>>& weights,
    const std::vector<double>& featureMeans,
    const std::vector<double>& featureStdDevs,
    const std::string& filename) {
    // Ouvrir le fichier en mode écriture
    std::ofstream outFile(filename);

    if (!outFile.is_open()) {
        std::cerr << "Error: Unable to open the file " << filename << " for writing." << std::endl;
        return;
    }

    // Enregistrer les caractéristiques (moyennes et écarts types)
    outFile << "FeatureMeans:";
    for (double mean : featureMeans) {
        outFile << " " << mean;
    }
    outFile << "\n";

    outFile << "FeatureStandardDeviations:";
    for (double stdDev : featureStdDevs) {
        outFile << " " << stdDev;
    }
    outFile << "\n\n";

    // Enregistrer les poids
    for (const auto& houseWeights : weights) {
        for (double weight : houseWeights) {
            outFile << weight << " ";
        }
        outFile << "\n";
    }

    // Fermer le fichier
    outFile.close();
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
        Utils::loadDataFile("dataset_train.csv", studentData, headers, featuresStartIndex);

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

        // Ajout des membres pour stocker les moyennes et les écarts types
        std::vector<double> featureMeans; // Moyennes des caractéristiques
        std::vector<double> featureStdDevs; // Écarts types des caractéristiques
        normalizeData(studentData, featureMeans, featureStdDevs);

        std::vector<size_t> featuresSelected = { 3, 4, 7 };
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

        training(weights, inputs, labels);

        std::cout << "Accuracy : " << calculateAccuracy(inputs, labels, weights) << "%" << std::endl;

        saveWeightsAndNormalizationParameters(weights, featureMeans, featureStdDevs, "weights.save");

    }
    catch (const std::exception& e) {
        // Handle exceptions and display error messages
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return 0;
}
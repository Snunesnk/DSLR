#include "calculate.h"
#include <algorithm>
#include <cmath>
#include <limits>

double Calculate::Mean(const std::vector<double>& data) {
    double sum = 0.0;
    double count = 0;
    for (const auto& value : data) {
        if (!std::isnan(value)) {
            sum += value;
            count++;
        }
    }
    return sum / count;
}

double Calculate::StandardDeviation(const std::vector<double>& data) {
    double m = Calculate::Mean(data);
    double variance = 0.0;
    double count = 0;
    for (const auto& value : data) {
        if (!std::isnan(value)) {
            variance += std::pow(value - m, 2);
            count++;
        }
    }
    return std::sqrt(variance / count);
}

double Calculate::Min(const std::vector<double>& data) {
    double minValue = data[0];
    for (const auto& value : data) {
        if (!std::isnan(value) && value < minValue) {
            minValue = value;
        }
    }
    return minValue;
}

double Calculate::Max(const std::vector<double>& data) {
    double maxValue = data[0];
    for (const auto& value : data) {
        if (!std::isnan(value) && value > maxValue) {
            maxValue = value;
        }
    }
    return maxValue;
}

void swap(double& a, double& b) {
    double temp = a;
    a = b;
    b = temp;
}

// Fonction utilitaire pour partitionner le tableau autour du pivot
int partition(std::vector<double>& arr, int low, int high) {
    double pivot = arr[high];
    int i = low - 1;

    for (int j = low; j < high; ++j) {
        if (arr[j] < pivot) {
            ++i;
            swap(arr[i], arr[j]);
        }
    }

    swap(arr[i + 1], arr[high]);
    return i + 1;
}

// Fonction principale de QuickSort pour des vecteurs de double
void quickSort(std::vector<double>& arr, int low, int high) {
    if (low < high) {
        // Partitionne le tableau et obtient l'index du pivot
        int pivotIndex = partition(arr, low, high);

        // Trie récursivement les sous-tableaux à gauche et à droite du pivot
        quickSort(arr, low, pivotIndex - 1);
        quickSort(arr, pivotIndex + 1, high);
    }
}

double Calculate::Quartile(const std::vector<double>& data, int n) {
    std::vector<double> sortedData;
    for (const auto& value : data) {
        if (!std::isnan(value)) {
            sortedData.push_back(value);
        }
    }
    quickSort(sortedData, 0, sortedData.size() - 1);
    double index = (static_cast<double>(n) * (static_cast<double>(sortedData.size()) - 1.0)) / 100.0;
    double ptc = index - static_cast<double>(static_cast<size_t>(index));
    return (sortedData[static_cast<size_t>(index)] * ptc + sortedData[static_cast<size_t>(index) + 1] * (1.0 - ptc)) / 2.0;
}

double Calculate::Covariance(const std::vector<double>& data1, const std::vector<double>& data2) {

    double meanData1 = Calculate::Mean(data1);
    double meanData2 = Calculate::Mean(data2);

    // Calcul de la covariance
    double covariance = 0.0;
    double count = 0;
    for (size_t i = 0; i < data1.size(); ++i) {
        if (!std::isnan(data1[i]) && !std::isnan(data2[i])) {
            covariance += (data1[i] - meanData1) * (data2[i] - meanData2);
            count++;
        }
    }

    return covariance / count;
}

double Calculate::PearsonCorrelation(const std::vector<double>& data1, const std::vector<double>& data2) {
    double covariance = Calculate::Covariance(data1, data2);

    double stdDevData1 = Calculate::StandardDeviation(data1);
    double stdDevData2 = Calculate::StandardDeviation(data2);

    // Calcul du coefficient de corrélation de Pearson
    double pearsonCorrelation = covariance / (stdDevData1 * stdDevData2);

    return pearsonCorrelation;
}

double Calculate::LogisticRegressionHypothesis(const std::vector<double>& weights, const std::vector<double>& inputs)
{  
    const size_t weightCount = weights.size();
    double weightedSum = 0;

    for (size_t i = 0; i < weightCount; ++i)
    {
        weightedSum += weights[i] * inputs[i];
    }

    double sigmoid = 1.0 / (1.0 + exp(-weightedSum));
    return sigmoid;
}

double Calculate::Accuracy(const std::vector<std::vector<double>>& inputs, const std::vector<std::vector<double>>& targets,
    const std::vector<std::vector<double>>& weights)
{
    const size_t dataSize = inputs.size();
    const size_t houseCount = weights.size();
    double correctPredictions = 0;

    for (size_t i = 0; i < dataSize; ++i)
    {
        double maxProbability = -1.0;
        size_t predictedHouse = 0;

        for (size_t house = 0; house < houseCount; ++house)
        {
            double probability = Calculate::LogisticRegressionHypothesis(weights[house], inputs[i]);
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

    return (correctPredictions / static_cast<double>(dataSize)) * 100.0;
}

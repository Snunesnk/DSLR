#include "Calculate.h"
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

double Calculate::Percentile(const std::vector<double>& data, int n) {
    std::vector<double> sortedData;
    for (const auto& value : data) {
        if (!std::isnan(value)) {
            sortedData.push_back(value);
        }
    }
    std::sort(sortedData.begin(), sortedData.end());
    double index = (n * (sortedData.size() - 1)) / 100.0;
    double ptc = index - static_cast<int>(index);
    return (sortedData[static_cast<int>(index)] * ptc + sortedData[static_cast<int>(index) + 1] * (1.0 - ptc)) / 2.0;
}

// Fonction pour calculer la covariance entre deux ensembles de données
double Calculate::Covariance(const std::vector<double>& data1, const std::vector<double>& data2) {

    // Calcul de la moyenne des ensembles de données
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

// Fonction pour calculer le coefficient de corrélation de Pearson entre deux ensembles de données
double Calculate::PearsonCorrelation(const std::vector<double>& data1, const std::vector<double>& data2) {
    // Calcul de la covariance entre les ensembles de données
    double covariance = Calculate::Covariance(data1, data2);

    // Calcul des écarts-types des ensembles de données
    double stdDevData1 = Calculate::StandardDeviation(data1);
    double stdDevData2 = Calculate::StandardDeviation(data2);

    // Calcul du coefficient de corrélation de Pearson
    double pearsonCorrelation = covariance / (stdDevData1 * stdDevData2);

    return pearsonCorrelation;
}

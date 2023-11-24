#ifndef CALCULATE_H
#define CALCULATE_H

#include <vector>

class Calculate {
public:
    // Calcule et retourne la moyenne d'un ensemble de donn�es
    static double Mean(const std::vector<double>& data);

    // Calcule et retourne l'�cart-type d'un ensemble de donn�es
    static double StandardDeviation(const std::vector<double>& data);

    // Calcule et retourne la valeur minimale d'un ensemble de donn�es
    static double Min(const std::vector<double>& data);

    // Calcule et retourne la valeur maximale d'un ensemble de donn�es
    static double Max(const std::vector<double>& data);

    // Calcule et retourne le percentile d'un ensemble de donn�es
    static double Percentile(const std::vector<double>& data, int n);

    // Calcule et retourne la covariance entre deux ensembles de donn�es
    static double Covariance(const std::vector<double>& data1, const std::vector<double>& data2);

    // Calcule et retourne le coefficient de corr�lation de Pearson entre deux ensembles de donn�es
    static double PearsonCorrelation(const std::vector<double>& data1, const std::vector<double>& data2);
};

#endif // CALCULATE_H

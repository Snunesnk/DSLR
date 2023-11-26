#ifndef CALCULATE_H
#define CALCULATE_H
#pragma warning(push, 0)
#include <vector>
#pragma warning(pop)
class Calculate {
public:
	// Calculate and return the mean of a dataset
	static double Mean(const std::vector<double>& data);

	// Calculate and return the standard deviation of a dataset
	static double StandardDeviation(const std::vector<double>& data);

	// Calculate and return the minimum value of a dataset
	static double Min(const std::vector<double>& data);

	// Calculate and return the maximum value of a dataset
	static double Max(const std::vector<double>& data);

	// Calculate and return the percentile of a dataset
	static double Percentile(const std::vector<double>& data, int n);

	// Calculate and return the covariance between two datasets
	static double Covariance(const std::vector<double>& data1, const std::vector<double>& data2);

	// Calculate and return the Pearson correlation coefficient between two datasets
	static double PearsonCorrelation(const std::vector<double>& data1, const std::vector<double>& data2);
};

#endif // CALCULATE_H
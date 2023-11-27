#include <cmath>
#include "utils.h"
#include "calculate.h"

// Function definition for generating scatter plot
void extensionScatterPlot(const std::vector<StudentInfo>& studentData, const size_t featuresCount)
{
    const size_t feature1Index = 2, feature2Index = 4;

    // Python script file name
    std::string pythonScript = "scatterplot.py";

    // Open Python script file
    std::ofstream pythonFile(pythonScript);
    if (pythonFile.is_open())
    {
        // Write Python script header
        pythonFile << "import numpy as np\n";
        pythonFile << "import matplotlib.pyplot as plt\n\n";

        // Retrieve feature values for each student
        std::vector<std::vector<double>> featuresValues(featuresCount);
        for (size_t i = 0; i < featuresCount; ++i)
        {
            for (const auto& student : studentData)
            {
                featuresValues[i].push_back(student.features[i]);
            }
        }

        // Print feature headers and compute correlations
        Utils::printFeatureHeader(featuresCount);
        for (size_t i = 0; i < featuresCount; ++i)
        {
            Utils::computeAndPrintFeatures("Feature " + std::to_string(i + 1), std::bind(Calculate::PearsonCorrelation, std::placeholders::_1, featuresValues[i]), featuresValues);
        }

        std::cout << std::endl;
        std::cout << "What are the two features that are similar?" << std::endl;

        // Find the highest linear correlation to choose the most similar features
        double highestLinearCorrelation = 0;
        size_t featureA = 0;
        size_t featureB = 0;
        for (size_t i = 0; i < featuresCount; ++i)
        {
            for (size_t j = i + 1; j < featuresCount; ++j)
            {
                double linearCorrelation = Calculate::PearsonCorrelation(featuresValues[i], featuresValues[j]);
                if (abs(linearCorrelation) > abs(highestLinearCorrelation))
                {
                    highestLinearCorrelation = linearCorrelation;
                    featureA = i + 1;
                    featureB = j + 1;
                }
            }
        }

        std::cout << "Highest linear correlation found (closest to 1 or -1) is " << highestLinearCorrelation << " between features " << featureA << " and " << featureB << std::endl;

        // Check for NaN values before creating the 'features' array in Python
        pythonFile << "features = np.array([";
        for (size_t k = 0; k < studentData.size(); ++k)
        {
            double feature1Value = featuresValues[feature1Index - 1][k];
            double feature2Value = featuresValues[feature2Index - 1][k];

            // Check if values are NaN
            if (!std::isnan(feature1Value) && !std::isnan(feature2Value))
            {
                pythonFile << "[" << feature1Value << ", " << feature2Value << "]";
            }
            else
            {
                pythonFile << "[np.nan, np.nan]";
            }

            // Add a comma if it's not the last element
            if (k < studentData.size() - 1)
            {
                pythonFile << ", ";
            }
        }
        pythonFile << "])\n";

        // Create the scatter plot
        pythonFile << "plt.figure()\n";
        pythonFile << "plt.scatter(features[:, 0], features[:, 1], marker='o')\n";
        pythonFile << "plt.xlabel('Feature " << feature1Index << "', fontsize=12)\n";
        pythonFile << "plt.ylabel('Feature " << feature2Index << "', fontsize=12)\n";
        pythonFile << "plt.title('Scatter Plot - Feature " << feature1Index << " vs Feature " << feature2Index << "', fontsize=14)\n";
        pythonFile << "plt.show()\n";

        pythonFile.close();
    }
    else
    {
        // Handle file writing error
        std::cerr << "Error writing Python file." << std::endl;
        return;
    }

#ifndef _MSC_VER

    // Execute the Python script
    Utils::executeCommand("python " + pythonScript);

#else

    // Execute the Python script
    Utils::executeCommand("python " + pythonScript);

#endif // MVS

    // Remove the temporary Python script file
    if (std::remove(pythonScript.c_str()) != 0)
    {
        std::cerr << "Error deleting the temporary Python file." << std::endl;
        return;
    }
}

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
        extensionScatterPlot(students, students[0].features.size());
    }
    catch (const std::exception& e) {
        // Handle exceptions and display error messages
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}
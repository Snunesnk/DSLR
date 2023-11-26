#include "utils.h"
#include "calculate.h"


// Function declaration
void extensionHistogram(const std::vector<StudentInfo>& studentData, const std::vector<std::string>& headers, const size_t featuresCount);

int main(int argc, char* argv[])
{
    try {
#ifndef _MSC_VER
        // Check for the correct number of command-line arguments
        if (argc != 2)
        {
            std::cerr << "Usage: " << argv[0] << " <dataset>.csv" << std::endl;
            return 1;
        }

        // Load data from the provided dataset
        std::vector<StudentInfo> studentData;
        std::vector<std::string> headers;
        size_t featuresStartIndex;
        Utils::loadDataFile(argv[1], studentData, headers, featuresStartIndex);
        size_t featuresCount = headers.size() - featuresStartIndex;
#else
        // If using Microsoft Visual Studio, use a default dataset
        std::vector<StudentInfo> studentData;
        std::vector<std::string> headers;
        size_t featuresStartIndex;
        Utils::loadDataFile("dataset_train.csv", studentData, headers, featuresStartIndex);
        size_t featuresCount = headers.size() - featuresStartIndex;
#endif // MVS

        // Call the function to generate the histogram
        extensionHistogram(studentData, headers, featuresCount);
    }
    catch (const std::exception& e) {
        // Handle exceptions and display error messages
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}

// Function definition for generating histogram
void extensionHistogram(const std::vector<StudentInfo>& studentData, const std::vector<std::string>& headers, const size_t featuresCount)
{
    // Constants for indices
    const size_t labelsCount = headers.size() - featuresCount - 1;
    const size_t labelsStartIndex = 1;
    const size_t featuresStartIndex = labelsCount + 1;

    // Extract feature names and labels
    std::vector<std::string> features;
    for (size_t i = featuresStartIndex; i < headers.size(); i++)
    {
        features.push_back(headers[i]);
    }

    std::vector<std::string> labels;
    for (size_t i = labelsStartIndex; i < featuresStartIndex; i++)
    {
        labels.push_back(headers[i]);
    }

    // Find the index of the "Hogwarts House" label
    size_t houseIndex = 0;
    for (size_t i = labelsStartIndex; i < featuresStartIndex; i++)
    {
        if (headers[i] == "Hogwarts House")
        {
            houseIndex = i - 1;
            break;
        }
    }

    // Number of Hogwarts houses
    const size_t housesCount = 4;

    // Initialize a data structure to store feature values by house
    std::vector<std::vector<std::vector<double>>> featuresValuesByHouse(housesCount, std::vector<std::vector<double>>(featuresCount));

    // Fill the data structure with feature values by house
    for (size_t i = 0; i < featuresCount; i++)
    {
        for (const auto& student : studentData)
        {
            if (student.labels[houseIndex] == "Ravenclaw")
            {
                featuresValuesByHouse[0][i].push_back(student.features[i]);
            }
            else if (student.labels[houseIndex] == "Slytherin")
            {
                featuresValuesByHouse[1][i].push_back(student.features[i]);
            }
            else if (student.labels[houseIndex] == "Gryffindor")
            {
                featuresValuesByHouse[2][i].push_back(student.features[i]);
            }
            else if (student.labels[houseIndex] == "Hufflepuff")
            {
                featuresValuesByHouse[3][i].push_back(student.features[i]);
            }
        }
    }

    // Display the header
    Utils::printFeatureHeader(featuresCount);

    // Calculate and display the standard deviation for each house
    Utils::computeAndPrintFeatures("Ravenclaw Std", Calculate::StandardDeviation, featuresValuesByHouse[0]);
    Utils::computeAndPrintFeatures("Slytherin Std", Calculate::StandardDeviation, featuresValuesByHouse[1]);
    Utils::computeAndPrintFeatures("Gryffindor Std", Calculate::StandardDeviation, featuresValuesByHouse[2]);
    Utils::computeAndPrintFeatures("Hufflepuff Std", Calculate::StandardDeviation, featuresValuesByHouse[3]);

    // Initialize vectors to store heterogeneity and standard deviation of features
    std::vector<double> heterogeneities;
    std::vector<std::vector<double>> featuresStd;

    // Calculate the standard deviation of the standard deviation (heterogeneity) for the current feature
    for (size_t i = 0; i < featuresCount; i++)
    {
        std::vector<double> featureStd;
        for (size_t j = 0; j < housesCount; j++)
        {
            featureStd.push_back(Calculate::StandardDeviation(featuresValuesByHouse[j][i]));
        }
        heterogeneities.push_back(Calculate::StandardDeviation(featureStd));
        featuresStd.push_back(featureStd);
    }

    // Calculate and display the heterogeneity of features
    Utils::computeAndPrintFeatures("Heterogeneity", Calculate::StandardDeviation, featuresStd);
    std::cout << std::endl;
    std::cout << "Which Hogwarts course has a homogeneous score distribution between all four houses ?" << std::endl;

    double HighestHomogeneity = std::numeric_limits<double>::max();
    size_t featureIndex = 0;

    // Find the feature with the highest homogeneity (lowest heterogeneity) value
    for (size_t i = 0; i < featuresCount; i++)
    {
        double heterogeneity = Calculate::StandardDeviation(featuresStd[i]);
        if (heterogeneity < HighestHomogeneity)
        {
            HighestHomogeneity = heterogeneity;
            featureIndex = i;
        }
    }

    // Display the feature with the highest homogeneity
    std::cout << "The highest homogeneity (lowest heterogeneity) value is " << HighestHomogeneity << " in the course (feature) " << featureIndex + 1 << " : " << headers[featuresStartIndex + featureIndex] << std::endl;

    // Create a Python script to generate the histogram
    std::string pythonScript = "histogram.py";
    std::ofstream pythonFile(pythonScript);
    if (pythonFile.is_open())
    {
        // Write the Python script
        pythonFile << "import matplotlib.pyplot as plt\n\n";
        pythonFile << "heterogeneities = [" + std::to_string(heterogeneities[0]);
        for (size_t i = 1; i < heterogeneities.size(); ++i)
        {
            pythonFile << ", " + std::to_string(heterogeneities[i]);
        }
        pythonFile << "]\n";

        pythonFile << "features = ['" + features[0] + "'";
        for (size_t i = 1; i < features.size(); ++i)
        {
            pythonFile << ", '" + features[i] + "'";
        }
        pythonFile << "]\n";

        // Configure and display the histogram in Matplotlib
        pythonFile << "plt.figure()\n";
        pythonFile << "plt.bar(features, heterogeneities)\n";
        pythonFile << "plt.xlabel('Subjects', fontsize=12)\n";
        pythonFile << "plt.ylabel('Heterogeneity (logarithmic scale)', fontsize=12)\n";
        pythonFile << "plt.title('Heterogeneity by Subject', fontsize=14)\n";
        pythonFile << "plt.xticks(features, rotation=45, ha='right', fontsize=6)\n";
        pythonFile << "plt.yscale('log')\n";
        pythonFile << "plt.subplots_adjust(bottom=0.29)\n";
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

    // Execute the Python script using Python3
    Utils::executeCommand("python " + pythonScript);

#else

    // Execute the Python script using Python
    Utils::executeCommand("python " + pythonScript);

#endif // MVS

    // Remove the temporary Python script file
    if (std::remove(pythonScript.c_str()) != 0)
    {
        std::cerr << "Error deleting temporary Python file." << std::endl;
        return;
    }
}

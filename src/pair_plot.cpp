#include <cmath>
#include "utils.h"
#include "calculate.h"

void extensionScatterPlotMatrix(const std::vector<StudentInfo>& studentData, const size_t featuresCount);

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
        // Call the function to generate scatter plot matrix
        extensionScatterPlotMatrix(students, students[0].features.size());
    }
    catch (const std::exception& e) {
        // Handle exceptions
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}

// Function definition for generating scatter plot matrix
void extensionScatterPlotMatrix(const std::vector<StudentInfo>& studentData, const size_t featuresCount)
{
    // Python script file name
    std::string pythonScript = "scatterplot.py";

    // Open Python script file
    std::ofstream pythonFile(pythonScript);
    if (pythonFile.is_open())
    {
        // Write Python script header
        pythonFile << "import numpy as np\n";
        pythonFile << "import matplotlib.pyplot as plt\n\n";

        // Create a matrix of features values
        std::vector<std::vector<double>> featuresValues(featuresCount);
        for (size_t i = 0; i < featuresCount; ++i)
        {
            for (const auto& student : studentData)
            {
                featuresValues[i].push_back(student.features[i]);
            }
        }

        // Define house indices and colors
        std::unordered_map<size_t, std::string> housesIndex;
        housesIndex[0] = "Slytherin";
        housesIndex[1] = "Ravenclaw";
        housesIndex[2] = "Gryffindor";
        housesIndex[3] = "Hufflepuff";
        std::vector<std::string> houseColors = { "green", "blue", "scarlet", "yellow" };

        // Loop over houses to create data arrays
        for (size_t h = 0; h < 4; ++h)
        {
            std::string house = housesIndex[h];
            pythonFile << "features" << h << " = np.array([";
            for (size_t k = 0; k < studentData.size(); ++k)
            {
                // Filter data for the specific house
                if (house != studentData[k].labels[0]) {
                    continue;
                }
                // Write feature values to the array
                pythonFile << "[";
                for (size_t i = 0; i < featuresCount; ++i)
                {
                    double featureValue = featuresValues[i][k];
                    if (!std::isnan(featureValue))
                    {
                        pythonFile << featureValue;
                    }
                    else
                    {
                        pythonFile << "np.nan";
                    }
                    if (i < featuresCount - 1)
                    {
                        pythonFile << ", ";
                    }
                }
                pythonFile << "]";
                if (k < studentData.size() - 1)
                {
                    pythonFile << ", ";
                }
            }
            pythonFile << "])\n";
        }

        // Set up parameters for the scatter plot matrix
        const double subplotsSizeX = 1;
        const double subplotsSizeY = 0.5;
        const double pointSize = 0.1;
        const std::string graphTitle = "Scatter Plot Matrix";

        // Write code for creating scatter plot matrix
        pythonFile << "fig, axs = plt.subplots(ncols=" << featuresCount << ", nrows=" << featuresCount << ", figsize=(" << subplotsSizeX * (double)(featuresCount) << ", " << subplotsSizeY * (double)(featuresCount) << "))\n";
        pythonFile << "fig.suptitle('" << graphTitle << "')\n";
        pythonFile << "plt.subplots_adjust(top=0.9, bottom=0.05, left=0.1, right=0.95, hspace=0.05, wspace=0.05)\n";

        // Add labels to the plots
        for (size_t feature1 = 0; feature1 < featuresCount; ++feature1)
        {
            pythonFile << "axs[" << feature1 << ", 0].text(0.0, 0.5, 'F " << (feature1 + 1) << "', transform=axs[" << feature1 << ", 0].transAxes, rotation=0, va='center', ha='right')\n";
        }

        for (size_t feature2 = 0; feature2 < featuresCount; ++feature2)
        {
            pythonFile << "axs[0, " << feature2 << "].text(0.5, 1.0, 'F " << (feature2 + 1) << "', transform=axs[0, " << feature2 << "].transAxes, va='bottom', ha='center')\n";
        }

        // Plot scatter plots for each house
        for (size_t h = 0; h < 4; ++h)
        {
            for (size_t feature1 = 0; feature1 < featuresCount; ++feature1)
            {
                for (size_t feature2 = 0; feature2 < featuresCount; ++feature2)
                {
                    // Scatter plot for each combination of features
                    pythonFile << "axs[" << feature1 << ", " << feature2 << "].scatter(features" << h << "[:, " << feature1 << "], features" << h << "[:, " << feature2 << "], marker = 'o', s = " << pointSize << ")\n";
                }
            }
        }

        // Remove ticks and labels
        pythonFile << "for ax in axs.flat:\n";
        pythonFile << "    ax.tick_params(axis='both', which='both', bottom=False, top=False, left=False, right=False, labelbottom=False, labelleft=False)\n";

        // Display the plot
        pythonFile << "plt.show()\n";
        pythonFile.close();
    }
    else
    {
        // Handle file writing error
        std::cerr << "Error writing Python file." << std::endl;
        return;
    }

    // Execute Python script based on the platform
#ifndef _MSC_VER
    Utils::executeCommand("python " + pythonScript);
#else
    Utils::executeCommand("python " + pythonScript);
#endif

    // Remove the temporary Python script file
    if (std::remove(pythonScript.c_str()) != 0)
    {
        std::cerr << "Error deleting the temporary Python file." << std::endl;
        return;
    }
}

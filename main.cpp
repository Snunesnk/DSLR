#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>


#include "calculate.h"
#include "utils.h"

// Fonction qui génère un histogramme basé sur les données des étudiants
void extensionHistogram(const std::vector<StudentInfo>& studentData) {
    // Déclaration du nombre de caractéristiques et de maisons
    int featureNumber = 13;
    int houseNumber = 4;

    // Initialisation d'une structure de données pour stocker les valeurs des caractéristiques par maison
    std::vector<std::vector<std::vector<double>>> featuresValuesByHouse(houseNumber, std::vector<std::vector<double>>(featureNumber));

    // Boucle pour remplir la structure de données avec les valeurs des caractéristiques par maison
    for (int i = 0; i < featureNumber; i++) {
        for (const auto& student : studentData) {
            if (student.house == "Ravenclaw") {
                featuresValuesByHouse[0][i].push_back(student.features[i]);
            }
            else if (student.house == "Slytherin") {
                featuresValuesByHouse[1][i].push_back(student.features[i]);
            }
            else if (student.house == "Gryffindor") {
                featuresValuesByHouse[2][i].push_back(student.features[i]);
            }
            else if (student.house == "Hufflepuff") {
                featuresValuesByHouse[3][i].push_back(student.features[i]);
            }
        }
    }
    std::cout << std::endl;

    // Affichage de l'en-tête
    Utils::printFeatureHeader(featureNumber);

    // Calcul et affichage de l'écart-type pour chaque maison
    Utils::computeAndPrintFeatures("Ravenclaw Std", Calculate::StandardDeviation, featuresValuesByHouse[0]);
    Utils::computeAndPrintFeatures("Slytherin Std", Calculate::StandardDeviation, featuresValuesByHouse[1]);
    Utils::computeAndPrintFeatures("Gryffindor Std", Calculate::StandardDeviation, featuresValuesByHouse[2]);
    Utils::computeAndPrintFeatures("Hufflepuff Std", Calculate::StandardDeviation, featuresValuesByHouse[3]);

    // Déclaration des matières étudiées
    std::vector<std::string> subjects = { "Arithmancy", "Astronomy", "Herbology", "Defense Against the Dark Arts",
                              "Divination", "Muggle Studies", "Ancient Runes", "History of Magic",
                              "Transfiguration", "Potions", "Care of Magical Creatures", "Charms", "Flying" };

    // Initialisation de vecteurs pour stocker l'hétérogénéité et l'écart-type des caractéristiques
    std::vector<double> heterogeneities;
    std::vector<std::vector<double>> featuresStd;

    // Calcul de l'écart-type de l'écart-type (hétérogénéité) pour la caractéristique actuelle
    for (int i = 0; i < featureNumber; i++) {
        std::vector<double> featureStd;
        for (int j = 0; j < houseNumber; j++) {
            featureStd.push_back(Calculate::StandardDeviation(featuresValuesByHouse[j][i]));
        }
        heterogeneities.push_back(Calculate::StandardDeviation(featureStd));
        featuresStd.push_back(featureStd);
    }

    // Calcul et affichage de l'hétérogénéité des caractéristiques
    Utils::computeAndPrintFeatures("Heterogeneity", Calculate::StandardDeviation, featuresStd);

    // Création d'un script Python pour générer l'histogramme
    std::string pythonScript = "histogram.py";
    std::ofstream pythonFile(pythonScript);
    if (pythonFile.is_open()) {
        // Écriture du script Python
        pythonFile << "import matplotlib.pyplot as plt\n\n";
        pythonFile << "heterogeneities = [" + std::to_string(heterogeneities[0]);
        for (int i = 1; i < heterogeneities.size(); ++i) {
            pythonFile << ", " + std::to_string(heterogeneities[i]);
        }
        pythonFile << "]\n";

        pythonFile << "subjects = ['" + subjects[0] + "'";
        for (size_t i = 1; i < subjects.size(); ++i) {
            pythonFile << ", '" + subjects[i] + "'";
        }
        pythonFile << "]\n";

        // Configuration et affichage de l'histogramme dans Matplotlib
        pythonFile << "plt.figure()\n";
        pythonFile << "plt.bar(subjects, heterogeneities)\n";
        pythonFile << "plt.xlabel('Subjects', fontsize=12)\n";
        pythonFile << "plt.ylabel('Heterogeneity (logarithmic scale)', fontsize=12)\n";
        pythonFile << "plt.title('Heterogeneity by Subject', fontsize=14)\n";
        pythonFile << "plt.xticks(subjects, rotation=45, ha='right', fontsize=6)\n";
        pythonFile << "plt.yscale('log')\n";
        pythonFile << "plt.subplots_adjust(bottom=0.29)\n";
        pythonFile << "plt.show()\n";

        pythonFile.close();
    }
    else {
        std::cerr << "Error writing Python file." << std::endl;
        return;
    }

    // Exécution du script Python
    Utils::executeCommand("python " + pythonScript);

    // Suppression du fichier Python temporaire
    if (std::remove(pythonScript.c_str()) != 0) {
        std::cerr << "Error deleting temporary Python file." << std::endl;
        return;
    }
}

// Fonction qui génère un graphique de dispersion (scatter plot) basé sur les données des étudiants
void extensionScatterPlot(const std::vector<StudentInfo>& studentData, size_t feature1Index, size_t feature2Index) {
    // Déclaration du nombre de caractéristiques
    int featureNumber = 13;

    // Création d'un script Python pour le graphique de dispersion
    std::string pythonScript = "scatterplot.py";
    std::ofstream pythonFile(pythonScript);
    if (pythonFile.is_open()) {
        // Ajout des imports nécessaires pour le script Python
        pythonFile << "import numpy as np\n";
        pythonFile << "import matplotlib.pyplot as plt\n\n";

        // Récupération des valeurs des caractéristiques pour chaque étudiant
        std::vector<std::vector<double>> featuresValues(featureNumber);
        for (int i = 0; i < featureNumber; ++i) {
            for (const auto& student : studentData) {
                featuresValues[i].push_back(student.features[i]);
            }
        }

        std::cout << std::endl;
        Utils::printFeatureHeader(featuresValues.size());
        for (int i = 0; i < featureNumber; ++i) {
            Utils::computeAndPrintFeatures("Feature " + std::to_string(i + 1), std::bind(Calculate::PearsonCorrelation, std::placeholders::_1, featuresValues[i]), featuresValues);
        }


        // Vérification des valeurs NaN avant de créer le tableau 'features' en Python
        pythonFile << "features = np.array([";
        for (size_t k = 0; k < studentData.size(); ++k) {
            double feature1Value = featuresValues[feature1Index - 1][k];
            double feature2Value = featuresValues[feature2Index - 1][k];

            // Vérification si les valeurs sont NaN
            if (!std::isnan(feature1Value) && !std::isnan(feature2Value)) {
                pythonFile << "[" << feature1Value << ", " << feature2Value << "]";
            }
            else {
                pythonFile << "[np.nan, np.nan]";
            }

            // Ajout d'une virgule si ce n'est pas le dernier élément
            if (k < studentData.size() - 1) {
                pythonFile << ", ";
            }
        }
        pythonFile << "])\n";

        // Création du graphique de dispersion
        pythonFile << "plt.figure()\n";
        pythonFile << "plt.scatter(features[:, 0], features[:, 1], marker='o')\n";
        pythonFile << "plt.xlabel('Feature " << feature1Index << "', fontsize=12)\n";
        pythonFile << "plt.ylabel('Feature " << feature2Index << "', fontsize=12)\n";
        pythonFile << "plt.title('Scatter Plot - Feature " << feature1Index << " vs Feature " << feature2Index << "', fontsize=14)\n";
        pythonFile << "plt.show()\n";

        pythonFile.close();
    }
    else {
        std::cerr << "Error writing Python file." << std::endl;
        return;
    }

    // Exécution du script Python
    Utils::executeCommand("python " + pythonScript);

    // Suppression du fichier Python temporaire
    if (std::remove(pythonScript.c_str()) != 0) {
        std::cerr << "Erreur lors de la suppression du fichier Python temporaire." << std::endl;
        return;
    }
}

// Fonction qui génère un graphique de dispersion (scatter plot) basé sur les données des étudiants
void extensionScatterPlotMatrix(const std::vector<StudentInfo>& studentData) {
    // "What features are you going to use for your logistic regression ?"
    // Sélection basée sur la Forme du Scatter Plot :
    // Cette approche est qualitative et se base sur la visualisation des formes des scatter plots plutôt que sur des mesures statistiques.
    // 
    // Sélection basée sur la Corrélation de Pearson :
    // Utilisez les coefficients de corrélation de Pearson pour évaluer la force de la corrélation entre chaque paire de caractéristiques.
    // Niveaux de corrélation(forte, modérée, faible).

    // Déclaration du nombre de caractéristiques
    int featureNumber = 13;

    // Création d'un script Python pour le graphique de dispersion
    std::string pythonScript = "scatterplot.py";
    std::ofstream pythonFile(pythonScript);
    if (pythonFile.is_open()) {
        // Ajout des imports nécessaires pour le script Python
        pythonFile << "import numpy as np\n";
        pythonFile << "import matplotlib.pyplot as plt\n\n";

        // Récupération des valeurs des caractéristiques pour chaque étudiant
        std::vector<std::vector<double>> featuresValues(featureNumber);
        for (int i = 0; i < featureNumber; ++i) {
            for (const auto& student : studentData) {
                featuresValues[i].push_back(student.features[i]);
            }
        }

        // Vérification des valeurs NaN avant de créer le tableau 'features' en Python
        pythonFile << "features = np.array([";
        for (size_t k = 0; k < studentData.size(); ++k) {
            pythonFile << "[";
            for (int i = 0; i < featureNumber; ++i) {
                double featureValue = featuresValues[i][k];

                // Vérification si la valeur est NaN
                if (!std::isnan(featureValue)) {
                    pythonFile << featureValue;
                }
                else {
                    pythonFile << "np.nan";
                }

                // Ajout d'une virgule si ce n'est pas le dernier élément
                if (i < featureNumber - 1) {
                    pythonFile << ", ";
                }
            }
            pythonFile << "]";

            // Ajout d'une virgule si ce n'est pas le dernier élément
            if (k < studentData.size() - 1) {
                pythonFile << ", ";
            }
        }
        pythonFile << "])\n";


        // Nombre total de combinaisons uniques de caractéristiques
        int totalCombinations = (featureNumber * (featureNumber - 1)) / 2;

        // Définir manuellement le nombre de lignes et de colonnes pour créer un rectangle
        int numRows = 6;
        int numCols = totalCombinations / numRows;

        // Taille des sous-graphiques
        const double subplotsSizeX = 1.0;
        const double subplotsSizeY = 0.5;

        // Taille des points
        const double pointSize = 0.1;  // ajustez cette valeur selon vos besoins

        // Titre du graphique
        const std::string graphTitle = "Scatter Plot Matrix";

        // Création du graphique de dispersion pour chaque combinaison unique de caractéristiques
        pythonFile << "fig, axs = plt.subplots(ncols=" << featureNumber << ", nrows=" << featureNumber << ", figsize=(" << subplotsSizeX * featureNumber << ", " << subplotsSizeY * featureNumber << "))\n";
        pythonFile << "fig.suptitle('" << graphTitle << "')\n";

        pythonFile << "plt.subplots_adjust(top=0.9, bottom=0.05, left=0.1, right=0.95, hspace=0.05, wspace=0.05)\n";


        // Ajouter les noms de caractéristiques à gauche
        for (int feature1 = 0; feature1 < featureNumber; ++feature1) {
            pythonFile << "axs[" << feature1 << ", 0].text(-0.5, 0.5, 'Feature " << (feature1 + 1) << "', transform=axs[" << feature1 << ", 0].transAxes, rotation=0, va='center', ha='center')\n";

        }

        // Ajouter les noms de caractéristiques en haut
        for (int feature2 = 0; feature2 < featureNumber; ++feature2) {
            pythonFile << "axs[0, " << feature2 << "].text(0.5, 1.5, 'Feature " << (feature2 + 1) << "', transform=axs[0, " << feature2 << "].transAxes, va='center', ha='center')\n";

        }

        for (int feature1 = 0; feature1 < featureNumber; ++feature1) {
            for (int feature2 = 0; feature2 < featureNumber; ++feature2) {
                pythonFile << "axs[" << feature1 << ", " << feature2 << "].scatter(features[:, " << feature1 << "], features[:, " << feature2 << "], marker='o', s=" << pointSize << ")\n";
            }
        }

        // Désactiver les étiquettes de l'axe x et y après avoir ajouté les noms des caractéristiques
        pythonFile << "for ax in axs.flat:\n";
        pythonFile << "    ax.tick_params(axis='both', which='both', bottom=False, top=False, left=False, right=False, labelbottom=False, labelleft=False)\n";

        pythonFile << "plt.show()\n";

        pythonFile.close();
    }
    else {
        std::cerr << "Error writing Python file." << std::endl;
        return;
    }

    // Exécution du script Python
    Utils::executeCommand("python " + pythonScript);

    // Suppression du fichier Python temporaire
    if (std::remove(pythonScript.c_str()) != 0) {
        std::cerr << "Erreur lors de la suppression du fichier Python temporaire." << std::endl;
        return;
    }
}

int main() {
    std::string filename = "dataset_test.csv";
    int featureNumber = 13;
    int houseNumber = 4;

    std::vector<StudentInfo> studentData = Utils::readCSV(filename);

    std::vector<std::vector<double>> featuresValues(featureNumber);
    for (int i = 0; i < featureNumber; i++) {
        for (const auto& student : studentData) {
            featuresValues[i].push_back(student.features[i]);
        }
    }

    Utils::printFeatureHeader(featuresValues.size());

    Utils::computeAndPrintFeatures("Count", [](const auto& data) { return static_cast<double>(data.size()); }, featuresValues);
    Utils::computeAndPrintFeatures("Mean", Calculate::Mean, featuresValues);
    Utils::computeAndPrintFeatures("Std", Calculate::StandardDeviation, featuresValues);
    Utils::computeAndPrintFeatures("Min", Calculate::Min, featuresValues);
    Utils::computeAndPrintFeatures("25%", std::bind(Calculate::Percentile, std::placeholders::_1, 25), featuresValues);
    Utils::computeAndPrintFeatures("50%", std::bind(Calculate::Percentile, std::placeholders::_1, 50), featuresValues);
    Utils::computeAndPrintFeatures("75%", std::bind(Calculate::Percentile, std::placeholders::_1, 75), featuresValues);
    Utils::computeAndPrintFeatures("Max", Calculate::Max, featuresValues);

    extensionHistogram(studentData);
    extensionScatterPlot(studentData, 2, 4);
    extensionScatterPlotMatrix(studentData);

    return EXIT_SUCCESS;
}
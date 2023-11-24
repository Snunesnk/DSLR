#include "utils.h"

std::vector<StudentInfo> Utils::readCSV(const std::string& filename) {
    std::vector<StudentInfo> students;

    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return students;
    }

    std::string header;
    std::getline(file, header);

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::vector<std::string> elements;
        std::string element;

        while (std::getline(iss, element, ',')) {
            elements.push_back(element);
        }

        if (elements.size() != 19) {
            std::cerr << "Error parsing line: " << line << std::endl;
            continue;
        }

        StudentInfo student;
        try {
            student.index = std::stoi(elements[0]);
            student.house = elements[1];
            student.firstName = elements[2];
            student.lastName = elements[3];
            student.birthday = elements[4];
            student.bestHand = elements[5];

            student.features.resize(13);
            for (size_t i = 0; i < student.features.size(); ++i) {
                student.features[i] = elements[6 + i].empty() ? std::numeric_limits<double>::quiet_NaN() : std::stod(elements[6 + i]);
            }

            students.push_back(student);
        }
        catch (const std::exception& e) {
            std::cerr << "Error parsing line: " << line << ". " << e.what() << std::endl;
        }
    }

    file.close();

    return students;
}

void Utils::executeCommand(const std::string& command) {
    int result = std::system(command.c_str());

    if (result != 0) {
        std::cerr << "Error executing command" << std::endl;
    }
}

void Utils::printFeatureHeader(const size_t max) {
    const int fieldWidth = 14;

    std::cout << std::setw(fieldWidth) << std::left << "";

    for (auto i = 1; i <= max; i++) {
        std::cout << std::setw(fieldWidth) << std::right << "Feature " + std::to_string(i);
    }

    std::cout << std::endl;
}

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

// Course object used by the advising program.
struct Course {
    string courseNumber;
    string courseTitle;
    vector<string> prerequisites;
};

// Remove leading and trailing whitespace from a string.
string trim(const string& value) {
    size_t start = value.find_first_not_of(" \t\r\n");
    if (start == string::npos) {
        return "";
    }

    size_t end = value.find_last_not_of(" \t\r\n");
    return value.substr(start, end - start + 1);
}

// Convert user input to uppercase so course lookups are not case-sensitive.
string toUpperCase(string value) {
    transform(value.begin(), value.end(), value.begin(), [](unsigned char character) {
        return static_cast<char>(toupper(character));
    });

    return value;
}

// Split a CSV line on commas. This assignment input does not require quoted CSV parsing.
vector<string> splitCsvLine(const string& line) {
    vector<string> fields;
    string field;
    stringstream lineStream(line);

    while (getline(lineStream, field, ',')) {
        fields.push_back(trim(field));
    }

    // Preserve a trailing empty field if the line ends with a comma.
    if (!line.empty() && line.back() == ',') {
        fields.push_back("");
    }

    return fields;
}

// Load courses from the specified file into a map keyed by course number.
bool loadCourses(const string& fileName, map<string, Course>& courses) {
    ifstream inputFile(fileName);

    if (!inputFile.is_open()) {
        cout << "Error: Could not open file " << fileName << "." << endl;
        return false;
    }

    map<string, Course> loadedCourses;
    string line;
    int lineNumber = 0;

    while (getline(inputFile, line)) {
        ++lineNumber;
        line = trim(line);

        if (line.empty()) {
            continue;
        }

        vector<string> fields = splitCsvLine(line);

        if (fields.size() < 2 || fields.at(0).empty() || fields.at(1).empty()) {
            cout << "Error: Invalid course data on line " << lineNumber << "." << endl;
            return false;
        }

        Course course;
        course.courseNumber = toUpperCase(fields.at(0));
        course.courseTitle = fields.at(1);

        for (size_t index = 2; index < fields.size(); ++index) {
            if (!fields.at(index).empty()) {
                course.prerequisites.push_back(toUpperCase(fields.at(index)));
            }
        }

        loadedCourses[course.courseNumber] = course;
    }


    courses = loadedCourses;
    cout << "Data loaded successfully." << endl;
    return true;
}

// Print all courses in alphanumeric order. std::map keeps keys sorted.
void printCourseList(const map<string, Course>& courses) {
    cout << "Here is a sample schedule:" << endl;

    for (const auto& coursePair : courses) {
        cout << coursePair.second.courseNumber << ", " << coursePair.second.courseTitle << endl;
    }
}

// Print one course and its prerequisites.
void printCourseInformation(const map<string, Course>& courses) {
    string courseNumber;

    cout << "What course do you want to know about? ";
    cin >> courseNumber;
    courseNumber = toUpperCase(trim(courseNumber));

    auto courseIterator = courses.find(courseNumber);

    if (courseIterator == courses.end()) {
        cout << "Course " << courseNumber << " was not found." << endl;
        return;
    }

    const Course& course = courseIterator->second;
    cout << course.courseNumber << ", " << course.courseTitle << endl;

    if (course.prerequisites.empty()) {
        cout << "Prerequisites: None" << endl;
        return;
    }

    cout << "Prerequisites: ";
    for (size_t index = 0; index < course.prerequisites.size(); ++index) {
        const string& prerequisiteNumber = course.prerequisites.at(index);
        auto prerequisiteIterator = courses.find(prerequisiteNumber);

        cout << prerequisiteNumber;
        if (prerequisiteIterator != courses.end()) {
            cout << " (" << prerequisiteIterator->second.courseTitle << ")";
        }

        if (index + 1 < course.prerequisites.size()) {
            cout << ", ";
        }
    }

    cout << endl;
}

// Display the program menu.
void displayMenu() {
    cout << "1. Load Data Structure." << endl;
    cout << "2. Print Course List." << endl;
    cout << "3. Print Course." << endl;
    cout << "9. Exit" << endl;
}

int main() {
    map<string, Course> courses;
    bool dataLoaded = false;
    int menuChoice = 0;

    cout << "Welcome to the course planner." << endl;

    while (menuChoice != 9) {
        displayMenu();
        cout << "What would you like to do? ";

        if (!(cin >> menuChoice)) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid input. Please enter a number from the menu." << endl;
            continue;
        }

        switch (menuChoice) {
            case 1: {
                string fileName;
                cout << "Enter the course data file name: ";
                cin.ignore(10000, '\n');
                getline(cin, fileName);
                dataLoaded = loadCourses(trim(fileName), courses);
                break;
            }
            case 2:
                if (!dataLoaded) {
                    cout << "Please load the data structure first." << endl;
                } else {
                    printCourseList(courses);
                }
                break;
            case 3:
                if (!dataLoaded) {
                    cout << "Please load the data structure first." << endl;
                } else {
                    printCourseInformation(courses);
                }
                break;
            case 9:
                cout << "Thank you for using the course planner!" << endl;
                break;
            default:
                cout << menuChoice << " is not a valid option." << endl;
                break;
        }
    }

    return 0;
}

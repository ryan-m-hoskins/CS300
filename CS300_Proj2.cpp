// CS300_Proj2.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <algorithm>
#include <climits>
#include <string> // atoi
#include <vector>
#include <sstream>
#include <unordered_map>

const unsigned int DEFAULT_SIZE = 179;

// Structure to hold Course info
struct Course {
    std::string courseNumber; // unique identifier
    std::string courseName;
    std::vector<std::string> prereqs;
    bool incomplete = true;
};

class HashTable {
private:
    struct Node {
        Course course;
        unsigned int key;
        Node* next;

        Node() {
            key = UINT_MAX;
            next = nullptr;
        }

        Node(Course aCourse) : Node() {
            course = aCourse;
        }

        Node(Course aCourse, unsigned int aKey) {
            key = aKey;
        }
    };

    std::vector<Node> table;

    unsigned int tableSize = DEFAULT_SIZE;

    // Calculating hash value using key
    unsigned int hash(const std::string& key) const {
        // Initialize hash_value to zero
        int hash_value = 0;
        // For each character in key
        for (char c : key) {
            // Calculation for determining hash value
            hash_value = (hash_value * 27 + c) % tableSize;
        }
        // Return hash value modulo tableSize
        return (hash_value) % tableSize;
    }

 // Class public methods
public: 
    HashTable(); 
    HashTable(unsigned int size);
    virtual ~HashTable();
    void Insert(Course course);
    bool Remove(const std::string& courseNumber);
    void PrintAll();
    Course Search(const std::string& courseNumber);
    void ValidatePrereq();
    bool Exists(const std::string& courseNumber, bool prereq_check);
    bool DupeCheck(const std::string& courseNumber);
};

// Default constructor
HashTable::HashTable() {
    table.resize(tableSize);
}

// Constructor for specifying size of table used
HashTable::HashTable(unsigned int size) {
    this->tableSize = size;
    table.resize(tableSize);
}

HashTable::~HashTable() {
    // Erase the beginning of node
    table.erase(table.begin());
}

// Method for inserting a Course object
void HashTable::Insert(Course course) {
    // Calculate hash value using course number
    unsigned key = hash(course.courseNumber);
    // Set current to index of key in table
    Node* current = &table[key];
    // If current is equal to nullptr,
    if (current == nullptr) {
        // Set Node object newNode to a new Node with course and key
        Node* newNode = new Node(course, key);
        // Insert into table
        table.insert(table.begin() + key, (*newNode));
    }
    // Otherwise, if current does not equal nullptr,
    else {
        // If key is unused, assign current attribute using key and course, and set next to nullptr
        if (current->key == UINT_MAX) {
            current->key = key;
            current->course = course;
            current->next = nullptr;
        }
        // Otherwise, 
        else {
            // While next pointer does not equal nullptr
            while (current->next != nullptr) {
                // Move to next 
                current = current->next;
            }
            // Set next to new Node with course and key
            current->next = new Node(course, key);
        }
    }
}

// Method to remove a course from the hash table
bool HashTable::Remove(const std::string& courseNumber) {
    // Generate hash from courseNumber, assign to key variable
    unsigned key = hash(courseNumber);
    // Set Node pointer current to reference index of key in table
    Node* current = &table[key];
    // Set node point prev to nullptr
    Node* prev = nullptr;

    // Begin loop as long as current isn't nullptr and course number does equal 
    while (current != nullptr && current->course.courseNumber != courseNumber) {
        // Cycle through, updating prev and current
        prev = current;
        current = current->next;
    }
    // If current pointer equal nullptr
    if (current == nullptr) {
        // Return false
        return false;
    }
    // If current key is used and current course number matches course number
    if (current->key != UINT_MAX && current->course.courseNumber == courseNumber) {
        // If prev pointer equals nullptr
        if (prev == nullptr) {
            // If current is the head of the linked list
            if (current->next != nullptr) {
                // Index key is set to pointer of next 
                table[key] = *(current->next);
                // Delete current node
                delete current;
            }
            // Otherwise, reset the head to an empty node
            else {
                table[key] = Node();
            }
        }
        else {
            // Update the prev node's next pointer
            prev->next = current->next;
            // Delete current
            delete current;
        }
    }
    return true;
}

// Look through hash table for course flagged as having prereqs, checking to ensure each is listed as a course.
void HashTable::ValidatePrereq() {
    // Begin iterating through table, incrementing by one
    for (int i = 0; i < table.size(); ++i) {
        // Set current to address of index i in table
        Node* current = &table[i];
        // While current is not null
        while (current != nullptr) {
            // If current course has incomplete flag 
            if (current->course.incomplete) {
                // Set boolean value allValid to true
                bool allValid = true;
                // iterate through each prerequisite in course's prerequisites
                for (const std::string& prereq : current->course.prereqs) {
                    // If Exists method does not return true,
                    if (!Exists(prereq, true)) {  
                        // Set allValid to false
                        allValid = false;
                        // Raise error message indicating which course has an invalid prerequisite, providing the invalid prerequisite as well
                        std::cerr << "Error: Course " << current->course.courseNumber << " has an invalid prerequisite: " << prereq << std::endl;
                        // If Remove function returns "true"
                        if (Remove(current->course.courseNumber)) {
                            std::cerr << "Course " << current->course.courseNumber << " removed due to invalid prerequisite." << std::endl;
                            std::cout << "" << std::endl;
                        }
                        else {
                            std::cerr << "Error: Failed to remove course "
                                << current->course.courseNumber << std::endl;
                        }
                        break;
                    }
                }
                // If allValid is true
                if (allValid) {
                    // Set incomplete flag to false for current course
                    current->course.incomplete = false; 
                }
            }
            // Set current to next
            current = current->next; 
        }
    }
}

// Method for checking whether a specific prerequisite exists as a course in the hash table
bool HashTable::Exists(const std::string& courseNumber, bool prereq_check) { 
    // Create hash value for key using course number
    unsigned int key = hash(courseNumber);
    // Set current to the address of the key's index in table
    Node* current = &table[key];
    // While curren is not nullptr
    while (current != nullptr) {
        // If the current course number matches its course number
        if (current->course.courseNumber == courseNumber) {
            // Return true, course exists
            return true;
        }
        // Move to next
        current = current->next;
    }
    // If prereq_check is true and course could not be found, return false
    if (prereq_check) {
        return false;
    }
    // Otherwise, raise error indicating course number does not exist
    else {
        throw std::runtime_error("Course " + courseNumber + " does not exist.");
    }
}

// Method for searching through HashTable for a course number
Course HashTable::Search(const std::string& courseNumber) {
    unsigned int key = hash(courseNumber);
    Course course;
    Node* current = &table[key];

    // If there is not a valid entry in hash table, return the empty course. 
    if (current == nullptr || current->key == UINT_MAX) {
        return course;
    }

    // If the first node matches and is not empty, return bid
    if (current->key != UINT_MAX && current->course.courseNumber == courseNumber) {
        return current->course;
    }
    // In the event there is a linked list, begin searching through it
    while (current != nullptr) {
        if (current->course.courseNumber == courseNumber) {
            return current->course;
        }
        // Move to next
        current = current->next; 
    }
    // Return course
    return course;
}

// Function to print all courses in hash table in alphanumerical order
void HashTable::PrintAll() {
    // Create vecotr of Course objects
    std::vector<Course> courses;
    // Begin loop through hash table, incrementing by one
    for (int i = 0; i < table.size(); ++i) {
        // Set current node's pointer to address of table index i
        Node* current = &table[i];
        // Begin loop as long as curent does not equal nullptr
        while (current != nullptr) {
            // If the current course number is not empty
            if (!current->course.courseNumber.empty()) {
                // Push back into courses vector
                courses.push_back(current->course);
            }
            // Move current to next 
            current = current->next;
        }
    }
    // Begin sorting courses vector 
    std::sort(courses.begin(), courses.end(), [](const Course& a, const Course& b) {
        return (a.courseNumber < b.courseNumber);
        });
    // For each course within the Courses vector, print course information
    for (const Course& course : courses) {
        std::cout << "Course Number: " << course.courseNumber << std::endl;
        std::cout << "Course Name: " << course.courseName << std::endl;
        std::cout << "Prerequisites: ";
        // If course has prerequisites,
        if (!course.prereqs.empty()) {
            // For each prerequisite within the course's prerequisites, 
            for (const std::string& prereq : course.prereqs) {
                // Print prerequisite followed by space
                std::cout << prereq << " ";
            }
            std::cout << std::endl;
        }
        // Otherwise, if course does not have prerequisites, display None. 
        else {
            std::cout << "None" << std::endl;
        }
        std::cout << "" << std::endl;
    }
}

bool HashTable::DupeCheck(const std::string& courseNumber) {
    // Create hash value for key using course number
    unsigned int key = hash(courseNumber);
    // Set current to the address of the key's index in table
    Node* current = &table[key];
    // While curren is not nullptr
    while (current != nullptr) {
        // If the current course number matches its course number
        if (current->course.courseNumber == courseNumber) {
            // Return true, course exists
            throw std::runtime_error("Duplicate found, Course " + courseNumber + " already exists.");
            return true;
        }
        // Move to next
        current = current->next;
    }
    // Otherwise, raise error indicating course number does not exist    
    return false;
}

// Parses each line in text file, checking if any fields are empty and validating any listed prerequisites
Course parseCourseData(const std::string& line, HashTable* courseTable) {
    Course course;
    std::stringstream ss(line);
    std::string field;

    // Takes in line and reviews first field, course number
    std::getline(ss, field, ',');
    if (field.empty()) {
        throw std::runtime_error("Course Number Missing"); // If course number field is empty, throw error
    }
    // Assign current field to courseNumber
    course.courseNumber = field;

    // Takes in line and review next field, course name
    std::getline(ss, field, ',');
    if (field.empty()) {
        throw std::runtime_error("Course Name Missing"); // If course name field is empty, throw error
    }
    // Assign current field to courseName
    course.courseName = field;

    // Begins loop to review remaining fields in line to review prerequisites
    while (std::getline(ss, field, ',')) {
        course.prereqs.push_back(field);
    }

    // If course.prereqs vector is empty, return course.
    if (course.prereqs.empty()) {
        course.incomplete = false;
        return course;
    }

    // Otherwise, if course.prereqs vector is not empty, set incomplete flag to true for second pass
    else {
        course.incomplete = true;
    }
    // return course obejct
    return course;
}

// Function to display menu options
void displayMenu() {
    std::cout << "Main Menu" << std::endl;
    std::cout << "1. Load Courses" << std::endl;
    std::cout << "2. Print Courses List" << std::endl;
    std::cout << "3. Print Course Info" << std::endl;
    std::cout << "9. Exit Program" << std::endl;
    std::cout << "Please enter choice: " << std::endl;
}

// Function to display course information.
void displayCourse(Course course) {
    std::cout << "Course Number: " << course.courseNumber << std::endl;
    std::cout << "Course Name: " << course.courseName << std::endl;
    std::cout << "Prerequisites: ";
    // Check if course has prerequisites or not, if it does, loop through them
    if (!course.prereqs.empty()) {
        // For each prerequisite in the course's prerequisites 
        for (const std::string& prereq : course.prereqs) {
            std::cout << prereq << " "; // Print prerequisite
        }
        std::cout << std::endl;
    }
    // Otherwise, if course has no prerequisites, display None
    else {
        std::cout << "None" << std::endl;
    }
    std::cout << "" << std::endl;
}

void loadCoursesFromFile(HashTable* courseTable, std::ifstream& file) {
    std::string line;
    Course course;
    if (file.is_open()) {
        // Removes BOM characters at beginning of file to ensure first course number can be searched for without issue
        char bom[3];
        file.read(bom, 3);

        if (bom[0] == '\xef' && bom[1] == '\xbb' && bom[2] == '\xbf') {
            // Skip the BOM
            file.seekg(3);
        }
        std::cout << "Loading courses from file" << std::endl;
        // Gets line for parsing
        while (std::getline(file, line)) {
            try {
                Course course = parseCourseData(line, courseTable); // Call parseCourseData function with current line and setting it to Course object
                // Check if course is a duplicate by calling DupeCheck Method
                if (!(courseTable->DupeCheck(course.courseNumber))) {
                    // If course is not a duplicate, insert it into the Hash Table
                    courseTable->Insert(course);
                }
            }
            // Catch errors if parsing could not be completed
            catch (const std::exception& e) {
                std::cerr << "Error parsing line: " << line << std::endl;
                std::cerr << e.what() << std::endl;
            }
        }
        // Access ValidatePrereq method for second pass to ensure prerequisites are all valid
        courseTable->ValidatePrereq();
    }
    // If unable to open file, provide error message
    else {
        std::cerr << "Error opening file." << std::endl;
        return;
    }
    // Display success message
    std::cout << "Files loaded successfully" << std::endl;
}

int main()
{   
    HashTable* courseTable;
    courseTable = new HashTable();

    std::string fileName;
    std::cout << "Please enter name of file to open: " << std::endl;
    std::cin >> fileName;
    std::ifstream file(fileName);

    int choice = 0;
    // Call displayMenu function to provide user with choices
    displayMenu();
    
    // Begin loop to interact with menu
    while (choice != 9) {
        std::cin >> choice;
        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input. Please enter a valid option: ";
            continue;
        }
        switch (choice) {

        case 1: {
            // Checks if file is open
            loadCoursesFromFile(courseTable, file);
            break;
        }
        case 2: {
            // Access PrintAll method 
            courseTable->PrintAll();
            break;
        }

        case 3: {
            // Initialize findCourse variable to null
            std::string findCourse = "null";
            // Prompt user to enter course number
            std::cout << "Please enter a valid course number (i.e. MATH201): " << std::endl;
            // Take user input and set as findCourse
            std::cin >> findCourse;
            // Access Search method with entered courseNumber, set to course
            Course course = courseTable->Search(findCourse);
            // Check if course number is empty
            if (!course.courseNumber.empty()) {
                displayCourse(course); // Call displayCourse function passing course
            }
            // If enetered course couldn't be found, display error message
            else {
                std::cout << "Course Number: " << course.courseNumber << " not found. Please enter valid course number." << std::endl;
            }
            break;
        }
        // Delete courseTable when user exits program
        case 9: {
            delete courseTable;
            return 0;
        }
        // Default option if user select invalid option.
        default: 
            std::cout << "Invalid choice. Please Enter an available option." << std::endl;
            break;
        }
    }
    // Close file 
    file.close();
    return 0;
}


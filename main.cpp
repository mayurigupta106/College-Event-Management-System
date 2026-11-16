
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <limits>
using namespace std;

// Structure to store event details
struct Event {
    int id;
    string name;
    string date;
    string venue;
    string category;
};

// Structure to store student registration details
struct Registration {
    int studentId;
    string studentName;
    string email;
    int eventId;
};

// Global vectors
vector<Event> events;
vector<Registration> registrations;

// Load events from file
void loadEvents() {
    ifstream file("data/events.txt");
    Event e;

    while (file >> e.id) {
        file.ignore();
        getline(file, e.name, '|');
        getline(file, e.date, '|');
        getline(file, e.venue, '|');
        getline(file, e.category);

        events.push_back(e);
    }

    file.close();
}

// Load registrations from file
void loadRegistrations() {
    ifstream file("data/registrations.txt");
    Registration r;

    while (file >> r.studentId) {
        file.ignore();
        getline(file, r.studentName, '|');
        getline(file, r.email, '|');
        file >> r.eventId;
        file.ignore(numeric_limits<streamsize>::max(), '\n');

        registrations.push_back(r);
    }

    file.close();
}

// Save all events to file
void saveEvents() {
    ofstream file("data/events.txt");

    for (const Event &e : events) {
        file << e.id << '\n'
             << e.name << '|'
             << e.date << '|'
             << e.venue << '|'
             << e.category << '\n';
    }

    file.close();
}

// Save all registrations to file
void saveRegistrations() {
    ofstream file("data/registrations.txt");

    for (const Registration &r : registrations) {
        file << r.studentId << '\n'
             << r.studentName << '|'
             << r.email << '|'
             << r.eventId << '\n';
    }

    file.close();
}

// Check whether event ID exists
bool eventExists(int id) {
    for (const Event &e : events) {
        if (e.id == id) {
            return true;
        }
    }
    return false;
}

// Add a new event
void addEvent() {
    Event e;

    cout << "\nEnter Event ID: ";
    cin >> e.id;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    if (eventExists(e.id)) {
        cout << "Event ID already exists!\n";
        return;
    }

    cout << "Enter Event Name: ";
    getline(cin, e.name);

    cout << "Enter Event Date: ";
    getline(cin, e.date);

    cout << "Enter Event Venue: ";
    getline(cin, e.venue);

    cout << "Enter Event Category: ";
    getline(cin, e.category);

    events.push_back(e);
    saveEvents();

    cout << "\nEvent added successfully!\n";
}

// Display all events
void viewEvents() {
    if (events.empty()) {
        cout << "\nNo events available.\n";
        return;
    }

    cout << "\n========== COLLEGE EVENTS ==========\n";

    for (const Event &e : events) {
        cout << "\nEvent ID: " << e.id;
        cout << "\nName: " << e.name;
        cout << "\nDate: " << e.date;
        cout << "\nVenue: " << e.venue;
        cout << "\nCategory: " << e.category;
        cout << "\n-----------------------------------\n";
    }
}

// Search event by ID
void searchEvent() {
    int id;

    cout << "\nEnter Event ID to search: ";
    cin >> id;

    for (const Event &e : events) {
        if (e.id == id) {
            cout << "\nEvent Found!\n";
            cout << "Name: " << e.name << '\n';
            cout << "Date: " << e.date << '\n';
            cout << "Venue: " << e.venue << '\n';
            cout << "Category: " << e.category << '\n';
            return;
        }
    }

    cout << "\nEvent not found!\n";
}

// Register a student
void registerStudent() {
    Registration r;

    if (events.empty()) {
        cout << "\nNo events available for registration.\n";
        return;
    }

    cout << "\nEnter Student ID: ";
    cin >> r.studentId;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    cout << "Enter Student Name: ";
    getline(cin, r.studentName);

    cout << "Enter Email: ";
    getline(cin, r.email);

    cout << "Enter Event ID: ";
    cin >> r.eventId;

    if (!eventExists(r.eventId)) {
        cout << "\nInvalid Event ID!\n";
        return;
    }

    for (const Registration &existing : registrations) {
        if (existing.studentId == r.studentId &&
            existing.eventId == r.eventId) {
            cout << "\nStudent already registered for this event!\n";
            return;
        }
    }

    registrations.push_back(r);
    saveRegistrations();

    cout << "\nStudent registered successfully!\n";
}

// Display all registrations
void viewRegistrations() {
    if (registrations.empty()) {
        cout << "\nNo registrations found.\n";
        return;
    }

    cout << "\n======= STUDENT REGISTRATIONS =======\n";

    for (const Registration &r : registrations) {
        cout << "\nStudent ID: " << r.studentId;
        cout << "\nStudent Name: " << r.studentName;
        cout << "\nEmail: " << r.email;
        cout << "\nEvent ID: " << r.eventId;
        cout << "\n------------------------------------\n";
    }
}

// Main function
int main() {

    loadEvents();
    loadRegistrations();

    int choice;

    do {
        cout << "\n\n====================================";
        cout << "\n   COLLEGE EVENT MANAGEMENT SYSTEM";
        cout << "\n====================================";
        cout << "\n1. Add Event";
        cout << "\n2. View All Events";
        cout << "\n3. Search Event";
        cout << "\n4. Register Student";
        cout << "\n5. View Registrations";
        cout << "\n6. Exit";
        cout << "\nEnter your choice: ";

        if (!(cin >> choice)) {
            cout << "\nInvalid input! Please enter a number.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        switch (choice) {
            case 1:
                addEvent();
                break;

            case 2:
                viewEvents();
                break;

            case 3:
                searchEvent();
                break;

            case 4:
                registerStudent();
                break;

            case 5:
                viewRegistrations();
                break;

            case 6:
                cout << "\nThank you for using our system!\n";
                break;

            default:
                cout << "\nInvalid choice! Try again.\n";
        }

    } while (choice != 6);

    return 0;
}

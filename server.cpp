
#include "httplib.h"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <limits>
#include <cctype>
#include <stdexcept>
#include <algorithm>
#include <cstdio>

using namespace std;

struct Event {
    int id;
    string name;
    string date;
    string venue;
    string category;
};

// JSON special character handling
string jsonEscape(const string& value) {
    string result;

    for (char ch : value) {
        if (ch == '"' || ch == '\\') {
            result += '\\';
            result += ch;
        }
        else if (ch == '\n') result += "\\n";
        else if (ch == '\r') result += "\\r";
        else if (ch == '\t') result += "\\t";
        else result += ch;
    }

    return result;
}

// Validate fields
bool isSafeField(const string& value) {
    return !value.empty() &&
           value.find('|') == string::npos &&
           value.find('\n') == string::npos &&
           value.find('\r') == string::npos;
}

// Validate roll number
bool isValidRollNumber(const string& roll) {
    if (roll.empty()) return false;

    for (char ch : roll) {
        if (!isdigit(static_cast<unsigned char>(ch))) {
            return false;
        }
    }

    return true;
}

// Validate email address
bool isValidEmail(const string& email) {
    if (email.empty()) return false;

    if (email.find(' ') != string::npos) return false;

    size_t at = email.find('@');

    if (at == string::npos) return false;
    if (email.find('@', at + 1) != string::npos) return false;

    if (at == 0 || at == email.length() - 1) return false;

    size_t dot = email.find('.', at + 1);

    if (dot == string::npos) return false;
    if (dot == at + 1) return false;
    if (dot == email.length() - 1) return false;

    return true;
}

// Read all events
vector<Event> loadEvents() {
    vector<Event> events;
    ifstream file("data/events.txt");

    int id;
    string name, date, venue, category;

    while (file >> id) {
        file.ignore(numeric_limits<streamsize>::max(), '\n');

        if (!getline(file, name, '|')) break;
        if (!getline(file, date, '|')) break;
        if (!getline(file, venue, '|')) break;
        if (!getline(file, category)) break;

        events.push_back({id, name, date, venue, category});
    }

    return events;
}

// Save all events
bool saveEvents(const vector<Event>& events) {
    ofstream file("data/events.tmp");

    if (!file.is_open()) return false;

    for (const Event& event : events) {
        file << event.id << '\n';
        file << event.name << '|'
             << event.date << '|'
             << event.venue << '|'
             << event.category << '\n';
    }

    file.close();

    if (!file) return false;

    // Replace old events file
    if (remove("data/events.txt") != 0) {
        return false;
    }

    if (rename("data/events.tmp", "data/events.txt") != 0) {
        return false;
    }

    return true;
}

// Find event ID by name
int findEventID(const string& eventName) {
    vector<Event> events = loadEvents();

    for (const Event& event : events) {
        if (event.name == eventName) return event.id;
    }

    return -1;
}

// Find event name by ID
string findEventName(int eventId) {
    vector<Event> events = loadEvents();

    for (const Event& event : events) {
        if (event.id == eventId) return event.name;
    }

    return "Unknown Event";
}

// Check duplicate registration
bool isAlreadyRegistered(int studentId, int eventId) {
    ifstream file("data/registrations.txt");

    if (!file.is_open()) return false;

    int savedStudentId, savedEventId;
    string name, email;

    while (file >> savedStudentId) {
        file.ignore(numeric_limits<streamsize>::max(), '\n');

        if (!getline(file, name, '|')) break;
        if (!getline(file, email, '|')) break;
        if (!(file >> savedEventId)) break;

        file.ignore(numeric_limits<streamsize>::max(), '\n');

        if (savedStudentId == studentId &&
            savedEventId == eventId) {
            return true;
        }
    }

    return false;
}

// Check whether event has registrations
bool hasRegistrations(int eventId) {
    ifstream file("data/registrations.txt");

    if (!file.is_open()) return false;

    int studentId, savedEventId;
    string name, email;

    while (file >> studentId) {
        file.ignore(numeric_limits<streamsize>::max(), '\n');

        if (!getline(file, name, '|')) break;
        if (!getline(file, email, '|')) break;
        if (!(file >> savedEventId)) break;

        file.ignore(numeric_limits<streamsize>::max(), '\n');

        if (savedEventId == eventId) return true;
    }

    return false;
}

int main() {

    httplib::Server server;

    server.set_default_headers({
        {"Access-Control-Allow-Origin", "*"},
        {"Access-Control-Allow-Methods", "GET, POST, OPTIONS"},
        {"Access-Control-Allow-Headers", "Content-Type"}
    });

    // ======================================
    // API 1: GET ALL EVENTS
    // ======================================

    server.Get("/events", [](const httplib::Request& req,
                             httplib::Response& res) {

        vector<Event> events = loadEvents();

        string json = "[";
        bool first = true;

        for (const Event& event : events) {
            if (!first) json += ",";

            json += "{\"id\":" + to_string(event.id) +
                    ",\"name\":\"" + jsonEscape(event.name) +
                    "\",\"date\":\"" + jsonEscape(event.date) +
                    "\",\"venue\":\"" + jsonEscape(event.venue) +
                    "\",\"category\":\"" + jsonEscape(event.category) + "\"}";

            first = false;
        }

        json += "]";
        res.set_content(json, "application/json");
    });

    // ======================================
    // API 2: ADD NEW EVENT
    // ======================================

    server.Post("/events", [](const httplib::Request& req,
                              httplib::Response& res) {

        if (!req.has_param("name") ||
            !req.has_param("date") ||
            !req.has_param("venue") ||
            !req.has_param("category")) {

            res.status = 400;
            res.set_content("Please fill all event fields.", "text/plain");
            return;
        }

        string name = req.get_param_value("name");
        string date = req.get_param_value("date");
        string venue = req.get_param_value("venue");
        string category = req.get_param_value("category");

        if (!isSafeField(name) || !isSafeField(date) ||
            !isSafeField(venue) || !isSafeField(category)) {

            res.status = 400;
            res.set_content("Invalid event details.", "text/plain");
            return;
        }

        vector<Event> events = loadEvents();
        int nextId = 101;

        for (const Event& event : events) {
            nextId = max(nextId, event.id + 1);

            if (event.name == name) {
                res.status = 409;
                res.set_content("Event already exists.", "text/plain");
                return;
            }
        }

        events.push_back({nextId, name, date, venue, category});

        if (!saveEvents(events)) {
            res.status = 500;
            res.set_content("Unable to save event.", "text/plain");
            return;
        }

        res.set_content("New event added successfully!", "text/plain");
    });

    // ======================================
    // API 3: EDIT EVENT
    // ======================================

    server.Post("/events/edit", [](const httplib::Request& req,
                                   httplib::Response& res) {

        if (!req.has_param("id") ||
            !req.has_param("name") ||
            !req.has_param("date") ||
            !req.has_param("venue") ||
            !req.has_param("category")) {

            res.status = 400;
            res.set_content("Missing event details.", "text/plain");
            return;
        }

        int id;

        try {
            id = stoi(req.get_param_value("id"));
        }
        catch (const exception&) {
            res.status = 400;
            res.set_content("Invalid event ID.", "text/plain");
            return;
        }

        string name = req.get_param_value("name");
        string date = req.get_param_value("date");
        string venue = req.get_param_value("venue");
        string category = req.get_param_value("category");

        if (!isSafeField(name) || !isSafeField(date) ||
            !isSafeField(venue) || !isSafeField(category)) {

            res.status = 400;
            res.set_content("Invalid event details.", "text/plain");
            return;
        }

        vector<Event> events = loadEvents();
        bool found = false;

        for (Event& event : events) {
            if (event.id == id) {
                found = true;
                continue;
            }

            if (event.name == name) {
                res.status = 409;
                res.set_content("Another event has this name.", "text/plain");
                return;
            }
        }

        if (!found) {
            res.status = 404;
            res.set_content("Event not found.", "text/plain");
            return;
        }

        for (Event& event : events) {
            if (event.id == id) {
                event.name = name;
                event.date = date;
                event.venue = venue;
                event.category = category;
                break;
            }
        }

        if (!saveEvents(events)) {
            res.status = 500;
            res.set_content("Unable to update event.", "text/plain");
            return;
        }

        res.set_content("Event updated successfully!", "text/plain");
    });

    // ======================================
    // API 4: DELETE EVENT
    // ======================================

    server.Post("/events/delete", [](const httplib::Request& req,
                                     httplib::Response& res) {

        if (!req.has_param("id")) {
            res.status = 400;
            res.set_content("Event ID is required.", "text/plain");
            return;
        }

        int id;

        try {
            id = stoi(req.get_param_value("id"));
        }
        catch (const exception&) {
            res.status = 400;
            res.set_content("Invalid event ID.", "text/plain");
            return;
        }

        if (hasRegistrations(id)) {
            res.status = 409;
            res.set_content(
                "This event has student registrations and cannot be deleted.",
                "text/plain"
            );
            return;
        }

        vector<Event> events = loadEvents();
        auto oldSize = events.size();

        events.erase(
            remove_if(events.begin(), events.end(),
                [id](const Event& event) {
                    return event.id == id;
                }),
            events.end()
        );

        if (events.size() == oldSize) {
            res.status = 404;
            res.set_content("Event not found.", "text/plain");
            return;
        }

        if (!saveEvents(events)) {
            res.status = 500;
            res.set_content("Unable to delete event.", "text/plain");
            return;
        }

        res.set_content("Event deleted successfully!", "text/plain");
    });

    // ======================================
    // API 5: STUDENT REGISTRATION
    // ======================================

    server.Post("/register", [](const httplib::Request& req,
                                httplib::Response& res) {

        if (!req.has_param("name") ||
            !req.has_param("roll") ||
            !req.has_param("email") ||
            !req.has_param("event")) {

            res.status = 400;
            res.set_content("Please fill all registration fields.",
                            "text/plain");
            return;
        }

        string name = req.get_param_value("name");
        string roll = req.get_param_value("roll");
        string email = req.get_param_value("email");
        string event = req.get_param_value("event");

        // Validate student details
        if (!isSafeField(name) ||
            !isValidRollNumber(roll) ||
            !isValidEmail(email) ||
            !isSafeField(event)) {

            res.status = 400;
            res.set_content(
                "Invalid details! Please enter a valid name, roll number, email and event.",
                "text/plain"
            );
            return;
        }

        int studentId;

        try {
            studentId = stoi(roll);
        }
        catch (const exception&) {
            res.status = 400;
            res.set_content("Invalid roll number.", "text/plain");
            return;
        }

        int eventId = findEventID(event);

        if (eventId == -1) {
            res.status = 404;
            res.set_content("Event not found.", "text/plain");
            return;
        }

        // Prevent duplicate registration
        if (isAlreadyRegistered(studentId, eventId)) {
            res.status = 409;
            res.set_content(
                "You have already registered for this event!",
                "text/plain"
            );
            return;
        }

        ofstream file("data/registrations.txt", ios::app);

        if (!file.is_open()) {
            res.status = 500;
            res.set_content("Unable to save registration.", "text/plain");
            return;
        }

        file << studentId << '\n';
        file << name << "|" << email << "|" << eventId << '\n';
        file.close();

        if (!file) {
            res.status = 500;
            res.set_content("Unable to save registration.", "text/plain");
            return;
        }

        res.set_content("Registration saved successfully!", "text/plain");
    });

    // ======================================
    // API 6: GET ALL REGISTRATIONS
    // ======================================

    server.Get("/registrations", [](const httplib::Request& req,
                                    httplib::Response& res) {

        ifstream file("data/registrations.txt");

        if (!file.is_open()) {
            res.status = 500;
            res.set_content("Unable to open registrations file.",
                            "text/plain");
            return;
        }

        string json = "[";
        bool first = true;

        int studentId, eventId;
        string name, email;

        while (file >> studentId) {
            file.ignore(numeric_limits<streamsize>::max(), '\n');

            if (!getline(file, name, '|')) break;
            if (!getline(file, email, '|')) break;
            if (!(file >> eventId)) break;

            file.ignore(numeric_limits<streamsize>::max(), '\n');

            string eventName = findEventName(eventId);

            if (!first) json += ",";

            json += "{\"roll\":\"" + to_string(studentId) +
                    "\",\"name\":\"" + jsonEscape(name) +
                    "\",\"email\":\"" + jsonEscape(email) +
                    "\",\"eventId\":" + to_string(eventId) +
                    ",\"eventName\":\"" + jsonEscape(eventName) + "\"}";

            first = false;
        }

        file.close();
        json += "]";

        res.set_content(json, "application/json");
    });

    // ======================================
    // START SERVER
    // ======================================

    cout << "======================================" << endl;
    cout << " College Event Management System" << endl;
    cout << "======================================" << endl;
    cout << "Server running at http://localhost:8080" << endl;
    cout << "======================================" << endl;

    server.listen("localhost", 8080);

    return 0;
}

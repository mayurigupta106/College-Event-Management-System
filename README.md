
# College Event Management System

A web-based College Event Management System developed using C++, HTML, CSS and JavaScript.

## About the Project

The College Event Management System helps students register for college events and allows administrators to manage events and monitor student registrations through an admin dashboard.

## Features

- View upcoming college events
- Student event registration
- Prevent duplicate registrations
- Admin dashboard
- Add new events
- Edit existing events
- Delete events
- View student registrations
- Event-wise registration count
- Store event and registration data using text files

## Technologies Used

- C++
- HTML5
- CSS3
- JavaScript
- cpp-httplib
- Visual Studio Code

## Project Structure

```text
College-Event-Management-System/
│
├── main.cpp
├── server.cpp
├── httplib.h
├── data/
│   └── events.txt
│
└── frontend/
    ├── index.html
    ├── style.css
    └── script.js
```

## How to Run the Project

### Requirements

- Windows
- GCC / MinGW compiler
- Visual Studio Code
- Live Server extension

### Step 1: Compile the C++ Server

Open the project folder in VS Code and run:

```bash
g++ server.cpp -o server.exe -lws2_32
```

### Step 2: Start the Backend

```bash
.\server.exe
```

The backend server runs at:

```text
http://localhost:8080
```

### Step 3: Start the Frontend

1. Open `frontend/index.html`.
2. Right-click and select Open with Live Server.
3. The website will open in your browser.

Keep the C++ server running while using the website.

## Data Storage

The project uses text files for storing:

- Event information: `data/events.txt`
- Student registrations: `data/registrations.txt`

Student registration data is excluded from the public Git repository.

## Future Improvements

- Admin login authentication
- Email confirmation
- Event search and filtering
- Database integration
- Deployment on a live server

## Author

Developed as an academic mini project for B.Tech CSE/IT.

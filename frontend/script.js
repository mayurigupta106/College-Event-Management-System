
// ==========================================
// COLLEGE EVENT MANAGEMENT SYSTEM
// COMPLETE JAVASCRIPT
// ==========================================

let allEvents = [];
let allRegistrations = [];
let registrationCounts = {};

// Safely display text
function escapeHTML(value) {
    return String(value ?? "").replace(/[&<>"']/g, function(character) {
        const entities = {
            "&": "&amp;",
            "<": "&lt;",
            ">": "&gt;",
            '"': "&quot;",
            "'": "&#39;"
        };

        return entities[character];
    });
}

// Format date
function formatDate(dateString) {
    const parts = dateString.split("-");

    if (parts.length !== 3) return dateString;

    const date = new Date(
        Number(parts[0]),
        Number(parts[1]) - 1,
        Number(parts[2])
    );

    return date.toLocaleDateString("en-IN", {
        day: "numeric",
        month: "long",
        year: "numeric"
    });
}

// Open registration form
function registerEvent(eventName) {
    const modal = document.getElementById("registrationModal");
    const select = document.getElementById("eventName");

    if (select) select.value = eventName;
    if (modal) modal.style.display = "flex";
}

// Close registration form
function closeForm() {
    const modal = document.getElementById("registrationModal");

    if (modal) modal.style.display = "none";
}

// ==========================================
// LOAD EVENTS
// ==========================================

async function loadEvents() {
    try {
        const response = await fetch("http://localhost:8080/events");

        if (!response.ok) {
            throw new Error("Unable to load events");
        }

        allEvents = await response.json();

        displayEvents();
        updateEventDropdown();
        displayManageEvents();
        updateEventStatistics();

    } catch (error) {
        console.error("Events Error:", error);

        const container = document.getElementById("eventContainer");

        if (container) {
            container.innerHTML =
                "<p>Unable to load events. Please start the server.</p>";
        }
    }
}

// Display event cards
function displayEvents() {
    const container = document.getElementById("eventContainer");

    if (!container) return;

    container.innerHTML = "";

    if (allEvents.length === 0) {
        container.innerHTML = "<p>No events available.</p>";
        return;
    }

    allEvents.forEach(function(event) {
        const card = document.createElement("div");
        card.className = "event-card";

        const heading = document.createElement("h3");
        heading.textContent = event.name;

        const date = document.createElement("p");
        date.textContent = "Date: " + formatDate(event.date);

        const venue = document.createElement("p");
        venue.textContent = "Venue: " + event.venue;

        const category = document.createElement("p");
        category.textContent = "Category: " + event.category;

        const button = document.createElement("button");
        button.type = "button";
        button.textContent = "Register Now";

        button.addEventListener("click", function() {
            registerEvent(event.name);
        });

        card.append(heading, date, venue, category, button);
        container.appendChild(card);
    });
}

// Update registration dropdown
function updateEventDropdown() {
    const select = document.getElementById("eventName");

    if (!select) return;

    const previousValue = select.value;

    select.innerHTML = '<option value="">Choose an event</option>';

    allEvents.forEach(function(event) {
        const option = document.createElement("option");
        option.value = event.name;
        option.textContent = event.name;
        select.appendChild(option);
    });

    if (allEvents.some(event => event.name === previousValue)) {
        select.value = previousValue;
    }
}

// ==========================================
// UPDATE EVENT STATISTICS
// ==========================================

function updateEventStatistics() {
    const totalEvents = document.getElementById("totalEvents");
    const upcomingEvents = document.getElementById("upcomingEvents");

    const today = new Date();

    const todayString =
        today.getFullYear() + "-" +
        String(today.getMonth() + 1).padStart(2, "0") + "-" +
        String(today.getDate()).padStart(2, "0");

    const upcomingCount = allEvents.filter(function(event) {
        return event.date >= todayString;
    }).length;

    if (totalEvents) {
        totalEvents.textContent = allEvents.length;
    }

    if (upcomingEvents) {
        upcomingEvents.textContent = upcomingCount;
    }
}

// ==========================================
// EVENT-WISE REGISTRATION COUNT
// ==========================================

function updateRegistrationCounts(registrations) {
    registrationCounts = {};

    registrations.forEach(function(student) {
        const eventId = Number(student.eventId);

        if (!registrationCounts[eventId]) {
            registrationCounts[eventId] = 0;
        }

        registrationCounts[eventId]++;
    });
}

// ==========================================
// DISPLAY EVENTS IN ADMIN TABLE
// ==========================================

function displayManageEvents() {
    const table = document.getElementById("manageEventsTable");

    if (!table) return;

    table.innerHTML = "";

    if (allEvents.length === 0) {
        table.innerHTML = `
            <tr><td colspan="7">No events available.</td></tr>
        `;
        return;
    }

    allEvents.forEach(function(event) {
        const row = document.createElement("tr");

        const values = [
            event.id,
            event.name,
            formatDate(event.date),
            event.venue,
            event.category
        ];

        values.forEach(function(value) {
            const cell = document.createElement("td");
            cell.textContent = value;
            row.appendChild(cell);
        });

        // Registration count column
        const countCell = document.createElement("td");

        countCell.textContent =
            registrationCounts[event.id] || 0;

        row.appendChild(countCell);

        // Action buttons
        const actionCell = document.createElement("td");

        const editButton = document.createElement("button");
        editButton.type = "button";
        editButton.textContent = "Edit";
        editButton.className = "event-action-btn";

        editButton.addEventListener("click", function() {
            editEvent(event.id);
        });

        const deleteButton = document.createElement("button");
        deleteButton.type = "button";
        deleteButton.textContent = "Delete";
        deleteButton.className = "event-action-btn delete-btn";

        deleteButton.addEventListener("click", function() {
            deleteEvent(event.id);
        });

        actionCell.append(editButton, deleteButton);
        row.appendChild(actionCell);

        table.appendChild(row);
    });
}

// ==========================================
// ADD NEW EVENT
// ==========================================

const addEventForm = document.getElementById("addEventForm");

if (addEventForm) {
    addEventForm.addEventListener("submit", async function(event) {
        event.preventDefault();

        const newEvent = {
            name: document.getElementById("newEventName").value.trim(),
            date: document.getElementById("newEventDate").value,
            venue: document.getElementById("newEventVenue").value.trim(),
            category: document.getElementById("newEventCategory").value
        };

        try {
            const response = await fetch("http://localhost:8080/events", {
                method: "POST",
                headers: {
                    "Content-Type": "application/x-www-form-urlencoded"
                },
                body: new URLSearchParams(newEvent)
            });

            const message = await response.text();

            if (!response.ok) {
                alert("Unable to add event:\n" + message);
                return;
            }

            alert(message);
            addEventForm.reset();

            await loadEvents();

        } catch (error) {
            console.error(error);
            alert("Cannot connect to server.");
        }
    });
}

// ==========================================
// EDIT EVENT
// ==========================================

function editEvent(eventId) {
    const selectedEvent = allEvents.find(function(event) {
        return event.id === eventId;
    });

    if (!selectedEvent) {
        alert("Event not found.");
        return;
    }

    document.getElementById("editEventId").value = selectedEvent.id;
    document.getElementById("editEventName").value = selectedEvent.name;
    document.getElementById("editEventDate").value = selectedEvent.date;
    document.getElementById("editEventVenue").value = selectedEvent.venue;
    document.getElementById("editEventCategory").value =
        selectedEvent.category;

    const editSection = document.getElementById("editEventSection");

    editSection.style.display = "block";
    editSection.scrollIntoView({ behavior: "smooth" });
}

// Cancel editing
function cancelEdit() {
    const editSection = document.getElementById("editEventSection");

    if (editSection) editSection.style.display = "none";

    const form = document.getElementById("editEventForm");

    if (form) form.reset();
}

// Submit edited event
const editEventForm = document.getElementById("editEventForm");

if (editEventForm) {
    editEventForm.addEventListener("submit", async function(event) {
        event.preventDefault();

        const updatedEvent = {
            id: document.getElementById("editEventId").value,
            name: document.getElementById("editEventName").value.trim(),
            date: document.getElementById("editEventDate").value,
            venue: document.getElementById("editEventVenue").value.trim(),
            category: document.getElementById("editEventCategory").value
        };

        try {
            const response = await fetch(
                "http://localhost:8080/events/edit",
                {
                    method: "POST",
                    headers: {
                        "Content-Type": "application/x-www-form-urlencoded"
                    },
                    body: new URLSearchParams(updatedEvent)
                }
            );

            const message = await response.text();

            if (!response.ok) {
                alert("Unable to update event:\n" + message);
                return;
            }

            alert(message);
            cancelEdit();

            await loadEvents();
            await renderDashboard();

        } catch (error) {
            console.error(error);
            alert("Cannot connect to server.");
        }
    });
}

// ==========================================
// DELETE EVENT
// ==========================================

async function deleteEvent(eventId) {
    const selectedEvent = allEvents.find(function(event) {
        return event.id === eventId;
    });

    if (!selectedEvent) return;

    const confirmation = confirm(
        "Are you sure you want to delete " +
        selectedEvent.name + "?"
    );

    if (!confirmation) return;

    try {
        const response = await fetch(
            "http://localhost:8080/events/delete",
            {
                method: "POST",
                headers: {
                    "Content-Type": "application/x-www-form-urlencoded"
                },
                body: new URLSearchParams({ id: eventId })
            }
        );

        const message = await response.text();

        if (!response.ok) {
            alert("Unable to delete event:\n" + message);
            return;
        }

        alert(message);

        await loadEvents();
        await renderDashboard();

    } catch (error) {
        console.error(error);
        alert("Cannot connect to server.");
    }
}

// ==========================================
// REGISTRATION DASHBOARD
// ==========================================

async function renderDashboard() {
    try {
        const response = await fetch(
            "http://localhost:8080/registrations"
        );

        if (!response.ok) {
            throw new Error("Unable to fetch registrations");
        }

        const registrations = await response.json();

        // Store registrations
        allRegistrations = registrations;

        // Calculate event-wise counts
        updateRegistrationCounts(registrations);

        // Refresh event management table
        displayManageEvents();

        const table = document.getElementById("registrationTable");

        const totalRegistrations =
            document.getElementById("totalRegistrations");

        if (!table) return;

        table.innerHTML = "";

        if (registrations.length === 0) {
            table.innerHTML = `
                <tr><td colspan="4">No registrations yet.</td></tr>
            `;
        } else {
            registrations.forEach(function(student) {
                const row = document.createElement("tr");

                [
                    student.name,
                    student.roll,
                    student.email,
                    student.eventName
                ].forEach(function(value) {
                    const cell = document.createElement("td");
                    cell.textContent = value ?? "";
                    row.appendChild(cell);
                });

                table.appendChild(row);
            });
        }

        if (totalRegistrations) {
            totalRegistrations.textContent = registrations.length;
        }

    } catch (error) {
        console.error("Dashboard Error:", error);

        const table = document.getElementById("registrationTable");

        if (table) {
            table.innerHTML = `
                <tr>
                    <td colspan="4">Unable to load registrations.</td>
                </tr>
            `;
        }
    }
}

// ==========================================
// STUDENT REGISTRATION
// ==========================================

const registrationForm = document.getElementById("registrationForm");

if (registrationForm) {
    registrationForm.addEventListener("submit", async function(event) {
        event.preventDefault();

        const student = {
            name: document.getElementById("studentName").value.trim(),
            roll: document.getElementById("rollNumber").value.trim(),
            email: document.getElementById("studentEmail").value.trim(),
            event: document.getElementById("eventName").value
        };

        try {
            const response = await fetch("http://localhost:8080/register", {
                method: "POST",
                headers: {
                    "Content-Type": "application/x-www-form-urlencoded"
                },
                body: new URLSearchParams(student)
            });

            const message = await response.text();

            if (response.ok) {
                alert("Registration Successful!\n" + message);

                registrationForm.reset();
                closeForm();

                await renderDashboard();

            } else {
                alert("Registration Failed!\n" + message);
            }

        } catch (error) {
            console.error(error);
            alert("Cannot connect to server.");
        }
    });
}

// ==========================================
// PAGE INITIALIZATION
// ==========================================

document.addEventListener("DOMContentLoaded", function() {
    loadEvents();
    renderDashboard();

    const today = new Date();

    const minDate =
        today.getFullYear() + "-" +
        String(today.getMonth() + 1).padStart(2, "0") + "-" +
        String(today.getDate()).padStart(2, "0");

    const addDate = document.getElementById("newEventDate");
    const editDate = document.getElementById("editEventDate");

    if (addDate) addDate.min = minDate;
    if (editDate) editDate.min = minDate;
});

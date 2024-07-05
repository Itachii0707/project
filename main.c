#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>

#define MAX_USERS 10
#define MAX_FOODS 10
#define MAX_CABS 3
#define MAX_HISTORY 50

typedef struct {
    char username[30];
    char password[20];
    char name[50];
    char email[50];
} User;

typedef struct {
    char name[50];
    float price;
    char category[30];
} Food;

typedef struct {
    char type[20];
    float pricePerKm;
} Cab;

typedef struct {
    char description[100];
    float cost;
} OrderHistory;

User users[MAX_USERS];
int userCount = 0;
User* currentUser = NULL;

Food foods[MAX_FOODS] = {
    {"Popcorn", 5.0, "Snacks"},
    {"Nachos", 6.5, "Snacks"},
    {"Soda", 3.0, "Drinks"},
    {"Candy", 2.0, "Snacks"},
    {"Hotdog", 4.5, "Snacks"},
    {"Burger", 7.5, "Meals"},
    {"Pizza", 8.0, "Meals"},
    {"Ice Cream", 3.5, "Desserts"},
    {"Coffee", 2.5, "Drinks"},
    {"Tea", 2.0, "Drinks"}
};

Cab cabList[MAX_CABS] = {
    {"Standard", 1.0},
    {"Luxury", 2.5},
    {"SUV", 3.0}
};

OrderHistory orderHistory[MAX_HISTORY];
int historyCount = 0;
sqlite3 *db;

void addOrderToHistory(const char* description, float cost) {
    if (historyCount < MAX_HISTORY) {
        strncpy(orderHistory[historyCount].description, description, sizeof(orderHistory[historyCount].description) - 1);
        orderHistory[historyCount].description[sizeof(orderHistory[historyCount].description) - 1] = '\0'; // Ensuring null termination
        orderHistory[historyCount].cost = cost;
        historyCount++;
    } else {
        printf("Order history is full.\n");
    }
}

void saveData() {
    FILE *file = fopen("users.dat", "wb");
    if (file) {
        fwrite(&userCount, sizeof(int), 1, file);
        fwrite(users, sizeof(User), userCount, file);
        fclose(file);
    }

    file = fopen("orderHistory.dat", "wb");
    if (file) {
        fwrite(&historyCount, sizeof(int), 1, file);
        fwrite(orderHistory, sizeof(OrderHistory), historyCount, file);
        fclose(file);
    }
}

void loadData() {
    FILE *file = fopen("users.dat", "rb");
    if (file) {
        fread(&userCount, sizeof(int), 1, file);
        fread(users, sizeof(User), userCount, file);
        fclose(file);
    }

    file = fopen("orderHistory.dat", "rb");
    if (file) {
        fread(&historyCount, sizeof(int), 1, file);
        fread(orderHistory, sizeof(OrderHistory), historyCount, file);
        fclose(file);
    }
}

void registerUser() {
    if (userCount >= MAX_USERS) {
        printf("User registration limit reached.\n");
        return;
    }

    char username[30], password[20], name[50], email[50];
    printf("Enter username: ");
    scanf("%s", username);
    printf("Enter password: ");
    scanf("%s", password);
    printf("Enter name: ");
    scanf("%s", name);
    printf("Enter email: ");
    scanf("%s", email);

    for (int i = 0; i < userCount; i++) {
        if (strcmp(users[i].username, username) == 0) {
            printf("Username already taken. Please choose another.\n");
            return;
        }
    }

    strcpy(users[userCount].username, username);
    strcpy(users[userCount].password, password);
    strcpy(users[userCount].name, name);
    strcpy(users[userCount].email, email);
    userCount++;
    printf("Registration successful. Please log in.\n");
    saveData();
}

void loginUser() {
    char username[30], password[20];
    printf("Enter username: ");
    scanf("%s", username);
    printf("Enter password: ");
    scanf("%s", password);

    for (int i = 0; i < userCount; i++) {
        if (strcmp(users[i].username, username) == 0 && strcmp(users[i].password, password) == 0) {
            currentUser = &users[i];
            printf("Login successful. Welcome, %s!\n", currentUser->name);
            return;
        }
    }

    printf("Invalid username or password.\n");
}

void logoutUser() {
    if (currentUser) {
        printf("Logged out successfully. Goodbye, %s!\n", currentUser->name);
        currentUser = NULL;
    } else {
        printf("No user is currently logged in.\n");
    }
}

void viewProfile() {
    if (currentUser) {
        printf("User Profile:\n");
        printf("Name: %s\n", currentUser->name);
        printf("Email: %s\n", currentUser->email);
        printf("Username: %s\n", currentUser->username);
    } else {
        printf("No user is currently logged in.\n");
    }
}

void updateProfile() {
    if (!currentUser) {
        printf("No user is currently logged in.\n");
        return;
    }

    char name[50], email[50];
    printf("Enter new name (current: %s): ", currentUser->name);
    scanf("%s", name);
    printf("Enter new email (current: %s): ", currentUser->email);
    scanf("%s", email);

    strcpy(currentUser->name, name);
    strcpy(currentUser->email, email);
    printf("Profile updated successfully.\n");
    saveData();
}

void initializeDatabase() {
    char *errMsg = 0;
    int rc = sqlite3_open("movies.db", &db);

    if (rc) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        exit(0);
    } else {
        fprintf(stderr, "Opened database successfully\n");
    }

    const char *sql = "CREATE TABLE IF NOT EXISTS MOVIES ("
                      "ID INTEGER PRIMARY KEY AUTOINCREMENT,"
                      "NAME TEXT NOT NULL,"
                      "SHOWTIME TEXT NOT NULL,"
                      "DESCRIPTION TEXT NOT NULL,"
                      "AVAILABLETICKETS INT NOT NULL,"
                      "PRICE REAL NOT NULL,"
                      "TOTALRATINGS INT NOT NULL,"
                      "SUMRATINGS REAL NOT NULL);";

    rc = sqlite3_exec(db, sql, 0, 0, &errMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", errMsg);
        sqlite3_free(errMsg);
    } else {
        fprintf(stderr, "Table created successfully\n");
    }
}

void addMovieToDatabase(const char* name, const char* showtime, const char* description, int availableTickets, float price) {
    char *errMsg = 0;
    char sql[512];
    snprintf(sql, sizeof(sql), "INSERT INTO MOVIES (NAME, SHOWTIME, DESCRIPTION, AVAILABLETICKETS, PRICE, TOTALRATINGS, SUMRATINGS) "
                               "VALUES ('%s', '%s', '%s', %d, %.2f, 0, 0.0);", name, showtime, description, availableTickets, price);

    int rc = sqlite3_exec(db, sql, 0, 0, &errMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", errMsg);
        sqlite3_free(errMsg);
    } else {
        printf("Movie added successfully\n");
    }
}

void displayMovies() {
    sqlite3_stmt *stmt;
    const char *sql = "SELECT * FROM MOVIES";

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to fetch data: %s\n", sqlite3_errmsg(db));
        return;
    }

    printf("Available Movies:\n");
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const char *name = (const char*)sqlite3_column_text(stmt, 1);
        const char *showtime = (const char*)sqlite3_column_text(stmt, 2);
        const char *description = (const char*)sqlite3_column_text(stmt, 3);
        int availableTickets = sqlite3_column_int(stmt, 4);
        float price = (float)sqlite3_column_double(stmt, 5);
        int totalRatings = sqlite3_column_int(stmt, 6);
        float sumRatings = (float)sqlite3_column_double(stmt, 7);
        float averageRating = (totalRatings > 0) ? (sumRatings / totalRatings) : 0.0;

        printf("%d. %s (%s) - %s - $%.2f [%d tickets available] - Average Rating: %.1f\n", id, name, showtime, description, price, availableTickets, averageRating);
    }
    sqlite3_finalize(stmt);
}

void searchMovie() {
    char query[50];
    printf("Enter movie name to search: ");
    scanf("%s", query);

    sqlite3_stmt *stmt;
    char sql[512];
    snprintf(sql, sizeof(sql), "SELECT * FROM MOVIES WHERE NAME LIKE '%%%s%%'", query);

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to fetch data: %s\n", sqlite3_errmsg(db));
        return;
    }

    printf("Search Results:\n");
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const char *name = (const char*)sqlite3_column_text(stmt, 1);
        const char *showtime = (const char*)sqlite3_column_text(stmt, 2);
        const char *description = (const char*)sqlite3_column_text(stmt, 3);
        int availableTickets = sqlite3_column_int(stmt, 4);
        float price = (float)sqlite3_column_double(stmt, 5);

        printf("%d. %s (%s) - %s - $%.2f [%d tickets available]\n", id, name, showtime, description, price, availableTickets);
    }
    sqlite3_finalize(stmt);
}

void rateMovie() {
    int choice;
    float rating;
    displayMovies();
    printf("Select a movie to rate by ID: ");
    scanf("%d", &choice);

    printf("Enter rating (1.0 to 5.0): ");
    scanf("%f", &rating);
    if (rating < 1.0 || rating > 5.0) {
        printf("Invalid rating.\n");
        return;
    }

    char sql[512];
    snprintf(sql, sizeof(sql), "UPDATE MOVIES SET TOTALRATINGS = TOTALRATINGS + 1, SUMRATINGS = SUMRATINGS + %.1f WHERE ID = %d;", rating, choice);

    char *errMsg = 0;
    int rc = sqlite3_exec(db, sql, 0, 0, &errMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", errMsg);
        sqlite3_free(errMsg);
    } else {
        printf("Rating submitted successfully.\n");
    }
}

void bookMovie() {
    int choice, tickets;
    displayMovies();
    printf("Select a movie by ID: ");
    scanf("%d", &choice);
    printf("Enter number of tickets: ");
    scanf("%d", &tickets);

    sqlite3_stmt *stmt;
    char sql[512];
    snprintf(sql, sizeof(sql), "SELECT AVAILABLETICKETS, PRICE FROM MOVIES WHERE ID = %d;", choice);

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to fetch data: %s\n", sqlite3_errmsg(db));
        return;
    }

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        int availableTickets = sqlite3_column_int(stmt, 0);
        float price = (float)sqlite3_column_double(stmt, 1);

        if (tickets > availableTickets) {
            printf("Not enough tickets available.\n");
            sqlite3_finalize(stmt);
            return;
        }

        float total = tickets * price;
        snprintf(sql, sizeof(sql), "UPDATE MOVIES SET AVAILABLETICKETS = AVAILABLETICKETS - %d WHERE ID = %d;", tickets, choice);
        rc = sqlite3_exec(db, sql, 0, 0, &errMsg);
        if (rc != SQLITE_OK) {
            fprintf(stderr, "SQL error: %s\n", errMsg);
            sqlite3_free(errMsg);
        } else {
            char orderDescription[100];
            snprintf(orderDescription, sizeof(orderDescription), "Booked %d tickets for movie ID %d", tickets, choice);
            addOrderToHistory(orderDescription, total);
            printf("Booked %d tickets. Total: $%.2f\n", tickets, total);
        }
    } else {
        printf("Invalid movie ID.\n");
    }
    sqlite3_finalize(stmt);
}

void addMovie() {
    char name[50], showtime[10], description[100];
    int availableTickets;
    float price;

    printf("Enter movie name: ");
    scanf("%s", name);
    printf("Enter showtime: ");
    scanf("%s", showtime);
    printf("Enter description: ");
    scanf("%s", description);
    printf("Enter available tickets: ");
    scanf("%d", &availableTickets);
    printf("Enter price: ");
    scanf("%f", &price);

    addMovieToDatabase(name, showtime, description, availableTickets, price);
}

void removeMovie() {
    int choice;
    displayMovies();
    printf("Select a movie to remove by ID: ");
    scanf("%d", &choice);

    char *errMsg = 0;
    char sql[128];
    snprintf(sql, sizeof(sql), "DELETE FROM MOVIES WHERE ID=%d;", choice);

    int rc = sqlite3_exec(db, sql, 0, 0, &errMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", errMsg);
        sqlite3_free(errMsg);
    } else {
        printf("Movie removed successfully\n");
    }
}

void adminMenu() {
    int choice;
    do {
        printf("\n--- Admin Menu ---\n");
        printf("1. Add Movie\n");
        printf("2. Remove Movie\n");
        printf("3. View All Movies\n");
        printf("4. View All Users\n");
        printf("0. Logout\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                addMovie();
                break;
            case 2:
                removeMovie();
                break;
            case 3:
                displayMovies();
                break;
            case 4:
                for (int i = 0; i < userCount; i++) {
                    printf("%d. %s (%s) - %s\n", i + 1, users[i].username, users[i].name, users[i].email);
                }
                break;
            case 0:
                logoutUser();
                break;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    } while (choice != 0);
}

int main() {
    int choice;
    loadData();
    initializeDatabase();
    do {
        printf("\n--- Movie Booking and Food Ordering System ---\n");
        printf("1. Register\n");
        printf("2. Login\n");
        printf("3. Logout\n");
        printf("4. View Profile\n");
        printf("5. Update Profile\n");
        printf("6. Search Movie\n");
        printf("7. Rate Movie\n");
        printf("8. Book Movie Ticket\n");
        printf("9. Order Food\n");
        printf("10. Book Cab\n");
        printf("11. View Order History\n");
        printf("12. Calculate Total Expenditure\n");
        printf("13. Filter Foods by Category\n");
        printf("14. Recommend Movies\n");
        printf("15. Admin Menu\n");
        printf("0. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                registerUser();
                break;
            case 2:
                loginUser();
                break;
            case 3:
                logoutUser();
                break;
            case 4:
                viewProfile();
                break;
            case 5:
                updateProfile();
                break;
            case 6:
                searchMovie();
                break;
            case 7:
                rateMovie();
                break;
            case 8:
                bookMovie();
                break;
            case 9:
                // orderFood(); // Implement this if necessary, omitted for brevity
                break;
            case 10:
                // bookCab(); // Implement this if necessary, omitted for brevity
                break;
            case 11:
                viewOrderHistory();
                break;
            case 12:
                calculateTotalExpenditure();
                break;
            case 13:
                filterFoodsByCategory();
                break;
            case 14:
                recommendMovies();
                break;
            case 15:
                adminMenu();
                break;
            case 0:
                saveData();
                sqlite3_close(db);
                printf("Exiting the program. Goodbye!\n");
                break;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    } while (choice != 0);
    return 0;
}

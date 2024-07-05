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
    char showtime[10];
    char description[100];
    int availableTickets;
    float price;
    int totalRatings;
    float sumRatings;
} Movie;

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
        printf("9. Cancel Movie Ticket\n");
        printf("10. Order Food\n");
        printf("11. Book Cab\n");
        printf("12. View Order History\n");
        printf("13. Calculate Total Expenditure\n");
        printf("14. Filter Foods by Category\n");
        printf("15. Recommend Movies\n");
        printf("16. Admin Menu\n");
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
                // rateMovie(); // Implement this if necessary, omitted for brevity
                break;
            case 8:
                // bookMovie(); // Implement this if necessary, omitted for brevity
                break;
            case 9:
                // cancelMovie(); // Implement this if necessary, omitted for brevity
                break;
            case 10:
                // orderFood(); // Implement this if necessary, omitted for brevity
                break;
            case 11:
                // bookCab(); // Implement this if necessary, omitted for brevity
                break;
            case 12:
                viewOrderHistory();
                break;
            case 13:
                calculateTotalExpenditure();
                break;
            case 14:
                filterFoodsByCategory();
                break;
            case 15:
                recommendMovies();
                break;
            case 16:
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

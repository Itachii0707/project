#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql/mysql.h>

#define MAX_USERS 10
#define MAX_FOODS 10
#define MAX_CABS 3
#define MAX_HISTORY 50

// MySQL connection details
#define SERVER "localhost"
#define USER "root"
#define PASSWORD "password"
#define DATABASE "cinema_db"

typedef struct {
    char username[30];
    char password[20];
    char name[50];
    char email[50];
} User;

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

Cab cabs[MAX_CABS] = {
    {"Standard", 1.0},
    {"Luxury", 2.5},
    {"SUV", 3.0}
};

OrderHistory orderHistory[MAX_HISTORY];
int historyCount = 0;

MYSQL *conn;

void addOrderToHistory(const char* description, float cost) {
    if (historyCount < MAX_HISTORY) {
        strncpy(orderHistory[historyCount].description, description, sizeof(orderHistory[historyCount].description));
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

void connectDatabase() {
    conn = mysql_init(NULL);
    if (!mysql_real_connect(conn, SERVER, USER, PASSWORD, DATABASE, 0, NULL, 0)) {
        fprintf(stderr, "%s\n", mysql_error(conn));
        exit(1);
    }
}

void disconnectDatabase() {
    mysql_close(conn);
}

void executeQuery(const char *query) {
    if (mysql_query(conn, query)) {
        fprintf(stderr, "%s\n", mysql_error(conn));
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

void searchMovie() {
    char query[50];
    printf("Enter movie name to search: ");
    scanf("%s", query);
    printf("Search Results:\n");

    char query_str[256];
    sprintf(query_str, "SELECT * FROM movies WHERE name LIKE '%%%s%%'", query);

    if (mysql_query(conn, query_str)) {
        fprintf(stderr, "%s\n", mysql_error(conn));
        return;
    }

    MYSQL_RES *result = mysql_store_result(conn);
    if (result == NULL) {
        fprintf(stderr, "%s\n", mysql_error(conn));
        return;
    }

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) {
        printf("%s (%s) - %s - $%s [%s tickets available]\n", row[1], row[2], row[3], row[4], row[5]);
    }

    mysql_free_result(result);
}

void displayMovies() {
    printf("Available Movies:\n");

    if (mysql_query(conn, "SELECT * FROM movies")) {
        fprintf(stderr, "%s\n", mysql_error(conn));
        return;
    }

    MYSQL_RES *result = mysql_store_result(conn);
    if (result == NULL) {
        fprintf(stderr, "%s\n", mysql_error(conn));
        return;
    }

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) {
        printf("%s (%s) - %s - $%s [%s tickets available] - Average Rating: %s\n", row[1], row[2], row[3], row[4], row[5], row[6]);
    }

    mysql_free_result(result);
}

void rateMovie() {
    int movie_id;
    float rating;
    displayMovies();
    printf("Enter the movie ID to rate: ");
    scanf("%d", &movie_id);
    printf("Enter rating (1.0 to 5.0): ");
    scanf("%f", &rating);

    char query_str[256];
    sprintf(query_str, "UPDATE movies SET totalRatings = totalRatings + 1, sumRatings = sumRatings + %.1f WHERE id = %d", rating, movie_id);

    executeQuery(query_str);

    printf("Rating submitted successfully.\n");
}

void displayFoods() {
    printf("Available Food Items:\n");
    for (int i = 0; i < MAX_FOODS; i++) {
        printf("%d. %s (%s) - $%.2f\n", i + 1, foods[i].name, foods[i].category, foods[i].price);
    }
}

void filterFoodsByCategory() {
    char category[30];
    printf("Enter food category to filter: ");
    scanf("%s", category);
    printf("Filtered Food Items:\n");
    for (int i = 0; i < MAX_FOODS; i++) {
        if (strcmp(foods[i].category, category) == 0) {
            printf("%d. %s (%s) - $%.2f\n", i + 1, foods[i].name, foods[i].category, foods[i].price);
        }
    }
}

void displayCabs() {
    printf("Available Cabs:\n");
    for (int i = 0; i < MAX_CABS; i++) {
        printf("%d. %s - $%.2f per km\n", i + 1, cabs[i].type, cabs[i].pricePerKm);
    }
}

float bookMovie() {
    int choice, tickets;
    displayMovies();
    printf("Select a movie by ID: ");
    scanf("%d", &choice);

    char query_str[256];
    sprintf(query_str, "SELECT * FROM movies WHERE id = %d", choice);

    if (mysql_query(conn, query_str)) {
        fprintf(stderr, "%s\n", mysql_error(conn));
        return 0.0;
    }

    MYSQL_RES *result = mysql_store_result(conn);
    if (result == NULL) {
        fprintf(stderr, "%s\n", mysql_error(conn));
        return 0.0;
    }

    MYSQL_ROW row = mysql_fetch_row(result);
    if (row == NULL) {
        printf("Invalid choice.\n");
        mysql_free_result(result);
        return 0.0;
    }

    int availableTickets = atoi(row[5]);
    float price = atof(row[4]);

    printf("Enter number of tickets: ");
    scanf("%d", &tickets);
    if (tickets > availableTickets) {
        printf("Not enough tickets available.\n");
        mysql_free_result(result);
        return 0.0;
    }

    availableTickets -= tickets;
    float total = tickets * price;

    sprintf(query_str, "UPDATE movies SET availableTickets = %d WHERE id = %d", availableTickets, choice);
    executeQuery(query_str);

    char orderDescription[100];
    sprintf(orderDescription, "Booked %d tickets for %s", tickets, row[1]);
    addOrderToHistory(orderDescription, total);

    printf("Booked %d tickets for %s. Total: $%.2f\n", tickets, row[1], total);

    mysql_free_result(result);
    return total;
}

void cancelMovie() {
    int choice, tickets;
    displayMovies();
    printf("Select a movie to cancel tickets by ID: ");
    scanf("%d", &choice);

    char query_str[256];
    sprintf(query_str, "SELECT * FROM movies WHERE id = %d", choice);

    if (mysql_query(conn, query_str)) {
        fprintf(stderr, "%s\n", mysql_error(conn));
        return;
    }

    MYSQL_RES *result = mysql_store_result(conn);
    if (result == NULL) {
        fprintf(stderr, "%s\n", mysql_error(conn));
        return;
    }

    MYSQL_ROW row = mysql_fetch_row(result);
    if (row == NULL) {
        printf("Invalid choice.\n");
        mysql_free_result(result);
        return;
    }

    int availableTickets = atoi(row[5]);
    float price = atof(row[4]);

    printf("Enter number of tickets to cancel: ");
    scanf("%d", &tickets);
    if (tickets > (50 - availableTickets)) {
        printf("Cannot cancel more tickets than booked.\n");
        mysql_free_result(result);
        return;
    }

    availableTickets += tickets;
    float refund = tickets * price;

    sprintf(query_str, "UPDATE movies SET availableTickets = %d WHERE id = %d", availableTickets, choice);
    executeQuery(query_str);

    printf("Cancelled %d tickets for %s. Refund: $%.2f\n", tickets, row[1], refund);

    mysql_free_result(result);
}

float orderFood() {
    int choice, quantity;
    float total = 0.0;
    char more;
    do {
        displayFoods();
        printf("Select a food item by number: ");
        scanf("%d", &choice);
        if (choice < 1 || choice > MAX_FOODS) {
            printf("Invalid choice.\n");
            continue;
        }
        choice--; // Adjust for 0-based index
        printf("Enter quantity: ");
        scanf("%d", &quantity);
        total += foods[choice].price * quantity;
        char orderDescription[100];
        sprintf(orderDescription, "Ordered %d %s", quantity, foods[choice].name);
        addOrderToHistory(orderDescription, foods[choice].price * quantity);
        printf("Added %d %s to order. Subtotal: $%.2f\n", quantity, foods[choice].name, total);
        printf("Order more food? (y/n): ");
        scanf(" %c", &more);
    } while (more == 'y');
    printf("Total food order cost: $%.2f\n", total);
    return total;
}

float bookCab() {
    int choice;
    float distance;
    displayCabs();
    printf("Select a cab type by number: ");
    scanf("%d", &choice);
    if (choice < 1 || choice > MAX_CABS) {
        printf("Invalid choice.\n");
        return 0.0;
    }
    choice--; // Adjust for 0-based index
    printf("Enter distance in km: ");
    scanf("%f", &distance);
    float total = distance * cabs[choice].pricePerKm;
    char orderDescription[100];
    sprintf(orderDescription, "Booked %s cab for %.2f km", cabs[choice].type, distance);
    addOrderToHistory(orderDescription, total);
    printf("Booked %s cab for %.2f km. Total: $%.2f\n", cabs[choice].type, distance, total);
    return total;
}

void viewCabBookingHistory() {
    printf("Cab Booking History:\n");
    for (int i = 0; i < historyCount; i++) {
        if (strstr(orderHistory[i].description, "cab")) {
            printf("%d. %s - $%.2f\n", i + 1, orderHistory[i].description, orderHistory[i].cost);
        }
    }
}

void viewOrderHistory() {
    if (historyCount == 0) {
        printf("No order history available.\n");
        return;
    }
    printf("Order History:\n");
    for (int i = 0; i < historyCount; i++) {
        printf("%d. %s - $%.2f\n", i + 1, orderHistory[i].description, orderHistory[i].cost);
    }
}

void calculateTotalExpenditure() {
    float total = 0.0;
    for (int i = 0; i < historyCount; i++) {
        total += orderHistory[i].cost;
    }
    printf("Total expenditure: $%.2f\n", total);
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

    char query_str[256];
    sprintf(query_str, "INSERT INTO movies (name, showtime, description, price, availableTickets) VALUES ('%s', '%s', '%s', %.2f, %d)",
            name, showtime, description, price, availableTickets);

    executeQuery(query_str);

    printf("Movie added successfully.\n");
}

void removeMovie() {
    int choice;
    displayMovies();
    printf("Select a movie to remove by ID: ");
    scanf("%d", &choice);

    char query_str[256];
    sprintf(query_str, "DELETE FROM movies WHERE id = %d", choice);

    executeQuery(query_str);

    printf("Movie removed successfully.\n");
}

void recommendMovies() {
    printf("Recommended Movies:\n");

    if (mysql_query(conn, "SELECT * FROM movies WHERE (sumRatings / totalRatings) >= 4.0")) {
        fprintf(stderr, "%s\n", mysql_error(conn));
        return;
    }

    MYSQL_RES *result = mysql_store_result(conn);
    if (result == NULL) {
        fprintf(stderr, "%s\n", mysql_error(conn));
        return;
    }

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) {
        printf("%s (%s) - %s - $%s [%s tickets available] - Average Rating: %s\n", row[1], row[2], row[3], row[4], row[5], row[6]);
    }

    mysql_free_result(result);
}

int main() {
    int choice;
    connectDatabase();
    loadData();

    do {
        printf("\n--- Cinema Booking System ---\n");
        printf("1. Register\n");
        printf("2. Login\n");
        printf("3. View Profile\n");
        printf("4. Update Profile\n");
        printf("5. Search Movie\n");
        printf("6. Display Movies\n");
        printf("7. Rate Movie\n");
        printf("8. Book Movie\n");
        printf("9. Cancel Movie\n");
        printf("10. Display Foods\n");
        printf("11. Filter Foods By Category\n");
        printf("12. Order Food\n");
        printf("13. Display Cabs\n");
        printf("14. Book Cab\n");
        printf("15. View Cab Booking History\n");
        printf("16. View Order History\n");
        printf("17. Calculate Total Expenditure\n");
        printf("18. Add Movie (Admin)\n");
        printf("19. Remove Movie (Admin)\n");
        printf("20. Recommend Movies\n");
        printf("21. Logout\n");
        printf("22. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: registerUser(); break;
            case 2: loginUser(); break;
            case 3: viewProfile(); break;
            case 4: updateProfile(); break;
            case 5: searchMovie(); break;
            case 6: displayMovies(); break;
            case 7: rateMovie(); break;
            case 8: bookMovie(); break;
            case 9: cancelMovie(); break;
            case 10: displayFoods(); break;
            case 11: filterFoodsByCategory(); break;
            case 12: orderFood(); break;
            case 13: displayCabs(); break;
            case 14: bookCab(); break;
            case 15: viewCabBookingHistory(); break;
            case 16: viewOrderHistory(); break;
            case 17: calculateTotalExpenditure(); break;
            case 18: addMovie(); break;
            case 19: removeMovie(); break;
            case 20: recommendMovies(); break;
            case 21: logoutUser(); break;
            case 22: printf("Exiting...\n"); break;
            default: printf("Invalid choice. Please try again.\n");
        }
    } while (choice != 22);

    disconnectDatabase();
    saveData();
    return 0;
}

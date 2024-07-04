#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_USERS 10
#define MAX_MOVIES 10
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

Movie movies[MAX_MOVIES] = {
    {"Movie A", "10:00 AM", "An action-packed thriller.", 50, 12.5, 0, 0.0},
    {"Movie B", "01:00 PM", "A heartwarming romantic comedy.", 40, 10.0, 0, 0.0},
    {"Movie C", "03:00 PM", "A gripping drama.", 30, 15.0, 0, 0.0},
    {"Movie D", "06:00 PM", "An exciting adventure.", 20, 20.0, 0, 0.0},
    {"Movie E", "09:00 PM", "A chilling horror movie.", 10, 25.0, 0, 0.0}
};

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

void displayMovies() {
    printf("Available Movies:\n");
    for (int i = 0; i < MAX_MOVIES; i++) {
        float averageRating = (movies[i].totalRatings > 0) ? (movies[i].sumRatings / movies[i].totalRatings) : 0.0;
        printf("%d. %s (%s) - %s - $%.2f [%d tickets available] - Average Rating: %.1f\n", i + 1, movies[i].name, movies[i].showtime, movies[i].description, movies[i].price, movies[i].availableTickets, averageRating);
    }
}

void searchMovie() {
    char query[50];
    printf("Enter movie name to search: ");
    scanf("%s", query);
    printf("Search Results:\n");
    for (int i = 0; i < MAX_MOVIES; i++) {
        if (strstr(movies[i].name, query)) {
            printf("%d. %s (%s) - %s - $%.2f [%d tickets available]\n", i + 1, movies[i].name, movies[i].showtime, movies[i].description, movies[i].price, movies[i].availableTickets);
        }
    }
}

void rateMovie() {
    int choice;
    float rating;
    displayMovies();
    printf("Select a movie to rate by number: ");
    scanf("%d", &choice);
    if (choice < 1 || choice > MAX_MOVIES) {
        printf("Invalid choice.\n");
        return;
    }
    choice--; // Adjust for 0-based index
    printf("Enter rating (1.0 to 5.0): ");
    scanf("%f", &rating);
    if (rating < 1.0 || rating > 5.0) {
        printf("Invalid rating.\n");
        return;
    }
    movies[choice].totalRatings++;
    movies[choice].sumRatings += rating;
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
        printf("%d. %s - $%.2f per km\n", i + 1, cabList[i].type, cabList[i].pricePerKm);
    }
}

float bookMovie() {
    int choice, tickets;
    displayMovies();
    
    printf("Select a movie by number: ");
    if (scanf("%d", &choice) != 1) {
        printf("Invalid input. Please enter a number.\n");
        while (getchar() != '\n'); // Clear the input buffer
        return 0.0;
    }
    
    if (choice < 1 || choice > MAX_MOVIES) {
        printf("Invalid choice.\n");
        return 0.0;
    }
    
    choice--; // Adjust for 0-based index
    
    printf("Enter number of tickets: ");
    if (scanf("%d", &tickets) != 1) {
        printf("Invalid input. Please enter a number.\n");
        while (getchar() != '\n'); // Clear the input buffer
        return 0.0;
    }
    
    if (tickets > movies[choice].availableTickets) {
        printf("Not enough tickets available.\n");
        return 0.0;
    }
    
    movies[choice].availableTickets -= tickets;
    float total = tickets * movies[choice].price;
    char orderDescription[100];
    snprintf(orderDescription, sizeof(orderDescription), "Booked %d tickets for %s", tickets, movies[choice].name);
    addOrderToHistory(orderDescription, total);
    
    printf("Booked %d tickets for %s. Total: $%.2f\n", tickets, movies[choice].name, total);
    return total;

}

void cancelMovie() {
    int choice, tickets;
    displayMovies();
    printf("Select a movie to cancel tickets by number: ");
    scanf("%d", &choice);
    if (choice < 1 || choice > MAX_MOVIES) {
        printf("Invalid choice.\n");
        return;
    }
    choice--; // Adjust for 0-based index
    printf("Enter number of tickets to cancel: ");
    scanf("%d", &tickets);
    if (tickets > (50 - movies[choice].availableTickets)) {
        printf("Cannot cancel more tickets than booked.\n");
        return;
    }
    movies[choice].availableTickets += tickets;
    float refund = tickets * movies[choice].price;
    printf("Cancelled %d tickets for %s. Refund: $%.2f\n", tickets, movies[choice].name, refund);
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
    float total = distance * cabList[choice].pricePerKm;
    char orderDescription[100];
    sprintf(orderDescription, "Booked %s cab for %.2f km", cabList[choice].type, distance);
    addOrderToHistory(orderDescription, total);
    printf("Booked %s cab for %.2f km. Total: $%.2f\n", cabList[choice].type, distance, total);
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
    if (userCount >= MAX_MOVIES) {
        printf("Movie list is full.\n");
        return;
    }
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
    strcpy(movies[userCount].name, name);
    strcpy(movies[userCount].showtime, showtime);
    strcpy(movies[userCount].description, description);
    movies[userCount].availableTickets = availableTickets;
    movies[userCount].price = price;
    movies[userCount].totalRatings = 0;
    movies[userCount].sumRatings = 0.0;
    printf("Movie added successfully.\n");
}

void removeMovie() {
    int choice;
    displayMovies();
    printf("Select a movie to remove by number: ");
    scanf("%d", &choice);
    if (choice < 1 || choice > userCount) {
        printf("Invalid choice.\n");
        return;
    }
    for (int i = choice - 1; i < userCount - 1; i++) {
        movies[i] = movies[i + 1];
    }
    userCount--;
    printf("Movie removed successfully.\n");
}

void addFood() {
    if (userCount >= MAX_FOODS) {
        printf("Food list is full.\n");
        return;
    }
    char name[50], category[30];
    float price;
    printf("Enter food name: ");
    scanf("%s", name);
    printf("Enter category: ");
    scanf("%s", category);
    printf("Enter price: ");
    scanf("%f", &price);
    strcpy(foods[userCount].name, name);
    strcpy(foods[userCount].category, category);
    foods[userCount].price = price;
    printf("Food added successfully.\n");
}

void removeFood() {
    int choice;
    displayFoods();
    printf("Select a food item to remove by number: ");
    scanf("%d", &choice);
    if (choice < 1 || choice > userCount) {
        printf("Invalid choice.\n");
        return;
    }
    for (int i = choice - 1; i < userCount - 1; i++) {
        foods[i] = foods[i + 1];
    }
    userCount--;
    printf("Food removed successfully.\n");
}

void recommendMovies() {
    printf("Recommended Movies:\n");
    for (int i = 0; i < MAX_MOVIES; i++) {
        float averageRating = (movies[i].totalRatings > 0) ? (movies[i].sumRatings / movies[i].totalRatings) : 0.0;
        if (averageRating >= 4.0) {
            printf("%d. %s (%s) - %s - $%.2f [%d tickets available] - Average Rating: %.1f\n", i + 1, movies[i].name, movies[i].showtime, movies[i].description, movies[i].price, movies[i].availableTickets, averageRating);
        }
    }
}

void adminMenu() {
    int choice;
    do {
        printf("\n--- Admin Menu ---\n");
        printf("1. Add Movie\n");
        printf("2. Remove Movie\n");
        printf("3. Add Food Item\n");
        printf("4. Remove Food Item\n");
        printf("5. View All Movies\n");
        printf("6. View All Food Items\n");
        printf("7. View All Users\n");
        printf("8. View All Orders\n");
        printf("9. View Cab Booking History\n");
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
                addFood();
                break;
            case 4:
                removeFood();
                break;
            case 5:
                displayMovies();
                break;
            case 6:
                displayFoods();
                break;
            case 7:
                for (int i = 0; i < userCount; i++) {
                    printf("%d. %s (%s) - %s\n", i + 1, users[i].username, users[i].name, users[i].email);
                }
                break;
            case 8:
                viewOrderHistory();
                break;
            case 9:
                viewCabBookingHistory();
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
                rateMovie();
                break;
            case 8:
                bookMovie();
                break;
            case 9:
                cancelMovie();
                break;
            case 10:
                orderFood();
                break;
            case 11:
                bookCab();
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
                printf("Exiting the program. Goodbye!\n");
                break;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    } while (choice != 0);
    return 0;
}
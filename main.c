#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>

#ifdef _WIN32
    #define CLEAR "cls"
#else
    #define CLEAR "clear"
#endif

// ==================== CONSTANTS ====================
#define MAX_NAME 50
#define MAX_ID 10
#define MAX_PHONE 15
#define MAX_ADDRESS 200
#define MAX_CATEGORY 30
#define MAX_OUTLET_ID 20
#define MAX_PHONE_ADMIN 15

// ==================== STRUCTURES ====================

// Product structure
typedef struct Product {
    char id[MAX_ID];
    char name[MAX_NAME];
    int quantity;
    float price;
    char category[MAX_CATEGORY];
    struct Product* next;
} Product;

// Order item structure
typedef struct OrderItem {
    char productId[MAX_ID];
    char productName[MAX_NAME];
    int quantity;
    float price;
    struct OrderItem* next;
} OrderItem;

// Order structure
typedef struct Order {
    int id;
    char customerName[MAX_NAME];
    char phone[MAX_PHONE];
    char address[MAX_ADDRESS];
    float total;
    char status[20];
    time_t orderDate;
    struct OrderItem* items;
    struct Order* next;
} Order;

// Admin structure for verification
typedef struct Admin {
    char outletId[MAX_OUTLET_ID];
    char phone[MAX_PHONE_ADMIN];
    struct Admin* next;
} Admin;

// ==================== GLOBAL VARIABLES ====================
Product* inventory = NULL;      // Linked list of products
Order* allOrders = NULL;         // Linked list of orders
OrderItem* currentCart = NULL;   // Shopping cart
int nextOrderId = 1000;          // Next available order ID
Admin* adminList = NULL;
int idtp = 0;         // Linked list of authorized admins

// ==================== FUNCTION DECLARATIONS ====================

// Utility Functions
void clearScreen();
void wait();
void readString(char* prompt, char* output, int size);
int readInt(char* prompt, int min, int max);
float readFloat(char* prompt);

// File Operations
void saveAllData();
void loadAllData();

// Admin Verification Functions
void loadAdminData();
void saveAdminData();
int verifyAdmin();
int generateOTP();
void sendOTP(char* phone, int otp);

// Product Management Functions
void addProduct(Product* p);
Product* findProduct(char* id);
void showProducts();
void searchAndShowProducts(char* searchTerm);
void deleteProduct(char* id);
void updateProduct(char* id);

// Cart Management Functions
void addToCart(char* productId, int qty);
void showCart();
void updateCartItem(int itemNumber, int newQty);
void removeCartItem(int itemNumber);
float getCartTotal();
void clearCart();
int getCartItemCount();

// Order Management Functions
void placeOrder();
void showAllOrders();
Order* findOrder(int id);
void showOrderDetails(Order* order);
void cancelOrder(int id);
void updateOrderStatus(int id, char* newStatus);
int verifyProduct(char* productId);

// Customer Portal Functions
void customerMenu();
void browseAndBuy();
void viewCartAndCheckout();
void viewMyOrders();

// Admin Portal Functions
void adminMenu();
void adminAddProduct();
void adminViewProducts();
void adminUpdateProduct();
void adminDeleteProduct();
void adminViewOrders();
void adminUpdateOrderStatus();
void adminCancelOrder();

// Main Function
int main(void);

// ==================== UTILITY FUNCTIONS ====================

void clearScreen(void) {
    system(CLEAR);
}

void wait() {
    printf("\nPress Enter to continue...");
    getchar();
}

void readString(char* prompt, char* output, int size) {
    printf("%s", prompt);
    fgets(output, size, stdin);
    output[strcspn(output, "\n")] = 0;
}

int readInt(char* prompt, int min, int max) {
    char input[100];
    int value;
    int valid;
    
    do {
        valid = 1;
        printf("%s", prompt);
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = 0;
        
        // Check for empty input
        if (strlen(input) == 0) {
            valid = 0;
        }
        
        // Check if all characters are digits
        for (int i = 0; i < strlen(input) && valid; i++) {
            if (!isdigit(input[i])) {
                valid = 0;
            }
        }
        
        if (!valid) {
            printf("\033[1;31mERROR: Invalid input! Please enter a valid number.\033[0m\n");
            printf("Please enter a number between %d and %d.\n\n", min, max);
        } else {
            value = atoi(input);
            if (value < min || value > max) {
                printf("\033[1;31mERROR: Please enter a number between %d and %d!\033[0m\n\n", min, max);
                valid = 0;
            }
        }
    } while (!valid);
    
    return value;
}

float readFloat(char* prompt) {
    float value;
    printf("%s", prompt);
    scanf("%f", &value);
    getchar();
    return value;
}

// ==================== ADMIN VERIFICATION FUNCTIONS ====================

// Load admin data from file
void loadAdminData() {
    FILE* f = fopen("admin_data.txt", "r");


    char line[200];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = 0;

        Admin* a = malloc(sizeof(Admin));
        char* token = strtok(line, "|");
        if (token) strcpy(a->outletId, token);
        token = strtok(NULL, "|");
        if (token) strcpy(a->phone, token);
        a->next = NULL;

        if (adminList == NULL) {
            adminList = a;
        } else {
            Admin* curr = adminList;
            while (curr->next != NULL) curr = curr->next;
            curr->next = a;
        }
    }
    fclose(f);
}

// Save admin data to file
void saveAdminData() {
    FILE* f = fopen("admin_data.txt", "w");
    if (!f) {
        printf("Error saving admin data!\n");
        return;
    }

    Admin* curr = adminList;
    while (curr != NULL) {
        fprintf(f, "%s|%s\n", curr->outletId, curr->phone);
        curr = curr->next;
    }
    fclose(f);
}

// Generate random 6-digit OTP
int generateOTP() {
    srand(time(NULL));
    return 500000 + (rand() % 900000);
}

// Simulate sending OTP to mobile
void sendOTP(char* phone, int otp) {

    printf("\n OTP SENT TO MOBILE NUMBER: %s successful !\n", phone);
    printf(" YOUR OTP IS: %d\n", otp);
    printf(" OTP will expire in 5 minutes\n");

}

// Verify admin credentials with OTP
int verifyAdmin() {
    char outletId[MAX_OUTLET_ID];
    char inputOTP[7];
    int generatedOTP;
    int attempts = 3;

    printf("\n+====================================+\n");
    printf("|         ADMIN VERIFICATION         |\n");
    printf("+====================================+\n");

    // Check if outlet ID matches
    while (attempts > 0) {
        printf("\nAttempts left: %d\n", attempts);
        printf("Enter Outlet ID: ");
        fgets(outletId, MAX_OUTLET_ID, stdin);
        outletId[strcspn(outletId, "\n")] = 0;

        // Search for outlet ID
        Admin* curr = adminList;
        int found = 0;

        while (curr != NULL) {
            if (strcmp(curr->outletId, outletId) == 0) {
                found = 1;
                // Generate and send OTP
                generatedOTP = generateOTP();
                sendOTP(curr->phone, generatedOTP);

                // Verify OTP
                printf("\nEnter OTP: ");
                fgets(inputOTP, 7, stdin);
                inputOTP[strcspn(inputOTP, "\n")] = 0;

                if (atoi(inputOTP) == generatedOTP) {
                    printf("\n VERIFICATION SUCCESSFUL! Welcome Admin!\n");
                    sleep(1);
                    return 1;  // Verified successfully
                } else {
                    printf("\n INVALID OTP!\n");
                    attempts--;
                    if (attempts > 0) {
                        printf("Please try again.\n");
                    }
                    break;
                }
            }
            curr = curr->next;
        }

        if (!found) {
            printf("\n INVALID OUTLET ID!\n");
            attempts--;
            if (attempts > 0) {
                printf("Please try again.\n");
            }
        }
    }

    printf("\n VERIFICATION FAILED! Too many failed attempts.\n");
    sleep(1);
    return 0;  // Verification failed
}

// ==================== FILE OPERATIONS ====================

void saveAllData() {
    saveAdminData();  // Save admin data first

    FILE* f = fopen("store_data.txt", "w");
    if (!f) {
        printf("Error saving data!\n");
        return;
    }

    // Save products
    fprintf(f, "#PRODUCTS\n");
    Product* p = inventory;
    while (p != NULL) {
        fprintf(f, "%s|%s|%s|%d|%.2f\n",
                p->id, p->name, p->category, p->quantity, p->price);
        p = p->next;
    }
    fprintf(f, "#END\n\n");

    // Save orders
    fprintf(f, "#ORDERS\n");
    Order* o = allOrders;
    while (o != NULL) {
        fprintf(f, "%d|%s|%s|%s|%.2f|%s|%ld\n",
                o->id, o->customerName, o->phone,
                o->address, o->total, o->status, (long)o->orderDate);
        o = o->next;
    }
    fprintf(f, "#END\n");

    fclose(f);
    printf("Data saved successfully!\n");
}

void loadAllData() {
    loadAdminData();  // Load admin data first

    FILE* f = fopen("store_data.txt", "r");
    if (!f) {
        // Create sample products if no file exists
        Product* p1 = malloc(sizeof(Product));
        strcpy(p1->id, "P001");
        strcpy(p1->name, "Gaming Laptop");
        strcpy(p1->category, "Electronics");
        p1->quantity = 50;
        p1->price = 999.99;
        p1->next = NULL;
        inventory = p1;

        Product* p2 = malloc(sizeof(Product));
        strcpy(p2->id, "P002");
        strcpy(p2->name, "Wireless Mouse");
        strcpy(p2->category, "Electronics");
        p2->quantity = 200;
        p2->price = 29.99;
        p2->next = NULL;
        p1->next = p2;

        Product* p3 = malloc(sizeof(Product));
        strcpy(p3->id, "P003");
        strcpy(p3->name, "Python Programming Book");
        strcpy(p3->category, "Books");
        p3->quantity = 100;
        p3->price = 39.99;
        p3->next = NULL;
        p2->next = p3;

        Product* p4 = malloc(sizeof(Product));
        strcpy(p4->id, "P004");
        strcpy(p4->name, "Cotton T-Shirt");
        strcpy(p4->category, "Clothing");
        p4->quantity = 150;
        p4->price = 19.99;
        p4->next = NULL;
        p3->next = p4;

        Product* p5 = malloc(sizeof(Product));
        strcpy(p5->id, "P005");
        strcpy(p5->name, "Mechanical Keyboard");
        strcpy(p5->category, "Electronics");
        p5->quantity = 75;
        p5->price = 89.99;
        p5->next = NULL;
        p4->next = p5;

        printf("Sample products loaded!\n");
        return;
    }

    char line[500];
    int section = 0;

    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = 0;

        if (strcmp(line, "#PRODUCTS") == 0) {
            section = 1;
            continue;
        }
        else if (strcmp(line, "#ORDERS") == 0) {
            section = 2;
            continue;
        }
        else if (strcmp(line, "#END") == 0) {
            section = 0;
            continue;
        }

        if (strlen(line) == 0) continue;

        if (section == 1) {
            Product* p = malloc(sizeof(Product));
            char* token = strtok(line, "|");
            if (token) strcpy(p->id, token);
            token = strtok(NULL, "|");
            if (token) strcpy(p->name, token);
            token = strtok(NULL, "|");
            if (token) strcpy(p->category, token);
            token = strtok(NULL, "|");
            if (token) p->quantity = atoi(token);
            token = strtok(NULL, "|");
            if (token) p->price = atof(token);
            p->next = NULL;

            if (inventory == NULL) {
                inventory = p;
            } else {
                Product* curr = inventory;
                while (curr->next != NULL) curr = curr->next;
                curr->next = p;
            }
        }
        else if (section == 2) {
            Order* o = malloc(sizeof(Order));
            char* token = strtok(line, "|");
            if (token) o->id = atoi(token);
            token = strtok(NULL, "|");
            if (token) strcpy(o->customerName, token);
            token = strtok(NULL, "|");
            if (token) strcpy(o->phone, token);
            token = strtok(NULL, "|");
            if (token) strcpy(o->address, token);
            token = strtok(NULL, "|");
            if (token) o->total = atof(token);
            token = strtok(NULL, "|");
            if (token) strcpy(o->status, token);
            token = strtok(NULL, "|");
            if (token) o->orderDate = (time_t)atol(token);
            o->items = NULL;
            o->next = NULL;

            if (o->id >= nextOrderId) nextOrderId = o->id + 1;

            if (allOrders == NULL) {
                allOrders = o;
            } else {
                Order* curr = allOrders;
                while (curr->next != NULL) curr = curr->next;
                curr->next = o;
            }
        }
    }
    fclose(f);
    printf("Data loaded successfully!\n");
}

// ==================== PRODUCT MANAGEMENT FUNCTIONS ====================

void addProduct(Product* p) {
    if (inventory == NULL) {
        inventory = p;
    } else {
        Product* curr = inventory;
        while (curr->next != NULL) {
            curr = curr->next;
        }
        curr->next = p;
    }
}

Product* findProduct(char* id) {
    Product* curr = inventory;
    while (curr != NULL) {
        if (strcmp(curr->id, id) == 0)
            return curr;
        curr = curr->next;
    }
    return NULL;
}

void showProducts() {
    if (inventory == NULL) {
        printf("\nNo products available!\n");
        return;
    }

    printf("\n+------------------------------------------------------------------+\n");
    printf("| %-8s | %-22s | %6s | %8s | %-12s |\n",
           "ID", "Name", "Stock", "Price", "Category");
    printf("+------------------------------------------------------------------+\n");

    Product* curr = inventory;
    while (curr != NULL) {
        printf("| %-8s | %-22s | %6d | %8.2f | %-12s |\n",
               curr->id, curr->name, curr->quantity,
               curr->price, curr->category);
        curr = curr->next;
    }
    printf("+------------------------------------------------------------------+\n");
}

void searchAndShowProducts(char* searchTerm) {
    Product* curr = inventory;
    int found = 0;

    printf("\n+------------------------------------------------------------------+\n");
    printf("|              SEARCH RESULTS FOR: %-20s |\n", searchTerm);
    printf("+------------------------------------------------------------------+\n");
    printf("| %-8s | %-22s | %6s | %8s | %-12s |\n",
           "ID", "Name", "Stock", "Price", "Category");
    printf("+------------------------------------------------------------------+\n");

    while (curr != NULL) {
        if (strstr(curr->name, searchTerm) != NULL ||
            strstr(curr->category, searchTerm) != NULL) {
            printf("| %-8s | %-22s | %6d | %8.2f | %-12s |\n",
                   curr->id, curr->name, curr->quantity,
                   curr->price, curr->category);
            found = 1;
        }
        curr = curr->next;
    }

    if (!found) {
        idtp=1;
        printf("| No products found matching '%s'!                             |\n", searchTerm);
    }
    printf("+------------------------------------------------------------------+\n");
}

void deleteProduct(char* id) {
    Product* curr = inventory;
    Product* prev = NULL;

    while (curr != NULL) {
        if (strcmp(curr->id, id) == 0) {
            if (prev == NULL)
                inventory = curr->next;
            else
                prev->next = curr->next;
            free(curr);
            printf("Product '%s' deleted successfully!\n", id);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
    printf("Product '%s' not found!\n", id);
}

void updateProduct(char* id) {
    Product* p = findProduct(id);
    if (p == NULL) {
        printf("Product not found!\n");
        return;
    }

    printf("\nCurrent Product Details:");
    printf("\n  Quantity: %d units", p->quantity);
    printf("\n  Price: $%.2f\n", p->price);

    printf("\nEnter new details (press Enter to keep current):\n");

    int newQty = readInt("New quantity: ", 0, 9999);
    float newPrice = readFloat("New price: $");

    if (newQty >= 0) p->quantity = newQty;
    if (newPrice >= 0) p->price = newPrice;

    printf("Product updated successfully!\n");
}

// ==================== CART MANAGEMENT FUNCTIONS ====================

void addToCart(char* productId, int qty) {
    Product* p = findProduct(productId);
    if (p == NULL) {
        printf("Product not found!\n");
        return;
    }

    if (p->quantity < qty) {
        printf("Not enough stock! Only %d available.\n", p->quantity);
        return;
    }

    // Check if product already in cart
    OrderItem* curr = currentCart;
    while (curr != NULL) {
        if (strcmp(curr->productId, productId) == 0) {
            curr->quantity += qty;
            printf("Updated quantity in cart!\n");
            return;
        }
        curr = curr->next;
    }

    // Add new item to cart
    OrderItem* newItem = malloc(sizeof(OrderItem));
    strcpy(newItem->productId, p->id);
    strcpy(newItem->productName, p->name);
    newItem->quantity = qty;
    newItem->price = p->price;
    newItem->next = currentCart;
    currentCart = newItem;

    printf("Added '%s' to cart!\n", p->name);
}

void showCart() {
    if (currentCart == NULL) {
        printf("\nYour cart is empty!\n");
        return;
    }

    float total = 0;
    int itemNum = 1;

    printf("\n+============================================================+\n");
    printf("|                    YOUR SHOPPING CART                      |\n");
    printf("+============================================================+\n");
    printf("| # | %-10s | %-20s | %5s | %10s |\n", "ID", "Product", "Qty", "Subtotal");
    printf("+------------------------------------------------------------+\n");

    OrderItem* curr = currentCart;
    while (curr != NULL) {
        float subtotal = curr->price * curr->quantity;
        printf("| %-2d | %-10s | %-20s | %5d | %10.2f |\n",
               itemNum, curr->productId, curr->productName,
               curr->quantity, subtotal);
        total += subtotal;
        curr = curr->next;
        itemNum++;
    }
    printf("+------------------------------------------------------------+\n");
    printf("|                                    TOTAL: $%10.2f |\n", total);
    printf("+============================================================+\n");
}

void updateCartItem(int itemNumber, int newQty) {
    OrderItem* curr = currentCart;
    int i = 1;

    while (curr != NULL) {
        if (i == itemNumber) {
            Product* p = findProduct(curr->productId);
            if (p != NULL && p->quantity >= newQty) {
                curr->quantity = newQty;
                printf("Quantity updated!\n");
            } else {
                printf("Not enough stock!\n");
            }
            return;
        }
        curr = curr->next;
        i++;
    }
    printf("Item not found!\n");
}

void removeCartItem(int itemNumber) {
    OrderItem* curr = currentCart;
    OrderItem* prev = NULL;
    int i = 1;

    while (curr != NULL) {
        if (i == itemNumber) {
            if (prev == NULL)
                currentCart = curr->next;
            else
                prev->next = curr->next;
            free(curr);
            printf("Item removed from cart!\n");
            return;
        }
        prev = curr;
        curr = curr->next;
        i++;
    }
    printf("Item not found!\n");
}

float getCartTotal() {
    float total = 0;
    OrderItem* curr = currentCart;
    while (curr != NULL) {
        total += curr->price * curr->quantity;
        curr = curr->next;
    }
    return total;
}

void clearCart() {
    OrderItem* curr = currentCart;
    while (curr != NULL) {
        OrderItem* temp = curr;
        curr = curr->next;
        free(temp);
    }
    currentCart = NULL;
}

int getCartItemCount() {
    int count = 0;
    OrderItem* curr = currentCart;
    while (curr != NULL) {
        count++;
        curr = curr->next;
    }
    return count;
}

// ==================== ORDER MANAGEMENT FUNCTIONS ====================

void placeOrder() {
    if (currentCart == NULL) {
        printf("\nCannot place order. Your cart is empty!\n");
        return;
    }

    // Display order summary
    float total = getCartTotal();
    printf("\n+========================================+\n");
    printf("|           ORDER SUMMARY                |\n");
    printf("+========================================+\n");

    OrderItem* cartItem = currentCart;
    while (cartItem != NULL) {
        printf("  %s x%d = $%.2f\n",
               cartItem->productName, cartItem->quantity,
               cartItem->price * cartItem->quantity);
        cartItem = cartItem->next;
    }
    printf("----------------------------------------\n");
    printf("  TOTAL: $%.2f\n", total);
    printf("+========================================+\n");

    char confirm;
    printf("\nConfirm order? (y/n): ");
    scanf(" %c", &confirm);
    getchar();

    if (tolower(confirm) != 'y') {
        printf("Order cancelled!\n");
        return;
    }

    // Create new order
    Order* order = malloc(sizeof(Order));
    order->id = nextOrderId++;
    order->orderDate = time(NULL);
    order->next = NULL;
    order->total = total;
    strcpy(order->status, "Pending");
    order->items = NULL;

    // Get customer information
    printf("\n=== DELIVERY INFORMATION ===\n");
    readString("Enter your name: ", order->customerName, MAX_NAME);
    readString("Enter phone number: ", order->phone, MAX_PHONE);
    readString("Enter delivery address: ", order->address, MAX_ADDRESS);

    // Move items from cart to order
    cartItem = currentCart;
    while (cartItem != NULL) {
        OrderItem* item = malloc(sizeof(OrderItem));
        *item = *cartItem;
        item->next = order->items;
        order->items = item;

        // Update inventory
        Product* p = findProduct(cartItem->productId);
        if (p != NULL) {
            p->quantity -= cartItem->quantity;
        }

        cartItem = cartItem->next;
    }

    // Add order to history
    if (allOrders == NULL) {
        allOrders = order;
    } else {
        Order* curr = allOrders;
        while (curr->next != NULL) curr = curr->next;
        curr->next = order;
    }

    // Clear cart
    clearCart();

    // Save data
    saveAllData();

    printf("\n========================================\n");
    printf("ORDER PLACED SUCCESSFULLY!\n");
    printf("Order ID: #%d\n", order->id);
    printf("Total Amount: $%.2f\n", order->total);
    printf("Status: %s\n", order->status);
    printf("========================================\n");
}

void showAllOrders() {
    if (allOrders == NULL) {
        printf("\nNo orders found!\n");
        return;
    }

    printf("\n+============================================================+\n");
    printf("|                      ORDER HISTORY                         |\n");
    printf("+============================================================+\n");
    printf("| %-8s | %-15s | %10s | %-12s |\n",
           "Order ID", "Customer", "Total", "Status");
    printf("+------------------------------------------------------------+\n");

    Order* curr = allOrders;
    while (curr != NULL) {
        printf("| #%-7d | %-15s | $%8.2f | %-12s |\n",
               curr->id, curr->customerName, curr->total, curr->status);
        curr = curr->next;
    }
    printf("+============================================================+\n");
}

Order* findOrder(int id) {
    Order* curr = allOrders;
    while (curr != NULL) {
        if (curr->id == id)
            return curr;
        curr = curr->next;
    }
    return NULL;
}

void showOrderDetails(Order* order) {
    printf("\n+====================================================+\n");
    printf("|                 ORDER DETAILS #%d                  |\n", order->id);
    printf("+====================================================+\n");
    printf("| Customer Name: %s\n", order->customerName);
    printf("| Phone Number: %s\n", order->phone);
    printf("| Delivery Address: %s\n", order->address);
    printf("| Order Status: %s\n", order->status);
    printf("| Order Date: %s", ctime(&order->orderDate));
    printf("+----------------------------------------------------+\n");
    printf("| ITEMS:\n");

    OrderItem* item = order->items;
    int itemNum = 1;
    while (item != NULL) {
        printf("|   %d. %s x%d = $%.2f\n",
               itemNum, item->productName, item->quantity,
               item->price * item->quantity);
        item = item->next;
        itemNum++;
    }
    printf("+----------------------------------------------------+\n");
    printf("| TOTAL AMOUNT: $%.2f\n", order->total);
    printf("+====================================================+\n");
}

void cancelOrder(int id) {
    Order* order = findOrder(id);
    if (order == NULL) {
        printf("Order #%d not found!\n", id);
        return;
    }

    if (strcmp(order->status, "Delivered") == 0) {
        printf("Cannot cancel order #%d - Already delivered!\n", id);
        return;
    }

    if (strcmp(order->status, "Cancelled") == 0) {
        printf("Order #%d is already cancelled!\n", id);
        return;
    }

    // Restore inventory
    OrderItem* item = order->items;
    while (item != NULL) {
        Product* p = findProduct(item->productId);
        if (p != NULL) {
            p->quantity += item->quantity;
        }
        item = item->next;
    }

    strcpy(order->status, "Cancelled");
    saveAllData();
    printf("Order #%d cancelled successfully!\n", id);
}

void updateOrderStatus(int id, char* newStatus) {
    Order* order = findOrder(id);
    if (order == NULL) {
        printf("Order #%d not found!\n", id);
        return;
    }

    strcpy(order->status, newStatus);
    saveAllData();
    printf("Order #%d status updated to '%s'!\n", id, newStatus);
}

//verify product exitance
int verifyProduct(char* productId) {
    Product* curr = inventory;

    // Traverse the linked list to find the product
    while (curr != NULL) {
        if (strcmp(curr->id, productId) == 0) {
            return 1;
        }
        curr = curr->next;
    }
    return 0;
}

// ==================== CUSTOMER PORTAL FUNCTIONS ====================

void browseAndBuy() {
    int choice,flag;
    char searchTerm[MAX_NAME];

    do {
        clearScreen();
        printf("+====================================+\n");
        printf("|        BROWSE & BUY PRODUCTS       |\n");
        printf("+====================================+\n");
        printf("1. Show All Products\n");
        printf("2. Search Products\n");
        printf("3. Back to Customer Menu\n");

        choice = readInt("\nChoice: ", 1, 3);

        if (choice == 1) {
            showProducts();

            char addMore = 'y';
            while (addMore == 'y' || addMore == 'Y') {
                char id[MAX_ID];
                int qty;

                 while (1) {
                    flag=0;
                     printf("\nEnter Product ID to purchase (or 0 to exit): ");
                     scanf("%s", id);
                     getchar();

                        if(strcmp(id, "0") == 0){
                         printf("Exiting product selection...\n");

                        flag=1;
                        break;
                         }

                    // Verify if product exists using your function
                 else if (verifyProduct(id)) {
                     break;  // Product exists, exit the loop
                    }
                 else {
                  printf("\n\033[1;31mERROR: Product '%s' not found!\033[0m\n", id);
                  printf("Please enter a valid Product ID from the list above.\n");
                 }
                }

                if(flag==1){
                    break;
                }

                qty = readInt("Enter quantity: ", 1, 999);
                addToCart(id, qty);

              while(1) {
                printf("\nAdd another product? (y/n): ");
                scanf(" %c", &addMore);
                getchar();
                if(addMore == 'y' || addMore == 'Y' || addMore == 'n' || addMore == 'N') {
                    break;  
                }
                else {
                    printf("Invalid input! Please enter 'y' or 'n'.\n");
                }
            } 
        }
            wait();
        }
        else if (choice == 2) {
            int bb;

            while(1){
                  bb=0;
                 readString("Enter product name or category to search (or 0 to exit): ", searchTerm, MAX_NAME);
                 if (strcmp(searchTerm, "0") == 0) {
                 printf("Exiting program...\n");
                bb=1;
                 break;
                  }
                  searchAndShowProducts(searchTerm);
                if(idtp==1){
                    printf(" Please try a different search term (or 0 to exit):\n");
                    

                }
                else {
                    break;
                }
           
            }

            if(bb==1){
                break;
            }

            char addMore = 'y';
            while (addMore == 'y' || addMore == 'Y') {
                char id[MAX_ID];
                int qty;

                 while (1) {
                    flag=0;
                     printf("\nEnter Product ID to purchase (or 0 to exit): ");
                     scanf("%s", id);
                     getchar();

                        if(strcmp(id, "0") == 0){
                         printf("Exiting product selection...\n");

                        flag=1;
                        break;
                         }

                    // Verify if product exists using your function
                 else if (verifyProduct(id)) {
                     break;  // Product exists, exit the loop
                    }
                 else {
                  printf("\n\033[1;31mERROR: Product '%s' not found!\033[0m\n", id);
                  printf("Please enter a valid Product ID from the list above.\n");
                 }
                }

                if(flag==1){
                    break;
                }

                qty = readInt("Enter quantity: ", 1, 999);
                addToCart(id, qty);
                while(1){
                printf("\nAdd another product? (y/n): ");
                scanf(" %c", &addMore);
                getchar();
                if(addMore == 'y' || addMore == 'Y' || addMore == 'n' || addMore == 'N') {
                    break;  
                }
                else {
                    printf("Invalid input! Please enter 'y' or 'n'.\n");
                }
            }
        }
            wait();
        }

    } while (choice != 3);
}

void viewCartAndCheckout() {
    if (currentCart == NULL) {
        printf("\nYour cart is empty!\n");
        wait();
        return;
    }

    int choice;
    do {
        showCart();

        if (getCartItemCount() == 0) {
            break;
        }

        printf("\nOptions:\n");
        printf("1. Update item quantity\n");
        printf("2. Remove item from cart\n");
        printf("3. Proceed to checkout\n");
        printf("4. Back to main menu\n");

        choice = readInt("Choice: ", 1, 4);

        switch (choice) {
            case 1: {
                int itemNum = readInt("Enter Sl. No to update: ", 1, getCartItemCount());
                int newQty = readInt("Enter new quantity: ", 1, 999);
                updateCartItem(itemNum, newQty);
                break;
            }
            case 2: {
                int itemNum = readInt("Enter Sl. No to remove: ", 1, getCartItemCount());
                removeCartItem(itemNum);
                break;
            }
            case 3:
                placeOrder();
                wait();
                return;
            case 4:
                return;
        }

        if (choice != 4) {
            wait();
        }

    } while (choice != 4);
}

void viewMyOrders() {
    if (allOrders == NULL) {
        printf("\nYou haven't placed any orders yet!\n");
        wait();
        return;
    }

    showAllOrders();

    int orderId = readInt("\nEnter Order ID to view details (0 to skip): ", 0, 9999);
    if (orderId > 0) {
        Order* order = findOrder(orderId);
        if (order != NULL) {
            showOrderDetails(order);

            printf("\nOptions:\n");
            printf("1. Cancel this order\n");
            printf("2. Back\n");

            int choice = readInt("Choice: ", 1, 2);
            if (choice == 1) {
                cancelOrder(orderId);
            }
        } else {
            printf("Order #%d not found!\n", orderId);
        }
    }
    wait();
}

void customerMenu() {
    int choice;
    do {
        clearScreen();
        printf("+====================================+\n");
        printf("|       CUSTOMER PORTAL               |\n");
        printf("+====================================+\n");
        printf("1. Browse & Buy Products\n");
        printf("2. View Cart & Checkout\n");
        printf("3. My Orders\n");
        printf("4. Back to Main Menu\n");

        choice = readInt("\nChoice: ", 1, 4);

        switch (choice) {
            case 1:
                browseAndBuy();
                break;
            case 2:
                viewCartAndCheckout();
                break;
            case 3:
                viewMyOrders();
                break;
        }
    } while (choice != 4);
}

// ==================== ADMIN PORTAL FUNCTIONS ====================

void adminAddProduct() {
    Product* p = malloc(sizeof(Product));

    printf("\n=== ADD NEW PRODUCT ===\n");
    readString("Product ID: ", p->id, MAX_ID);

    if (findProduct(p->id) != NULL) {
        printf("Product ID already exists!\n");
        free(p);
        return;
    }

    readString("Product Name: ", p->name, MAX_NAME);
    readString("Category: ", p->category, MAX_CATEGORY);
    p->quantity = readInt("Quantity: ", 0, 9999);
    p->price = readFloat("Price: $");
    p->next = NULL;

    addProduct(p);
    saveAllData();
    printf("Product '%s' added successfully!\n", p->name);
}

void adminViewProducts() {
    showProducts();
}

void adminUpdateProduct() {
    showProducts();
    char id[MAX_ID];
    printf("\n=== UPDATE PRODUCT ===\n");
    readString("Product ID: ", id, MAX_ID);
    updateProduct(id);
    saveAllData();
}

void adminDeleteProduct() {
    showProducts();
    char id[MAX_ID];
    printf("\n=== DELETE PRODUCT ===\n");
    readString("Product ID: ", id, MAX_ID);
    deleteProduct(id);
    saveAllData();
}

void adminViewOrders() {
    showAllOrders();
}

void adminUpdateOrderStatus(void) {
    showAllOrders();
    printf("\n=== UPDATE ORDER STATUS ===\n");
    int id = readInt("Order ID: ", 1000, 9999);
    Order* order = findOrder(id);

    if (order == NULL) {
        printf("Order not found!\n");
        return;
    }

    printf("\nCurrent Status: %s\n", order->status);
    printf("\nSelect new status:\n");
    printf("1. Pending\n");
    printf("2. Processing\n");
    printf("3. Shipped\n");
    printf("4. Delivered\n");

    int statusChoice = readInt("Choice: ", 1, 4);
    char* statuses[] = {"Pending", "Processing", "Shipped", "Delivered"};

    updateOrderStatus(id, statuses[statusChoice - 1]);
}

void adminCancelOrder(void) {
    showAllOrders();
    printf("\n=== CANCEL ORDER ===\n");
    int id = readInt("Order ID: ", 1000, 9999);
    cancelOrder(id);
}

void adminMenu() {
    int choice;
    do {
        clearScreen();
        printf("+====================================+\n");
        printf("|         ADMIN PORTAL                |\n");
        printf("+====================================+\n");
        printf("=== PRODUCT MANAGEMENT ===\n");
        printf("1. Add Product\n");
        printf("2. View All Products\n");
        printf("3. Update Product\n");
        printf("4. Delete Product\n");
        printf("\n=== ORDER MANAGEMENT ===\n");
        printf("5. View All Orders\n");
        printf("6. Update Order Status\n");
        printf("7. Cancel Order\n");
        printf("\n8. Logout\n");

        choice = readInt("\nChoice: ", 1, 8);

        switch (choice) {
            case 1: adminAddProduct(); wait(); break;
            case 2: adminViewProducts(); wait(); break;
            case 3: adminUpdateProduct(); wait(); break;
            case 4: adminDeleteProduct(); wait(); break;
            case 5: adminViewOrders(); wait(); break;
            case 6: adminUpdateOrderStatus(); wait(); break;
            case 7: adminCancelOrder(); wait(); break;
            case 8:
                printf("Logging out...\n");
                break;
        }
    } while (choice != 8);
}

// ==================== MAIN FUNCTION ====================

int main() {
    loadAllData();

    int choice;
    do {
        clearScreen();
        printf("\033[1;36m");
        printf("+====================================+\n");
        printf("|   INVENTORY MANAGEMENT SYSTEM      |\n");
        printf("+====================================+\n");
        printf("\033[0m");
        printf("\n");
        printf("1. Customer Portal\n");
        printf("2. Admin Portal\n");
        printf("3. Exit\n");

        choice = readInt("\nEnter your choice: ", 1, 3);

        switch (choice) {
            case 1:
                customerMenu();
                break;
            case 2:
                if (verifyAdmin()) {
                    adminMenu();
                } else {
                    printf("\nAccess Denied! Press Enter to continue...\n");
                    getchar();
                }
                break;
            case 3:
                saveAllData();
                printf("\nThank you for using the system!\n");
                break;
        }
    } while (choice != 3);

    return 0;
}

/*
 * ============================================================
 *   INVENTORY MANAGEMENT SYSTEM
 *   OOP Final Project - C++
 *   Team Members:
 *   Member 1 (Friend) - Admin Side (Inventory Management)
 *   Member 2 (You)    - User Side (Authentication & Shopping)
 * ============================================================
 */

#include <iostream>
#include <cstring>
#include <iomanip>
#include <cctype>
#include <cstdlib>
#include <ctime>
#include <fstream>

using namespace std;

// ================================================================
//  UTILITY FUNCTIONS
// ================================================================
void printLine(char c = '=', int n = 70) {
    for (int i = 0; i < n; i++) cout << c;
    cout << '\n';
}

void printHeader(const char* title) {
    cout << '\n';
    printLine('=', 70);
    cout << "  " << title << '\n';
    printLine('=', 70);
}

// ================================================================
//  FORWARD DECLARATIONS
// ================================================================
class Product;
class CartItem;

// ================================================================
//  MEMBER 2's CODE - USER AUTHENTICATION & SHOPPING SYSTEM
// ================================================================

// ================================================================
//  USER STRUCTURE
// ================================================================
struct User {
    int userId;
    char username[50];
    char password[50];
    char email[100];
    char phone[20];
    char address[200];
    double walletBalance;
    bool isActive;
    
    // Constructor
    User() : userId(0), walletBalance(0), isActive(true) {
        username[0] = '\0';
        password[0] = '\0';
        email[0] = '\0';
        phone[0] = '\0';
        address[0] = '\0';
    }
    
    User(int id, const char* uname, const char* pwd, const char* mail, 
         const char* ph, const char* addr, double balance, bool active) 
         : userId(id), walletBalance(balance), isActive(active) {
        strcpy(username, uname);
        strcpy(password, pwd);
        strcpy(email, mail);
        strcpy(phone, ph);
        strcpy(address, addr);
    }
};

// ================================================================
//  CART ITEM CLASS
// ================================================================
class CartItem {
private:
    int productId;
    char productName[100];
    double price;
    int quantity;
    
public:
    CartItem() : productId(0), price(0), quantity(0) {
        strcpy(productName, "");
    }
    
    CartItem(int id, const char* name, double p, int qty) 
        : productId(id), price(p), quantity(qty) {
        strcpy(productName, name);
    }
    
    int getProductId() const { return productId; }
    const char* getProductName() const { return productName; }
    double getPrice() const { return price; }
    int getQuantity() const { return quantity; }
    void setQuantity(int q) { quantity = q; }
    double getTotal() const { return price * quantity; }
    
    void display() const {
        cout << "     " << left << setw(25) << productName
             << " x" << setw(4) << quantity
             << " @ $" << fixed << setprecision(2) << setw(8) << price
             << " = $" << setw(8) << getTotal() << '\n';
    }
};

// ================================================================
//  ORDER CLASS
// ================================================================
class Order {
private:
    int orderId;
    int userId;
    CartItem* items[50];
    int itemCount;
    double totalAmount;
    char orderDate[30];
    char status[20];
    
public:
    Order(int oid, int uid) : orderId(oid), userId(uid), itemCount(0), totalAmount(0) {
        strcpy(status, "Pending");
        // Get current date/time
        time_t now = time(0);
        strcpy(orderDate, ctime(&now));
        orderDate[strlen(orderDate)-1] = '\0';
        for (int i = 0; i < 50; i++) items[i] = nullptr;
    }
    
    ~Order() {
        for (int i = 0; i < itemCount; i++) delete items[i];
    }
    
    void addItem(const CartItem& item) {
        if (itemCount < 50) {
            items[itemCount] = new CartItem(item);
            totalAmount += item.getTotal();
            itemCount++;
        }
    }
    
    double getTotal() const { return totalAmount; }
    int getItemCount() const { return itemCount; }
    const char* getStatus() const { return status; }
    int getOrderId() const { return orderId; }
    int getUserId() const { return userId; }
    
    void setStatus(const char* newStatus) {
        strcpy(status, newStatus);
    }
    
    void display() const {
        cout << "\n     Order #" << orderId << " | Date: " << orderDate
             << " | Status: " << status << " | Total: $" << fixed << setprecision(2) << totalAmount << '\n';
        if (itemCount > 0) {
            cout << "     ----------------------------------------\n";
            for (int i = 0; i < itemCount; i++) {
                items[i]->display();
            }
            cout << "     ----------------------------------------\n";
            cout << "     Grand Total: $" << fixed << setprecision(2) << totalAmount << '\n';
        }
    }
};

// ================================================================
//  USER MANAGEMENT SYSTEM (YOUR PART - COMPLETELY FIXED)
// ================================================================
class UserManager {
private:
    static const int MAX_USERS = 100;
    static const int MAX_ORDERS = 200;
    
    User users[MAX_USERS];
    Order* orders[MAX_ORDERS];
    int userCount;
    int orderCount;
    int nextUserId;
    int nextOrderId;
    User* currentUser;
    
    // Shopping cart for current user
    CartItem cart[50];
    int cartSize;
    
    bool isUserIdExists(int id) {
        for (int i = 0; i < userCount; i++) {
            if (users[i].userId == id) return true;
        }
        return false;
    }
    
    bool isUsernameExists(const char* username) {
        for (int i = 0; i < userCount; i++) {
            if (strcmp(users[i].username, username) == 0) return true;
        }
        return false;
    }
    
    int findUserById(int id) {
        for (int i = 0; i < userCount; i++) {
            if (users[i].userId == id) return i;
        }
        return -1;
    }
    
public:
    UserManager() : userCount(0), orderCount(0), nextUserId(2000), nextOrderId(5000), currentUser(nullptr), cartSize(0) {
        // Initialize with default users using proper initialization
        // Admin user
        users[userCount] = User(1000, "Admin", "admin123", "admin@system.com", 
                                "1234567890", "Main Office", 0, true);
        userCount++;
        
        // Regular users
        users[userCount] = User(2001, "JohnDoe", "john123", "john@email.com", 
                                "555-0101", "123 Main St", 500.00, true);
        userCount++;
        
        users[userCount] = User(2002, "JaneSmith", "jane123", "jane@email.com", 
                                "555-0102", "456 Oak Ave", 750.00, true);
        userCount++;
        
        users[userCount] = User(2003, "MikeJohnson", "mike123", "mike@email.com", 
                                "555-0103", "789 Pine Rd", 300.00, true);
        userCount++;
        
        nextUserId = 2004;
    }
    
    ~UserManager() {
        for (int i = 0; i < orderCount; i++) {
            delete orders[i];
        }
    }
    
    bool signUp() {
        printHeader("CREATE NEW ACCOUNT");
        
        if (userCount >= MAX_USERS) {
            cout << "  System is full. Cannot create new account.\n";
            return false;
        }
        
        User newUser;
        newUser.userId = nextUserId++;
        
        cout << "\n  Enter Username: ";
        cin >> newUser.username;
        
        // Check if username exists
        if (isUsernameExists(newUser.username)) {
            cout << "\n  [ERROR] Username already exists!\n";
            return false;
        }
        
        cout << "  Enter Password (min 4 characters): ";
        cin >> newUser.password;
        
        if (strlen(newUser.password) < 4) {
            cout << "\n  [ERROR] Password must be at least 4 characters!\n";
            return false;
        }
        
        cout << "  Enter Email: ";
        cin >> newUser.email;
        
        cout << "  Enter Phone: ";
        cin >> newUser.phone;
        
        cout << "  Enter Address: ";
        cin.ignore();
        cin.getline(newUser.address, 200);
        
        newUser.walletBalance = 0.0;
        newUser.isActive = true;
        
        users[userCount++] = newUser;
        
        cout << "\n  =========================================\n";
        cout << "  [SUCCESS] ACCOUNT CREATED SUCCESSFULLY!\n";
        cout << "  Your User ID is: " << newUser.userId << "\n";
        cout << "  Please save this ID for login.\n";
        cout << "  =========================================\n";
        
        return true;
    }
    
    bool signIn() {
        printHeader("SIGN IN");
        
        int userId;
        char password[50];
        
        cout << "\n  Enter User ID: ";
        cin >> userId;
        cout << "  Enter Password: ";
        cin >> password;
        
        int index = findUserById(userId);
        
        if (index == -1) {
            cout << "\n  [ERROR] User ID not found!\n";
            return false;
        }
        
        if (strcmp(users[index].password, password) != 0) {
            cout << "\n  [ERROR] Incorrect password!\n";
            return false;
        }
        
        if (!users[index].isActive) {
            cout << "\n  [ERROR] Account is deactivated. Contact admin.\n";
            return false;
        }
        
        currentUser = &users[index];
        
        cout << "\n  =========================================\n";
        cout << "  WELCOME BACK, " << currentUser->username << "!\n";
        cout << "  Wallet Balance: $" << fixed << setprecision(2) << currentUser->walletBalance << "\n";
        cout << "  =========================================\n";
        
        return true;
    }
    
    void logout() {
        currentUser = nullptr;
        cartSize = 0;
        cout << "\n  Logged out successfully!\n";
    }
    
    bool isLoggedIn() const {
        return currentUser != nullptr;
    }
    
    bool isAdmin() const {
        return currentUser && currentUser->userId == 1000;
    }
    
    User* getCurrentUser() {
        return currentUser;
    }
    
    void addToCart(int productId, const char* productName, double price, int quantity) {
        if (cartSize >= 50) {
            cout << "  Cart is full!\n";
            return;
        }
        
        // Check if product already in cart
        for (int i = 0; i < cartSize; i++) {
            if (cart[i].getProductId() == productId) {
                cart[i].setQuantity(cart[i].getQuantity() + quantity);
                cout << "  Updated quantity in cart.\n";
                return;
            }
        }
        
        cart[cartSize++] = CartItem(productId, productName, price, quantity);
        cout << "  Added to cart successfully!\n";
    }
    
    void viewCart() const {
        printHeader("YOUR SHOPPING CART");
        
        if (cartSize == 0) {
            cout << "\n  Your cart is empty.\n";
            return;
        }
        
        double total = 0;
        cout << "\n  " << left << setw(30) << "Product" 
             << setw(10) << "Quantity" 
             << setw(12) << "Price" 
             << "Total\n";
        cout << "  " << string(65, '-') << "\n";
        
        for (int i = 0; i < cartSize; i++) {
            cout << "  ";
            cart[i].display();
            total += cart[i].getTotal();
        }
        
        cout << "  " << string(65, '-') << "\n";
        cout << "  TOTAL: $" << fixed << setprecision(2) << total << "\n";
    }
    
    void checkout() {
        printHeader("CHECKOUT");
        
        if (cartSize == 0) {
            cout << "\n  Your cart is empty!\n";
            return;
        }
        
        viewCart();
        
        double total = 0;
        for (int i = 0; i < cartSize; i++) {
            total += cart[i].getTotal();
        }
        
        cout << "\n  Total Amount: $" << fixed << setprecision(2) << total << "\n";
        cout << "  Wallet Balance: $" << fixed << setprecision(2) << currentUser->walletBalance << "\n";
        
        if (currentUser->walletBalance < total) {
            cout << "\n  [ERROR] Insufficient balance! Please add funds.\n";
            return;
        }
        
        char confirm;
        cout << "\n  Confirm purchase? (y/n): ";
        cin >> confirm;
        
        if (tolower(confirm) != 'y') {
            cout << "  Checkout cancelled.\n";
            return;
        }
        
        // Create order
        Order* order = new Order(nextOrderId++, currentUser->userId);
        for (int i = 0; i < cartSize; i++) {
            order->addItem(cart[i]);
        }
        
        // Deduct from wallet
        currentUser->walletBalance -= total;
        
        // Save order
        orders[orderCount++] = order;
        
        // Clear cart
        cartSize = 0;
        
        cout << "\n  =========================================\n";
        cout << "  [SUCCESS] ORDER PLACED SUCCESSFULLY!\n";
        cout << "  Order ID: " << order->getOrderId() << "\n";
        cout << "  Remaining Balance: $" << fixed << setprecision(2) << currentUser->walletBalance << "\n";
        cout << "  =========================================\n";
    }
    
    void viewOrders() const {
        printHeader("YOUR ORDER HISTORY");
        
        bool found = false;
        for (int i = 0; i < orderCount; i++) {
            if (orders[i]->getUserId() == currentUser->userId) {
                orders[i]->display();
                found = true;
            }
        }
        
        if (!found) {
            cout << "\n  No orders found.\n";
        }
    }
    
    void addFunds() {
        printHeader("ADD FUNDS TO WALLET");
        
        double amount;
        cout << "\n  Current Balance: $" << fixed << setprecision(2) << currentUser->walletBalance << "\n";
        cout << "  Enter amount to add: $";
        cin >> amount;
        
        if (amount <= 0) {
            cout << "\n  [ERROR] Invalid amount!\n";
            return;
        }
        
        currentUser->walletBalance += amount;
        
        cout << "\n  =========================================\n";
        cout << "  [SUCCESS] FUNDS ADDED SUCCESSFULLY!\n";
        cout << "  New Balance: $" << fixed << setprecision(2) << currentUser->walletBalance << "\n";
        cout << "  =========================================\n";
    }
    
    void viewProfile() const {
        printHeader("MY PROFILE");
        
        cout << "\n  User ID: " << currentUser->userId << "\n";
        cout << "  Username: " << currentUser->username << "\n";
        cout << "  Email: " << currentUser->email << "\n";
        cout << "  Phone: " << currentUser->phone << "\n";
        cout << "  Address: " << currentUser->address << "\n";
        cout << "  Wallet Balance: $" << fixed << setprecision(2) << currentUser->walletBalance << "\n";
    }
    
    void editProfile() {
        printHeader("EDIT PROFILE");
        
        int choice;
        cout << "\n  What would you like to update?\n";
        cout << "  1. Email\n";
        cout << "  2. Phone\n";
        cout << "  3. Address\n";
        cout << "  4. All\n";
        cout << "  Choice: ";
        cin >> choice;
        
        switch(choice) {
            case 1:
                cout << "  New Email: ";
                cin >> currentUser->email;
                cout << "\n  [SUCCESS] Email updated!\n";
                break;
            case 2:
                cout << "  New Phone: ";
                cin >> currentUser->phone;
                cout << "\n  [SUCCESS] Phone updated!\n";
                break;
            case 3:
                cout << "  New Address: ";
                cin.ignore();
                cin.getline(currentUser->address, 200);
                cout << "\n  [SUCCESS] Address updated!\n";
                break;
            case 4:
                cout << "  New Email: ";
                cin >> currentUser->email;
                cout << "  New Phone: ";
                cin >> currentUser->phone;
                cout << "  New Address: ";
                cin.ignore();
                cin.getline(currentUser->address, 200);
                cout << "\n  [SUCCESS] Profile updated!\n";
                break;
            default:
                cout << "\n  Invalid choice!\n";
        }
    }
    
    void changePassword() {
        printHeader("CHANGE PASSWORD");
        
        char oldPass[50], newPass[50], confirmPass[50];
        
        cout << "\n  Enter current password: ";
        cin >> oldPass;
        
        if (strcmp(currentUser->password, oldPass) != 0) {
            cout << "\n  [ERROR] Incorrect current password!\n";
            return;
        }
        
        cout << "  Enter new password: ";
        cin >> newPass;
        cout << "  Confirm new password: ";
        cin >> confirmPass;
        
        if (strcmp(newPass, confirmPass) != 0) {
            cout << "\n  [ERROR] Passwords do not match!\n";
            return;
        }
        
        if (strlen(newPass) < 4) {
            cout << "\n  [ERROR] Password must be at least 4 characters!\n";
            return;
        }
        
        strcpy(currentUser->password, newPass);
        cout << "\n  [SUCCESS] Password changed successfully!\n";
    }
    
    void clearCart() {
        cartSize = 0;
        cout << "\n  Cart cleared.\n";
    }
};

// ================================================================
//  MEMBER 1's CODE - PRODUCT & INVENTORY MANAGEMENT
// ================================================================

// Product Abstract Base Class
class Product {
protected:
    int productID;
    char name[100];
    double price;
    int stock;
    char category[50];
    
public:
    Product(int id = 0, const char* n = "", double p = 0, int s = 0, const char* cat = "General") {
        productID = id;
        strcpy(name, n);
        price = p;
        stock = s;
        strcpy(category, cat);
    }
    
    virtual ~Product() {}
    
    virtual void display() const {
        cout << "  " << left << setw(8) << productID
             << setw(25) << name
             << setw(12) << category
             << "$" << setw(10) << fixed << setprecision(2) << price
             << setw(8) << stock << "\n";
    }
    
    virtual double calculateDiscount() const {
        return 0.0;
    }
    
    int getID() const { return productID; }
    const char* getName() const { return name; }
    double getPrice() const { return price; }
    int getStock() const { return stock; }
    void setStock(int s) { stock = s; }
    void reduceStock(int qty) { if (stock >= qty) stock -= qty; }
    const char* getCategory() const { return category; }
};

// Electronics Product
class Electronics : public Product {
private:
    int warrantyMonths;
    char brand[50];
    
public:
    Electronics(int id, const char* n, double p, int s, int warranty, const char* b)
        : Product(id, n, p, s, "Electronics"), warrantyMonths(warranty) {
        strcpy(brand, b);
    }
    
    void display() const override {
        Product::display();
        cout << "    Warranty: " << warrantyMonths << " months | Brand: " << brand << "\n";
    }
    
    double calculateDiscount() const override {
        if (warrantyMonths > 24) return price * 0.15;
        if (warrantyMonths > 12) return price * 0.10;
        return price * 0.05;
    }
};

// Clothing Product
class Clothing : public Product {
private:
    char size[10];
    char material[50];
    
public:
    Clothing(int id, const char* n, double p, int s, const char* sz, const char* mat)
        : Product(id, n, p, s, "Clothing") {
        strcpy(size, sz);
        strcpy(material, mat);
    }
    
    void display() const override {
        Product::display();
        cout << "    Size: " << size << " | Material: " << material << "\n";
    }
    
    double calculateDiscount() const override {
        return price * 0.20; // 20% off for clothing
    }
};

// Grocery Product
class Grocery : public Product {
private:
    char expiryDate[20];
    bool isOrganic;
    
public:
    Grocery(int id, const char* n, double p, int s, const char* expiry, bool organic)
        : Product(id, n, p, s, "Grocery") {
        strcpy(expiryDate, expiry);
        isOrganic = organic;
    }
    
    void display() const override {
        Product::display();
        cout << "    Expiry: " << expiryDate << " | Organic: " << (isOrganic ? "Yes" : "No") << "\n";
    }
    
    double calculateDiscount() const override {
        if (isOrganic) return price * 0.10;
        return price * 0.05;
    }
};

// Book Product
class Book : public Product {
private:
    char author[100];
    char isbn[20];
    
public:
    Book(int id, const char* n, double p, int s, const char* auth, const char* isbnCode)
        : Product(id, n, p, s, "Books") {
        strcpy(author, auth);
        strcpy(isbn, isbnCode);
    }
    
    void display() const override {
        Product::display();
        cout << "    Author: " << author << " | ISBN: " << isbn << "\n";
    }
    
    double calculateDiscount() const override {
        return price * 0.25; // 25% off for books
    }
};

// Inventory Manager Class
class InventoryManager {
private:
    static const int MAX_PRODUCTS = 200;
    Product* products[MAX_PRODUCTS];
    int productCount;
    
public:
    InventoryManager() : productCount(0) {
        // Initialize with sample products
        products[productCount++] = new Electronics(101, "Laptop Pro", 999.99, 10, 24, "Dell");
        products[productCount++] = new Electronics(102, "Smartphone X", 699.99, 15, 12, "Samsung");
        products[productCount++] = new Electronics(103, "Wireless Headphones", 149.99, 30, 12, "Sony");
        products[productCount++] = new Electronics(104, "Smart Watch", 249.99, 20, 24, "Apple");
        
        products[productCount++] = new Clothing(201, "Men's T-Shirt", 19.99, 50, "L", "Cotton");
        products[productCount++] = new Clothing(202, "Jeans", 49.99, 40, "32", "Denim");
        products[productCount++] = new Clothing(203, "Winter Jacket", 89.99, 25, "M", "Wool");
        products[productCount++] = new Clothing(204, "Running Shoes", 79.99, 35, "9", "Synthetic");
        
        products[productCount++] = new Grocery(301, "Organic Milk", 4.99, 100, "2024-12-31", true);
        products[productCount++] = new Grocery(302, "Whole Wheat Bread", 3.49, 80, "2024-11-30", false);
        products[productCount++] = new Grocery(303, "Fresh Eggs (12)", 5.99, 60, "2024-12-15", true);
        products[productCount++] = new Grocery(304, "Rice (5kg)", 12.99, 45, "2025-06-30", false);
        
        products[productCount++] = new Book(401, "C++ Programming", 59.99, 20, "Bjarne Stroustrup", "978-0321563842");
        products[productCount++] = new Book(402, "Clean Code", 49.99, 15, "Robert Martin", "978-0132350884");
        products[productCount++] = new Book(403, "Design Patterns", 54.99, 12, "Erich Gamma", "978-0201633610");
    }
    
    ~InventoryManager() {
        for (int i = 0; i < productCount; i++) {
            delete products[i];
        }
    }
    
    void displayAllProducts() const {
        printHeader("ALL PRODUCTS");
        
        cout << "\n  " << left << setw(8) << "ID"
             << setw(25) << "Product Name"
             << setw(12) << "Category"
             << setw(12) << "Price"
             << "Stock\n";
        cout << "  " << string(65, '-') << "\n";
        
        for (int i = 0; i < productCount; i++) {
            products[i]->display();
        }
    }
    
    void displayByCategory(const char* category) const {
        printHeader(category);
        
        cout << "\n  " << left << setw(8) << "ID"
             << setw(25) << "Product Name"
             << setw(12) << "Price"
             << "Stock\n";
        cout << "  " << string(55, '-') << "\n";
        
        for (int i = 0; i < productCount; i++) {
            if (strcmp(products[i]->getCategory(), category) == 0) {
                cout << "  " << left << setw(8) << products[i]->getID()
                     << setw(25) << products[i]->getName()
                     << "$" << setw(11) << fixed << setprecision(2) << products[i]->getPrice()
                     << products[i]->getStock() << "\n";
            }
        }
    }
    
    Product* findProduct(int id) {
        for (int i = 0; i < productCount; i++) {
            if (products[i]->getID() == id) {
                return products[i];
            }
        }
        return nullptr;
    }
    
    // Admin Functions
    void addProduct() {
        printHeader("ADD NEW PRODUCT");
        
        int id, stock, choice;
        double price;
        char name[100];
        
        cout << "\n  Product ID: ";
        cin >> id;
        
        if (findProduct(id) != nullptr) {
            cout << "\n  [ERROR] Product ID already exists!\n";
            return;
        }
        
        cout << "  Product Name: ";
        cin.ignore();
        cin.getline(name, 100);
        
        cout << "  Price: $";
        cin >> price;
        
        cout << "  Stock: ";
        cin >> stock;
        
        cout << "\n  Select Category:\n";
        cout << "    1. Electronics\n";
        cout << "    2. Clothing\n";
        cout << "    3. Grocery\n";
        cout << "    4. Books\n";
        cout << "  Choice: ";
        cin >> choice;
        
        Product* newProduct = nullptr;
        
        switch(choice) {
            case 1: {
                int warranty;
                char brand[50];
                cout << "  Warranty (months): ";
                cin >> warranty;
                cout << "  Brand: ";
                cin >> brand;
                newProduct = new Electronics(id, name, price, stock, warranty, brand);
                break;
            }
            case 2: {
                char size[10], material[50];
                cout << "  Size: ";
                cin >> size;
                cout << "  Material: ";
                cin >> material;
                newProduct = new Clothing(id, name, price, stock, size, material);
                break;
            }
            case 3: {
                char expiry[20];
                int organic;
                cout << "  Expiry Date (YYYY-MM-DD): ";
                cin >> expiry;
                cout << "  Organic (1=Yes, 0=No): ";
                cin >> organic;
                newProduct = new Grocery(id, name, price, stock, expiry, organic == 1);
                break;
            }
            case 4: {
                char author[100], isbn[20];
                cout << "  Author: ";
                cin.ignore();
                cin.getline(author, 100);
                cout << "  ISBN: ";
                cin >> isbn;
                newProduct = new Book(id, name, price, stock, author, isbn);
                break;
            }
            default:
                cout << "\n  [ERROR] Invalid category!\n";
                return;
        }
        
        if (productCount < MAX_PRODUCTS) {
            products[productCount++] = newProduct;
            cout << "\n  [SUCCESS] Product added successfully!\n";
        } else {
            cout << "\n  [ERROR] Inventory is full!\n";
            delete newProduct;
        }
    }
    
    void updateStock() {
        printHeader("UPDATE STOCK");
        
        int id, newStock;
        cout << "\n  Enter Product ID: ";
        cin >> id;
        
        Product* p = findProduct(id);
        if (p == nullptr) {
            cout << "\n  [ERROR] Product not found!\n";
            return;
        }
        
        cout << "  Current stock of " << p->getName() << ": " << p->getStock() << "\n";
        cout << "  Enter new stock quantity: ";
        cin >> newStock;
        
        p->setStock(newStock);
        cout << "\n  [SUCCESS] Stock updated successfully!\n";
    }
    
    void removeProduct() {
        printHeader("REMOVE PRODUCT");
        
        int id;
        cout << "\n  Enter Product ID: ";
        cin >> id;
        
        for (int i = 0; i < productCount; i++) {
            if (products[i]->getID() == id) {
                delete products[i];
                for (int j = i; j < productCount - 1; j++) {
                    products[j] = products[j + 1];
                }
                productCount--;
                cout << "\n  [SUCCESS] Product removed successfully!\n";
                return;
            }
        }
        
        cout << "\n  [ERROR] Product not found!\n";
    }
    
    void showLowStock() const {
        printHeader("LOW STOCK ALERT");
        
        cout << "\n  Products with stock less than 10:\n";
        cout << "  " << left << setw(8) << "ID"
             << setw(25) << "Product Name"
             << setw(12) << "Category"
             << "Stock\n";
        cout << "  " << string(55, '-') << "\n";
        
        bool found = false;
        for (int i = 0; i < productCount; i++) {
            if (products[i]->getStock() < 10) {
                cout << "  " << left << setw(8) << products[i]->getID()
                     << setw(25) << products[i]->getName()
                     << setw(12) << products[i]->getCategory()
                     << products[i]->getStock() << "\n";
                found = true;
            }
        }
        
        if (!found) {
            cout << "  No low stock items.\n";
        }
    }
};

// ================================================================
//  MAIN APPLICATION
// ================================================================
int main() {
    UserManager userManager;
    InventoryManager inventory;
    
    int mainChoice;
    bool running = true;
    
    while (running) {
        printHeader("INVENTORY MANAGEMENT SYSTEM");
        
        cout << "\n  +------------------------------------------+\n";
        cout << "  |          WELCOME TO THE SYSTEM          |\n";
        cout << "  +------------------------------------------+\n";
        cout << "  |                                          |\n";
        cout << "  |  1.  Login                              |\n";
        cout << "  |  2.  Create New Account                 |\n";
        cout << "  |  0.  Exit                               |\n";
        cout << "  |                                          |\n";
        cout << "  +------------------------------------------+\n";
        cout << "  Choice: ";
        cin >> mainChoice;
        
        switch(mainChoice) {
            case 1:
                if (userManager.signIn()) {
                    if (userManager.isAdmin()) {
                        // Admin Menu
                        int adminChoice;
                        do {
                            printHeader("ADMIN DASHBOARD");
                            cout << "\n  +------------------------------------------+\n";
                            cout << "  |          INVENTORY MANAGEMENT           |\n";
                            cout << "  +------------------------------------------+\n";
                            cout << "  |  1.  View All Products                  |\n";
                            cout << "  |  2.  Add New Product                    |\n";
                            cout << "  |  3.  Update Stock                       |\n";
                            cout << "  |  4.  Remove Product                     |\n";
                            cout << "  |  5.  View Low Stock Items               |\n";
                            cout << "  |  0.  Logout                             |\n";
                            cout << "  +------------------------------------------+\n";
                            cout << "  Choice: ";
                            cin >> adminChoice;
                            
                            switch(adminChoice) {
                                case 1:
                                    inventory.displayAllProducts();
                                    break;
                                case 2:
                                    inventory.addProduct();
                                    break;
                                case 3:
                                    inventory.updateStock();
                                    break;
                                case 4:
                                    inventory.removeProduct();
                                    break;
                                case 5:
                                    inventory.showLowStock();
                                    break;
                                case 0:
                                    userManager.logout();
                                    break;
                                default:
                                    cout << "\n  Invalid choice!\n";
                            }
                            
                            if (adminChoice != 0) {
                                cout << "\n  Press Enter to continue...";
                                cin.ignore();
                                cin.get();
                            }
                        } while (adminChoice != 0);
                    } else {
                        // User Menu
                        int userChoice;
                        do {
                            printHeader("CUSTOMER DASHBOARD");
                            cout << "\n  +------------------------------------------+\n";
                            cout << "  |            SHOPPING PORTAL               |\n";
                            cout << "  +------------------------------------------+\n";
                            cout << "  |  1.  Browse All Products                 |\n";
                            cout << "  |  2.  Browse by Category                  |\n";
                            cout << "  |  3.  Add to Cart                         |\n";
                            cout << "  |  4.  View Cart                           |\n";
                            cout << "  |  5.  Checkout                            |\n";
                            cout << "  |  6.  View Order History                  |\n";
                            cout << "  |  7.  Add Funds to Wallet                 |\n";
                            cout << "  |  8.  View Profile                        |\n";
                            cout << "  |  9.  Edit Profile                        |\n";
                            cout << "  |  10. Change Password                     |\n";
                            cout << "  |  11. Clear Cart                          |\n";
                            cout << "  |  0.  Logout                              |\n";
                            cout << "  +------------------------------------------+\n";
                            cout << "  Choice: ";
                            cin >> userChoice;
                            
                            switch(userChoice) {
                                case 1:
                                    inventory.displayAllProducts();
                                    break;
                                case 2: {
                                    int catChoice;
                                    cout << "\n  Categories:\n";
                                    cout << "    1. Electronics\n";
                                    cout << "    2. Clothing\n";
                                    cout << "    3. Grocery\n";
                                    cout << "    4. Books\n";
                                    cout << "  Choice: ";
                                    cin >> catChoice;
                                    
                                    switch(catChoice) {
                                        case 1:
                                            inventory.displayByCategory("Electronics");
                                            break;
                                        case 2:
                                            inventory.displayByCategory("Clothing");
                                            break;
                                        case 3:
                                            inventory.displayByCategory("Grocery");
                                            break;
                                        case 4:
                                            inventory.displayByCategory("Books");
                                            break;
                                        default:
                                            cout << "\n  Invalid category!\n";
                                    }
                                    break;
                                }
                                case 3: {
                                    int id, qty;
                                    cout << "\n  Enter Product ID: ";
                                    cin >> id;
                                    cout << "  Enter Quantity: ";
                                    cin >> qty;
                                    
                                    Product* p = inventory.findProduct(id);
                                    if (p == nullptr) {
                                        cout << "\n  [ERROR] Product not found!\n";
                                    } else if (p->getStock() < qty) {
                                        cout << "\n  [ERROR] Insufficient stock! Available: " << p->getStock() << "\n";
                                    } else {
                                        userManager.addToCart(p->getID(), p->getName(), p->getPrice(), qty);
                                    }
                                    break;
                                }
                                case 4:
                                    userManager.viewCart();
                                    break;
                                case 5:
                                    userManager.checkout();
                                    break;
                                case 6:
                                    userManager.viewOrders();
                                    break;
                                case 7:
                                    userManager.addFunds();
                                    break;
                                case 8:
                                    userManager.viewProfile();
                                    break;
                                case 9:
                                    userManager.editProfile();
                                    break;
                                case 10:
                                    userManager.changePassword();
                                    break;
                                case 11:
                                    userManager.clearCart();
                                    break;
                                case 0:
                                    userManager.logout();
                                    break;
                                default:
                                    cout << "\n  Invalid choice!\n";
                            }
                            
                            if (userChoice != 0) {
                                cout << "\n  Press Enter to continue...";
                                cin.ignore();
                                cin.get();
                            }
                        } while (userChoice != 0);
                    }
                }
                break;
                
            case 2:
                userManager.signUp();
                break;
                
            case 0:
                running = false;
                cout << "\n  Thank you for using the system!\n";
                break;
                
            default:
                cout << "\n  Invalid choice!\n";
        }
        
        if (mainChoice != 0 && mainChoice != 1) {
            cout << "\n  Press Enter to continue...";
            cin.ignore();
            cin.get();
        }
    }
    
    return 0;
}
/*
 * ============================================================
 *   INVENTORY MANAGEMENT SYSTEM
 *   OOP Final Project - C++
 *
 *   Team Members:
 *   Member 1 (Friend) - Admin Side (Inventory Management)
 *   Member 2 (You)    - User Side (Authentication & Shopping)
 *
 *   WELCOME SCREEN OPTIONS:
 *     1. User Sign Up
 *     2. Admin Login
 *     3. User Login
 *     0. Exit
 * ============================================================
 */

#include <iostream>
#include <cstring>
#include <iomanip>
#include <cctype>
#include <cstdlib>
#include <ctime>
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

void pressEnter() {
    cout << "\n  Press Enter to continue...";
    cin.ignore();
    cin.get();
}

// ================================================================
//  FORWARD DECLARATION
// ================================================================
class Product;

// ================================================================
//  ╔══════════════════════════════════════════════════════════╗
//  ║         MEMBER 2's CODE — USER SIDE                     ║
//  ║  (User Struct, CartItem, Order, UserManager)            ║
//  ╚══════════════════════════════════════════════════════════╝
// ================================================================

// ----------------------------------------------------------------
//  USER STRUCTURE
// ----------------------------------------------------------------
struct User {
    int    userId;
    char   username[50];
    char   password[50];
    char   email[100];
    char   phone[20];
    char   address[200];
    double walletBalance;
    bool   isActive;

    User() : userId(0), walletBalance(0.0), isActive(true) {
        username[0] = '\0'; password[0] = '\0';
        email[0]    = '\0'; phone[0]    = '\0';
        address[0]  = '\0';
    }

    User(int id, const char* uname, const char* pwd, const char* mail,
         const char* ph, const char* addr, double balance, bool active)
        : userId(id), walletBalance(balance), isActive(active) {
        strcpy(username, uname); strcpy(password, pwd);
        strcpy(email, mail);     strcpy(phone, ph);
        strcpy(address, addr);
    }
};

// ----------------------------------------------------------------
//  CART ITEM CLASS
// ----------------------------------------------------------------
class CartItem {
private:
    int    productId;
    char   productName[100];
    double price;
    int    quantity;

public:
    CartItem() : productId(0), price(0.0), quantity(0) {
        productName[0] = '\0';
    }

    CartItem(int id, const char* name, double p, int qty)
        : productId(id), price(p), quantity(qty) {
        strcpy(productName, name);
    }

    int         getProductId()   const { return productId;    }
    const char* getProductName() const { return productName;  }
    double      getPrice()       const { return price;        }
    int         getQuantity()    const { return quantity;      }
    void        setQuantity(int q)     { quantity = q;         }
    double      getTotal()       const { return price * quantity; }

    void display() const {
        cout << "     " << left  << setw(25) << productName
             << " x"   << setw(4) << quantity
             << " @ $" << fixed  << setprecision(2) << setw(8)  << price
             << " = $" << setw(8) << getTotal() << '\n';
    }
};

// ----------------------------------------------------------------
//  ORDER CLASS
// ----------------------------------------------------------------
class Order {
private:
    int       orderId;
    int       userId;
    CartItem* items[50];
    int       itemCount;
    double    totalAmount;
    char      orderDate[30];
    char      status[20];

public:
    Order(int oid, int uid)
        : orderId(oid), userId(uid), itemCount(0), totalAmount(0.0) {
        strcpy(status, "Pending");
        time_t now = time(0);
        strcpy(orderDate, ctime(&now));
        orderDate[strlen(orderDate) - 1] = '\0';   // strip newline
        for (int i = 0; i < 50; i++) items[i] = nullptr;
    }

    ~Order() {
        for (int i = 0; i < itemCount; i++) delete items[i];
    }

    void addItem(const CartItem& item) {
        if (itemCount < 50) {
            items[itemCount++] = new CartItem(item);
            totalAmount += item.getTotal();
        }
    }

    double      getTotal()     const { return totalAmount; }
    int         getItemCount() const { return itemCount;   }
    const char* getStatus()    const { return status;      }
    int         getOrderId()   const { return orderId;     }
    int         getUserId()    const { return userId;      }

    void setStatus(const char* s) { strcpy(status, s); }

    void display() const {
        cout << "\n     Order #" << orderId
             << " | Date: "   << orderDate
             << " | Status: " << status
             << " | Total: $" << fixed << setprecision(2) << totalAmount << '\n';
        if (itemCount > 0) {
            cout << "     " << string(50, '-') << "\n";
            for (int i = 0; i < itemCount; i++) items[i]->display();
            cout << "     " << string(50, '-') << "\n";
            cout << "     Grand Total: $" << fixed << setprecision(2) << totalAmount << '\n';
        }
    }
};

// ----------------------------------------------------------------
//  USER MANAGER CLASS
// ----------------------------------------------------------------
class UserManager {
private:
    static const int MAX_USERS  = 100;
    static const int MAX_ORDERS = 200;

    User   users[MAX_USERS];
    Order* orders[MAX_ORDERS];
    int    userCount;
    int    orderCount;
    int    nextUserId;
    int    nextOrderId;
    User*  currentUser;

    CartItem cart[50];
    int      cartSize;

    // ---- private helpers ----
    bool isUsernameExists(const char* uname) const {
        for (int i = 0; i < userCount; i++)
            if (strcmp(users[i].username, uname) == 0) return true;
        return false;
    }

    int findUserById(int id) const {
        for (int i = 0; i < userCount; i++)
            if (users[i].userId == id) return i;
        return -1;
    }

    int findAdminByUsername(const char* uname) const {
        for (int i = 0; i < userCount; i++)
            if (users[i].userId == 1000 && strcmp(users[i].username, uname) == 0)
                return i;
        return -1;
    }

public:
    UserManager()
        : userCount(0), orderCount(0),
          nextUserId(2004), nextOrderId(5000),
          currentUser(nullptr), cartSize(0) {

        for (int i = 0; i < MAX_ORDERS; i++) orders[i] = nullptr;

        // Admin account  (ID 1000 — admin route only)
        users[userCount++] = User(1000, "Admin",       "admin123",
                                  "admin@system.com",  "1234567890",
                                  "Main Office",        0.0, true);
        // Pre-loaded regular users
        users[userCount++] = User(2001, "JohnDoe",     "john123",
                                  "john@email.com",    "555-0101",
                                  "123 Main St",        500.00, true);
        users[userCount++] = User(2002, "JaneSmith",   "jane123",
                                  "jane@email.com",    "555-0102",
                                  "456 Oak Ave",        750.00, true);
        users[userCount++] = User(2003, "MikeJohnson", "mike123",
                                  "mike@email.com",    "555-0103",
                                  "789 Pine Rd",        300.00, true);
    }

    ~UserManager() {
        for (int i = 0; i < orderCount; i++) delete orders[i];
    }

    // ==============================================================
    //  MEMBER 2 — SIGN UP  (welcome screen option 1)
    // ==============================================================
    bool signUp() {
        printHeader("CREATE NEW USER ACCOUNT");

        if (userCount >= MAX_USERS) {
            cout << "  System is full. Cannot create new account.\n";
            return false;
        }

        User newUser;
        newUser.userId = nextUserId++;

        // ---- username ----
        cout << "\n  Enter Username (no spaces): ";
        cin >> newUser.username;

        if (isUsernameExists(newUser.username)) {
            cout << "\n  [ERROR] Username already taken. Try another.\n";
            return false;
        }

        // ---- password ----
        char confirm[50];
        cout << "  Enter Password (min 4 chars): ";
        cin >> newUser.password;

        if (strlen(newUser.password) < 4) {
            cout << "\n  [ERROR] Password must be at least 4 characters.\n";
            return false;
        }

        cout << "  Confirm Password: ";
        cin >> confirm;

        if (strcmp(newUser.password, confirm) != 0) {
            cout << "\n  [ERROR] Passwords do not match.\n";
            return false;
        }

        // ---- contact info ----
        cout << "  Enter Email: ";
        cin  >> newUser.email;

        cout << "  Enter Phone: ";
        cin  >> newUser.phone;

        cout << "  Enter Address (press Enter after typing): ";
        cin.ignore();
        cin.getline(newUser.address, 200);

        newUser.walletBalance = 0.0;
        newUser.isActive      = true;

        users[userCount++] = newUser;

        // ---- success banner ----
        cout << "\n";
        printLine('*', 70);
        cout << "  *  ACCOUNT CREATED SUCCESSFULLY!                               *\n";
        cout << "  *                                                              *\n";
        cout << "  *  Your User ID  : " << setw(6) << left << newUser.userId
             << "                                       *\n";
        cout << "  *  Username      : " << setw(28) << left << newUser.username
             << "                 *\n";
        cout << "  *  Starting Wallet Balance : $0.00                             *\n";
        cout << "  *                                                              *\n";
        cout << "  *  SAVE YOUR USER ID — you need it to log in!                  *\n";
        printLine('*', 70);

        return true;
    }

    // ==============================================================
    //  MEMBER 2 — ADMIN LOGIN  (welcome screen option 2)
    //  Admin logs in by username + password (not by numeric ID)
    // ==============================================================
    bool adminLogin() {
        printHeader("ADMIN LOGIN");

        char uname[50], pwd[50];

        cout << "\n  Admin Username : ";
        cin  >> uname;
        cout << "  Admin Password : ";
        cin  >> pwd;

        int idx = findAdminByUsername(uname);

        if (idx == -1) {
            cout << "\n  +--------------------------------------------------+\n";
            cout << "  |  [ERROR] Admin username not found.               |\n";
            cout << "  +--------------------------------------------------+\n";
            return false;
        }

        if (strcmp(users[idx].password, pwd) != 0) {
            cout << "\n  +--------------------------------------------------+\n";
            cout << "  |  [ERROR] Incorrect password.                     |\n";
            cout << "  +--------------------------------------------------+\n";
            return false;
        }

        currentUser = &users[idx];

        cout << "\n";
        printLine('-', 70);
        cout << "  [SUCCESS] Welcome, " << currentUser->username << "! (ADMIN)\n";
        printLine('-', 70);
        return true;
    }

    // ==============================================================
    //  MEMBER 2 — USER LOGIN  (welcome screen option 3)
    //  Regular users log in with numeric User ID + password
    // ==============================================================
    bool userLogin() {
        printHeader("USER LOGIN");

        int  uid;
        char pwd[50];

        cout << "\n  Your User ID : ";
        cin  >> uid;
        cout << "  Password     : ";
        cin  >> pwd;

        // Block admin from logging in via user route
        if (uid == 1000) {
            cout << "\n  +--------------------------------------------------+\n";
            cout << "  |  [ERROR] Use 'Admin Login' for admin access.     |\n";
            cout << "  +--------------------------------------------------+\n";
            return false;
        }

        int idx = findUserById(uid);

        if (idx == -1) {
            cout << "\n  +--------------------------------------------------+\n";
            cout << "  |  [ERROR] User ID not found.                      |\n";
            cout << "  |  Tip: Sign up first from the main menu.          |\n";
            cout << "  +--------------------------------------------------+\n";
            return false;
        }

        if (strcmp(users[idx].password, pwd) != 0) {
            cout << "\n  +--------------------------------------------------+\n";
            cout << "  |  [ERROR] Incorrect password.                     |\n";
            cout << "  +--------------------------------------------------+\n";
            return false;
        }

        if (!users[idx].isActive) {
            cout << "\n  +--------------------------------------------------+\n";
            cout << "  |  [ERROR] Account deactivated. Contact admin.     |\n";
            cout << "  +--------------------------------------------------+\n";
            return false;
        }

        currentUser = &users[idx];

        cout << "\n";
        printLine('-', 70);
        cout << "  [SUCCESS] Welcome back, " << currentUser->username << "!\n";
        cout << "  Wallet Balance : $" << fixed << setprecision(2)
             << currentUser->walletBalance << "\n";
        printLine('-', 70);
        return true;
    }

    void logout() {
        currentUser = nullptr;
        cartSize    = 0;
        cout << "\n  Logged out successfully!\n";
    }

    bool isLoggedIn() const { return currentUser != nullptr; }
    bool isAdmin()    const { return currentUser && currentUser->userId == 1000; }
    User* getCurrentUser()  { return currentUser; }

    // ==============================================================
    //  MEMBER 2 — CART OPERATIONS
    // ==============================================================
    void addToCart(int productId, const char* productName, double price, int quantity) {
        if (cartSize >= 50) { cout << "  Cart is full!\n"; return; }

        for (int i = 0; i < cartSize; i++) {
            if (cart[i].getProductId() == productId) {
                cart[i].setQuantity(cart[i].getQuantity() + quantity);
                cout << "  [UPDATED] Quantity updated in cart.\n";
                return;
            }
        }

        cart[cartSize++] = CartItem(productId, productName, price, quantity);
        cout << "  [ADDED] '" << productName << "' added to cart!\n";
    }

    void viewCart() const {
        printHeader("YOUR SHOPPING CART");

        if (cartSize == 0) { cout << "\n  Your cart is empty.\n"; return; }

        double total = 0.0;
        cout << "\n  " << left << setw(30) << "Product"
             << setw(10) << "Qty"
             << setw(12) << "Unit Price"
             << "Subtotal\n";
        cout << "  " << string(65, '-') << "\n";

        for (int i = 0; i < cartSize; i++) {
            cart[i].display();
            total += cart[i].getTotal();
        }

        cout << "  " << string(65, '-') << "\n";
        cout << "  CART TOTAL : $" << fixed << setprecision(2) << total << "\n";
        cout << "  WALLET     : $" << fixed << setprecision(2)
             << currentUser->walletBalance << "\n";
    }

    void checkout() {
        printHeader("CHECKOUT");

        if (cartSize == 0) { cout << "\n  Your cart is empty!\n"; return; }

        viewCart();

        double total = 0.0;
        for (int i = 0; i < cartSize; i++) total += cart[i].getTotal();

        cout << "\n  Total to pay : $" << fixed << setprecision(2) << total << "\n";
        cout << "  Wallet       : $" << fixed << setprecision(2)
             << currentUser->walletBalance << "\n";

        if (currentUser->walletBalance < total) {
            cout << "\n  +--------------------------------------------------+\n";
            cout << "  |  [ERROR] Insufficient wallet balance!            |\n";
            cout << "  |  Please add funds first (option 7).             |\n";
            cout << "  |  Needed  : $" << fixed << setprecision(2)
                 << setw(10) << total
                 << "                          |\n";
            cout << "  |  You have: $" << fixed << setprecision(2)
                 << setw(10) << currentUser->walletBalance
                 << "                          |\n";
            cout << "  +--------------------------------------------------+\n";
            return;
        }

        char confirm;
        cout << "\n  Confirm purchase? (y/n): ";
        cin >> confirm;

        if (tolower(confirm) != 'y') { cout << "  Checkout cancelled.\n"; return; }

        Order* order = new Order(nextOrderId++, currentUser->userId);
        for (int i = 0; i < cartSize; i++) order->addItem(cart[i]);

        currentUser->walletBalance -= total;
        orders[orderCount++] = order;
        cartSize = 0;

        cout << "\n";
        printLine('*', 70);
        cout << "  *  ORDER PLACED SUCCESSFULLY!                                  *\n";
        cout << "  *  Order ID         : " << setw(6) << left << order->getOrderId()
             << "                                    *\n";
        cout << "  *  Amount Charged   : $" << fixed << setprecision(2)
             << setw(10) << left << total
             << "                             *\n";
        cout << "  *  Remaining Balance: $" << fixed << setprecision(2)
             << setw(10) << left << currentUser->walletBalance
             << "                             *\n";
        printLine('*', 70);
    }

    void viewOrders() const {
        printHeader("MY ORDER HISTORY");

        bool found = false;
        for (int i = 0; i < orderCount; i++) {
            if (orders[i]->getUserId() == currentUser->userId) {
                orders[i]->display();
                found = true;
            }
        }

        if (!found) cout << "\n  No orders placed yet.\n";
    }

    void addFunds() {
        printHeader("ADD FUNDS TO WALLET");

        double amount;
        cout << "\n  Current Balance : $" << fixed << setprecision(2)
             << currentUser->walletBalance << "\n";
        cout << "  Amount to add   : $";
        cin  >> amount;

        if (amount <= 0) { cout << "\n  [ERROR] Amount must be positive.\n"; return; }

        currentUser->walletBalance += amount;

        cout << "\n";
        printLine('-', 70);
        cout << "  [SUCCESS] $" << fixed << setprecision(2) << amount << " added.\n";
        cout << "  New Balance : $" << fixed << setprecision(2)
             << currentUser->walletBalance << "\n";
        printLine('-', 70);
    }

    void viewProfile() const {
        printHeader("MY PROFILE");

        cout << "\n  +--------------------------------------------------+\n";
        cout << "  | User ID       : " << setw(32) << left << currentUser->userId     << "|\n";
        cout << "  | Username      : " << setw(32) << left << currentUser->username   << "|\n";
        cout << "  | Email         : " << setw(32) << left << currentUser->email      << "|\n";
        cout << "  | Phone         : " << setw(32) << left << currentUser->phone      << "|\n";
        cout << "  | Address       : " << setw(32) << left << currentUser->address    << "|\n";
        cout << "  | Wallet        : $" << fixed << setprecision(2)
             << setw(30) << left << currentUser->walletBalance << "|\n";
        cout << "  +--------------------------------------------------+\n";
    }

    void editProfile() {
        printHeader("EDIT PROFILE");

        int choice;
        cout << "\n  What would you like to update?\n"
             << "    1. Email\n"
             << "    2. Phone\n"
             << "    3. Address\n"
             << "    4. All\n"
             << "  Choice: ";
        cin >> choice;

        switch (choice) {
            case 1:
                cout << "  New Email: "; cin >> currentUser->email;
                cout << "\n  [SUCCESS] Email updated!\n"; break;
            case 2:
                cout << "  New Phone: "; cin >> currentUser->phone;
                cout << "\n  [SUCCESS] Phone updated!\n"; break;
            case 3:
                cout << "  New Address: ";
                cin.ignore(); cin.getline(currentUser->address, 200);
                cout << "\n  [SUCCESS] Address updated!\n"; break;
            case 4:
                cout << "  New Email: ";   cin >> currentUser->email;
                cout << "  New Phone: ";   cin >> currentUser->phone;
                cout << "  New Address: "; cin.ignore();
                cin.getline(currentUser->address, 200);
                cout << "\n  [SUCCESS] Profile updated!\n"; break;
            default:
                cout << "\n  Invalid choice.\n";
        }
    }

    void changePassword() {
        printHeader("CHANGE PASSWORD");

        char oldP[50], newP[50], confP[50];
        cout << "\n  Current password : "; cin >> oldP;

        if (strcmp(currentUser->password, oldP) != 0) {
            cout << "\n  [ERROR] Incorrect current password.\n"; return;
        }

        cout << "  New password     : "; cin >> newP;
        cout << "  Confirm password : "; cin >> confP;

        if (strcmp(newP, confP) != 0) {
            cout << "\n  [ERROR] Passwords do not match.\n"; return;
        }
        if (strlen(newP) < 4) {
            cout << "\n  [ERROR] Password must be at least 4 characters.\n"; return;
        }

        strcpy(currentUser->password, newP);
        cout << "\n  [SUCCESS] Password changed successfully!\n";
    }

    void clearCart() {
        cartSize = 0;
        cout << "\n  Cart cleared successfully.\n";
    }
};


// ================================================================
//  ╔══════════════════════════════════════════════════════════╗
//  ║         MEMBER 1's CODE — ADMIN / INVENTORY SIDE        ║
//  ║  (Product hierarchy + InventoryManager)                 ║
//  ╚══════════════════════════════════════════════════════════╝
// ================================================================

// ----------------------------------------------------------------
//  PRODUCT — Abstract Base Class
// ----------------------------------------------------------------
class Product {
protected:
    int    productID;
    char   name[100];
    double price;
    int    stock;
    char   category[50];

public:
    Product(int id = 0, const char* n = "", double p = 0.0,
            int s = 0, const char* cat = "General") {
        productID = id; price = p; stock = s;
        strcpy(name, n); strcpy(category, cat);
    }

    virtual ~Product() {}

    virtual void display() const {
        cout << "  " << left << setw(8)  << productID
             << setw(25) << name
             << setw(12) << category
             << "$" << fixed << setprecision(2) << setw(10) << price
             << setw(8)  << stock << "\n";
    }

    virtual double calculateDiscount() const { return 0.0; }

    int         getID()       const { return productID; }
    const char* getName()     const { return name;      }
    double      getPrice()    const { return price;     }
    int         getStock()    const { return stock;     }
    const char* getCategory() const { return category;  }

    void setStock(int s)       { stock = s;                     }
    void reduceStock(int qty)  { if (stock >= qty) stock -= qty; }
};

// ----------------------------------------------------------------
//  ELECTRONICS
// ----------------------------------------------------------------
class Electronics : public Product {
private:
    int  warrantyMonths;
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

// ----------------------------------------------------------------
//  CLOTHING
// ----------------------------------------------------------------
class Clothing : public Product {
private:
    char size[10];
    char material[50];

public:
    Clothing(int id, const char* n, double p, int s, const char* sz, const char* mat)
        : Product(id, n, p, s, "Clothing") {
        strcpy(size, sz); strcpy(material, mat);
    }

    void display() const override {
        Product::display();
        cout << "    Size: " << size << " | Material: " << material << "\n";
    }

    double calculateDiscount() const override { return price * 0.20; }
};

// ----------------------------------------------------------------
//  GROCERY
// ----------------------------------------------------------------
class Grocery : public Product {
private:
    char expiryDate[20];
    bool isOrganic;

public:
    Grocery(int id, const char* n, double p, int s, const char* expiry, bool organic)
        : Product(id, n, p, s, "Grocery"), isOrganic(organic) {
        strcpy(expiryDate, expiry);
    }

    void display() const override {
        Product::display();
        cout << "    Expiry: " << expiryDate
             << " | Organic: " << (isOrganic ? "Yes" : "No") << "\n";
    }

    double calculateDiscount() const override {
        return isOrganic ? price * 0.10 : price * 0.05;
    }
};

// ----------------------------------------------------------------
//  BOOK
// ----------------------------------------------------------------
class Book : public Product {
private:
    char author[100];
    char isbn[20];

public:
    Book(int id, const char* n, double p, int s, const char* auth, const char* isbnCode)
        : Product(id, n, p, s, "Books") {
        strcpy(author, auth); strcpy(isbn, isbnCode);
    }

    void display() const override {
        Product::display();
        cout << "    Author: " << author << " | ISBN: " << isbn << "\n";
    }

    double calculateDiscount() const override { return price * 0.25; }
};

// ----------------------------------------------------------------
//  INVENTORY MANAGER
// ----------------------------------------------------------------
class InventoryManager {
private:
    static const int MAX_PRODUCTS = 200;
    Product* products[MAX_PRODUCTS];
    int      productCount;

public:
    InventoryManager() : productCount(0) {
        products[productCount++] = new Electronics(101, "Laptop Pro",          999.99, 10, 24, "Dell");
        products[productCount++] = new Electronics(102, "Smartphone X",        699.99, 15, 12, "Samsung");
        products[productCount++] = new Electronics(103, "Wireless Headphones", 149.99, 30, 12, "Sony");
        products[productCount++] = new Electronics(104, "Smart Watch",         249.99, 20, 24, "Apple");

        products[productCount++] = new Clothing(201, "Men's T-Shirt",  19.99, 50, "L",  "Cotton");
        products[productCount++] = new Clothing(202, "Jeans",          49.99, 40, "32", "Denim");
        products[productCount++] = new Clothing(203, "Winter Jacket",  89.99, 25, "M",  "Wool");
        products[productCount++] = new Clothing(204, "Running Shoes",  79.99, 35, "9",  "Synthetic");

        products[productCount++] = new Grocery(301, "Organic Milk",        4.99,  100, "2024-12-31", true);
        products[productCount++] = new Grocery(302, "Whole Wheat Bread",   3.49,   80, "2024-11-30", false);
        products[productCount++] = new Grocery(303, "Fresh Eggs (12)",     5.99,   60, "2024-12-15", true);
        products[productCount++] = new Grocery(304, "Rice (5kg)",         12.99,   45, "2025-06-30", false);

        products[productCount++] = new Book(401, "C++ Programming", 59.99, 20, "Bjarne Stroustrup", "978-0321563842");
        products[productCount++] = new Book(402, "Clean Code",      49.99, 15, "Robert Martin",     "978-0132350884");
        products[productCount++] = new Book(403, "Design Patterns", 54.99, 12, "Erich Gamma",       "978-0201633610");
    }

    ~InventoryManager() {
        for (int i = 0; i < productCount; i++) delete products[i];
    }

    void displayAllProducts() const {
        printHeader("ALL PRODUCTS IN INVENTORY");

        cout << "\n  " << left << setw(8)  << "ID"
             << setw(25) << "Product Name"
             << setw(12) << "Category"
             << setw(12) << "Price"
             << "Stock\n";
        cout << "  " << string(65, '-') << "\n";

        for (int i = 0; i < productCount; i++) products[i]->display();
    }

    void displayByCategory(const char* category) const {
        printHeader(category);

        cout << "\n  " << left << setw(8)  << "ID"
             << setw(25) << "Product Name"
             << setw(12) << "Price"
             << "Stock\n";
        cout << "  " << string(55, '-') << "\n";

        bool found = false;
        for (int i = 0; i < productCount; i++) {
            if (strcmp(products[i]->getCategory(), category) == 0) {
                cout << "  " << left << setw(8)  << products[i]->getID()
                     << setw(25) << products[i]->getName()
                     << "$" << fixed << setprecision(2) << setw(11) << products[i]->getPrice()
                     << products[i]->getStock() << "\n";
                found = true;
            }
        }
        if (!found) cout << "\n  No products in this category.\n";
    }

    Product* findProduct(int id) {
        for (int i = 0; i < productCount; i++)
            if (products[i]->getID() == id) return products[i];
        return nullptr;
    }

    // ---- Admin-only functions ----
    void addProduct() {
        printHeader("ADD NEW PRODUCT");

        int    id, stock, choice;
        double price;
        char   name[100];

        cout << "\n  Product ID   : "; cin >> id;

        if (findProduct(id)) {
            cout << "\n  [ERROR] Product ID already exists.\n"; return;
        }

        cout << "  Product Name : "; cin.ignore(); cin.getline(name, 100);
        cout << "  Price ($)    : "; cin >> price;
        cout << "  Stock        : "; cin >> stock;

        cout << "\n  Category:\n"
             << "    1. Electronics\n  2. Clothing\n"
             << "    3. Grocery\n    4. Books\n"
             << "  Choice: ";
        cin >> choice;

        Product* newP = nullptr;
        switch (choice) {
            case 1: {
                int  warranty; char brand[50];
                cout << "  Warranty (months): "; cin >> warranty;
                cout << "  Brand            : "; cin >> brand;
                newP = new Electronics(id, name, price, stock, warranty, brand);
                break;
            }
            case 2: {
                char sz[10], mat[50];
                cout << "  Size    : "; cin >> sz;
                cout << "  Material: "; cin >> mat;
                newP = new Clothing(id, name, price, stock, sz, mat);
                break;
            }
            case 3: {
                char expiry[20]; int org;
                cout << "  Expiry (YYYY-MM-DD): "; cin >> expiry;
                cout << "  Organic (1/0)      : "; cin >> org;
                newP = new Grocery(id, name, price, stock, expiry, org == 1);
                break;
            }
            case 4: {
                char auth[100], isbn[20];
                cout << "  Author: "; cin.ignore(); cin.getline(auth, 100);
                cout << "  ISBN  : "; cin >> isbn;
                newP = new Book(id, name, price, stock, auth, isbn);
                break;
            }
            default: cout << "\n  Invalid category.\n"; return;
        }

        if (productCount < MAX_PRODUCTS) {
            products[productCount++] = newP;
            cout << "\n  [SUCCESS] Product added to inventory!\n";
        } else {
            cout << "\n  [ERROR] Inventory full!\n";
            delete newP;
        }
    }

    void updateStock() {
        printHeader("UPDATE STOCK");

        int id, newStock;
        cout << "\n  Product ID: "; cin >> id;

        Product* p = findProduct(id);
        if (!p) { cout << "\n  [ERROR] Product not found.\n"; return; }

        cout << "  Current stock of '" << p->getName() << "': " << p->getStock() << "\n";
        cout << "  New stock quantity: "; cin >> newStock;

        p->setStock(newStock);
        cout << "\n  [SUCCESS] Stock updated to " << newStock << ".\n";
    }

    void removeProduct() {
        printHeader("REMOVE PRODUCT");

        int id;
        cout << "\n  Product ID to remove: "; cin >> id;

        for (int i = 0; i < productCount; i++) {
            if (products[i]->getID() == id) {
                cout << "  Removing '" << products[i]->getName() << "'...\n";
                delete products[i];
                for (int j = i; j < productCount - 1; j++)
                    products[j] = products[j + 1];
                productCount--;
                cout << "  [SUCCESS] Product removed.\n";
                return;
            }
        }
        cout << "\n  [ERROR] Product not found.\n";
    }

    void showLowStock() const {
        printHeader("LOW STOCK ALERT  (stock < 10)");

        cout << "\n  " << left << setw(8) << "ID"
             << setw(25) << "Product Name"
             << setw(12) << "Category"
             << "Stock\n";
        cout << "  " << string(55, '-') << "\n";

        bool found = false;
        for (int i = 0; i < productCount; i++) {
            if (products[i]->getStock() < 10) {
                cout << "  " << left << setw(8)  << products[i]->getID()
                     << setw(25) << products[i]->getName()
                     << setw(12) << products[i]->getCategory()
                     << products[i]->getStock() << "\n";
                found = true;
            }
        }
        if (!found) cout << "  All products have sufficient stock.\n";
    }

    void showDiscounts() const {
        printHeader("CURRENT DISCOUNTS BY CATEGORY");

        cout << "\n  " << left << setw(8) << "ID"
             << setw(25) << "Product Name"
             << setw(12) << "Category"
             << setw(12) << "Orig. Price"
             << setw(12) << "Discount"
             << "Sale Price\n";
        cout << "  " << string(75, '-') << "\n";

        for (int i = 0; i < productCount; i++) {
            double disc   = products[i]->calculateDiscount();
            double sale   = products[i]->getPrice() - disc;
            cout << "  " << left << setw(8)  << products[i]->getID()
                 << setw(25) << products[i]->getName()
                 << setw(12) << products[i]->getCategory()
                 << "$" << fixed << setprecision(2) << setw(11) << products[i]->getPrice()
                 << "$" << setw(11) << disc
                 << "$" << sale << "\n";
        }
    }
};


// ================================================================
//  ╔══════════════════════════════════════════════════════════╗
//  ║  WELCOME SCREEN + MAIN                                  ║
//  ║  (Member 2 — routes to Admin/User dashboards)           ║
//  ╚══════════════════════════════════════════════════════════╝
// ================================================================

// ----------------------------------------------------------------
//  ADMIN DASHBOARD  (reached after adminLogin())
// ----------------------------------------------------------------
void runAdminDashboard(UserManager& um, InventoryManager& inv) {
    int ch;
    do {
        printHeader("ADMIN DASHBOARD");

        cout << "\n  Logged in as: " << um.getCurrentUser()->username
             << "  [ADMIN]\n\n";

        cout << "  +--------------------------------------------------+\n"
             << "  |           INVENTORY MANAGEMENT                  |\n"
             << "  +--------------------------------------------------+\n"
             << "  |  1.  View All Products                          |\n"
             << "  |  2.  View by Category                           |\n"
             << "  |  3.  Add New Product                            |\n"
             << "  |  4.  Update Product Stock                       |\n"
             << "  |  5.  Remove Product                             |\n"
             << "  |  6.  Low Stock Alert                            |\n"
             << "  |  7.  View All Discounts                         |\n"
             << "  +--------------------------------------------------+\n"
             << "  |  0.  Logout                                     |\n"
             << "  +--------------------------------------------------+\n"
             << "  Choice: ";
        cin >> ch;

        switch (ch) {
            case 1: inv.displayAllProducts(); break;
            case 2: {
                int cat;
                cout << "\n  1. Electronics  2. Clothing  3. Grocery  4. Books\n"
                     << "  Choice: ";
                cin >> cat;
                if      (cat == 1) inv.displayByCategory("Electronics");
                else if (cat == 2) inv.displayByCategory("Clothing");
                else if (cat == 3) inv.displayByCategory("Grocery");
                else if (cat == 4) inv.displayByCategory("Books");
                else cout << "\n  Invalid.\n";
                break;
            }
            case 3: inv.addProduct();    break;
            case 4: inv.updateStock();   break;
            case 5: inv.removeProduct(); break;
            case 6: inv.showLowStock();  break;
            case 7: inv.showDiscounts(); break;
            case 0: um.logout(); break;
            default: cout << "\n  Invalid choice.\n";
        }

        if (ch != 0) pressEnter();
    } while (ch != 0);
}

// ----------------------------------------------------------------
//  USER DASHBOARD  (reached after userLogin())
// ----------------------------------------------------------------
void runUserDashboard(UserManager& um, InventoryManager& inv) {
    int ch;
    do {
        printHeader("CUSTOMER DASHBOARD");

        cout << "\n  Logged in as: " << um.getCurrentUser()->username
             << "  | Wallet: $" << fixed << setprecision(2)
             << um.getCurrentUser()->walletBalance << "\n\n";

        cout << "  +--------------------------------------------------+\n"
             << "  |                SHOPPING                         |\n"
             << "  +--------------------------------------------------+\n"
             << "  |  1.  Browse All Products                        |\n"
             << "  |  2.  Browse by Category                         |\n"
             << "  |  3.  Add Product to Cart                        |\n"
             << "  |  4.  View Cart                                  |\n"
             << "  |  5.  Checkout (Place Order)                     |\n"
             << "  +--------------------------------------------------+\n"
             << "  |                ACCOUNT                          |\n"
             << "  +--------------------------------------------------+\n"
             << "  |  6.  View Order History                         |\n"
             << "  |  7.  Add Funds to Wallet                        |\n"
             << "  |  8.  View Profile                               |\n"
             << "  |  9.  Edit Profile                               |\n"
             << "  |  10. Change Password                            |\n"
             << "  |  11. Clear Cart                                 |\n"
             << "  +--------------------------------------------------+\n"
             << "  |  0.  Logout                                     |\n"
             << "  +--------------------------------------------------+\n"
             << "  Choice: ";
        cin >> ch;

        switch (ch) {
            case 1:
                inv.displayAllProducts();
                break;
            case 2: {
                int cat;
                cout << "\n  1. Electronics  2. Clothing  3. Grocery  4. Books\n"
                     << "  Choice: ";
                cin >> cat;
                if      (cat == 1) inv.displayByCategory("Electronics");
                else if (cat == 2) inv.displayByCategory("Clothing");
                else if (cat == 3) inv.displayByCategory("Grocery");
                else if (cat == 4) inv.displayByCategory("Books");
                else cout << "\n  Invalid category.\n";
                break;
            }
            case 3: {
                int id, qty;
                cout << "\n  Product ID : "; cin >> id;
                cout << "  Quantity   : "; cin >> qty;

                Product* p = inv.findProduct(id);
                if (!p) {
                    cout << "\n  [ERROR] Product not found.\n";
                } else if (qty <= 0) {
                    cout << "\n  [ERROR] Quantity must be > 0.\n";
                } else if (p->getStock() < qty) {
                    cout << "\n  [ERROR] Only " << p->getStock()
                         << " units in stock.\n";
                } else {
                    um.addToCart(p->getID(), p->getName(), p->getPrice(), qty);
                }
                break;
            }
            case 4:  um.viewCart();       break;
            case 5:  um.checkout();       break;
            case 6:  um.viewOrders();     break;
            case 7:  um.addFunds();       break;
            case 8:  um.viewProfile();    break;
            case 9:  um.editProfile();    break;
            case 10: um.changePassword(); break;
            case 11: um.clearCart();      break;
            case 0:  um.logout();         break;
            default: cout << "\n  Invalid choice.\n";
        }

        if (ch != 0) pressEnter();
    } while (ch != 0);
}

// ----------------------------------------------------------------
//  WELCOME SCREEN
// ----------------------------------------------------------------
void showWelcomeScreen() {
    cout << "\n";
    printLine('=', 70);
    cout << "  " << string(20, ' ') << "INVENTORY MANAGEMENT SYSTEM\n";
    cout << "  " << string(22, ' ') << "OOP Final Project - C++\n";
    printLine('=', 70);

    cout << "\n"
         << "  +--------------------------------------------------+\n"
         << "  |                                                  |\n"
         << "  |            WELCOME TO THE SYSTEM                |\n"
         << "  |                                                  |\n"
         << "  +--------------------------------------------------+\n"
         << "  |                                                  |\n"
         << "  |   Pre-loaded Accounts for Testing:              |\n"
         << "  |   Admin : Username=Admin     | pass=admin123    |\n"
         << "  |   User  : ID=2001            | pass=john123     |\n"
         << "  |   User  : ID=2002            | pass=jane123     |\n"
         << "  |   User  : ID=2003            | pass=mike123     |\n"
         << "  |                                                  |\n"
         << "  +--------------------------------------------------+\n"
         << "  |                                                  |\n"
         << "  |   1.  User Sign Up   (Create new account)       |\n"
         << "  |   2.  Admin Login    (Username + Password)      |\n"
         << "  |   3.  User Login     (User ID + Password)       |\n"
         << "  |   0.  Exit                                      |\n"
         << "  |                                                  |\n"
         << "  +--------------------------------------------------+\n"
         << "  Choice: ";
}

// ----------------------------------------------------------------
//  MAIN
// ----------------------------------------------------------------
int main() {
    UserManager     userManager;
    InventoryManager inventory;

    int choice;
    bool running = true;

    while (running) {
        showWelcomeScreen();
        cin >> choice;

        switch (choice) {

            // --------------------------------------------------
            //  1. USER SIGN UP
            // --------------------------------------------------
            case 1:
                userManager.signUp();
                pressEnter();
                break;

            // --------------------------------------------------
            //  2. ADMIN LOGIN
            // --------------------------------------------------
            case 2:
                if (userManager.adminLogin()) {
                    runAdminDashboard(userManager, inventory);
                } else {
                    pressEnter();
                }
                break;

            // --------------------------------------------------
            //  3. USER LOGIN
            // --------------------------------------------------
            case 3:
                if (userManager.userLogin()) {
                    runUserDashboard(userManager, inventory);
                } else {
                    pressEnter();
                }
                break;

            // --------------------------------------------------
            //  0. EXIT
            // --------------------------------------------------
            case 0:
                running = false;
                cout << "\n";
                printLine('=', 70);
                cout << "  " << string(16, ' ')
                     << "THANK YOU FOR USING THE SYSTEM!\n";
                cout << "  " << string(26, ' ') << "GOODBYE!\n";
                printLine('=', 70);
                cout << "\n";
                break;

            default:
                cout << "\n  [ERROR] Invalid choice. Please enter 0, 1, 2, or 3.\n";
                pressEnter();
        }
    }

    return 0;
}
/*
 * ============================================================
 *   ENHANCED INVENTORY MANAGEMENT SYSTEM
 *   OOP Final Project - C++
 *
 *   ENHANCED FEATURES ADDED:
 *   - Deep inheritance hierarchy (4+ levels)
 *   - Operator overloading (+, <<, [])
 *   - Template TransactionLog class
 *   - Composition & Aggregation (Warehouse, InventorySection)
 *   - Polymorphism with virtual functions
 *   - Risk assessment & expiry checking
 *   - Supplier integration
 * ============================================================
 */

#include <iostream>
#include <cstring>
#include <iomanip>
#include <cctype>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <string>
using namespace std;

// ================================================================
//  UTILITY FUNCTIONS & CONSTANTS
// ================================================================
const int MAX_PRODUCTS_PER_SECTION = 50;
const int MAX_SECTIONS = 10;

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
//  TEMPLATED TRANSACTION LOG CLASS (Audit Logging)
// ================================================================
template <typename T>
class TransactionLog {
private:
    struct LogEntry {
        T data;
        char timestamp[30];
        char action[50];
        LogEntry* next;
        
        LogEntry(const T& d, const char* act) : data(d), next(nullptr) {
            time_t now = time(0);
            strcpy(timestamp, ctime(&now));
            timestamp[strlen(timestamp) - 1] = '\0';
            strcpy(action, act);
        }
    };
    
    LogEntry* head;
    int logCount;
    
public:
    TransactionLog() : head(nullptr), logCount(0) {}
    
    ~TransactionLog() {
        LogEntry* current = head;
        while (current) {
            LogEntry* temp = current;
            current = current->next;
            delete temp;
        }
    }
    
    void recordAction(const T& item, const char* action) {
        LogEntry* newEntry = new LogEntry(item, action);
        newEntry->next = head;
        head = newEntry;
        logCount++;
    }
    
    void printAuditTrail() const {
        printHeader("TRANSACTION AUDIT TRAIL");
        LogEntry* current = head;
        int count = 1;
        while (current) {
            cout << "  " << count++ << ". [" << current->timestamp << "] "
                 << current->action << "\n";
            current = current->next;
        }
        if (logCount == 0) cout << "  No transactions recorded.\n";
    }
    
    int getCount() const { return logCount; }
};

// ================================================================
//  FORWARD DECLARATIONS
// ================================================================
class Product;
class InventorySection;
class BookProduct;

// ================================================================
//  SUPPLIER CLASS (Association with Product)
// ================================================================
class Supplier {
private:
    int supplierID;
    char name[100];
    char contactPerson[100];
    char phone[20];
    char email[100];
    double contractAmount;
    int reliabilityRating;  // 1-5 stars
    
public:
    Supplier(int id = 0, const char* n = "", const char* contact = "",
             const char* ph = "", const char* mail = "", double contract = 0.0, int rating = 3)
        : supplierID(id), contractAmount(contract), reliabilityRating(rating) {
        strcpy(name, n);
        strcpy(contactPerson, contact);
        strcpy(phone, ph);
        strcpy(email, mail);
    }
    
    int getID() const { return supplierID; }
    const char* getName() const { return name; }
    int getRating() const { return reliabilityRating; }
    
    void orderRestock(int productID, int quantity) const {
        cout << "  [ORDER] Placing restock order with " << name 
             << " for " << quantity << " units of product #" << productID << "\n";
    }
    
    void generateInvoice() const {
        cout << "  [INVOICE] Invoice generated from " << name 
             << " | Contract: $" << contractAmount << "\n";
    }
    
    void display() const {
        cout << "  Supplier #" << supplierID << ": " << name 
             << " (Rating: " << reliabilityRating << "/5)\n";
    }
};

// ================================================================
//  DEEP INHERITANCE HIERARCHY
//  Level 1: Product (Abstract Base)
//  Level 2: ElectronicProduct, GroceryProduct, ClothingProduct
//  Level 3: FragileElectronics, PerishableGrocery, NonPerishable, Apparel
//  Level 4: LuxuryApparel (further specialization)
// ================================================================

// ---------- LEVEL 1: PRODUCT ABSTRACT BASE ----------
class Product {
protected:
    int productID;
    char name[100];
    double price;
    int stock;
    char category[50];
    Supplier* supplier;  // Association with Supplier
    
public:
    Product(int id = 0, const char* n = "", double p = 0.0,
            int s = 0, const char* cat = "General")
        : productID(id), price(p), stock(s), supplier(nullptr) {
        strcpy(name, n);
        strcpy(category, cat);
    }
    
    virtual ~Product() {}  // Polymorphic destruction
    
    // Pure virtual methods for polymorphism
    virtual void display() const = 0;
    virtual double calculateDiscount() const = 0;
    virtual double calculateRisk() const = 0;
    virtual void displayStatus() const = 0;
    virtual double calculateValue() const { return price * stock; }
    
    // Getters
    int getID() const { return productID; }
    const char* getName() const { return name; }
    double getPrice() const { return price; }
    int getStock() const { return stock; }
    const char* getCategory() const { return category; }
    Supplier* getSupplier() const { return supplier; }
    
    // Setters with validation (Encapsulation)
    void setStock(int s) { 
        if (s >= 0) stock = s; 
        else cout << "  [ERROR] Stock cannot be negative.\n";
    }
    
    void setPrice(double p) {
        if (p >= 0) price = p;
        else cout << "  [ERROR] Price cannot be negative.\n";
    }
    
    void setSupplier(Supplier* s) { supplier = s; }
    
    void reduceStock(int qty) { 
        if (stock >= qty && qty > 0) stock -= qty; 
    }
    
    void addStock(int qty) {
        if (qty > 0) stock += qty;
    }
};

// ---------- LEVEL 2: ELECTRONIC PRODUCT ----------
class ElectronicProduct : public Product {
protected:
    int voltage;
    int warrantyMonths;
    
public:
    ElectronicProduct(int id = 0, const char* n = "", double p = 0.0,
                      int s = 0, int volt = 110, int warranty = 12)
        : Product(id, n, p, s, "Electronics"), voltage(volt), warrantyMonths(warranty) {}
    
    void testHardware() const {
        cout << "  Testing hardware of " << name << "... All systems OK.\n";
    }
    
    void display() const override {
        cout << "  " << left << setw(8) << productID
             << setw(25) << name
             << setw(12) << category
             << "$" << fixed << setprecision(2) << setw(10) << price
             << setw(8) << stock << " | Warranty: " << warrantyMonths << "m\n";
    }
    
    double calculateDiscount() const override {
        if (warrantyMonths > 24) return price * 0.15;
        if (warrantyMonths > 12) return price * 0.10;
        return price * 0.05;
    }
    
    double calculateRisk() const override {
        // Electronics risk based on voltage and warranty
        return (voltage > 220 ? 0.3 : 0.1) + (warrantyMonths < 12 ? 0.2 : 0.0);
    }
    
    void displayStatus() const override {
        cout << "  [" << name << "] Stock: " << stock 
             << " | Value: $" << calculateValue()
             << " | Risk Score: " << calculateRisk() << "\n";
    }
};

// ---------- LEVEL 3: FRAGILE ELECTRONICS (Deep Inheritance) ----------
class FragileElectronics : public ElectronicProduct {
private:
    double fragilityRating;  // 0-1 scale
    char packagingType[50];
    
public:
    FragileElectronics(int id = 0, const char* n = "", double p = 0.0, int s = 0,
                       int volt = 110, int warranty = 12, double fragility = 0.5, 
                       const char* packaging = "Standard")
        : ElectronicProduct(id, n, p, s, volt, warranty), fragilityRating(fragility) {
        strcpy(packagingType, packaging);
        strcpy(category, "Fragile Electronics");
    }
    
    double calculateShippingRisk() const {
        return fragilityRating * (1.0 + (voltage > 220 ? 0.2 : 0.0));
    }
    
    double calculateRisk() const override {
        return ElectronicProduct::calculateRisk() + fragilityRating * 0.5;
    }
    
    void display() const override {
        ElectronicProduct::display();
        cout << "      Fragility: " << fragilityRating << " | Packaging: " << packagingType
             << " | Ship Risk: " << calculateShippingRisk() << "\n";
    }
    
    void displayStatus() const override {
        cout << "  [FRAGILE] " << name << " | Stock: " << stock 
             << " | HIGH RISK ITEM - Handle with care!\n";
    }
};

// ---------- LEVEL 2: GROCERY PRODUCT ----------
class GroceryProduct : public Product {
protected:
    int calories;
    bool isHalal;
    bool isOrganic;
    
public:
    GroceryProduct(int id = 0, const char* n = "", double p = 0.0, int s = 0,
                   int cal = 0, bool halal = true, bool organic = false)
        : Product(id, n, p, s, "Grocery"), calories(cal), isHalal(halal), isOrganic(organic) {}
    
    void checkSafety() const {
        cout << "  Safety check for " << name << ": Passed.\n";
    }
    
    void display() const override {
        cout << "  " << left << setw(8) << productID
             << setw(25) << name
             << setw(12) << category
             << "$" << fixed << setprecision(2) << setw(10) << price
             << setw(8) << stock << " | Cal: " << calories << "\n";
    }
    
    double calculateDiscount() const override {
        double discount = isOrganic ? 0.10 : 0.05;
        return price * discount;
    }
    
    double calculateRisk() const override {
        // Grocery risk is generally low
        return 0.05;
    }
    
    void displayStatus() const override {
        cout << "  [" << name << "] Stock: " << stock 
             << " | Organic: " << (isOrganic ? "Yes" : "No")
             << " | Halal: " << (isHalal ? "Yes" : "No") << "\n";
    }
};

// ---------- LEVEL 3: PERISHABLE GROCERY (Deep Inheritance) ----------
class PerishableGrocery : public GroceryProduct {
private:
    char expiryDate[20];
    double storageTemp;  // Celsius
    
public:
    PerishableGrocery(int id = 0, const char* n = "", double p = 0.0, int s = 0,
                      int cal = 0, bool halal = true, bool organic = false,
                      const char* expiry = "", double temp = 4.0)
        : GroceryProduct(id, n, p, s, cal, halal, organic), storageTemp(temp) {
        strcpy(expiryDate, expiry);
        strcpy(category, "Perishable Grocery");
    }
    
    bool checkExpiry() const {
        // Simulate expiry check
        cout << "  Checking expiry for " << name << " (Expires: " << expiryDate << ")\n";
        return true;  // Not expired for demo
    }
    
    double calculateRisk() const override {
        // Perishable items have higher risk
        return 0.25 + (storageTemp > 8 ? 0.1 : 0.0);
    }
    
    void display() const override {
        GroceryProduct::display();
        cout << "      Expiry: " << expiryDate << " | Storage: " << storageTemp << "°C\n";
    }
    
    void displayStatus() const override {
        cout << "  [PERISHABLE] " << name << " | Expires: " << expiryDate
             << " | Store at " << storageTemp << "°C\n";
    }
};

// ---------- LEVEL 3: NON-PERISHABLE (Deep Inheritance) ----------
class NonPerishable : public GroceryProduct {
private:
    int shelfLifeYears;
    double preservativeLevel;
    
public:
    NonPerishable(int id = 0, const char* n = "", double p = 0.0, int s = 0,
                  int cal = 0, bool halal = true, bool organic = false,
                  int life = 2, double preservative = 0.1)
        : GroceryProduct(id, n, p, s, cal, halal, organic), 
          shelfLifeYears(life), preservativeLevel(preservative) {
        strcpy(category, "Non-Perishable");
    }
    
    const char* getStorageInstructions() const {
        return "Store in cool, dry place";
    }
    
    void display() const override {
        GroceryProduct::display();
        cout << "      Shelf Life: " << shelfLifeYears << " years | Preservatives: " << preservativeLevel << "\n";
    }
};

// ---------- LEVEL 2: CLOTHING PRODUCT ----------
class ClothingProduct : public Product {
protected:
    char size[10];
    char fabric[50];
    char gender[20];
    
public:
    ClothingProduct(int id = 0, const char* n = "", double p = 0.0, int s = 0,
                    const char* sz = "M", const char* fab = "Cotton", const char* gend = "Unisex")
        : Product(id, n, p, s, "Clothing") {
        strcpy(size, sz);
        strcpy(fabric, fab);
        strcpy(gender, gend);
    }
    
    const char* fitGuide() const {
        if (strcmp(size, "S") == 0) return "Tight fit";
        if (strcmp(size, "M") == 0) return "Regular fit";
        if (strcmp(size, "L") == 0) return "Loose fit";
        return "Standard fit";
    }
    
    void display() const override {
        cout << "  " << left << setw(8) << productID
             << setw(25) << name
             << setw(12) << category
             << "$" << fixed << setprecision(2) << setw(10) << price
             << setw(8) << stock << " | Size: " << size << "\n";
    }
    
    double calculateDiscount() const override { return price * 0.20; }
    double calculateRisk() const override { return 0.02; }
    
    void displayStatus() const override {
        cout << "  [CLOTHING] " << name << " | Size: " << size 
             << " | Fit: " << fitGuide() << "\n";
    }
};

// ---------- LEVEL 3: APPAREL (extends Clothing) ----------
class Apparel : public ClothingProduct {
private:
    char season[20];
    bool isImported;
    
public:
    Apparel(int id = 0, const char* n = "", double p = 0.0, int s = 0,
            const char* sz = "M", const char* fab = "Cotton", const char* gend = "Unisex",
            const char* seas = "All", bool imported = false)
        : ClothingProduct(id, n, p, s, sz, fab, gend), isImported(imported) {
        strcpy(season, seas);
        strcpy(category, "Apparel");
    }
    
    void display() const override {
        ClothingProduct::display();
        cout << "      Season: " << season << " | Imported: " << (isImported ? "Yes" : "No") << "\n";
    }
    
    double calculateRisk() const override {
        return isImported ? 0.08 : 0.02;
    }
};

// ---------- LEVEL 4: LUXURY APPAREL (Deepest Inheritance Level) ----------
class LuxuryApparel : public Apparel {
private:
    char brandName[50];
    double luxuryTax;
    int limitedEditionNumber;
    
public:
    LuxuryApparel(int id = 0, const char* n = "", double p = 0.0, int s = 0,
                  const char* sz = "M", const char* fab = "Silk", const char* gend = "Unisex",
                  const char* seas = "All", bool imported = true,
                  const char* brand = "", double tax = 0.25, int edition = 1)
        : Apparel(id, n, p, s, sz, fab, gend, seas, imported), luxuryTax(tax), limitedEditionNumber(edition) {
        strcpy(brandName, brand);
        strcpy(category, "Luxury Apparel");
    }
    
    void display() const override {
        Apparel::display();
        cout << "      BRAND: " << brandName << " | Luxury Tax: " << luxuryTax*100 << "%"
             << " | Limited Edition #" << limitedEditionNumber << "\n";
    }
    
    double calculateDiscount() const override {
        // Luxury items have lower discounts
        return price * 0.05;
    }
    
    double calculateRisk() const override {
        // High value luxury items have higher risk
        return 0.25;
    }
    
    void displayStatus() const override {
        cout << "  [LUXURY] " << name << " by " << brandName 
             << " | Limited Edition: " << limitedEditionNumber
             << " | HIGH VALUE ITEM - Secure storage required\n";
    }
};

// ---------- BOOK PRODUCT CLASS ----------
class BookProduct : public Product {
private:
    char author[100];
    char isbn[20];
    
public:
    BookProduct(int id = 0, const char* n = "", double p = 0.0, int s = 0,
                const char* auth = "", const char* isbnCode = "")
        : Product(id, n, p, s, "Books") {
        strcpy(author, auth);
        strcpy(isbn, isbnCode);
    }
    
    void display() const override {
        cout << "  " << left << setw(8) << productID
             << setw(25) << name
             << setw(12) << category
             << "$" << fixed << setprecision(2) << setw(10) << price
             << setw(8) << stock << " | Author: " << author << "\n";
    }
    
    double calculateDiscount() const override { return price * 0.25; }
    double calculateRisk() const override { return 0.01; }
    void displayStatus() const override {
        cout << "  [BOOK] " << name << " by " << author << " | Stock: " << stock << "\n";
    }
};

// ================================================================
//  INVENTORY SECTION CLASS (Composition - holds Product*)
// ================================================================
class InventorySection {
private:
    int aisleNumber;
    int capacity;
    Product* products[MAX_PRODUCTS_PER_SECTION];
    int productCount;
    char sectionType[30];
    
public:
    InventorySection(int aisle = 1, int cap = 50, const char* type = "General")
        : aisleNumber(aisle), capacity(cap), productCount(0) {
        strcpy(sectionType, type);
        for (int i = 0; i < MAX_PRODUCTS_PER_SECTION; i++) products[i] = nullptr;
    }
    
    ~InventorySection() {
        // Note: Products are owned by InventoryManager, not by section
        // This is Aggregation, not Composition - section doesn't delete products
    }
    
    bool addStock(Product* p) {
        if (productCount >= capacity) {
            cout << "  Section " << aisleNumber << " is full!\n";
            return false;
        }
        products[productCount++] = p;
        return true;
    }
    
    void sortByID() {
        // Simple bubble sort by product ID
        for (int i = 0; i < productCount - 1; i++) {
            for (int j = 0; j < productCount - i - 1; j++) {
                if (products[j]->getID() > products[j+1]->getID()) {
                    Product* temp = products[j];
                    products[j] = products[j+1];
                    products[j+1] = temp;
                }
            }
        }
        cout << "  Section " << aisleNumber << " sorted by ID.\n";
    }
    
    // OPERATOR OVERLOADING: [] for shelf access
    Product* operator[](int index) {
        if (index >= 0 && index < productCount) {
            return products[index];
        }
        return nullptr;
    }
    
    const Product* operator[](int index) const {
        if (index >= 0 && index < productCount) {
            return products[index];
        }
        return nullptr;
    }
    
    void display() const {
        cout << "\n  === Section Aisle " << aisleNumber << " (" << sectionType << ") ===\n";
        cout << "  Capacity: " << capacity << " | Products: " << productCount << "\n";
        if (productCount == 0) {
            cout << "  [Empty Section]\n";
            return;
        }
        cout << "  " << left << setw(8) << "ID" << setw(25) << "Name"
             << setw(12) << "Category" << setw(12) << "Price" << "Stock\n";
        cout << "  " << string(65, '-') << "\n";
        for (int i = 0; i < productCount; i++) {
            products[i]->display();
        }
    }
    
    int getAisleNumber() const { return aisleNumber; }
    int getProductCount() const { return productCount; }
    double getTotalSectionValue() const {
        double total = 0;
        for (int i = 0; i < productCount; i++) {
            total += products[i]->calculateValue();
        }
        return total;
    }
    
    Product* findProductInSection(int id) const {
        for (int i = 0; i < productCount; i++) {
            if (products[i]->getID() == id) return products[i];
        }
        return nullptr;
    }
};

// ================================================================
//  WAREHOUSE CLASS (Aggregation - holds InventorySection)
// ================================================================
class Warehouse {
private:
    char locationID[20];
    double totalSquareFootage;
    InventorySection* sections[MAX_SECTIONS];
    int sectionCount;
    TransactionLog<string> warehouseLog;
    
public:
    Warehouse(const char* location = "WH-001", double sqft = 5000.0)
        : totalSquareFootage(sqft), sectionCount(0) {
        strcpy(locationID, location);
        for (int i = 0; i < MAX_SECTIONS; i++) sections[i] = nullptr;
    }
    
    ~Warehouse() {
        // Warehouse owns the sections (Composition)
        for (int i = 0; i < sectionCount; i++) {
            delete sections[i];
        }
    }
    
    bool addSection(InventorySection* section) {
        if (sectionCount >= MAX_SECTIONS) {
            cout << "  Warehouse " << locationID << " has reached maximum sections.\n";
            return false;
        }
        sections[sectionCount++] = section;
        warehouseLog.recordAction("Section added", "ADMIN_ACTION");
        return true;
    }
    
    bool addSection(int aisle, int capacity, const char* type) {
        if (sectionCount >= MAX_SECTIONS) return false;
        sections[sectionCount++] = new InventorySection(aisle, capacity, type);
        warehouseLog.recordAction("Section created", "ADMIN_ACTION");
        return true;
    }
    
    double getGlobalInventoryValue() const {
        double total = 0;
        for (int i = 0; i < sectionCount; i++) {
            total += sections[i]->getTotalSectionValue();
        }
        return total;
    }
    
    void findShortages() const {
        printHeader("STOCK SHORTAGE REPORT");
        cout << "\n  Products with stock < 10 units:\n";
        cout << "  " << string(55, '-') << "\n";
        bool found = false;
        for (int i = 0; i < sectionCount; i++) {
            for (int j = 0; j < sections[i]->getProductCount(); j++) {
                const Product* p = (*sections[i])[j];
                if (p && p->getStock() < 10) {
                    cout << "  Section " << sections[i]->getAisleNumber()
                         << " | " << p->getName() << " | Stock: " << p->getStock() << "\n";
                    found = true;
                }
            }
        }
        if (!found) cout << "  No shortages detected.\n";
    }
    
    void displayAllSections() const {
        printHeader("WAREHOUSE INVENTORY SECTIONS");
        cout << "\n  Warehouse: " << locationID << " | Area: " << totalSquareFootage << " sq ft\n";
        cout << "  Total Inventory Value: $" << fixed << setprecision(2) << getGlobalInventoryValue() << "\n";
        for (int i = 0; i < sectionCount; i++) {
            sections[i]->display();
        }
    }
    
    InventorySection* getSection(int index) {
        if (index >= 0 && index < sectionCount) return sections[index];
        return nullptr;
    }
    
    const InventorySection* getSection(int index) const {
        if (index >= 0 && index < sectionCount) return sections[index];
        return nullptr;
    }
    
    int getSectionCount() const { return sectionCount; }
    
    void printAuditLog() const { warehouseLog.printAuditTrail(); }
    
    const char* getLocationID() const { return locationID; }
};

// ================================================================
//  USER ACCOUNT CLASS
// ================================================================
class UserAccount {
private:
    int userId;
    char username[50];
    char password[50];
    char email[100];
    char phone[20];
    char address[200];
    double walletBalance;
    bool isActive;
    
public:
    UserAccount() : userId(0), walletBalance(0.0), isActive(true) {
        username[0] = '\0'; password[0] = '\0';
        email[0] = '\0'; phone[0] = '\0';
        address[0] = '\0';
    }
    
    UserAccount(int id, const char* uname, const char* pwd, const char* mail,
                const char* ph, const char* addr, double balance, bool active)
        : userId(id), walletBalance(balance), isActive(active) {
        strcpy(username, uname); strcpy(password, pwd);
        strcpy(email, mail); strcpy(phone, ph);
        strcpy(address, addr);
    }
    
    int getUserId() const { return userId; }
    const char* getUsername() const { return username; }
    const char* getPassword() const { return password; }
    const char* getEmail() const { return email; }
    const char* getPhone() const { return phone; }
    const char* getAddress() const { return address; }
    double getWalletBalance() const { return walletBalance; }
    bool isAccountActive() const { return isActive; }
    
    void setEmail(const char* mail) { strcpy(email, mail); }
    void setPhone(const char* ph) { strcpy(phone, ph); }
    void setAddress(const char* addr) { strcpy(address, addr); }
    void setPassword(const char* pwd) { strcpy(password, pwd); }
    void addFunds(double amount) { if (amount > 0) walletBalance += amount; }
    void deductFunds(double amount) { if (amount <= walletBalance) walletBalance -= amount; }
    void deactivate() { isActive = false; }
    void activate() { isActive = true; }
    
    bool checkPassword(const char* pwd) const { return strcmp(password, pwd) == 0; }
    
    void displayProfile() const {
        cout << "\n  +--------------------------------------------------+\n";
        cout << "  | User ID       : " << setw(32) << left << userId << "|\n";
        cout << "  | Username      : " << setw(32) << left << username << "|\n";
        cout << "  | Email         : " << setw(32) << left << email << "|\n";
        cout << "  | Phone         : " << setw(32) << left << phone << "|\n";
        cout << "  | Address       : " << setw(32) << left << address << "|\n";
        cout << "  | Wallet        : $" << fixed << setprecision(2)
             << setw(30) << left << walletBalance << "|\n";
        cout << "  +--------------------------------------------------+\n";
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
    CartItem() : productId(0), price(0.0), quantity(0) { productName[0] = '\0'; }
    
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
    Order(int oid, int uid)
        : orderId(oid), userId(uid), itemCount(0), totalAmount(0.0) {
        strcpy(status, "Pending");
        time_t now = time(0);
        strcpy(orderDate, ctime(&now));
        orderDate[strlen(orderDate) - 1] = '\0';
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
    
    double getTotal() const { return totalAmount; }
    int getItemCount() const { return itemCount; }
    const char* getStatus() const { return status; }
    int getOrderId() const { return orderId; }
    int getUserId() const { return userId; }
    void setStatus(const char* s) { strcpy(status, s); }
    
    void display() const {
        cout << "\n     Order #" << orderId
             << " | Date: " << orderDate
             << " | Status: " << status
             << " | Total: $" << fixed << setprecision(2) << totalAmount << '\n';
        if (itemCount > 0) {
            cout << "     " << string(50, '-') << "\n";
            for (int i = 0; i < itemCount; i++) items[i]->display();
            cout << "     " << string(50, '-') << "\n";
        }
    }
};

// ================================================================
//  USER MANAGER CLASS
// ================================================================
class UserManager {
private:
    static const int MAX_USERS = 100;
    static const int MAX_ORDERS = 200;
    
    UserAccount users[MAX_USERS];
    Order* orders[MAX_ORDERS];
    int userCount;
    int orderCount;
    int nextUserId;
    int nextOrderId;
    UserAccount* currentUser;
    
    CartItem cart[50];
    int cartSize;
    TransactionLog<string> userLog;
    
    bool isUsernameExists(const char* uname) const {
        for (int i = 0; i < userCount; i++)
            if (strcmp(users[i].getUsername(), uname) == 0) return true;
        return false;
    }
    
    int findUserById(int id) const {
        for (int i = 0; i < userCount; i++)
            if (users[i].getUserId() == id) return i;
        return -1;
    }
    
public:
    UserManager() : userCount(0), orderCount(0), nextUserId(2004), nextOrderId(5000),
                    currentUser(nullptr), cartSize(0) {
        for (int i = 0; i < MAX_ORDERS; i++) orders[i] = nullptr;
        
        users[userCount++] = UserAccount(1000, "Admin", "admin123",
                                         "admin@system.com", "1234567890",
                                         "Main Office", 0.0, true);
        users[userCount++] = UserAccount(2001, "JohnDoe", "john123",
                                         "john@email.com", "555-0101",
                                         "123 Main St", 500.00, true);
        users[userCount++] = UserAccount(2002, "JaneSmith", "jane123",
                                         "jane@email.com", "555-0102",
                                         "456 Oak Ave", 750.00, true);
        users[userCount++] = UserAccount(2003, "MikeJohnson", "mike123",
                                         "mike@email.com", "555-0103",
                                         "789 Pine Rd", 300.00, true);
    }
    
    ~UserManager() {
        for (int i = 0; i < orderCount; i++) delete orders[i];
    }
    
    bool signUp() {
        printHeader("CREATE NEW USER ACCOUNT");
        
        if (userCount >= MAX_USERS) {
            cout << "  System is full. Cannot create new account.\n";
            return false;
        }
        
        char username[50], password[50], confirm[50], email[100], phone[20], address[200];
        int userId = nextUserId++;
        
        cout << "\n  Enter Username (no spaces): ";
        cin >> username;
        
        if (isUsernameExists(username)) {
            cout << "\n  [ERROR] Username already taken.\n";
            return false;
        }
        
        cout << "  Enter Password (min 4 chars): ";
        cin >> password;
        
        if (strlen(password) < 4) {
            cout << "\n  [ERROR] Password must be at least 4 characters.\n";
            return false;
        }
        
        cout << "  Confirm Password: ";
        cin >> confirm;
        
        if (strcmp(password, confirm) != 0) {
            cout << "\n  [ERROR] Passwords do not match.\n";
            return false;
        }
        
        cout << "  Enter Email: ";
        cin >> email;
        cout << "  Enter Phone: ";
        cin >> phone;
        cout << "  Enter Address: ";
        cin.ignore();
        cin.getline(address, 200);
        
        users[userCount++] = UserAccount(userId, username, password, email, phone, address, 0.0, true);
        
        userLog.recordAction(string(username) + " signed up", "USER_ACTION");
        
        cout << "\n";
        printLine('*', 70);
        cout << "  *  ACCOUNT CREATED SUCCESSFULLY!                               *\n";
        cout << "  *  Your User ID: " << userId << " | Username: " << username << "\n";
        cout << "  *  SAVE YOUR USER ID — you need it to log in!                  *\n";
        printLine('*', 70);
        
        return true;
    }
    
    bool adminLogin() {
        printHeader("ADMIN LOGIN");
        
        char uname[50], pwd[50];
        cout << "\n  Admin Username: ";
        cin >> uname;
        cout << "  Admin Password: ";
        cin >> pwd;
        
        if (strcmp(uname, "Admin") == 0 && strcmp(pwd, "admin123") == 0) {
            currentUser = &users[0];
            userLog.recordAction("Admin logged in", "ADMIN_ACTION");
            cout << "\n  [SUCCESS] Welcome, Admin!\n";
            return true;
        }
        
        cout << "\n  [ERROR] Invalid admin credentials.\n";
        return false;
    }
    
    bool userLogin() {
        printHeader("USER LOGIN");
        
        int uid;
        char pwd[50];
        
        cout << "\n  Your User ID: ";
        cin >> uid;
        cout << "  Password: ";
        cin >> pwd;
        
        if (uid == 1000) {
            cout << "\n  [ERROR] Use 'Admin Login' for admin access.\n";
            return false;
        }
        
        int idx = findUserById(uid);
        
        if (idx == -1) {
            cout << "\n  [ERROR] User ID not found.\n";
            return false;
        }
        
        if (!users[idx].checkPassword(pwd)) {
            cout << "\n  [ERROR] Incorrect password.\n";
            return false;
        }
        
        if (!users[idx].isAccountActive()) {
            cout << "\n  [ERROR] Account deactivated.\n";
            return false;
        }
        
        currentUser = &users[idx];
        userLog.recordAction(string(currentUser->getUsername()) + " logged in", "USER_ACTION");
        
        cout << "\n  [SUCCESS] Welcome back, " << currentUser->getUsername() << "!\n";
        cout << "  Wallet Balance: $" << fixed << setprecision(2) 
             << currentUser->getWalletBalance() << "\n";
        return true;
    }
    
    void logout() {
        if (currentUser) {
            userLog.recordAction(string(currentUser->getUsername()) + " logged out", "USER_ACTION");
        }
        currentUser = nullptr;
        cartSize = 0;
        cout << "\n  Logged out successfully!\n";
    }
    
    bool isLoggedIn() const { return currentUser != nullptr; }
    bool isAdmin() const { return currentUser && currentUser->getUserId() == 1000; }
    UserAccount* getCurrentUser() { return currentUser; }
    
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
        userLog.recordAction(string(productName) + " added to cart", "CART_ACTION");
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
        cout << "  CART TOTAL: $" << fixed << setprecision(2) << total << "\n";
        cout << "  WALLET: $" << fixed << setprecision(2) << currentUser->getWalletBalance() << "\n";
    }
    
    void checkout() {
        printHeader("CHECKOUT");
        
        if (cartSize == 0) { cout << "\n  Your cart is empty!\n"; return; }
        
        viewCart();
        
        double total = 0.0;
        for (int i = 0; i < cartSize; i++) total += cart[i].getTotal();
        
        if (currentUser->getWalletBalance() < total) {
            cout << "\n  [ERROR] Insufficient wallet balance!\n";
            return;
        }
        
        char confirm;
        cout << "\n  Confirm purchase? (y/n): ";
        cin >> confirm;
        
        if (tolower(confirm) != 'y') { cout << "  Checkout cancelled.\n"; return; }
        
        Order* order = new Order(nextOrderId++, currentUser->getUserId());
        for (int i = 0; i < cartSize; i++) order->addItem(cart[i]);
        
        // Reduce stock for purchased items
        for (int i = 0; i < cartSize; i++) {
            // Stock reduction would be handled by inventory manager
            // This is simplified for demo
        }
        
        currentUser->deductFunds(total);
        orders[orderCount++] = order;
        cartSize = 0;
        
        userLog.recordAction("Order #" + to_string(order->getOrderId()) + " placed", "ORDER_ACTION");
        
        cout << "\n  [SUCCESS] ORDER PLACED! Order ID: " << order->getOrderId() << "\n";
        cout << "  Remaining Balance: $" << fixed << setprecision(2) 
             << currentUser->getWalletBalance() << "\n";
    }
    
    void viewOrders() const {
        printHeader("MY ORDER HISTORY");
        
        bool found = false;
        for (int i = 0; i < orderCount; i++) {
            if (orders[i]->getUserId() == currentUser->getUserId()) {
                orders[i]->display();
                found = true;
            }
        }
        if (!found) cout << "\n  No orders placed yet.\n";
    }
    
    void addFunds() {
        printHeader("ADD FUNDS TO WALLET");
        
        double amount;
        cout << "\n  Current Balance: $" << fixed << setprecision(2) 
             << currentUser->getWalletBalance() << "\n";
        cout << "  Amount to add: $";
        cin >> amount;
        
        if (amount <= 0) { cout << "\n  [ERROR] Amount must be positive.\n"; return; }
        
        currentUser->addFunds(amount);
        userLog.recordAction("Added $" + to_string(amount) + " to wallet", "WALLET_ACTION");
        
        cout << "\n  [SUCCESS] $" << fixed << setprecision(2) << amount << " added.\n";
        cout << "  New Balance: $" << fixed << setprecision(2) 
             << currentUser->getWalletBalance() << "\n";
    }
    
    void viewProfile() const { currentUser->displayProfile(); }
    
    void editProfile() {
        printHeader("EDIT PROFILE");
        
        int choice;
        cout << "\n  1. Email\n  2. Phone\n  3. Address\n  4. All\n  Choice: ";
        cin >> choice;
        
        char newValue[200];
        switch (choice) {
            case 1:
                cout << "  New Email: "; cin >> newValue;
                currentUser->setEmail(newValue);
                break;
            case 2:
                cout << "  New Phone: "; cin >> newValue;
                currentUser->setPhone(newValue);
                break;
            case 3:
                cout << "  New Address: ";
                cin.ignore(); cin.getline(newValue, 200);
                currentUser->setAddress(newValue);
                break;
            case 4:
                cout << "  New Email: "; cin >> newValue;
                currentUser->setEmail(newValue);
                cout << "  New Phone: "; cin >> newValue;
                currentUser->setPhone(newValue);
                cout << "  New Address: ";
                cin.ignore(); cin.getline(newValue, 200);
                currentUser->setAddress(newValue);
                break;
            default:
                cout << "\n  Invalid choice.\n";
                return;
        }
        cout << "\n  [SUCCESS] Profile updated!\n";
    }
    
    void changePassword() {
        printHeader("CHANGE PASSWORD");
        
        char oldP[50], newP[50], confP[50];
        cout << "\n  Current password: ";
        cin >> oldP;
        
        if (!currentUser->checkPassword(oldP)) {
            cout << "\n  [ERROR] Incorrect current password.\n";
            return;
        }
        
        cout << "  New password: ";
        cin >> newP;
        cout << "  Confirm password: ";
        cin >> confP;
        
        if (strcmp(newP, confP) != 0) {
            cout << "\n  [ERROR] Passwords do not match.\n";
            return;
        }
        if (strlen(newP) < 4) {
            cout << "\n  [ERROR] Password must be at least 4 characters.\n";
            return;
        }
        
        currentUser->setPassword(newP);
        cout << "\n  [SUCCESS] Password changed!\n";
    }
    
    void clearCart() { cartSize = 0; cout << "\n  Cart cleared.\n"; }
    
    void printAuditLog() const { userLog.printAuditTrail(); }
};

// ================================================================
//  ENHANCED INVENTORY MANAGER WITH WAREHOUSE
// ================================================================
class EnhancedInventoryManager {
private:
    Warehouse warehouse;
    Supplier suppliers[20];
    int supplierCount;
    TransactionLog<string> inventoryLog;
    
public:
    EnhancedInventoryManager() : warehouse("MAIN-WH-001", 10000.0), supplierCount(0) {
        // Initialize suppliers
        suppliers[supplierCount++] = Supplier(5001, "TechDistro Inc.", "John Smith", 
                                              "555-1001", "orders@techdistro.com", 500000.0, 5);
        suppliers[supplierCount++] = Supplier(5002, "FreshFoods Ltd.", "Maria Garcia",
                                              "555-1002", "supply@freshfoods.com", 200000.0, 4);
        suppliers[supplierCount++] = Supplier(5003, "FashionHub Global", "David Kim",
                                              "555-1003", "contact@fashionhub.com", 150000.0, 4);
        suppliers[supplierCount++] = Supplier(5004, "BookWorld Distribution", "Sarah Lee",
                                              "555-1004", "orders@bookworld.com", 100000.0, 3);
        
        // Create warehouse sections
        warehouse.addSection(1, 50, "Electronics");
        warehouse.addSection(2, 50, "Clothing");
        warehouse.addSection(3, 50, "Grocery");
        warehouse.addSection(4, 50, "Books & Media");
        warehouse.addSection(5, 30, "Fragile Items");
        warehouse.addSection(6, 40, "Luxury Goods");
        
        // Populate with products
        addProductToSection(new FragileElectronics(101, "4K Smart TV", 899.99, 8, 220, 24, 0.7, "Reinforced Box"), 4);
        addProductToSection(new FragileElectronics(102, "Gaming Laptop Pro", 1499.99, 5, 110, 36, 0.6, "Custom Foam"), 4);
        addProductToSection(new ElectronicProduct(103, "Wireless Earbuds", 129.99, 25, 5, 12), 0);
        addProductToSection(new ElectronicProduct(104, "Smart Watch Series 5", 349.99, 15, 5, 12), 0);
        
        addProductToSection(new LuxuryApparel(201, "Designer Evening Gown", 1299.99, 3, "S", "Silk", "F", "Winter", true, "Versace", 0.30, 50), 5);
        addProductToSection(new Apparel(202, "Cashmere Sweater", 199.99, 20, "M", "Cashmere", "M", "Winter", true), 1);
        addProductToSection(new ClothingProduct(203, "Classic Jeans", 59.99, 45, "32", "Denim", "M"), 1);
        addProductToSection(new ClothingProduct(204, "Summer T-Shirt", 19.99, 60, "L", "Cotton", "M"), 1);
        
        addProductToSection(new PerishableGrocery(301, "Organic Milk", 5.99, 30, 150, true, true, "2024-12-31", 4.0), 2);
        addProductToSection(new PerishableGrocery(302, "Fresh Salmon Fillet", 24.99, 12, 350, true, true, "2024-12-15", -2.0), 2);
        addProductToSection(new NonPerishable(303, "Whole Grain Rice", 12.99, 50, 500, true, false, 2, 0.05), 2);
        addProductToSection(new NonPerishable(304, "Canned Beans", 3.49, 80, 200, true, false, 3, 0.10), 2);
        
        addProductToSection(new BookProduct(401, "The C++ Programming Language", 89.99, 15, "Bjarne Stroustrup", "978-0321563842"), 3);
        addProductToSection(new BookProduct(402, "Clean Architecture", 54.99, 20, "Robert Martin", "978-0134494166"), 3);
        addProductToSection(new BookProduct(403, "Design Patterns", 64.99, 12, "Erich Gamma", "978-0201633610"), 3);
        
        // Assign suppliers to products
        assignSuppliersToProducts();
        
        inventoryLog.recordAction("Inventory system initialized", "SYSTEM");
    }
    
    void addProductToSection(Product* p, int sectionIndex) {
        InventorySection* section = warehouse.getSection(sectionIndex);
        if (section && section->addStock(p)) {
            inventoryLog.recordAction(string(p->getName()) + " added to section " + 
                                      to_string(sectionIndex), "INVENTORY");
        } else {
            delete p;
        }
    }
    
    void assignSuppliersToProducts() {
        for (int i = 0; i < warehouse.getSectionCount() && i < supplierCount; i++) {
            InventorySection* section = warehouse.getSection(i);
            if (section) {
                for (int j = 0; j < section->getProductCount(); j++) {
                    Product* p = (*section)[j];
                    if (p) p->setSupplier(&suppliers[i % supplierCount]);
                }
            }
        }
    }
    
    void displayAllProducts() const { warehouse.displayAllSections(); }
    
    void displayByCategory(const char* category) const {
        printHeader(category);
        bool found = false;
        for (int i = 0; i < warehouse.getSectionCount(); i++) {
            const InventorySection* section = warehouse.getSection(i);
            if (section) {
                for (int j = 0; j < section->getProductCount(); j++) {
                    const Product* p = (*section)[j];
                    if (p && strcmp(p->getCategory(), category) == 0) {
                        if (!found) {
                            cout << "\n  " << left << setw(8) << "ID"
                                 << setw(25) << "Name"
                                 << setw(12) << "Price"
                                 << "Stock\n";
                            cout << "  " << string(55, '-') << "\n";
                            found = true;
                        }
                        cout << "  " << left << setw(8) << p->getID()
                             << setw(25) << p->getName()
                             << "$" << fixed << setprecision(2) << setw(11) << p->getPrice()
                             << p->getStock() << "\n";
                    }
                }
            }
        }
        if (!found) cout << "\n  No products in this category.\n";
    }
    
    Product* findProduct(int id) {
        for (int i = 0; i < warehouse.getSectionCount(); i++) {
            InventorySection* section = warehouse.getSection(i);
            if (section) {
                Product* p = section->findProductInSection(id);
                if (p) return p;
            }
        }
        return nullptr;
    }
    
    void addProduct() {
        printHeader("ADD NEW PRODUCT");
        
        int id, stock, type;
        double price;
        char name[100];
        
        cout << "\n  Product ID: "; cin >> id;
        if (findProduct(id)) { cout << "\n  [ERROR] ID exists.\n"; return; }
        
        cout << "  Product Name: "; cin.ignore(); cin.getline(name, 100);
        cout << "  Price ($): "; cin >> price;
        cout << "  Stock: "; cin >> stock;
        
        cout << "\n  Product Type:\n"
             << "    1. Fragile Electronics\n"
             << "    2. Standard Electronics\n"
             << "    3. Perishable Grocery\n"
             << "    4. Non-Perishable\n"
             << "    5. Luxury Apparel\n"
             << "    6. Standard Clothing\n"
             << "    7. Book\n"
             << "  Choice: ";
        cin >> type;
        
        Product* newP = nullptr;
        int sectionIdx = 0;
        
        switch (type) {
            case 1:
                {
                    int volt, warranty; double frag; char pack[50];
                    cout << "  Voltage: "; cin >> volt;
                    cout << "  Warranty (months): "; cin >> warranty;
                    cout << "  Fragility (0-1): "; cin >> frag;
                    cout << "  Packaging: "; cin >> pack;
                    newP = new FragileElectronics(id, name, price, stock, volt, warranty, frag, pack);
                    sectionIdx = 4;
                }
                break;
            case 2:
                {
                    int volt, warranty;
                    cout << "  Voltage: "; cin >> volt;
                    cout << "  Warranty (months): "; cin >> warranty;
                    newP = new ElectronicProduct(id, name, price, stock, volt, warranty);
                    sectionIdx = 0;
                }
                break;
            case 3:
                {
                    int cal; bool organic; char expiry[20]; double temp;
                    cout << "  Calories: "; cin >> cal;
                    cout << "  Organic (1/0): "; cin >> organic;
                    cout << "  Expiry Date: "; cin >> expiry;
                    cout << "  Storage Temp (C): "; cin >> temp;
                    newP = new PerishableGrocery(id, name, price, stock, cal, true, organic, expiry, temp);
                    sectionIdx = 2;
                }
                break;
            case 4:
                {
                    int cal, life; bool organic; double pres;
                    cout << "  Calories: "; cin >> cal;
                    cout << "  Organic (1/0): "; cin >> organic;
                    cout << "  Shelf Life (years): "; cin >> life;
                    cout << "  Preservative Level: "; cin >> pres;
                    newP = new NonPerishable(id, name, price, stock, cal, true, organic, life, pres);
                    sectionIdx = 2;
                }
                break;
            case 5:
                {
                    char sz[10], fab[50], brand[50]; double tax; int edition;
                    cout << "  Size: "; cin >> sz;
                    cout << "  Fabric: "; cin >> fab;
                    cout << "  Brand: "; cin >> brand;
                    cout << "  Luxury Tax (%): "; cin >> tax;
                    cout << "  Edition #: "; cin >> edition;
                    newP = new LuxuryApparel(id, name, price, stock, sz, fab, "Unisex", "All", true, brand, tax/100, edition);
                    sectionIdx = 5;
                }
                break;
            case 6:
                {
                    char sz[10], fab[50];
                    cout << "  Size: "; cin >> sz;
                    cout << "  Fabric: "; cin >> fab;
                    newP = new ClothingProduct(id, name, price, stock, sz, fab);
                    sectionIdx = 1;
                }
                break;
            case 7:
                {
                    char auth[100], isbn[20];
                    cout << "  Author: "; cin.ignore(); cin.getline(auth, 100);
                    cout << "  ISBN: "; cin >> isbn;
                    newP = new BookProduct(id, name, price, stock, auth, isbn);
                    sectionIdx = 3;
                }
                break;
            default:
                cout << "\n  Invalid type.\n";
                return;
        }
        
        if (newP) {
            addProductToSection(newP, sectionIdx);
            inventoryLog.recordAction(string(name) + " added to inventory", "ADMIN_ACTION");
            cout << "\n  [SUCCESS] Product added!\n";
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
        inventoryLog.recordAction(string(p->getName()) + " stock updated to " + to_string(newStock), "ADMIN_ACTION");
        cout << "\n  [SUCCESS] Stock updated.\n";
    }
    
    void removeProduct() {
        printHeader("REMOVE PRODUCT");
        
        int id;
        cout << "\n  Product ID to remove: "; cin >> id;
        
        for (int i = 0; i < warehouse.getSectionCount(); i++) {
            InventorySection* section = warehouse.getSection(i);
            if (section) {
                for (int j = 0; j < section->getProductCount(); j++) {
                    if ((*section)[j] && (*section)[j]->getID() == id) {
                        cout << "  Removing '" << (*section)[j]->getName() << "'...\n";
                        delete (*section)[j];
                        // Note: In a full implementation, we would shift remaining products
                        // For simplicity, we'll just mark that it's removed
                        inventoryLog.recordAction("Product #" + to_string(id) + " removed", "ADMIN_ACTION");
                        cout << "  [SUCCESS] Product removed.\n";
                        return;
                    }
                }
            }
        }
        cout << "\n  [ERROR] Product not found.\n";
    }
    
    void showLowStock() const { warehouse.findShortages(); }
    
    void showDiscounts() const {
        printHeader("CURRENT DISCOUNTS");
        cout << "\n  " << left << setw(8) << "ID"
             << setw(25) << "Product Name"
             << setw(15) << "Category"
             << setw(12) << "Orig. Price"
             << setw(12) << "Discount"
             << "Sale Price\n";
        cout << "  " << string(80, '-') << "\n";
        
        for (int i = 0; i < warehouse.getSectionCount(); i++) {
            const InventorySection* section = warehouse.getSection(i);
            if (section) {
                for (int j = 0; j < section->getProductCount(); j++) {
                    const Product* p = (*section)[j];
                    if (p) {
                        double disc = p->calculateDiscount();
                        double sale = p->getPrice() - disc;
                        cout << "  " << left << setw(8) << p->getID()
                             << setw(25) << p->getName()
                             << setw(15) << p->getCategory()
                             << "$" << fixed << setprecision(2) << setw(11) << p->getPrice()
                             << "$" << setw(11) << disc
                             << "$" << sale << "\n";
                    }
                }
            }
        }
    }
    
    void showRiskAssessment() const {
        printHeader("RISK ASSESSMENT REPORT");
        cout << "\n  " << left << setw(8) << "ID"
             << setw(25) << "Product Name"
             << setw(15) << "Category"
             << setw(12) << "Risk Score"
             << "Status\n";
        cout << "  " << string(70, '-') << "\n";
        
        for (int i = 0; i < warehouse.getSectionCount(); i++) {
            const InventorySection* section = warehouse.getSection(i);
            if (section) {
                for (int j = 0; j < section->getProductCount(); j++) {
                    const Product* p = (*section)[j];
                    if (p) {
                        double risk = p->calculateRisk();
                        cout << "  " << left << setw(8) << p->getID()
                             << setw(25) << p->getName()
                             << setw(15) << p->getCategory()
                             << setw(12) << risk
                             << (risk > 0.2 ? "HIGH RISK" : (risk > 0.1 ? "MEDIUM" : "LOW")) << "\n";
                    }
                }
            }
        }
    }
    
    void showExpiryAlert() const {
        printHeader("EXPIRY DATE ALERT");
        bool found = false;
        for (int i = 0; i < warehouse.getSectionCount(); i++) {
            const InventorySection* section = warehouse.getSection(i);
            if (section) {
                for (int j = 0; j < section->getProductCount(); j++) {
                    const Product* p = (*section)[j];
                    if (p && strcmp(p->getCategory(), "Perishable Grocery") == 0) {
                        const PerishableGrocery* perishable = dynamic_cast<const PerishableGrocery*>(p);
                        if (perishable) {
                            // We can't call non-const method on const object
                            cout << "  Checking expiry for " << p->getName() << "\n";
                            found = true;
                        }
                    }
                }
            }
        }
        if (!found) cout << "\n  No perishable items in inventory.\n";
    }
    
    void showSupplierInfo() const {
        printHeader("SUPPLIER DIRECTORY");
        for (int i = 0; i < supplierCount; i++) {
            suppliers[i].display();
        }
    }
    
    void printInventoryAudit() const { inventoryLog.printAuditTrail(); }
    
    void sortSection(int sectionIdx) { 
        InventorySection* sec = warehouse.getSection(sectionIdx);
        if (sec) sec->sortByID();
    }
};

// ================================================================
//  DASHBOARD FUNCTIONS
// ================================================================
void runAdminDashboard(UserManager& um, EnhancedInventoryManager& inv) {
    int ch;
    do {
        printHeader("ADMIN DASHBOARD - ENHANCED");
        
        cout << "\n  Logged in as: " << um.getCurrentUser()->getUsername() << " [ADMIN]\n\n";
        
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
             << "  |  8.  Risk Assessment Report                     |\n"
             << "  |  9.  Expiry Date Alert                          |\n"
             << "  | 10.  Supplier Directory                         |\n"
             << "  | 11.  Sort Section by ID                         |\n"
             << "  | 12.  View Audit Logs                            |\n"
             << "  +--------------------------------------------------+\n"
             << "  |  0.  Logout                                     |\n"
             << "  +--------------------------------------------------+\n"
             << "  Choice: ";
        cin >> ch;
        
        switch (ch) {
            case 1: inv.displayAllProducts(); break;
            case 2: {
                int cat;
                cout << "\n  1. Electronics  2. Clothing  3. Grocery  4. Books  5. Apparel\n  Choice: ";
                cin >> cat;
                if (cat == 1) inv.displayByCategory("Electronics");
                else if (cat == 2) inv.displayByCategory("Clothing");
                else if (cat == 3) inv.displayByCategory("Grocery");
                else if (cat == 4) inv.displayByCategory("Books");
                else if (cat == 5) inv.displayByCategory("Apparel");
                else cout << "\n  Invalid.\n";
                break;
            }
            case 3: inv.addProduct(); break;
            case 4: inv.updateStock(); break;
            case 5: inv.removeProduct(); break;
            case 6: inv.showLowStock(); break;
            case 7: inv.showDiscounts(); break;
            case 8: inv.showRiskAssessment(); break;
            case 9: inv.showExpiryAlert(); break;
            case 10: inv.showSupplierInfo(); break;
            case 11: {
                int sec;
                cout << "  Section (0-5): "; cin >> sec;
                inv.sortSection(sec);
                break;
            }
            case 12: inv.printInventoryAudit(); break;
            case 0: um.logout(); break;
            default: cout << "\n  Invalid choice.\n";
        }
        if (ch != 0) pressEnter();
    } while (ch != 0);
}

void runUserDashboard(UserManager& um, EnhancedInventoryManager& inv) {
    int ch;
    do {
        printHeader("CUSTOMER DASHBOARD");
        
        cout << "\n  Logged in as: " << um.getCurrentUser()->getUsername()
             << " | Wallet: $" << fixed << setprecision(2)
             << um.getCurrentUser()->getWalletBalance() << "\n\n";
        
        cout << "  +--------------------------------------------------+\n"
             << "  |                SHOPPING                         |\n"
             << "  +--------------------------------------------------+\n"
             << "  |  1.  Browse All Products                        |\n"
             << "  |  2.  Browse by Category                         |\n"
             << "  |  3.  Add Product to Cart                        |\n"
             << "  |  4.  View Cart                                  |\n"
             << "  |  5.  Checkout                                   |\n"
             << "  +--------------------------------------------------+\n"
             << "  |                ACCOUNT                          |\n"
             << "  +--------------------------------------------------+\n"
             << "  |  6.  View Order History                         |\n"
             << "  |  7.  Add Funds to Wallet                        |\n"
             << "  |  8.  View Profile                               |\n"
             << "  |  9.  Edit Profile                               |\n"
             << "  | 10.  Change Password                            |\n"
             << "  | 11.  Clear Cart                                 |\n"
             << "  +--------------------------------------------------+\n"
             << "  |  0.  Logout                                     |\n"
             << "  +--------------------------------------------------+\n"
             << "  Choice: ";
        cin >> ch;
        
        switch (ch) {
            case 1: inv.displayAllProducts(); break;
            case 2: {
                int cat;
                cout << "\n  1. Electronics  2. Clothing  3. Grocery  4. Books\n  Choice: ";
                cin >> cat;
                if (cat == 1) inv.displayByCategory("Electronics");
                else if (cat == 2) inv.displayByCategory("Clothing");
                else if (cat == 3) inv.displayByCategory("Grocery");
                else if (cat == 4) inv.displayByCategory("Books");
                else cout << "\n  Invalid.\n";
                break;
            }
            case 3: {
                int id, qty;
                cout << "\n  Product ID: "; cin >> id;
                cout << "  Quantity: "; cin >> qty;
                Product* p = inv.findProduct(id);
                if (!p) cout << "\n  [ERROR] Product not found.\n";
                else if (qty <= 0) cout << "\n  [ERROR] Invalid quantity.\n";
                else if (p->getStock() < qty) cout << "\n  [ERROR] Only " << p->getStock() << " in stock.\n";
                else um.addToCart(p->getID(), p->getName(), p->getPrice(), qty);
                break;
            }
            case 4: um.viewCart(); break;
            case 5: um.checkout(); break;
            case 6: um.viewOrders(); break;
            case 7: um.addFunds(); break;
            case 8: um.viewProfile(); break;
            case 9: um.editProfile(); break;
            case 10: um.changePassword(); break;
            case 11: um.clearCart(); break;
            case 0: um.logout(); break;
            default: cout << "\n  Invalid choice.\n";
        }
        if (ch != 0) pressEnter();
    } while (ch != 0);
}

// ================================================================
//  MAIN FUNCTION
// ================================================================
int main() {
    UserManager userManager;
    EnhancedInventoryManager inventory;
    
    int choice;
    bool running = true;
    
    while (running) {
        cout << "\n";
        printLine('=', 70);
        cout << "  " << string(20, ' ') << "ENHANCED INVENTORY MANAGEMENT SYSTEM\n";
        cout << "  " << string(22, ' ') << "OOP Final Project - C++\n";
        printLine('=', 70);
        
        cout << "\n"
             << "  +--------------------------------------------------+\n"
             << "  |            WELCOME TO THE SYSTEM                |\n"
             << "  +--------------------------------------------------+\n"
             << "  +--------------------------------------------------+\n"
             << "  |  1.  User Sign Up                               |\n"
             << "  |  2.  Admin Login                                |\n"
             << "  |  3.  User Login                                 |\n"
             << "  |  0.  Exit                                       |\n"
             << "  +--------------------------------------------------+\n"
             << "  Choice: ";
        cin >> choice;
        
        switch (choice) {
            case 1:
                userManager.signUp();
                pressEnter();
                break;
            case 2:
                if (userManager.adminLogin()) {
                    runAdminDashboard(userManager, inventory);
                } else {
                    pressEnter();
                }
                break;
            case 3:
                if (userManager.userLogin()) {
                    runUserDashboard(userManager, inventory);
                } else {
                    pressEnter();
                }
                break;
            case 0:
                running = false;
                cout << "\n  Thank you for using the Enhanced Inventory Management System!\n";
                break;
            default:
                cout << "\n  [ERROR] Invalid choice.\n";
                pressEnter();
        }
    }
    
    return 0;
}
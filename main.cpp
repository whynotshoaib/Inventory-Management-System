/*
 * ============================================================
 *   INVENTORY MANAGEMENT SYSTEM
 *   OOP Final Project - C++
 *   Single-file implementation (all classes + main)
 * ============================================================
 */

#include <iostream>
#include <cstring>
#include <iomanip>
#include <cctype>
using namespace std;

// ================================================================
//  UTILITY HELPERS
// ================================================================
static void printLine(char c = '=', int n = 70) {
    for (int i = 0; i < n; i++) cout << c;
    cout << '\n';
}

static void printHeader(const char* title) {
    cout << '\n';
    printLine('=', 70);
    cout << "  " << title << '\n';
    printLine('=', 70);
}

static void printSubHeader(const char* title) {
    cout << "\n  --- " << title << " ---\n";
}

// ================================================================
//  USER STORE  (simple in-memory user DB)
// ================================================================
struct UserRecord {
    int     userId;
    char    username[32];
    char    password[32];
    bool    isAdmin;
};

static const int MAX_USERS = 50;
static UserRecord userDB[MAX_USERS];
static int userCount = 0;
static int nextUserId = 1001;

static void seedDefaultUsers() {
    // Admin user (only one admin)
    userDB[0].userId = 1000;
    strcpy(userDB[0].username, "Administrator");
    strcpy(userDB[0].password, "admin123");
    userDB[0].isAdmin = true;

    // Pre-loaded student users
    userDB[1].userId = 1001;
    strcpy(userDB[1].username, "John Smith");
    strcpy(userDB[1].password, "john123");
    userDB[1].isAdmin = false;
    
    userDB[2].userId = 1002;
    strcpy(userDB[2].username, "Emma Watson");
    strcpy(userDB[2].password, "emma123");
    userDB[2].isAdmin = false;
    
    userDB[3].userId = 1003;
    strcpy(userDB[3].username, "Michael Brown");
    strcpy(userDB[3].password, "mike123");
    userDB[3].isAdmin = false;
    
    userCount = 4;
    nextUserId = 1004;
}

static bool userIdExists(int uid) {
    for (int i = 0; i < userCount; i++)
        if (userDB[i].userId == uid) return true;
    return false;
}

static bool usernameExists(const char* uname) {
    for (int i = 0; i < userCount; i++)
        if (strcmp(userDB[i].username, uname) == 0) return true;
    return false;
}

// Returns pointer to matched record, or nullptr
static UserRecord* authenticate(int uid, const char* pwd) {
    for (int i = 0; i < userCount; i++)
        if (userDB[i].userId == uid && strcmp(userDB[i].password, pwd) == 0)
            return &userDB[i];
    return nullptr;
}

// ================================================================
//  TEMPLATE: TRANSACTION LOG
// ================================================================
template <typename T>
class TransactionLog {
    T**  entries;
    int  cap, cnt;
public:
    TransactionLog(int c = 50) : cap(c), cnt(0) {
        entries = new T*[cap];
        for (int i = 0; i < cap; i++) entries[i] = nullptr;
    }
    ~TransactionLog() {
        for (int i = 0; i < cnt; i++) delete entries[i];
        delete[] entries;
    }
    void record(const T& item, const char* action) {
        if (cnt < cap) {
            entries[cnt++] = new T(item);
            cout << "  [LOG] " << action << " -> " << item << '\n';
        } else cout << "  [LOG] Log full!\n";
    }
    void printAll() const {
        if (cnt == 0) { cout << "  (no entries)\n"; return; }
        for (int i = 0; i < cnt; i++)
            cout << "  " << setw(3) << i+1 << ". " << *entries[i] << '\n';
    }
    int getCount() const { return cnt; }
};

// ================================================================
//  LEVEL 1: PRODUCT  (abstract base)
// ================================================================
class Product {
protected:
    int    productID;
    double price;
    int    quantity;
    char*  brandName;

public:
    Product(int id=0, double pr=0.0, int qty=0, const char* brand="Unknown")
        : productID(id), price(pr), quantity(qty) {
        brandName = new char[strlen(brand)+1];
        strcpy(brandName, brand);
    }
    virtual ~Product() { delete[] brandName; }

    Product(const Product& o) : productID(o.productID), price(o.price), quantity(o.quantity) {
        brandName = new char[strlen(o.brandName)+1];
        strcpy(brandName, o.brandName);
    }
    Product& operator=(const Product& o) {
        if (this != &o) {
            productID = o.productID; price = o.price; quantity = o.quantity;
            delete[] brandName;
            brandName = new char[strlen(o.brandName)+1];
            strcpy(brandName, o.brandName);
        }
        return *this;
    }

    virtual void   displayStatus() const = 0;
    virtual double calculateRisk()  const { return (quantity > 100) ? 0.7 : 0.2; }
    virtual double calculateValue() const { return price * quantity; }
    virtual void   applyDiscount(double pct) { price *= (1.0 - pct/100.0); }

    int    getID()       const { return productID; }
    double getPrice()    const { return price; }
    int    getQuantity() const { return quantity; }
    const char* getBrand() const { return brandName; }
    void setQuantity(int q) { quantity = q; }
    void setPrice(double p) { price = p; }

    bool operator==(const Product& o) const { return productID == o.productID; }
    Product& operator+(const Product& o) {
        if (productID == o.productID) quantity += o.quantity;
        return *this;
    }
    friend ostream& operator<<(ostream& os, const Product& p) {
        os << "ID:" << setw(4) << p.productID
           << " | " << setw(22) << left << p.brandName << right
           << " | Qty:" << setw(4) << p.quantity
           << " | $" << fixed << setprecision(2) << setw(8) << p.price;
        return os;
    }
};

// ================================================================
//  LEVEL 2: GROCERY PRODUCT
// ================================================================
class GroceryProduct : public Product {
protected:
    int   calories;
    bool  isHalal;
    char* nutritionGrade;
public:
    GroceryProduct(int id=0, double pr=0.0, int qty=0, const char* brand="Unknown",
                   int cal=0, bool halal=true, const char* grade="A")
        : Product(id,pr,qty,brand), calories(cal), isHalal(halal) {
        nutritionGrade = new char[strlen(grade)+1];
        strcpy(nutritionGrade, grade);
    }
    virtual ~GroceryProduct() { delete[] nutritionGrade; }
    GroceryProduct(const GroceryProduct& o) : Product(o), calories(o.calories), isHalal(o.isHalal) {
        nutritionGrade = new char[strlen(o.nutritionGrade)+1];
        strcpy(nutritionGrade, o.nutritionGrade);
    }
    GroceryProduct& operator=(const GroceryProduct& o) {
        if (this != &o) {
            Product::operator=(o); calories=o.calories; isHalal=o.isHalal;
            delete[] nutritionGrade;
            nutritionGrade = new char[strlen(o.nutritionGrade)+1];
            strcpy(nutritionGrade, o.nutritionGrade);
        }
        return *this;
    }
    void displayStatus() const override {
        cout << "[Grocery] " << getBrand()
             << " | Cal:" << calories
             << " | Grade:" << nutritionGrade
             << " | Halal:" << (isHalal?"Yes":"No");
    }
    double calculateRisk() const override {
        double r = 0.2;
        if (calories > 500) r += 0.3;
        if (!isHalal)       r += 0.1;
        return r;
    }
    virtual void checkSafety() const {
        cout << "  Safety OK: " << getBrand() << " Grade " << nutritionGrade << '\n';
    }
    int  getCalories()      const { return calories; }
    bool isHalalCertified() const { return isHalal;  }
};

// ================================================================
//  LEVEL 3: PERISHABLE GROCERY
// ================================================================
class PerishableGrocery : public GroceryProduct {
private:
    int   expiryDays;
    double storageTemp;
    char* storageInstructions;
public:
    PerishableGrocery(int id=0, double pr=0.0, int qty=0, const char* brand="Unknown",
                      int cal=0, bool halal=true, const char* grade="A",
                      int expiry=30, double temp=4.0, const char* inst="Keep refrigerated")
        : GroceryProduct(id,pr,qty,brand,cal,halal,grade), expiryDays(expiry), storageTemp(temp) {
        storageInstructions = new char[strlen(inst)+1];
        strcpy(storageInstructions, inst);
    }
    virtual ~PerishableGrocery() { delete[] storageInstructions; }
    PerishableGrocery(const PerishableGrocery& o)
        : GroceryProduct(o), expiryDays(o.expiryDays), storageTemp(o.storageTemp) {
        storageInstructions = new char[strlen(o.storageInstructions)+1];
        strcpy(storageInstructions, o.storageInstructions);
    }
    PerishableGrocery& operator=(const PerishableGrocery& o) {
        if (this != &o) {
            GroceryProduct::operator=(o);
            expiryDays=o.expiryDays; storageTemp=o.storageTemp;
            delete[] storageInstructions;
            storageInstructions = new char[strlen(o.storageInstructions)+1];
            strcpy(storageInstructions, o.storageInstructions);
        }
        return *this;
    }
    void displayStatus() const override {
        cout << "[Perishable] " << getBrand()
             << " | Cal:" << calories
             << " | Expires:" << expiryDays << "d"
             << " | " << storageTemp << "C";
    }
    double calculateRisk() const override {
        double r = GroceryProduct::calculateRisk();
        if (expiryDays < 5)  r += 0.6;
        else if (expiryDays < 10) r += 0.3;
        return r;
    }
    void checkSafety() const override {
        cout << "  Perishable check: " << getBrand()
             << " expires in " << expiryDays << " day(s)";
        if (expiryDays < 3) cout << " [!!! URGENT]";
        cout << '\n';
    }
    void checkExpiry() const {
        if (expiryDays <= 0)    cout << "  EXPIRED! Dispose immediately!\n";
        else if (expiryDays < 3)cout << "  EXPIRING SOON (" << expiryDays << "d)\n";
        else cout << "  Fresh for " << expiryDays << " more days\n";
    }
    int getExpiryDays() const { return expiryDays; }
};

// ================================================================
//  LEVEL 3: NON-PERISHABLE
// ================================================================
class NonPerishable : public GroceryProduct {
private:
    int   shelfLifeYears;
    double preservativeLevel;
    char* storageType;
public:
    NonPerishable(int id=0, double pr=0.0, int qty=0, const char* brand="Unknown",
                  int cal=0, bool halal=true, const char* grade="A",
                  int shelf=2, double pres=0.5, const char* storage="CoolDry")
        : GroceryProduct(id,pr,qty,brand,cal,halal,grade), shelfLifeYears(shelf), preservativeLevel(pres) {
        storageType = new char[strlen(storage)+1];
        strcpy(storageType, storage);
    }
    virtual ~NonPerishable() { delete[] storageType; }
    NonPerishable(const NonPerishable& o)
        : GroceryProduct(o), shelfLifeYears(o.shelfLifeYears), preservativeLevel(o.preservativeLevel) {
        storageType = new char[strlen(o.storageType)+1];
        strcpy(storageType, o.storageType);
    }
    NonPerishable& operator=(const NonPerishable& o) {
        if (this != &o) {
            GroceryProduct::operator=(o);
            shelfLifeYears=o.shelfLifeYears; preservativeLevel=o.preservativeLevel;
            delete[] storageType;
            storageType = new char[strlen(o.storageType)+1];
            strcpy(storageType, o.storageType);
        }
        return *this;
    }
    void displayStatus() const override {
        cout << "[NonPerish] " << getBrand()
             << " | Cal:" << calories
             << " | Shelf:" << shelfLifeYears << "yr"
             << " | Pres:" << preservativeLevel << "%";
    }
    double calculateRisk() const override {
        double r = GroceryProduct::calculateRisk();
        if (preservativeLevel > 1.0) r += 0.2;
        return r * 0.5;
    }
    void getStorageInstructions() const {
        cout << "  Store in " << storageType << " for up to " << shelfLifeYears << " yr(s)\n";
    }
};

// ================================================================
//  LEVEL 2: ELECTRONIC PRODUCT
// ================================================================
class ElectronicProduct : public Product {
protected:
    int   voltage, warrantyMonths;
    char* powerRating;
public:
    ElectronicProduct(int id=0, double pr=0.0, int qty=0, const char* brand="Unknown",
                      int volt=220, int warranty=12, const char* power="Standard")
        : Product(id,pr,qty,brand), voltage(volt), warrantyMonths(warranty) {
        powerRating = new char[strlen(power)+1];
        strcpy(powerRating, power);
    }
    virtual ~ElectronicProduct() { delete[] powerRating; }
    ElectronicProduct(const ElectronicProduct& o)
        : Product(o), voltage(o.voltage), warrantyMonths(o.warrantyMonths) {
        powerRating = new char[strlen(o.powerRating)+1];
        strcpy(powerRating, o.powerRating);
    }
    ElectronicProduct& operator=(const ElectronicProduct& o) {
        if (this != &o) {
            Product::operator=(o); voltage=o.voltage; warrantyMonths=o.warrantyMonths;
            delete[] powerRating;
            powerRating = new char[strlen(o.powerRating)+1];
            strcpy(powerRating, o.powerRating);
        }
        return *this;
    }
    void displayStatus() const override {
        cout << "[Electronic] " << getBrand()
             << " | " << voltage << "V"
             << " | Warranty:" << warrantyMonths << "mo"
             << " | " << powerRating;
    }
    double calculateRisk() const override { return (voltage > 200) ? 0.5 : 0.1; }
    void testHardware() const { cout << "  Hardware test: " << getBrand() << " OK\n"; }
    int getVoltage()       const { return voltage; }
    int getWarrantyMonths()const { return warrantyMonths; }
};

// ================================================================
//  LEVEL 3: FRAGILE ELECTRONICS
// ================================================================
class FragileElectronics : public ElectronicProduct {
private:
    double fragilityRating;
    char*  packagingType;
public:
    FragileElectronics(int id=0, double pr=0.0, int qty=0, const char* brand="Unknown",
                       int volt=220, int warranty=12, const char* power="Standard",
                       double frag=5.0, const char* pack="BubbleWrap")
        : ElectronicProduct(id,pr,qty,brand,volt,warranty,power), fragilityRating(frag) {
        packagingType = new char[strlen(pack)+1];
        strcpy(packagingType, pack);
    }
    virtual ~FragileElectronics() { delete[] packagingType; }
    FragileElectronics(const FragileElectronics& o)
        : ElectronicProduct(o), fragilityRating(o.fragilityRating) {
        packagingType = new char[strlen(o.packagingType)+1];
        strcpy(packagingType, o.packagingType);
    }
    FragileElectronics& operator=(const FragileElectronics& o) {
        if (this != &o) {
            ElectronicProduct::operator=(o); fragilityRating=o.fragilityRating;
            delete[] packagingType;
            packagingType = new char[strlen(o.packagingType)+1];
            strcpy(packagingType, o.packagingType);
        }
        return *this;
    }
    void displayStatus() const override {
        cout << "[Fragile] " << getBrand()
             << " | Fragility:" << fragilityRating << "/10"
             << " | Pack:" << packagingType;
    }
    double calculateRisk() const override { return fragilityRating * 1.5; }
    void calculateShippingRisk() const {
        cout << "  Shipping risk for " << getBrand()
             << ": " << (fragilityRating > 7 ? "HIGH" : "MEDIUM") << '\n';
    }
};

// ================================================================
//  LEVEL 2: CLOTHING PRODUCT
// ================================================================
class ClothingProduct : public Product {
private:
    char* size;
    char* fabric;
    char  gender;
public:
    ClothingProduct(int id=0, double pr=0.0, int qty=0, const char* brand="Unknown",
                    const char* sz="M", const char* fab="Cotton", char gen='U')
        : Product(id,pr,qty,brand), gender(gen) {
        size   = new char[strlen(sz)+1];  strcpy(size,   sz);
        fabric = new char[strlen(fab)+1]; strcpy(fabric, fab);
    }
    virtual ~ClothingProduct() { delete[] size; delete[] fabric; }
    ClothingProduct(const ClothingProduct& o) : Product(o), gender(o.gender) {
        size   = new char[strlen(o.size)+1];   strcpy(size,   o.size);
        fabric = new char[strlen(o.fabric)+1]; strcpy(fabric, o.fabric);
    }
    ClothingProduct& operator=(const ClothingProduct& o) {
        if (this != &o) {
            Product::operator=(o); gender=o.gender;
            delete[] size; delete[] fabric;
            size   = new char[strlen(o.size)+1];   strcpy(size,   o.size);
            fabric = new char[strlen(o.fabric)+1]; strcpy(fabric, o.fabric);
        }
        return *this;
    }
    void displayStatus() const override {
        cout << "[Clothing] " << getBrand()
             << " | Size:" << size
             << " | " << fabric
             << " | Gender:" << gender;
    }
    double calculateRisk() const override { return 0.1; }
    void fitGuide() const { cout << "  Size " << size << " - check size chart\n"; }
};

// ================================================================
//  SUPPLIER
// ================================================================
class Supplier {
private:
    int    supplierID;
    char*  contractTerms;
    double restockEfficiency;
public:
    Supplier(int id=0, const char* terms="Standard", double eff=0.8)
        : supplierID(id), restockEfficiency(eff) {
        contractTerms = new char[strlen(terms)+1];
        strcpy(contractTerms, terms);
    }
    ~Supplier() { delete[] contractTerms; }
    Supplier(const Supplier& o) : supplierID(o.supplierID), restockEfficiency(o.restockEfficiency) {
        contractTerms = new char[strlen(o.contractTerms)+1];
        strcpy(contractTerms, o.contractTerms);
    }
    Supplier& operator=(const Supplier& o) {
        if (this != &o) {
            supplierID=o.supplierID; restockEfficiency=o.restockEfficiency;
            delete[] contractTerms;
            contractTerms = new char[strlen(o.contractTerms)+1];
            strcpy(contractTerms, o.contractTerms);
        }
        return *this;
    }
    void orderRestock(int pid, int qty) const {
        cout << "  Ordering " << qty << " units of P#" << pid
             << " from Supplier " << supplierID << '\n';
    }
    void generateInvoice() const {
        cout << "  Invoice | Supplier:" << supplierID
             << " | Terms:" << contractTerms
             << " | Efficiency:" << restockEfficiency*100 << "%\n";
    }
    int getID() const { return supplierID; }
    const char* getContractTerms() const { return contractTerms; }
    bool operator==(const Supplier& o) const { return supplierID==o.supplierID; }
    friend ostream& operator<<(ostream& os, const Supplier& s) {
        os << "Supplier#" << s.supplierID << " | " << s.contractTerms
           << " | Eff:" << s.restockEfficiency*100 << "%";
        return os;
    }
};

// ================================================================
//  INVENTORY SECTION
// ================================================================
class InventorySection {
private:
    int       aisleNumber, capacity, currentCount;
    Product** products;
public:
    InventorySection(int aisle=1, int cap=10)
        : aisleNumber(aisle), capacity(cap), currentCount(0) {
        products = new Product*[cap];
        for (int i=0;i<cap;i++) products[i]=nullptr;
    }
    ~InventorySection() { delete[] products; }
    InventorySection(const InventorySection& o)
        : aisleNumber(o.aisleNumber), capacity(o.capacity), currentCount(o.currentCount) {
        products = new Product*[capacity];
        for (int i=0;i<currentCount;i++) products[i]=o.products[i];
        for (int i=currentCount;i<capacity;i++) products[i]=nullptr;
    }
    InventorySection& operator=(const InventorySection& o) {
        if (this!=&o) {
            delete[] products;
            aisleNumber=o.aisleNumber; capacity=o.capacity; currentCount=o.currentCount;
            products=new Product*[capacity];
            for (int i=0;i<currentCount;i++) products[i]=o.products[i];
            for (int i=currentCount;i<capacity;i++) products[i]=nullptr;
        }
        return *this;
    }
    bool addProduct(Product* p) {
        if (currentCount < capacity) { products[currentCount++]=p; return true; }
        return false;
    }
    void displayAll() const {
        if (currentCount==0) { cout << "  [Empty]\n"; return; }
        for (int i=0;i<currentCount;i++) {
            cout << "  [" << setw(2) << i+1 << "] ";
            products[i]->displayStatus();
            cout << " | Value: $" << fixed << setprecision(2)
                 << products[i]->calculateValue() << '\n';
        }
    }
    double getSectionValue() const {
        double t=0;
        for (int i=0;i<currentCount;i++) t+=products[i]->calculateValue();
        return t;
    }
    int getAisle()        const { return aisleNumber; }
    int getProductCount() const { return currentCount; }
    int getCapacity()     const { return capacity;     }
    Product* operator[](int i) {
        return (i>=0 && i<currentCount) ? products[i] : nullptr;
    }
    InventorySection& operator+(const InventorySection& o) {
        for (int i=0; i<o.currentCount && currentCount<capacity; i++)
            products[currentCount++] = o.products[i];
        return *this;
    }
};

// ================================================================
//  WAREHOUSE
// ================================================================
class Warehouse {
private:
    int               warehouseID;
    char*             location;
    int               numSections;
    InventorySection* sections;
    Product**         ownedProducts;
    int               ownedCount, ownedCap;
public:
    Warehouse(int id=0, const char* loc="Unknown", int ns=3)
        : warehouseID(id), numSections(ns), ownedCount(0), ownedCap(200) {
        location = new char[strlen(loc)+1]; strcpy(location,loc);
        sections = new InventorySection[ns];
        for (int i=0;i<ns;i++) sections[i]=InventorySection(i+1, 15);
        ownedProducts = new Product*[ownedCap];
        for (int i=0;i<ownedCap;i++) ownedProducts[i]=nullptr;
    }
    ~Warehouse() {
        delete[] location;
        delete[] sections;
        for (int i=0;i<ownedCount;i++) delete ownedProducts[i];
        delete[] ownedProducts;
    }
    bool addToSection(int secIdx, Product* p) {
        if (secIdx<0||secIdx>=numSections) return false;
        if (ownedCount<ownedCap) ownedProducts[ownedCount++]=p;
        return sections[secIdx].addProduct(p);
    }
    void displayWarehouse() const {
        cout << "\n  Warehouse #" << warehouseID << " - " << location << '\n';
        cout << "  Total Value: $" << fixed << setprecision(2) << getTotalValue() << '\n';
        for (int i=0;i<numSections;i++) {
            cout << "\n  -- Aisle " << i+1
                 << " (" << sections[i].getProductCount()
                 << "/" << sections[i].getCapacity() << " products) --\n";
            sections[i].displayAll();
        }
    }
    double getTotalValue() const {
        double t=0;
        for (int i=0;i<numSections;i++) t+=sections[i].getSectionValue();
        return t;
    }
    int getNumSections() const { return numSections; }
    InventorySection& operator[](int i) { return sections[i]; }
    friend ostream& operator<<(ostream& os, const Warehouse& w) {
        os << "Warehouse#" << w.warehouseID << " (" << w.location
           << ") Sections:" << w.numSections
           << " TotalValue:$" << fixed << setprecision(2) << w.getTotalValue();
        return os;
    }
};

// ================================================================
//  ADMIN MODULE
// ================================================================
class AdminModule {
private:
    Warehouse*              warehouse;
    TransactionLog<Supplier> supplierLog;
    char productLogBrands[200][64];
    char productLogActions[200][32];
    int  productLogCount;

    int pickSection() const {
        int s;
        cout << "  Select section (0-" << warehouse->getNumSections()-1 << "): ";
        cin >> s;
        return s;
    }

    void menuAddProduct() {
        printHeader("ADD NEW PRODUCT");
        cout << "  Product types:\n"
             << "    1. Grocery (Perishable)\n"
             << "    2. Grocery (Non-Perishable)\n"
             << "    3. Electronic Product\n"
             << "    4. Fragile Electronics\n"
             << "    5. Clothing Product\n"
             << "  Choice: ";
        int ch; cin >> ch;

        int id, qty; double pr;
        char brand[64];
        cout << "  Product ID: "; cin >> id;
        cout << "  Brand name: "; cin >> brand;
        cout << "  Price ($): "; cin >> pr;
        cout << "  Quantity: "; cin >> qty;

        Product* p = nullptr;
        if (ch==1) {
            int cal,exp; bool hal; char grade[4]; double temp;
            cout << "  Calories: "; cin >> cal;
            cout << "  Halal (1/0): "; cin >> hal;
            cout << "  Grade (A/B/C): "; cin >> grade;
            cout << "  Expiry days: "; cin >> exp;
            cout << "  Storage temp: "; cin >> temp;
            p = new PerishableGrocery(id,pr,qty,brand,cal,hal,grade,exp,temp);
        } else if (ch==2) {
            int cal,shelf; bool hal; char grade[4]; double pres; char stor[32];
            cout << "  Calories: "; cin >> cal;
            cout << "  Halal (1/0): "; cin >> hal;
            cout << "  Grade: "; cin >> grade;
            cout << "  Shelf life(yr): "; cin >> shelf;
            cout << "  Preservatives: "; cin >> pres;
            cout << "  Storage type: "; cin >> stor;
            p = new NonPerishable(id,pr,qty,brand,cal,hal,grade,shelf,pres,stor);
        } else if (ch==3) {
            int volt,warranty; char power[32];
            cout << "  Voltage (V): "; cin >> volt;
            cout << "  Warranty (mo): "; cin >> warranty;
            cout << "  Power rating: "; cin >> power;
            p = new ElectronicProduct(id,pr,qty,brand,volt,warranty,power);
        } else if (ch==4) {
            int volt,warranty; char power[32],pack[32]; double frag;
            cout << "  Voltage (V): "; cin >> volt;
            cout << "  Warranty (mo): "; cin >> warranty;
            cout << "  Power rating: "; cin >> power;
            cout << "  Fragility(0-10): "; cin >> frag;
            cout << "  Packaging type: "; cin >> pack;
            p = new FragileElectronics(id,pr,qty,brand,volt,warranty,power,frag,pack);
        } else if (ch==5) {
            char sz[8],fab[32]; char gen;
            cout << "  Size (S/M/L/XL): "; cin >> sz;
            cout << "  Fabric: "; cin >> fab;
            cout << "  Gender (M/F/U): "; cin >> gen;
            p = new ClothingProduct(id,pr,qty,brand,sz,fab,gen);
        } else { cout << "  Invalid choice.\n"; return; }

        int sec = pickSection();
        if (warehouse->addToSection(sec, p)) {
            logProduct(p, "ADD");
            cout << "  Product added to section " << sec << " successfully.\n";
        } else {
            cout << "  Section full or invalid. Product NOT added.\n";
            delete p;
        }
    }

    void menuViewInventory() const {
        printHeader("FULL INVENTORY REPORT");
        cout << *warehouse << '\n';
        warehouse->displayWarehouse();
    }

    void menuRemoveExpired() const {
        printHeader("EXPIRY / RISK ASSESSMENT");
        printSubHeader("HIGH RISK PRODUCTS (>0.7)");
        int ns = warehouse->getNumSections();
        bool found = false;
        for (int i=0; i<ns; i++) {
            for (int j=0; j<(*const_cast<Warehouse*>(warehouse))[i].getProductCount(); j++) {
                Product* p = (*const_cast<Warehouse*>(warehouse))[i][j];
                if (!p) continue;
                double risk = p->calculateRisk();
                if (risk > 0.7) {
                    cout << "  [HIGH RISK " << fixed << setprecision(2) << risk << "] ";
                    p->displayStatus(); cout << '\n';
                    found = true;
                }
            }
        }
        if (!found) cout << "  No high-risk products found.\n";
    }

    void menuApplyDiscount() {
        printHeader("APPLY DISCOUNT");
        cout << "  Enter product ID: "; int pid; cin >> pid;
        cout << "  Discount (%): ";     double pct; cin >> pct;
        int ns = warehouse->getNumSections();
        bool done = false;
        for (int i=0; i<ns && !done; i++) {
            for (int j=0; j<(*warehouse)[i].getProductCount() && !done; j++) {
                Product* p = (*warehouse)[i][j];
                if (p && p->getID()==pid) {
                    p->applyDiscount(pct);
                    cout << "  Discount applied. New price: $"
                         << fixed << setprecision(2) << p->getPrice() << '\n';
                    done = true;
                }
            }
        }
        if (!done) cout << "  Product ID not found.\n";
    }

    void menuAddSupplier() {
        printHeader("ADD SUPPLIER");
        int id; char terms[64]; double eff;
        cout << "  Supplier ID: "; cin >> id;
        cout << "  Contract terms: "; cin >> terms;
        cout << "  Restock efficiency (0-1): "; cin >> eff;
        Supplier s(id, terms, eff);
        supplierLog.record(s, "SUPPLIER ADD");
        s.generateInvoice();
    }

    void menuRestockOrder() const {
        printHeader("RESTOCK ORDER");
        if (supplierLog.getCount()==0) {
            cout << "  No suppliers registered.\n"; return;
        }
        int pid, qty;
        cout << "  Product ID to restock: "; cin >> pid;
        cout << "  Quantity to order: "; cin >> qty;
        Supplier s(9001,"Default-30Day",0.9);
        s.orderRestock(pid, qty);
    }

    void menuMergeSections() {
        printHeader("MERGE SECTIONS");
        int a, b;
        cout << "  Merge FROM section: "; cin >> a;
        cout << "  Merge INTO section: "; cin >> b;
        int ns = warehouse->getNumSections();
        if (a<0||a>=ns||b<0||b>=ns||a==b) {
            cout << "  Invalid section indices.\n"; return;
        }
        (*warehouse)[b] + (*warehouse)[a];
        cout << "  Section " << a << " merged into Section " << b << ".\n";
    }

    void menuAuditLog() const {
        printHeader("AUDIT TRAIL");
        printSubHeader("Product Transaction Log");
        if (productLogCount == 0) cout << "  (no entries)\n";
        for (int i = 0; i < productLogCount; i++)
            cout << "  " << setw(3) << i+1 << ". [" << productLogActions[i] << "] " << productLogBrands[i] << '\n';
        printSubHeader("Supplier Transaction Log");
        supplierLog.printAll();
    }

    void menuDemoPolymorphism() const {
        printHeader("DEMONSTRATION: POLYMORPHISM");
        printSubHeader("Virtual Function Behavior at Runtime");
        Product* demo[4];
        demo[0] = new GroceryProduct(9001, 4.99, 100, "CerealBrand", 200, true, "A");
        demo[1] = new PerishableGrocery(9002, 3.49, 50, "MilkBrand", 120, true, "A+", 4, 4.0);
        demo[2] = new ElectronicProduct(9003, 499.99, 10, "SonyTV", 220, 24);
        demo[3] = new ClothingProduct(9004, 29.99, 200, "NikeShirt", "L", "Cotton", 'U');
        
        cout << "\n  Different products responding to same interface:\n\n";
        for (int i=0; i<4; i++) {
            cout << "  ";
            demo[i]->displayStatus();
            cout << " | Risk: " << fixed << setprecision(2) << demo[i]->calculateRisk() << '\n';
            delete demo[i];
        }
        cout << "\n  [SUCCESS] Each product calculates its own risk based on its type!\n";
    }

    void menuDemoInheritance() const {
        printHeader("DEMONSTRATION: DEEP INHERITANCE");
        cout << "  Class Hierarchy:\n"
             << "    Product (L1 - Abstract)\n"
             << "      |-- GroceryProduct (L2)\n"
             << "      |    |-- PerishableGrocery (L3)\n"
             << "      |    |-- NonPerishable (L3)\n"
             << "      |-- ElectronicProduct (L2)\n"
             << "      |    |-- FragileElectronics (L3)\n"
             << "      |-- ClothingProduct (L2)\n\n";
        
        printSubHeader("Real Examples");
        GroceryProduct gp(8001, 5.99, 80, "Kelloggs", 150, true, "A");
        cout << "  L2 (Grocery): "; gp.displayStatus(); cout << '\n';
        
        PerishableGrocery pg(8002, 3.99, 40, "FreshMilk", 120, true, "A+", 4, 4.0);
        cout << "  L3 (Perishable): "; pg.displayStatus(); cout << '\n';
        pg.checkExpiry();
        
        NonPerishable np(8003, 2.49, 500, "RicePalace", 350, true, "B", 5, 0.1);
        cout << "  L3 (NonPerishable): "; np.displayStatus(); cout << '\n';
        np.getStorageInstructions();
        
        cout << "\n  [SUCCESS] Each level adds new features while reusing parent code!\n";
    }

    void menuDemoTemplates() const {
        printHeader("DEMONSTRATION: TEMPLATES");
        printSubHeader("Generic TransactionLog<T> Class");
        TransactionLog<int> intLog(5);
        int v1 = 100, v2 = 200;
        intLog.record(v1, "Integer value 100");
        intLog.record(v2, "Integer value 200");
        cout << "\n  Log with int type:\n";
        intLog.printAll();
        
        cout << "\n  [SUCCESS] Same TransactionLog<T> works for:\n"
             << "    - Product logs (strings)\n"
             << "    - Supplier logs\n"
             << "    - Integer values\n"
             << "    - Any data type!\n";
    }

    void menuDemoOperators() const {
        printHeader("DEMONSTRATION: OPERATOR OVERLOADING");
        printSubHeader("Custom Operators");
        cout << "  1. Stream insertion (<<) operator:\n";
        cout << "     " << *warehouse << '\n';
        
        cout << "\n  2. Array subscript ([]) operator:\n";
        cout << "     Section 0 has " << const_cast<Warehouse*>(warehouse)->operator[](0).getProductCount() << " products\n";
        
        cout << "\n  3. Addition (+) operator for merging:\n";
        GroceryProduct a(7001,2.0,100,"BrandA",200,true,"B");
        GroceryProduct b(7001,2.0,50,"BrandA",200,true,"B");
        cout << "     Before merge: "; a.displayStatus(); cout << '\n';
        a + b;
        cout << "     After merge:  "; a.displayStatus(); cout << '\n';
        
        cout << "\n  [SUCCESS] Operators make classes behave like built-in types!\n";
    }

    void menuChangePassword(UserRecord* user) {
        printHeader("CHANGE PASSWORD");
        char curr[32], newpwd[32];
        cout << "  Current password: "; cin >> curr;
        if (strcmp(curr, user->password)!=0) { 
            cout << "  Wrong password.\n"; return; 
        }
        cout << "  New password: "; cin >> newpwd;
        strcpy(user->password, newpwd);
        cout << "  Password updated successfully!\n";
    }

public:
    void logProduct(const Product* p, const char* action) {
        if (productLogCount < 200) {
            strncpy(productLogBrands[productLogCount], p->getBrand(), 63);
            strncpy(productLogActions[productLogCount], action, 31);
            productLogCount++;
        }
    }

    AdminModule() : productLogCount(0) {
        warehouse = new Warehouse(101, "Main Distribution Center", 3);
        // Seed demo products
        warehouse->addToSection(0, new GroceryProduct(101,4.99,100,"Kelloggs Cereal",150,true,"A"));
        warehouse->addToSection(0, new PerishableGrocery(102,3.99,60,"Fresh Milk",120,true,"A+",4,4.0));
        warehouse->addToSection(0, new NonPerishable(103,2.49,500,"Rice Palace",350,true,"B",5,0.1,"Cool Dry"));
        warehouse->addToSection(1, new ElectronicProduct(201,499.99,30,"Samsung TV",220,24,"Standard"));
        warehouse->addToSection(1, new FragileElectronics(202,1299.99,15,"DJI Drone",12,12,"45W",8.5,"Hard Case"));
        warehouse->addToSection(2, new ClothingProduct(301,29.99,500,"Nike Shirt","L","Cotton",'U'));
        warehouse->addToSection(2, new ClothingProduct(302,39.99,200,"Zara Dress","M","Polyester",'F'));
    }
    
    ~AdminModule() { delete warehouse; }

    void run(UserRecord* user) {
        int ch;
        do {
            printHeader("ADMIN CONTROL PANEL");
            cout << "  Logged in as: " << user->username 
                 << " (ID: " << user->userId << ")  [ADMIN]\n\n";
            
            cout << "  +---------------------------------------------------+\n"
                 << "  |            INVENTORY MANAGEMENT                  |\n"
                 << "  +---------------------------------------------------+\n"
                 << "  |  1.  Add New Product                             |\n"
                 << "  |  2.  View Full Inventory                         |\n"
                 << "  |  3.  Risk Assessment                             |\n"
                 << "  |  4.  Apply Discount to Product                   |\n"
                 << "  |  5.  Merge Two Sections                          |\n"
                 << "  +---------------------------------------------------+\n"
                 << "  |            SUPPLIER MANAGEMENT                   |\n"
                 << "  +---------------------------------------------------+\n"
                 << "  |  6.  Add Supplier                                |\n"
                 << "  |  7.  Place Restock Order                         |\n"
                 << "  +---------------------------------------------------+\n"
                 << "  |                 REPORTS                           |\n"
                 << "  +---------------------------------------------------+\n"
                 << "  |  8.  View Audit Log                              |\n"
                 << "  +---------------------------------------------------+\n"
                 << "  |          OOP DEMONSTRATIONS                       |\n"
                 << "  +---------------------------------------------------+\n"
                 << "  |  9.  Polymorphism Demo                           |\n"
                 << "  |  10. Deep Inheritance Demo                       |\n"
                 << "  |  11. Templates Demo                              |\n"
                 << "  |  12. Operator Overloading Demo                   |\n"
                 << "  +---------------------------------------------------+\n"
                 << "  |                SETTINGS                           |\n"
                 << "  +---------------------------------------------------+\n"
                 << "  |  13. Change Password                             |\n"
                 << "  |  0.  Logout                                      |\n"
                 << "  +---------------------------------------------------+\n"
                 << "  Choice: ";
            cin >> ch;
            cout << '\n';
            
            switch (ch) {
                case 1:  menuAddProduct();           break;
                case 2:  menuViewInventory();        break;
                case 3:  menuRemoveExpired();        break;
                case 4:  menuApplyDiscount();        break;
                case 5:  menuMergeSections();        break;
                case 6:  menuAddSupplier();          break;
                case 7:  menuRestockOrder();         break;
                case 8:  menuAuditLog();             break;
                case 9:  menuDemoPolymorphism();     break;
                case 10: menuDemoInheritance();      break;
                case 11: menuDemoTemplates();        break;
                case 12: menuDemoOperators();        break;
                case 13: menuChangePassword(user);   break;
                case 0:  cout << "  Logging out...\n\n"; break;
                default: cout << "  Invalid choice.\n";
            }
            if (ch!=0) { 
                cout << "\n  Press Enter to continue..."; 
                cin.ignore(); 
                cin.get(); 
            }
        } while (ch != 0);
    }
};

// ================================================================
//  STUDENT PANEL
// ================================================================
class StudentPanel {
public:
    void run(UserRecord* user) {
        int ch;
        do {
            printHeader("STUDENT LEARNING PORTAL");
            cout << "  Welcome, " << user->username 
                 << " (ID: " << user->userId << ")  [STUDENT]\n\n";
            
            cout << "  +---------------------------------------------------+\n"
                 << "  |            OOP LEARNING MODULES                   |\n"
                 << "  +---------------------------------------------------+\n"
                 << "  |  1.  Inheritance Concept                         |\n"
                 << "  |  2.  Polymorphism Concept                        |\n"
                 << "  |  3.  Encapsulation Concept                       |\n"
                 << "  |  4.  Templates Concept                           |\n"
                 << "  |  5.  View Class Hierarchy                        |\n"
                 << "  |  6.  Browse Product Samples                      |\n"
                 << "  |  0.  Logout                                      |\n"
                 << "  +---------------------------------------------------+\n"
                 << "  Choice: ";
            cin >> ch;
            cout << '\n';
            
            switch (ch) {
                case 1: showInheritance();    break;
                case 2: showPolymorphism();   break;
                case 3: showEncapsulation();  break;
                case 4: showTemplates();      break;
                case 5: showHierarchy();      break;
                case 6: showSampleData();     break;
                case 0: cout << "  Logging out...\n\n"; break;
                default: cout << "  Invalid choice.\n";
            }
            if (ch!=0) { 
                cout << "\n  Press Enter to continue..."; 
                cin.ignore(); 
                cin.get(); 
            }
        } while (ch!=0);
    }
    
private:
    void showHierarchy() const {
        printHeader("COMPLETE CLASS HIERARCHY");
        cout << "  Product [Abstract - L1]\n"
             << "  |-- GroceryProduct [L2]\n"
             << "  |    |-- PerishableGrocery [L3]\n"
             << "  |    |-- NonPerishable [L3]\n"
             << "  |-- ElectronicProduct [L2]\n"
             << "  |    |-- FragileElectronics [L3]\n"
             << "  |-- ClothingProduct [L2]\n\n"
             << "  Supporting Classes:\n"
             << "  |-- InventorySection  (Aggregates Product*)\n"
             << "  |-- Warehouse         (Owns InventorySection[])\n"
             << "  |-- Supplier          (Manages restocking)\n"
             << "  |-- AdminModule       (Admin functionality)\n"
             << "  |-- StudentPanel      (Student learning)\n"
             << "  |-- TransactionLog<T> (Template class)\n";
    }
    
    void showInheritance() const {
        printHeader("CONCEPT: INHERITANCE");
        cout << "  Definition: Inheritance allows a class to inherit\n"
             << "  properties and methods from another class.\n\n"
             << "  Example from this project:\n"
             << "    Product -> GroceryProduct -> PerishableGrocery\n\n"
             << "  PerishableGrocery inherits:\n"
             << "    * From Product: ID, price, quantity, brandName\n"
             << "    * From GroceryProduct: calories, isHalal, nutritionGrade\n"
             << "    * Its own: expiryDays, storageTemp, storageInstructions\n\n"
             << "  Benefits:\n"
             << "    + Code reusability\n"
             << "    + Hierarchical classification\n"
             << "    + Easy maintenance\n\n"
             << "  Code Example:\n"
             << "    class PerishableGrocery : public GroceryProduct {\n"
             << "        // Adds new features while reusing parent code\n"
             << "    };\n";
    }
    
    void showPolymorphism() const {
        printHeader("CONCEPT: POLYMORPHISM");
        cout << "  Definition: Polymorphism enables objects to\n"
             << "  behave differently based on their actual type.\n\n"
             << "  How it works:\n"
             << "    Base class pointer -> Derived class object\n"
             << "    Virtual functions -> Runtime binding\n\n"
             << "  Demonstration:\n";
        
        Product* demo[3];
        demo[0] = new GroceryProduct(1,5.0,100,"Kelloggs",200,true,"A");
        demo[1] = new ElectronicProduct(2,200.0,10,"Samsung",220,12);
        demo[2] = new ClothingProduct(3,30.0,50,"Nike","L","Cotton",'U');
        
        for (int i=0;i<3;i++) {
            cout << "    ";
            demo[i]->displayStatus();
            cout << " -> Risk: " << demo[i]->calculateRisk() << '\n';
            delete demo[i];
        }
        
        cout << "\n  Benefits:\n"
             << "    + Same interface, different implementations\n"
             << "    + Extensible design\n"
             << "    + Runtime flexibility\n";
    }
    
    void showEncapsulation() const {
        printHeader("CONCEPT: ENCAPSULATION");
        cout << "  Definition: Encapsulation bundles data and\n"
             << "  methods together, hiding internal details.\n\n"
             << "  Example from this project:\n";
        
        PerishableGrocery milk(500, 3.49, 40, "FreshMilk", 120, true, "A", 10, 4.0);
        
        cout << "  Public Interface (Getters):\n";
        cout << "    - getID()       = " << milk.getID()       << '\n'
             << "    - getBrand()    = " << milk.getBrand()    << '\n'
             << "    - getPrice()    = $" << milk.getPrice()   << '\n'
             << "    - getQuantity() = " << milk.getQuantity() << '\n'
             << "    - getCalories() = " << milk.getCalories() << '\n';
        
        cout << "\n  Public Interface (Setters):\n";
        milk.setQuantity(75);
        cout << "    - setQuantity(75) -> New quantity: " << milk.getQuantity() << '\n';
        milk.applyDiscount(10.0);
        cout << "    - applyDiscount(10%) -> New price: $" 
             << fixed << setprecision(2) << milk.getPrice() << '\n';
        
        cout << "\n  Benefits:\n"
             << "    + Data protection (private members)\n"
             << "    + Controlled access via methods\n"
             << "    + Easy to maintain and modify\n";
    }
    
    void showTemplates() const {
        printHeader("CONCEPT: TEMPLATES");
        cout << "  Definition: Templates allow generic programming\n"
             << "  where the same code works with different data types.\n\n"
             << "  TransactionLog<T> Class:\n";
        
        cout << "  Works with multiple types:\n";
        TransactionLog<int> intLog(3);
        intLog.record(100,"First int");
        intLog.record(200,"Second int");
        
        cout << "\n  Integer Log:\n";
        intLog.printAll();
        
        cout << "\n  Also works with:\n"
             << "    - TransactionLog<Product>  (Product logs)\n"
             << "    - TransactionLog<Supplier> (Supplier logs)\n"
             << "    - TransactionLog<double>   (Price logs)\n"
             << "    - TransactionLog<string>   (Text logs)\n\n"
             << "  Benefits:\n"
             << "    + Code reusability\n"
             << "    + Type safety\n"
             << "    + No code duplication\n";
    }
    
    void showSampleData() const {
        printHeader("PRODUCT SAMPLES");
        cout << "  Real products in the system:\n\n";
        
        Product* demo[5];
        demo[0] = new PerishableGrocery(101,3.99,60,"Fresh Milk",120,true,"A+",4,4.0);
        demo[1] = new NonPerishable(102,2.49,500,"Rice 5kg",350,true,"B",5,0.1,"Cool Dry");
        demo[2] = new ElectronicProduct(201,499.99,30,"Samsung TV",220,24,"Standard");
        demo[3] = new FragileElectronics(202,1299.0,15,"DJI Drone",12,12,"45W",8.5,"Hard Case");
        demo[4] = new ClothingProduct(301,29.99,200,"Nike Shirt","L","Cotton",'U');
        
        for (int i=0;i<5;i++) {
            cout << "  Product " << i+1 << ":\n";
            cout << "    ";
            demo[i]->displayStatus();
            cout << "\n    Value: $" << fixed << setprecision(2) << demo[i]->calculateValue()
                 << "  Risk: " << demo[i]->calculateRisk() << "\n\n";
            delete demo[i];
        }
    }
};

// ================================================================
//  AUTH SCREEN
// ================================================================
static void handleSignUp() {
    printHeader("CREATE NEW ACCOUNT");
    if (userCount >= MAX_USERS) { 
        cout << "  User limit reached. Cannot create more accounts.\n"; 
        return; 
    }
    
    char uname[32], pwd[32];
    cout << "  Full Name: "; 
    cin.ignore();
    cin.getline(uname, 32);
    
    if (usernameExists(uname)) { 
        cout << "  Username already taken.\n"; 
        return; 
    }
    
    cout << "  Password: "; 
    cin >> pwd;
    
    userDB[userCount].userId = nextUserId++;
    strcpy(userDB[userCount].username, uname);
    strcpy(userDB[userCount].password, pwd);
    userDB[userCount].isAdmin = false;  // Only one admin exists
    
    cout << "\n  [SUCCESS] Account created successfully!\n";
    cout << "  Your User ID: " << userDB[userCount].userId << '\n';
    cout << "  Please remember your ID for login.\n";
    userCount++;
}

static UserRecord* handleSignIn() {
    printHeader("SIGN IN");
    int uid;
    char pwd[32];
    
    cout << "  User ID: "; 
    cin >> uid;
    cout << "  Password: "; 
    cin >> pwd;
    
    UserRecord* u = authenticate(uid, pwd);
    if (!u) { 
        cout << "\n  [ERROR] Invalid credentials. Please try again.\n"; 
        return nullptr; 
    }
    
    cout << "\n  [SUCCESS] Login successful!\n";
    cout << "  Welcome back, " << u->username << "!\n";
    return u;
}

// ================================================================
//  MAIN
// ================================================================
int main() {
    seedDefaultUsers();
    
    // Create the single AdminModule instance (persists across sessions)
    AdminModule adminMod;
    StudentPanel studentPanel;
    
    int choice;
    do {
        printHeader("INVENTORY MANAGEMENT SYSTEM");
        cout << "  +---------------------------------------------------+\n"
             << "  |                 WELCOME TO THE SYSTEM             |\n"
             << "  +---------------------------------------------------+\n"
             << "  |                                                   |\n"
             << "  |  1.  Sign In                                      |\n"
             << "  |  2.  Create New Account                           |\n"
             << "  |  0.  Exit                                         |\n"
             << "  |                                                   |\n"
             << "  +---------------------------------------------------+\n"
             << "  Choice: ";
        cin >> choice;
        cout << '\n';
        
        if (choice == 2) {
            handleSignUp();
        } else if (choice == 1) {
            UserRecord* user = handleSignIn();
            if (user) {
                if (user->isAdmin)
                    adminMod.run(user);
                else
                    studentPanel.run(user);
            }
        } else if (choice != 0) {
            cout << "  Invalid choice.\n";
        }
        
        if (choice != 0 && choice != 1) {
            cout << "\n  Press Enter to continue...";
            cin.ignore();
            cin.get();
        }
    } while (choice != 0);
    
    cout << "\n  +---------------------------------------------------+\n"
         << "  |         THANK YOU FOR USING THE SYSTEM!            |\n"
         << "  |                  GOODBYE!                          |\n"
         << "  +---------------------------------------------------+\n";

         cout<<"Hello from shoaib 1";
    
    return 0;
}
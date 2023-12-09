//Preston Mesecher
//pmmesecher@dmacc.edu

//This program is a basic warehouse setup which allow receipt, ordering, and shipping of product.

#include <iostream>
#include <limits>
#include <vector>

//For clearing console
#ifdef _WIN32
#define CLEAR_CONSOLE "cls"
#else
#define CLEAR_CONSOLE "clear"
#endif

using namespace std;

// Define product structure
struct Product {
    int id;
    int quantity;
    string location;
};

// Define order structure, products and shipping information
struct Order {
    int orderNumber;
    vector<pair<int, int>> products;
    vector<string> shippedFromLocations;
};

// Warehouse class to manage inventory and orders
class Warehouse {
private:
    vector<Product> inventory;
    vector<Order> orders;
    int nextOrderNumber;

public:
    //Initialize the Warehouse
    Warehouse() : nextOrderNumber(1) {}

    // Check if a location is valid (non-blank and not already full)
    bool isLocationValid(const string& location) const {
        if (location.empty()) {
            return false;
        }

        for (const Product& product : inventory) {
            if (product.location == location) {
                return false;
            }
        }

        return true;
    }

    // Add a new product to the inventory
    void addProduct() {
        Product newProduct;
        cout << "Enter product ID: ";
        cin >> newProduct.id;
        cout << "Enter product quantity: ";
        cin >> newProduct.quantity;

        do {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Enter product location: ";

            getline(cin, newProduct.location);

            if (!isLocationValid(newProduct.location)) {
                cout << "Invalid location. Please enter a non-blank and non-repeated location." << endl;
                cin.clear();
            }

        } while (!isLocationValid(newProduct.location));

        inventory.push_back(newProduct);
        cout << "Product added to the inventory. Press enter." << endl;
        cin.get();
    }

    // Check if a location has already been emptied by previous orders
    bool isLocationEmptied(const string& location) const {
        for (const auto& order : orders) {
            for (const auto& shippedLocation : order.shippedFromLocations) {
                if (shippedLocation == location) {
                    return true;
                }
            }
        }
        return false;
    }

    // Create a new order based on available inventory
    void createOrder(int productId, int shippedQuantity) {
        Order newOrder;
        int remainingQuantity = shippedQuantity;

        for (size_t i = 0; i < inventory.size(); ++i) {
            auto& product = inventory[i];
            if (product.id == productId && product.quantity > 0) {
                if (!isLocationEmptied(product.location)) {
                    int quantityToAllocate = min(remainingQuantity, product.quantity);
                    newOrder.products.push_back({productId, quantityToAllocate});
                    newOrder.shippedFromLocations.push_back(product.location);

                    remainingQuantity -= quantityToAllocate;

                    if (remainingQuantity == 0) {
                        break;
                    }
                }
            }
        }

        if (remainingQuantity == 0) {
            newOrder.orderNumber = nextOrderNumber++;
            orders.push_back(newOrder);
            cout << "Order created. Order number: " << newOrder.orderNumber << ". Press enter." << endl;
        } else {
            cout << "Not enough stock or location already emptied by other orders. Order creation failed. Press enter." << endl;
        }

        cin.ignore();
        cin.get();
    }

    // Ship products based on the order number
    void shipProductByOrderNumber(int orderNumber) {
        size_t orderIndex = orders.size();
        for (size_t i = 0; i < orders.size(); ++i) {
            if (orders[i].orderNumber == orderNumber) {
                orderIndex = i;
                break;
            }
        }

        if (orderIndex != orders.size()) {
            for (size_t j = 0; j < orders[orderIndex].products.size(); ++j) {
                int productId = orders[orderIndex].products[j].first;
                int shippedQuantity = orders[orderIndex].products[j].second;
                const vector<string>& shippedFromLocations = orders[orderIndex].shippedFromLocations;

                for (const string& shippedLocation : shippedFromLocations) {
                    for (size_t i = 0; i < inventory.size(); ++i) {
                        auto& product = inventory[i];
                        if (product.id == productId && product.quantity > 0 && product.location == shippedLocation) {
                            int quantityToShip = min(shippedQuantity, product.quantity);

                            if (quantityToShip > 0) {
                                cout << quantityToShip << " units shipped from location " << product.location << ". Press enter." << endl;

                                if (quantityToShip == product.quantity) {
                                    inventory.erase(inventory.begin() + i);
                                } else {
                                    product.quantity -= quantityToShip;
                                }
                            }

                            shippedQuantity -= quantityToShip;

                            if (shippedQuantity == 0) {
                                break;
                            }
                        }
                    }
                }
            }

            orders[orderIndex].products.clear();

            if (orders[orderIndex].products.empty()) {
                orders.erase(orders.begin() + orderIndex);
            }

            cout << "Order number " << orderNumber << " has been shipped. Thank you! Press enter." << endl;
        } else {
            cout << "Order not found. Press enter." << endl;
        }

        cin.ignore();
        cin.get();
    }

    // Check if a product is pending in any order
    bool isProductPending(int productId) const {
        for (const auto &order : orders) {
            for (const auto &product : order.products) {
                if (product.first == productId && product.second > 0) {
                    return true;
                }
            }
        }
        return false;
    }

    // Display the current inventory
    void displayInventory() const {
        cout << "Inventory:" << endl;
        for (size_t i = 0; i < inventory.size(); ++i) {
            const auto &product = inventory[i];
            cout << "ID: " << product.id << ", Quantity: " << product.quantity << ", Location: " << product.location << endl;
        }
        cout << "Press enter to go back." << endl;
        cin.ignore();
        cin.get();
    }

    // Display the list of orders
    void displayOrders() const {
        cout << "Orders:" << endl;
        for (size_t i = 0; i < orders.size(); ++i) {
            const auto &order = orders[i];
            cout << "Order Number: " << order.orderNumber << ", Products: ";

            for (size_t j = 0; j < order.products.size(); ++j) {
                const auto &productInfo = order.products[j];
                cout << "ID: " << productInfo.first << ", Quantity: " << productInfo.second << "; ";
            }

            cout << ", Shipped From Locations: ";

            for (size_t j = 0; j < order.shippedFromLocations.size(); ++j) {
                cout << order.shippedFromLocations[j] << ", ";
            }
            cout << endl;
        }
        cout << "Press enter to go back." << endl;
        cin.ignore();
        cin.get();
    }
};

// Display the main menu options
void displayMainMenu() {
    cout << "Main Menu" << endl;
    cout << "1. Receiving" << endl;
    cout << "2. Shipping" << endl;
    cout << "3. Exit" << endl;
    cout << "Enter your choice: ";
}

// Display the receiving menu options
void displayReceivingMenu() {
    system(CLEAR_CONSOLE);
    cout << "Receiving Menu" << endl;
    cout << "1. Add Product" << endl;
    cout << "2. Display Inventory" << endl;
    cout << "3. Back to Main Menu" << endl;
    cout << "Enter your choice: ";
}

// Display the shipping menu options
void displayShippingMenu() {
    system(CLEAR_CONSOLE);
    cout << "Shipping Menu" << endl;
    cout << "1. Display Orders" << endl;
    cout << "2. Ship Product by Order Number" << endl;
    cout << "3. Create Order" << endl;
    cout << "4. Back to Main Menu" << endl;
    cout << "Enter your choice: ";
}

int main() {
    Warehouse warehouse;

    int mainChoice, receivingChoice, shippingChoice;

    do {
        displayMainMenu();
        cin >> mainChoice;

        switch (mainChoice) {
        case 1:
            do {
                displayReceivingMenu();
                cin >> receivingChoice;

                switch (receivingChoice) {
                case 1:
                    system(CLEAR_CONSOLE);
                    warehouse.addProduct();
                    break;
                case 2:
                    system(CLEAR_CONSOLE);
                    warehouse.displayInventory();
                    break;
                case 3:
                    system(CLEAR_CONSOLE);
                    break;
                default:
                    cout << "Invalid choice. Press enter." << endl;
                    cin.ignore();
                    cin.get();
                }
            } while (receivingChoice != 3);
            break;

        case 2:
            do {
                displayShippingMenu();
                cin >> shippingChoice;

                switch (shippingChoice) {
                case 1:
                    system(CLEAR_CONSOLE);
                    warehouse.displayOrders();
                    break;
                case 2: {
                    system(CLEAR_CONSOLE);
                    int orderNumber;
                    cout << "Enter order number to ship: ";
                    cin >> orderNumber;
                    warehouse.shipProductByOrderNumber(orderNumber);
                    break;
                }
                case 3: {
                    system(CLEAR_CONSOLE);
                    int productId, shippedQuantity;
                    cout << "Enter product ID for the order: ";
                    cin >> productId;
                    cout << "Enter quantity for the order: ";
                    cin >> shippedQuantity;
                    warehouse.createOrder(productId, shippedQuantity);
                    break;
                }
                case 4:
                    system(CLEAR_CONSOLE);
                    break;
                default:
                    cout << "Invalid choice. Press enter." << endl;
                    cin.ignore();
                    cin.get();
                }
            } while (shippingChoice != 4);
            break;

        case 3:
            cout << "Exiting the program. Goodbye!" << endl;
            break;

        default:
            cout << "Invalid choice. Press enter." << endl;
            cin.ignore();
            cin.get();
        }
    } while (mainChoice != 3);

    return 0;
}

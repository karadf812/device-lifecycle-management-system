#include <string>
#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>

class ElectronicDevice {
protected:
    int imei;
    int priceOfPledge;
    std::string manufacturer;
    bool allowedForSale;
    bool sold;

public:
    ElectronicDevice(int imei, int price, const std::string& manufacturer, bool forSale)
    : imei(imei), priceOfPledge(price), manufacturer(manufacturer), allowedForSale(forSale), sold(false) {}

    virtual ~ElectronicDevice() {}

    virtual std::string getDeviceInfo() const {
        return "IMEI: " + std::to_string(imei) +
               "\nManufacturer: " + manufacturer +
               "\nPrice of Pledge: " + std::to_string(priceOfPledge) +
               "\nAllowed for Sale: " + (allowedForSale ? "Yes" : "No") +
               "\nSold: " + (sold ? "Yes" : "No");
    }

    virtual float calculateSellingPrice() const {
        // The base class just returns the price of pledge; 
        // derived classes will override this to implement their own pricing logic.
        return static_cast<float>(priceOfPledge);
    }
    int getImei() const {
        return imei;
    }

    int getPriceOfPledge() const {
        return priceOfPledge;
    }

    bool isSold() const {
        return sold;
    }

    void markAsSold() {
        if (allowedForSale) {
            sold = true;
        } else {
            std::cout << "Device is not allowed for sale." << std::endl;
        }
    }

    // Additional methods to set and get properties can be added here.
};



class MobilePhone : public ElectronicDevice {
private:
    bool supported5GNetwork;

public:
    MobilePhone(int imei, int price, const std::string& manufacturer, bool forSale, bool supports5G)
    : ElectronicDevice(imei, price, manufacturer, forSale), supported5GNetwork(supports5G) {}

    std::string getDeviceInfo() const override {
        return ElectronicDevice::getDeviceInfo() + 
               "\nSupports 5G Network: " + (supported5GNetwork ? "Yes" : "No");
    }

    float calculateSellingPrice() const override {
        // If the phone supports 5G, the selling price is 40% higher than the pledge price.
        // If it does not support 5G, the selling price is 30% higher.
        float additionalPercentage = supported5GNetwork ? 0.40f : 0.30f;
        return priceOfPledge * (1.0f + additionalPercentage);
    }
};

class Tablet : public ElectronicDevice {
private:
    bool allowedPhoneCall;

public:
    Tablet(int imei, int price, const std::string& manufacturer, bool forSale, bool canMakePhoneCall)
    : ElectronicDevice(imei, price, manufacturer, forSale), allowedPhoneCall(canMakePhoneCall) {}

    std::string getDeviceInfo() const override {
        return ElectronicDevice::getDeviceInfo() + 
               "\nCan Make Phone Calls: " + (allowedPhoneCall ? "Yes" : "No");
    }

    float calculateSellingPrice() const override {
        // The selling price of tablets is always 30% higher than the pledge price.
        return priceOfPledge * 1.30f;
    }
};



class PawnShop {
private:
    int account;
    std::vector<std::unique_ptr<ElectronicDevice>> electronicDevices;

public:
    PawnShop() : account(0) {}

    void pledgeDevice(std::unique_ptr<ElectronicDevice> device) {
        // Add the pledged device to the collection and update account balance
        account += device->getPriceOfPledge();
        electronicDevices.push_back(std::move(device));
    }

    bool sellDevice(int imei) {
        // Attempt to sell a device by IMEI number
        auto it = std::find_if(electronicDevices.begin(), electronicDevices.end(),
            [imei](const std::unique_ptr<ElectronicDevice>& device) { return device->getImei() == imei && !device->isSold(); });

        if (it != electronicDevices.end()) {
            account += (*it)->calculateSellingPrice();
            (*it)->markAsSold();
            return true;
        }
        return false;
    }

    bool releaseDevice(int imei) {
        // Remove the device from the collection, if not sold
        auto it = std::remove_if(electronicDevices.begin(), electronicDevices.end(),
            [imei](const std::unique_ptr<ElectronicDevice>& device) { return device->getImei() == imei && !device->isSold(); });

        if (it != electronicDevices.end()) {
            electronicDevices.erase(it, electronicDevices.end());
            return true;
        }
        return false;
    }

    std::string showAvailableDevices() const {
        std::string info;
        for (const auto& device : electronicDevices) {
            if (!device->isSold()) {
                info += device->getDeviceInfo() + "\n";
            }
        }
        return info;
    }

    std::string showSoldDevices() const {
        std::string info;
        for (const auto& device : electronicDevices) {
            if (device->isSold()) {
                info += device->getDeviceInfo() + "\n";
            }
        }
        return info;
    }

    // Getter for account balance for display purposes
    int getAccountBalance() const {
        return account;
    }

    // Additional helper functions can be implemented here.
};

int main() {
    PawnShop myPawnShop;
    int choice;

    do {
        std::cout << "Welcome to the PawnShop Manager" << std::endl;
        std::cout << "1. Add a Mobile Phone" << std::endl;
        std::cout << "2. Add a Tablet" << std::endl;
        std::cout << "3. Sell a Device" << std::endl;
        std::cout << "4. Show Available Devices" << std::endl;
        std::cout << "5. Show Sold Devices" << std::endl;
        std::cout << "6. Exit" << std::endl;
        std::cout << "Enter your choice: ";
        std::cin >> choice;

        int imei, price;
        std::string manufacturer;
        bool forSale, supports5G, canMakePhoneCall;
        if(std::cin.fail()) {
            std::cin.clear(); // Clear the error flags
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Ignore remaining input
            std::cout << "Invalid input, please try again.\n";
            continue; // Skip the rest of the loop and start a new iteration
        }
        
        switch (choice) {
case 1: {
    std::cout << "Enter IMEI: ";
    std::cin >> imei;
    std::cout << "Enter Price: ";
    std::cin >> price;
    std::cin.ignore(); // Ignore leftover newline character
    std::cout << "Enter Manufacturer: ";
    std::getline(std::cin, manufacturer);
    std::cout << "Enter For Sale (1 for Yes, 0 for No): ";
    std::cin >> forSale;
    std::cout << "Enter Supports 5G (1 for Yes, 0 for No): ";
    std::cin >> supports5G;
    std::unique_ptr<ElectronicDevice> phone(new MobilePhone(imei, price, manufacturer, forSale, supports5G));
    myPawnShop.pledgeDevice(std::move(phone));
    std::cout << "Mobile phone added successfully.\n";
    break;
}
            case 2: {
                std::cout << "Enter IMEI: ";
                std::cin >> imei;
                std::cout << "Enter Price: ";
                std::cin >> price;
                std::cin.ignore();
                std::cout << "Enter Manufacturer: ";
                std::getline(std::cin, manufacturer);
                std::cout << "Enter For Sale (1 for Yes, 0 for No): ";
                std::cin >> forSale;
                std::cout << "Enter Can Make Phone Calls (1 for Yes, 0 for No): ";
                std::cin >> canMakePhoneCall;
                std::unique_ptr<ElectronicDevice> tablet(new Tablet(imei, price, manufacturer, forSale, canMakePhoneCall));
                myPawnShop.pledgeDevice(std::move(tablet));
                std::cout << "Tablet added successfully.\n";
                break;
            }
            case 3: {
                std::cout << "Enter IMEI of device to sell: ";
                std::cin >> imei;
                if (myPawnShop.sellDevice(imei)) {
                    std::cout << "Device sold." << std::endl;
                } else {
                    std::cout << "Device could not be sold (not found or already sold)." << std::endl;
                }
                break;
            }
            case 4: {
                 std::cout << "\nAvailable Devices:\n";
                std::cout << myPawnShop.showAvailableDevices() << std::endl;
                break;
            }
            case 5: {
                std::cout << "\nSold Devices:\n";
                std::cout << myPawnShop.showSoldDevices() << std::endl;
                break;
            }
            case 6: {
                std::cout << "Thank you for using PawnShop Manager. Goodbye!" << std::endl;
                break;
            }
            default: {
                std::cout << "Invalid choice. Please try again." << std::endl;
                break;
            }
        }

        // Clear the input buffer at the end of each loop iteration
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    } while (choice != 6);

    return 0;
}


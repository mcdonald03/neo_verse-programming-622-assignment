#ifndef CITY_COMPONENTS_H
#define CITY_COMPONENTS_H

#include <iostream>
#include <string>
#include "CityModels.h"

// =============================================================================
// Section 4 - Object-Oriented City Architecture
//
// CityComponent is the abstract base class.
// PowerSystem, TransportSystem, HealthSystem, SecuritySystem all inherit from
// it and override the pure-virtual processEvent() - demonstrating:
//   * Encapsulation  (private/protected members)
//   * Inheritance    (derived classes extend the base)
//   * Polymorphism   (processEvent called through CityComponent* at runtime)
//   * Constructors & Destructors (virtual destructor for proper cleanup)
// =============================================================================
class CityComponent {
protected:
    int         componentID;
    std::string name;
    bool        isActive;

public:
    CityComponent(int id, std::string n)
        : componentID(id), name(std::move(n)), isActive(false) {}

    // Virtual destructor - ensures derived destructors run via base pointer
    virtual ~CityComponent() {}

    void activate() {
        isActive = true;
        std::cout << "  [INIT] " << name << " [ID:" << componentID << "] activated.\n";
    }

    void deactivate() {
        isActive = false;
        std::cout << "  " << name << " [ID:" << componentID << "] deactivated.\n";
    }

    std::string getStatus() const { return isActive ? "Active" : "Inactive"; }
    std::string getName()   const { return name; }  // Added for report readability

    // Pure virtual - every derived system MUST handle events its own way
    virtual void processEvent(const Event& e) = 0;
};

// -----------------------------------------------------------------------------
class PowerSystem : public CityComponent {
private:
    double powerLevel; // 0–100 %
public:
    PowerSystem(int id, double pl)
        : CityComponent(id, "Power System"), powerLevel(pl) {}

    void processEvent(const Event& e) override {
        if (e.type == "Power failure") {
            powerLevel -= 20.0;
            if (powerLevel < 0) powerLevel = 0;
            std::cout << "    [PowerSystem]  Failure handled! Level -> "
                      << powerLevel << " %\n";
        }
    }
};

// -----------------------------------------------------------------------------
class TransportSystem : public CityComponent {
private:
    int trafficFlow; // vehicles / hour
public:
    TransportSystem(int id, int tf)
        : CityComponent(id, "Transport System"), trafficFlow(tf) {}

    void processEvent(const Event& e) override {
        if (e.type == "Traffic accident") {
            trafficFlow -= 30;
            if (trafficFlow < 0) trafficFlow = 0;
            std::cout << "    [TransportSystem] Rerouting! Traffic flow -> "
                      << trafficFlow << " veh/hr\n";
        }
    }
};

// -----------------------------------------------------------------------------
class HealthSystem : public CityComponent {
private:
    int hospitalCnt;
public:
    HealthSystem(int id, int hc)
        : CityComponent(id, "Health System"), hospitalCnt(hc) {}

    void provideCare() {
        std::cout << "    [HealthSystem]  Deploying ambulances from "
                  << hospitalCnt << " hospital(s).\n";
    }

    void processEvent(const Event& e) override {
        if (e.severity > 7) {
            std::cout << "    [HealthSystem]  High-severity event detected.\n";
            provideCare();
        }
    }
};

// -----------------------------------------------------------------------------
class SecuritySystem : public CityComponent {
private:
    int threatLevel; // 0–10
public:
    SecuritySystem(int id, int tl)
        : CityComponent(id, "Security System"), threatLevel(tl) {}

    void monitorCity() {
        std::cout << "    [SecuritySystem] Threat level: " << threatLevel << "/10\n";
    }

    void processEvent(const Event& e) override {
        if (e.type == "Network overload") {
            threatLevel = std::min(10, threatLevel + 2);
            std::cout << "    [SecuritySystem] Cyber threat! Threat raised to "
                      << threatLevel << "\n";
            monitorCity();
        }
    }
};

#endif // CITY_COMPONENTS_H

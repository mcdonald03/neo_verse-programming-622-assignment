#ifndef CITY_ENGINE_H
#define CITY_ENGINE_H

#include <vector>
#include <list>
#include <queue>
#include <stack>
#include <map>
#include <string>
#include "CityModels.h"
#include "CityComponents.h"

// =============================================================================
// CityEngine - Central controller for the NeoVerse AI City Simulation
//
// Container justifications (Section 2.1):
//   std::vector<Engineer>      - O(1) random access; engineers list is bounded
//   std::vector<SensorReading> - O(1) amortised push_back; supports sort/find
//   std::list<std::string>     - O(1) insertion anywhere; NO reallocation;
//                                ideal for an unbounded, append-only log
//   std::queue<Event>          - FIFO: events handled in arrival order (Section 3.1)
//   std::stack<Event>          - LIFO: most-recent emergency resolved first (Section 3.2)
//   std::map<string,int>       - O(log N) lookup; tracks event-type frequency
//   std::vector<Event>         - full event history; required for std::find (Section 5.1)
// =============================================================================
class CityEngine {
private:
    // --- Containers (Section 2) ---
    std::vector<Engineer>       engineers;
    std::vector<SensorReading>  dailySensors;
    std::list<std::string>      historicalLogs;   // unbounded, O(1) insert
    std::queue<Event>           eventQueue;        // FIFO - Section 3.1
    std::stack<Event>           emergencyOverrides;// LIFO - Section 3.2

    // --- OOP Components (Section 4) ---
    std::vector<CityComponent*> cityComponents;    // polymorphic base pointers

    // --- Analytics State (Section 6) ---
    int                         totalEventsProcessed;
    std::vector<double>         responseTimes;         // ms per processed event
    std::map<std::string, int>  emergencyTypeCounts;   // type -> occurrence count
    std::vector<Event>          allTriggeredEvents;    // full history for std::find

    // Pointer to the currently authenticated engineer (nullptr until login)
    const Engineer*             currentEngineer;

public:
    CityEngine();
    ~CityEngine();

    // 1. Authentication
    void loadEngineers();
    bool login(const std::string& username, const std::string& password);

    // 2. Data Management
    void insertSensorData(int id, const std::string& type, double value);
    void removeOutdatedData(double threshold);
    void displaySensorData()     const;
    void logAction(const std::string& action);
    void displayHistoricalLogs() const;  // uses explicit iterators (Section 6)

    // 3. Event Processing
    void triggerEvent(int id, const std::string& type, int severity, bool isEmergency);
    void processNextEvent();

    // 5. STL Algorithms
    void runAnalytics();
    void searchEvent(int targetID);   // uses std::find (Section 5.1)

    // 6. Reports
    void generateReport();

    // 7. File Handling
    void saveSystemState();
    void loadSystemState();
};

#endif // CITY_ENGINE_H

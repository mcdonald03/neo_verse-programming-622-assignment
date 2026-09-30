#include <iostream>
#include <string>
#include <limits>
#include "CityEngine.h"

// =============================================================================
// Menu display helper
// =============================================================================
void displayMenu() {
    std::cout << "\n+======================================+\n"
              << "|   NeoVerse AI City Control Core      |\n"
              << "+--------------------------------------+\n"
              << "|  [Data Management]                   |\n"
              << "|   1. Add Sensor Data                 |\n"
              << "|   2. Remove Outdated Sensor Data     |\n"
              << "|   3. View Sensor Readings            |\n"
              << "|   4. View Historical Logs            |\n"
              << "+--------------------------------------+\n"
              << "|  [Event Processing]                  |\n"
              << "|   5. Trigger Traffic Accident        |\n"
              << "|   6. Trigger Power Failure [EMERG]   |\n"
              << "|   7. Trigger Network Overload [EMERG]|\n"
              << "|   8. Trigger Weather Alert           |\n"
              << "|   9. Process Next Event              |\n"
              << "+--------------------------------------+\n"
              << "|  [Analytics & Reports]               |\n"
              << "|  10. Run STL Analytics               |\n"
              << "|  11. Search Event by ID              |\n"
              << "|  12. Generate System Report          |\n"
              << "+--------------------------------------+\n"
              << "|  [System]                            |\n"
              << "|  13. Save System State               |\n"
              << "|  14. Load System State               |\n"
              << "|   0. Exit                            |\n"
              << "+======================================+\n"
              << "Select: ";
}

// =============================================================================
// main() — Entry point
// =============================================================================
int main() {
    std::cout << "+==========================================+\n"
              << "|    NEOVERSE AI CITY SURVIVAL ENGINE      |\n"
              << "|        Prototype v2.0  -  Year 2035      |\n"
              << "+==========================================+\n";

    CityEngine engine;

    // -------------------------------------------------------------------------
    // Section 1 — Authentication gate: loop until valid credentials supplied
    // -------------------------------------------------------------------------
    bool loggedIn = false;
    while (!loggedIn) {
        std::string user, pass;
        std::cout << "\nUsername: ";
        std::cin  >> user;
        std::cout << "Password: ";
        std::cin  >> pass;
        loggedIn = engine.login(user, pass);
    }

    // Seed three initial sensor readings for a realistic starting state
    engine.insertSensorData(1, "Energy",     85.5);
    engine.insertSensorData(2, "Traffic",    45.0);
    engine.insertSensorData(3, "Population", 95.2);

    int choice = -1;
    do {
        displayMenu();
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "  Please enter a number.\n";
            continue;
        }

        switch (choice) {

            // ---- Data Management ----------------------------------------
            case 1: {
                int id; std::string type; double val;
                std::cout << "  Sensor ID   : "; std::cin >> id;
                std::cout << "  Type        : "; std::cin >> type;
                std::cout << "  Value       : "; std::cin >> val;
                engine.insertSensorData(id, type, val);
                break;
            }
            case 2: {
                double thresh;
                std::cout << "  Remove readings below threshold: ";
                std::cin  >> thresh;
                engine.removeOutdatedData(thresh);
                break;
            }
            case 3:
                engine.displaySensorData();
                break;
            case 4:
                engine.displayHistoricalLogs();
                break;

            // ---- Event Processing ----------------------------------------
            case 5:
                engine.triggerEvent(10, "Traffic accident",  5, false);
                break;
            case 6:
                engine.triggerEvent(20, "Power failure",    10, true);
                break;
            case 7:
                engine.triggerEvent(30, "Network overload",  8, true);
                break;
            case 8:
                engine.triggerEvent(40, "Weather alert",     3, false);
                break;
            case 9:
                engine.processNextEvent();
                break;

            // ---- Analytics & Reports -------------------------------------
            case 10:
                engine.runAnalytics();
                break;
            case 11: {
                int id;
                std::cout << "  Enter Event ID to search: ";
                std::cin  >> id;
                engine.searchEvent(id);
                break;
            }
            case 12:
                engine.generateReport();
                break;

            // ---- System --------------------------------------------------
            case 13:
                engine.saveSystemState();
                break;
            case 14:
                engine.loadSystemState();
                break;
            case 0:
                engine.saveSystemState();
                std::cout << "\n  NeoVerse Control Core shutting down. Goodbye.\n";
                break;
            default:
                std::cout << "  Invalid option. Please try again.\n";
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    } while (choice != 0);

    return 0;
}

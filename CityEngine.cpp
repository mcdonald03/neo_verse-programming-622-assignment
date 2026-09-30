#include "CityEngine.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>   // sort, find, find_if, binary_search, remove_if,
                       // min_element, max_element, count_if
#include <iomanip>
#include <numeric>     // std::accumulate
#include <limits>

// =============================================================================
// Constructor / Destructor
// =============================================================================
CityEngine::CityEngine() : totalEventsProcessed(0), currentEngineer(nullptr) {
    std::cout << "\n[INIT] Booting NeoVerse AI City Engine...\n";

    // Instantiate all city subsystems (Section 4 — OOP)
    cityComponents.push_back(new PowerSystem(101, 100.0));
    cityComponents.push_back(new TransportSystem(102, 500));
    cityComponents.push_back(new HealthSystem(103, 5));
    cityComponents.push_back(new SecuritySystem(104, 1));
    for (auto* comp : cityComponents) comp->activate();

    loadEngineers();    // Section 1 — load (or seed) engineer accounts
    loadSystemState();  // Section 7 — restore previous session if files exist
}

CityEngine::~CityEngine() {
    // Delete dynamically allocated city components (prevent memory leak)
    for (auto* comp : cityComponents) delete comp;
}

// =============================================================================
// Section 1 — Authentication
// =============================================================================

void CityEngine::loadEngineers() {
    // --- Try loading from engineers.dat first (Section 7 — persistence) ---
    std::ifstream inFile("engineers.dat");
    if (inFile.is_open()) {
        std::string line;
        while (std::getline(inFile, line)) {
            if (line.empty() || line[0] == '#') continue; // skip comments
            std::istringstream ss(line);
            Engineer eng;
            std::getline(ss, eng.engineerID,    ',');
            std::getline(ss, eng.username,       ',');
            std::getline(ss, eng.password,       ','); // already encrypted on disk
            std::getline(ss, eng.clearanceLevel, ',');
            if (!eng.engineerID.empty()) engineers.push_back(eng);
        }
        inFile.close();
        std::cout << "[INIT] Loaded " << engineers.size()
                  << " engineer(s) from engineers.dat\n";
    }

    // --- Fall back to hardcoded defaults if no file exists (first run) ---
    if (engineers.empty()) {
        std::cout << "[INIT] engineers.dat not found — seeding defaults.\n";
        // Passwords are ROT-13 encrypted before storage
        engineers.push_back({"ENG001", "admin",  rot13Encrypt("admin123"), "High"});
        engineers.push_back({"ENG002", "tech",   rot13Encrypt("tech123"),  "Medium"});
        engineers.push_back({"ENG003", "viewer", rot13Encrypt("view123"),  "Low"});
    }

    // Sort by engineerID — O(N log N)
    // Required so std::binary_search can be demonstrated in login() — O(log N)
    std::sort(engineers.begin(), engineers.end());
    std::cout << "[INIT] " << engineers.size() << " engineer(s) registered.\n";
}

bool CityEngine::login(const std::string& username, const std::string& password) {
    std::cout << "\n[AUTH] Attempting login for: " << username << "\n";

    // -------------------------------------------------------------------------
    // Method 1: std::find_if  — Linear Search  O(N)
    //   Searches every engineer record until credentials match.
    //   Practical when N (engineer count) is small.
    //   The input password is encrypted before comparison.
    // -------------------------------------------------------------------------
    std::string encPass = rot13Encrypt(password);
    auto it = std::find_if(engineers.begin(), engineers.end(),
        [&](const Engineer& e) {
            return e.username == username && e.password == encPass;
        });

    if (it == engineers.end()) {
        std::cout << "[AUTH] Access Denied - invalid credentials.\n";
        return false;
    }

    // -------------------------------------------------------------------------
    // Method 2: std::binary_search  — O(log N) existence check
    //   Engineers are pre-sorted by engineerID (done in loadEngineers).
    //   binary_search confirms the ID exists in O(log N) vs O(N) for find_if.
    //   LIMITATION: binary_search only returns bool — cannot retrieve the object.
    //   For full lookup, use std::lower_bound instead (also O(log N)).
    //   VERDICT: linear search is preferred here because N is small AND we need
    //            to match on username+password, not just ID.
    // -------------------------------------------------------------------------
    Engineer demoKey;
    demoKey.engineerID = it->engineerID;
    bool bsResult = std::binary_search(engineers.begin(), engineers.end(), demoKey);
    std::cout << "[AUTH] Binary search demo (ID=" << it->engineerID << "): "
              << (bsResult ? "confirmed in O(log N)." : "not found.") << "\n";

    currentEngineer = &(*it);
    std::cout << "\n  *** Access Granted ***\n"
              << "  Welcome, " << it->engineerID
              << " | Clearance: " << it->clearanceLevel << "\n";
    logAction("Login: " + it->engineerID + " [" + it->clearanceLevel + "]");
    return true;
}

// =============================================================================
// Section 2 — City Data Management
// =============================================================================

// Why vector? O(1) amortised push_back, O(1) random access by index,
//             excellent cache locality. Ideal for a fixed daily reading set.
void CityEngine::insertSensorData(int id, const std::string& type, double value) {
    dailySensors.push_back({id, type, value}); // O(1) amortised
    logAction("Sensor inserted: [" + type + "] = " + std::to_string(value));
    std::cout << "  Sensor added - ID:" << id << " | " << type
              << " | Value: " << value << "\n";
}

// Erase-remove idiom: O(N) scan + O(N) shift = O(N) total
void CityEngine::removeOutdatedData(double threshold) {
    size_t before = dailySensors.size();
    dailySensors.erase(
        std::remove_if(dailySensors.begin(), dailySensors.end(),
            [threshold](const SensorReading& s) { return s.value < threshold; }),
        dailySensors.end()
    );
    size_t removed = before - dailySensors.size();
    std::cout << "  Removed " << removed << " reading(s) below " << threshold << ".\n";
    logAction("Removed " + std::to_string(removed) +
              " outdated sensor(s) (threshold=" + std::to_string(threshold) + ")");
}

void CityEngine::displaySensorData() const {
    std::cout << "\n--- Daily Sensor Readings ("
              << dailySensors.size() << " entries) ---\n";
    if (dailySensors.empty()) {
        std::cout << "  No sensor data available.\n";
        return;
    }
    std::cout << std::left
              << std::setw(6)  << "ID"
              << std::setw(16) << "Type"
              << std::setw(10) << "Value" << "\n"
              << std::string(32, '-') << "\n";
    for (const auto& s : dailySensors) {
        std::cout << std::setw(6)  << s.sensorID
                  << std::setw(16) << s.type
                  << std::setw(10) << s.value << "\n";
    }
}

// Why list? O(1) push_back with NO reallocation ever — critical for an
//           append-only log that grows without bound (no upper limit).
void CityEngine::logAction(const std::string& action) {
    historicalLogs.push_back(action); // std::list::push_back — O(1)
}

// Section 6 requirement: explicit iterator syntax (not range-for)
void CityEngine::displayHistoricalLogs() const {
    std::cout << "\n--- Historical City Logs ("
              << historicalLogs.size() << " entries) ---\n";
    if (historicalLogs.empty()) {
        std::cout << "  No logs yet.\n";
        return;
    }
    int index = 1;
    // Explicit iterator traversal — demonstrates iterator usage for Section 6
    for (std::list<std::string>::const_iterator it = historicalLogs.begin();
         it != historicalLogs.end(); ++it) {
        std::cout << "  [" << std::setw(3) << index++ << "] " << *it << "\n";
    }
}

// =============================================================================
// Section 3 — Event Processing
// =============================================================================

// Why queue? FIFO ensures events are handled in the EXACT order they arrived —
//            fair and chronologically correct. push/pop both O(1).
// Why stack? LIFO ensures the MOST RECENT emergency is always resolved first —
//            critical for cascading crises where the latest failure is most urgent.
//            push/pop both O(1).
void CityEngine::triggerEvent(int id, const std::string& type,
                              int severity, bool isEmergency) {
    Event e = {id, type, severity, isEmergency, nowMs()};
    allTriggeredEvents.push_back(e); // maintain full history for std::find

    if (isEmergency) {
        emergencyOverrides.push(e); // LIFO stack — O(1)
        std::cout << "  [!!!] EMERGENCY pushed to override stack: "
                  << type << " (Severity " << severity << ")\n";
        logAction("EMERGENCY queued: " + type +
                  " | Severity: " + std::to_string(severity));
    } else {
        eventQueue.push(e); // FIFO queue — O(1)
        std::cout << "  [ ! ] Event queued: "
                  << type << " (Severity " << severity << ")\n";
        logAction("Event queued: " + type +
                  " | Severity: " + std::to_string(severity));
    }
}

void CityEngine::processNextEvent() {
    Event current = {0, "", 0, false, 0};
    bool hasEvent = false;

    // Emergency stack (LIFO) always takes priority over the normal FIFO queue
    if (!emergencyOverrides.empty()) {
        current  = emergencyOverrides.top();
        emergencyOverrides.pop();
        hasEvent = true;
        std::cout << "\n>>> PROCESSING EMERGENCY OVERRIDE (LIFO) <<<\n";
    } else if (!eventQueue.empty()) {
        current  = eventQueue.front();
        eventQueue.pop();
        hasEvent = true;
        std::cout << "\n>>> PROCESSING NORMAL EVENT (FIFO) <<<\n";
    }

    if (!hasEvent) {
        std::cout << "\n  No pending events in the city.\n";
        return;
    }

    // Compute response time (ms from enqueue to now)
    double responseMs = static_cast<double>(nowMs() - current.enqueuedAt);
    responseTimes.push_back(responseMs);

    // Track event-type frequency for the "most common" report (Section 6)
    emergencyTypeCounts[current.type]++;

    std::cout << "  Type     : " << current.type << "\n"
              << "  Severity : " << current.severity << "\n"
              << "  Response : " << std::fixed << std::setprecision(1)
              << responseMs << " ms\n";

    // Section 4 — Polymorphism: each component's processEvent() is called
    // through the base pointer; runtime dispatch selects the correct override.
    for (auto* comp : cityComponents) comp->processEvent(current);

    totalEventsProcessed++;
    logAction("Processed: " + current.type +
              " | Response: " + std::to_string(responseMs) + " ms");
}

// =============================================================================
// Section 5 — STL Algorithms & Performance Optimisation
// =============================================================================

void CityEngine::runAnalytics() {
    std::cout << "\n--- STL Analytics ---\n";

    if (dailySensors.empty()) {
        std::cout << "  No sensor data to analyse.\n";
    } else {
        // 1. std::sort  — O(N log N)  [introsort in practice]
        //    Sorts sensor readings ascending by value (uses SensorReading::operator<).
        //    Chosen because we need the full sorted order for display.
        std::sort(dailySensors.begin(), dailySensors.end());
        std::cout << "[sort]         Sensors sorted by value — O(N log N)\n";

        // 2. std::min_element  — O(N)
        //    Single linear scan to find the smallest element. O(N) is optimal
        //    for an unsorted (or just-sorted) range without extra data structures.
        auto minIt = std::min_element(dailySensors.begin(), dailySensors.end());
        std::cout << "[min_element]  Lowest  : " << minIt->type
                  << " = " << minIt->value << "  — O(N)\n";

        // 3. std::max_element  — O(N)
        auto maxIt = std::max_element(dailySensors.begin(), dailySensors.end());
        std::cout << "[max_element]  Highest : " << maxIt->type
                  << " = " << maxIt->value << "  — O(N)\n";

        // 4. std::count_if  — O(N)
        //    Counts sensors whose value exceeds the critical threshold (90.0).
        //    Chosen over a manual loop for conciseness and STL idiom compliance.
        int critical = std::count_if(dailySensors.begin(), dailySensors.end(),
            [](const SensorReading& s) { return s.value > 90.0; });
        std::cout << "[count_if]     Critical (>90): " << critical << "  — O(N)\n";
    }

    // 5. std::find  — O(N)
    //    Searches for a specific event in the full triggered-events history.
    //    Unlike find_if (predicate-based), std::find matches by operator==.
    //    Chosen here to satisfy the Section 5.1 requirement explicitly.
    if (!allTriggeredEvents.empty()) {
        // Build a vector of IDs and search it with std::find
        std::vector<int> ids;
        ids.reserve(allTriggeredEvents.size());
        for (const auto& e : allTriggeredEvents) ids.push_back(e.eventID);

        int searchID = allTriggeredEvents.front().eventID;
        auto foundIt = std::find(ids.begin(), ids.end(), searchID);
        std::cout << "[find]         Search event ID " << searchID << ": "
                  << (foundIt != ids.end() ? "Found" : "Not found")
                  << "  — O(N)\n";
    }

    logAction("runAnalytics() executed.");
}

// Section 5.1 — std::find: user-driven event search
void CityEngine::searchEvent(int targetID) {
    std::cout << "\n--- Searching for Event ID: " << targetID << " ---\n";

    if (allTriggeredEvents.empty()) {
        std::cout << "  No events have been triggered yet.\n";
        return;
    }

    // Build a parallel vector of just the IDs for a clean std::find call.
    // std::find matches using operator== (defined on int).  Time: O(N).
    std::vector<int> ids;
    ids.reserve(allTriggeredEvents.size());
    for (const auto& e : allTriggeredEvents) ids.push_back(e.eventID);

    // std::find — O(N) linear search
    auto foundIt = std::find(ids.begin(), ids.end(), targetID);
    if (foundIt != ids.end()) {
        size_t idx    = static_cast<size_t>(std::distance(ids.begin(), foundIt));
        const Event& e = allTriggeredEvents[idx];
        std::cout << "  [FOUND] ID:" << e.eventID
                  << " | Type: "      << e.type
                  << " | Severity: "  << e.severity
                  << " | Emergency: " << (e.isEmergency ? "YES" : "NO") << "\n";
    } else {
        std::cout << "  Event ID " << targetID
                  << " not found in triggered events.\n";
    }
}

// =============================================================================
// Section 6 — System Reports & Analytics
// =============================================================================

void CityEngine::generateReport() {
    std::cout << "\n======== NeoVerse City System Report ========\n";

    // General stats
    std::cout << std::left
              << std::setw(28) << "Total Events Processed"  << ": "
              << totalEventsProcessed << "\n"
              << std::setw(28) << "Pending Normal Events"    << ": "
              << eventQueue.size()    << "\n"
              << std::setw(28) << "Pending Emergencies"      << ": "
              << emergencyOverrides.size() << "\n"
              << std::setw(28) << "Total Sensor Readings"   << ": "
              << dailySensors.size() << "\n"
              << std::setw(28) << "Total Historical Logs"   << ": "
              << historicalLogs.size() << "\n";

    // Average response time — std::accumulate O(N)
    if (!responseTimes.empty()) {
        double total = std::accumulate(responseTimes.begin(), responseTimes.end(), 0.0);
        double avg   = total / static_cast<double>(responseTimes.size());
        std::cout << std::setw(28) << "Avg Response Time"
                  << ": " << std::fixed << std::setprecision(2) << avg << " ms\n";
    } else {
        std::cout << std::setw(28) << "Avg Response Time"
                  << ": N/A (no events processed yet)\n";
    }

    // Most common event type — iterator-based map traversal (Section 6)
    if (!emergencyTypeCounts.empty()) {
        // Explicit iterator over std::map to find the max-frequency entry
        std::map<std::string,int>::iterator mostCommon = emergencyTypeCounts.begin();
        for (std::map<std::string,int>::iterator it = emergencyTypeCounts.begin();
             it != emergencyTypeCounts.end(); ++it) {
            if (it->second > mostCommon->second) mostCommon = it;
        }
        std::cout << std::setw(28) << "Most Common Event Type"
                  << ": " << mostCommon->first
                  << " (" << mostCommon->second << " occurrence(s))\n";

        // Full event-frequency breakdown
        std::cout << "\n  Event Type Frequency:\n";
        for (std::map<std::string,int>::const_iterator it = emergencyTypeCounts.begin();
             it != emergencyTypeCounts.end(); ++it) {
            std::cout << "    " << std::setw(24) << std::left << it->first
                      << ": " << it->second << "\n";
        }
    } else {
        std::cout << std::setw(28) << "Most Common Event Type"
                  << ": N/A\n";
    }

    // City component status
    std::cout << "\n  City Component Status:\n";
    for (const auto* comp : cityComponents) {
        std::cout << "    " << std::setw(22) << comp->getName()
                  << " — " << comp->getStatus() << "\n";
    }

    std::cout << "=============================================\n";
    logAction("System report generated.");
}

// =============================================================================
// Section 7 — File Handling & Persistence
// =============================================================================

void CityEngine::saveSystemState() {
    std::cout << "\n[SAVE] Persisting system state to disk...\n";

    // --- 1. engineers.dat ---
    {
        std::ofstream f("engineers.dat");
        if (f.is_open()) {
            f << "# NeoVerse Engineer Registry (passwords are ROT-13 encrypted)\n";
            for (const auto& e : engineers) {
                f << e.engineerID << "," << e.username << ","
                  << e.password   << "," << e.clearanceLevel << "\n";
            }
            std::cout << "  [OK] engineers.dat saved.\n";
        } else {
            std::cout << "  [ERR] Could not write engineers.dat\n";
        }
    }

    // --- 2. events.dat ---
    {
        std::ofstream f("events.dat");
        if (f.is_open()) {
            f << "# NeoVerse Triggered Event Log\n";
            f << "eventID,type,severity,isEmergency\n";
            for (const auto& e : allTriggeredEvents) {
                f << e.eventID << ","
                  << e.type    << ","
                  << e.severity << ","
                  << (e.isEmergency ? 1 : 0) << "\n";
            }
            std::cout << "  [OK] events.dat saved ("
                      << allTriggeredEvents.size() << " events).\n";
        } else {
            std::cout << "  [ERR] Could not write events.dat\n";
        }
    }

    // --- 3. city_logs.dat --- (binary-safe text log)
    {
        std::ofstream f("city_logs.dat");
        if (f.is_open()) {
            f << "# NeoVerse Historical Logs\n";
            // Explicit iterator used for writing the std::list (Section 6)
            for (std::list<std::string>::const_iterator it = historicalLogs.begin();
                 it != historicalLogs.end(); ++it) {
                f << *it << "\n";
            }
            std::cout << "  [OK] city_logs.dat saved ("
                      << historicalLogs.size() << " entries).\n";
        } else {
            std::cout << "  [ERR] Could not write city_logs.dat\n";
        }
    }

    // --- 4. city_logs.csv --- (human-readable export)
    {
        std::ofstream f("city_logs.csv");
        if (f.is_open()) {
            f << "Index,Log Entry\n";
            int i = 1;
            for (std::list<std::string>::const_iterator it = historicalLogs.begin();
                 it != historicalLogs.end(); ++it) {
                f << i++ << ",\"" << *it << "\"\n";
            }
            std::cout << "  [OK] city_logs.csv exported.\n";
        }
    }

    // --- 5. config.txt ---
    {
        std::ofstream f("config.txt");
        if (f.is_open()) {
            f << "# NeoVerse System Configuration\n";
            f << "totalEventsProcessed=" << totalEventsProcessed << "\n";
            f << "engineerCount="        << engineers.size()     << "\n";
            f << "sensorCount="          << dailySensors.size()  << "\n";
            std::cout << "  [OK] config.txt saved.\n";
        } else {
            std::cout << "  [ERR] Could not write config.txt\n";
        }
    }

    logAction("System state saved to disk (5 files).");
}

void CityEngine::loadSystemState() {
    std::cout << "[INIT] Loading previous session data...\n";

    // --- config.txt ---
    {
        std::ifstream f("config.txt");
        if (f.is_open()) {
            std::string line;
            while (std::getline(f, line)) {
                if (line.empty() || line[0] == '#') continue;
                auto pos = line.find('=');
                if (pos == std::string::npos) continue;
                std::string key = line.substr(0, pos);
                std::string val = line.substr(pos + 1);
                if (key == "totalEventsProcessed")
                    totalEventsProcessed = std::stoi(val);
            }
            std::cout << "[INIT] config.txt restored.\n";
        }
    }

    // --- city_logs.dat ---
    {
        std::ifstream f("city_logs.dat");
        if (f.is_open() && historicalLogs.empty()) {
            std::string line;
            int count = 0;
            while (std::getline(f, line)) {
                if (line.empty() || line[0] == '#') continue;
                historicalLogs.push_back(line);
                count++;
            }
            std::cout << "[INIT] city_logs.dat restored (" << count << " entries).\n";
        }
    }

    // --- events.dat ---
    {
        std::ifstream f("events.dat");
        if (f.is_open() && allTriggeredEvents.empty()) {
            std::string line;
            while (std::getline(f, line)) {
                if (line.empty() || line[0] == '#') continue;
                if (line.substr(0, 7) == "eventID") continue; // skip CSV header
                std::istringstream ss(line);
                std::string field;
                Event e = {0, "", 0, false, 0};
                try {
                    std::getline(ss, field, ','); e.eventID   = std::stoi(field);
                    std::getline(ss, e.type,  ',');
                    std::getline(ss, field,   ','); e.severity = std::stoi(field);
                    std::getline(ss, field,   ','); e.isEmergency = (field == "1");
                    allTriggeredEvents.push_back(e);
                    emergencyTypeCounts[e.type]++;
                } catch (...) {
                    // Skip malformed lines silently
                }
            }
            std::cout << "[INIT] events.dat restored ("
                      << allTriggeredEvents.size() << " events).\n";
        }
    }
}

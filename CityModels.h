#ifndef CITY_MODELS_H
#define CITY_MODELS_H

#include <string>
#include <chrono>   // for nowMs() response-time helper
#include <cctype>

// =============================================================================
// Section 1.1 — Password Encryption Utility
// Uses ROT-13 (Caesar cipher, shift=13).  ROT-13 is its own inverse, so the
// same function both encrypts AND decrypts.  All output chars are printable,
// which is important for safe file storage.
// In production, use a proper one-way hash (bcrypt / Argon2).
// =============================================================================
inline std::string rot13Encrypt(const std::string& text) {
    std::string result = text;
    for (char& c : result) {
        if      (c >= 'a' && c <= 'z') c = 'a' + (c - 'a' + 13) % 26;
        else if (c >= 'A' && c <= 'Z') c = 'A' + (c - 'A' + 13) % 26;
        // Digits and symbols are left unchanged so strings stay printable.
    }
    return result;
}

// Helper: milliseconds since process start (steady_clock) — used for
// response-time tracking in Section 6.
inline long long nowMs() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()
    ).count();
}

// =============================================================================
// 1. AI Engineer Model  (Section 1.1)
// =============================================================================
struct Engineer {
    std::string engineerID;     // "ENG001", "ENG002", …
    std::string username;
    std::string password;       // Stored ROT-13 encrypted
    std::string clearanceLevel; // "Low" | "Medium" | "High"

    // Operators required by std::sort and std::binary_search (sorted by ID)
    bool operator<(const Engineer& o) const { return engineerID < o.engineerID; }
    bool operator==(const Engineer& o) const { return engineerID == o.engineerID; }
};

// =============================================================================
// 2. Sensor Reading Model  (Section 2.1)
// =============================================================================
struct SensorReading {
    int         sensorID;
    std::string type;   // "Energy" | "Traffic" | "Population" | "Water"
    double      value;

    // Enables std::sort by value in runAnalytics()
    bool operator<(const SensorReading& o) const { return value < o.value; }
};

// =============================================================================
// 3. Event Model  (Section 3)
// =============================================================================
struct Event {
    int         eventID;
    std::string type;        // "Traffic accident" | "Power failure" | etc.
    int         severity;    // 1-10
    bool        isEmergency; // true -> pushed to LIFO stack
    long long   enqueuedAt;  // steady_clock ms snapshot - used to compute
                             // response time (Section 6)

    // Enables std::find by eventID in searchEvent() (Section 5.1)
    bool operator==(const Event& o) const { return eventID == o.eventID; }
};

#endif // CITY_MODELS_H

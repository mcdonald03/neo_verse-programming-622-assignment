
  NEOVERSE - AI CITY SURVIVAL ENGINE
  Console-Based C++  | Programming Assignment
================================================================================

OVERVIEW
--------
By the year 2035, urban environments are governed by AI Control Cores that
autonomously manage energy, transport, digital citizens, and emergency response.

NeoVerse is a prototype console-based C++ simulation used to train engineers
before they work on real smart cities. It demonstrates:

  - Dynamic STL data structures (vector, list, queue, stack, map)
  - STL algorithms (sort, find, find_if, binary_search, min_element,
    max_element, count_if, accumulate)
  - Object-Oriented Programming (encapsulation, inheritance, polymorphism)
  - File I/O and session persistence
  - Big-O complexity analysis


--------------------------------------------------------------------------------
PROJECT FILES- FOUND IN FOLDER "Project files"
--------------------------------------------------------------------------------

  main.cpp            - Entry point: login gate + menu loop
  CityEngine.h        - CityEngine class declaration
  CityEngine.cpp      - Full engine implementation (all 7 sections)
  CityComponents.h    - OOP class hierarchy (Section 4)
  CityModels.h        - Data models + ROT-13 encryption + time utilities
  BigO_Analysis.md    - Algorithm complexity justifications
  README.txt          - This file

  The following files are AUTO-GENERATED at runtime (not required to run):
  engineers.dat       - Encrypted engineer registry
  events.dat          - Triggered event log
  city_logs.dat       - Historical action log
  city_logs.csv       - Human-readable CSV log export
  config.txt          - System configuration


--------------------------------------------------------------------------------
HOW TO COMPILE
--------------------------------------------------------------------------------

IMPORTANT: You must compile BOTH main.cpp AND CityEngine.cpp together.
           Compiling only main.cpp will cause linker errors.

Standard library only - no external dependencies needed.

--------------------------------------------------------------------------------
EASIEST METHOD - compile.bat (Windows, just double-click)
--------------------------------------------------------------------------------
  1. Double-click compile.bat in the project folder
  2. It automatically finds your compiler and builds the project
  3. If successful, it launches the program immediately
  4. If no compiler is found, it shows you download instructions

--------------------------------------------------------------------------------
METHOD 1 - Command Prompt / Terminal (G++ / MinGW)
--------------------------------------------------------------------------------
  Navigate to the project folder, then run:

    g++ -std=c++17 -Wall -o NeoVerseEngine.exe main.cpp CityEngine.cpp
    NeoVerseEngine.exe

  If C++17 fails on your machine, try:
    g++ -std=c++14 -Wall -o NeoVerseEngine.exe main.cpp CityEngine.cpp
    g++ -std=c++11 -Wall -o NeoVerseEngine.exe main.cpp CityEngine.cpp

--------------------------------------------------------------------------------
METHOD 2 - Dev-C++ IDE 
--------------------------------------------------------------------------------
  1. Open Dev-C++
  2. File > New > Project > Console Application > C++ > OK
  3. Save project inside the neo_verse folder
  4. Delete the auto-generated main.cpp Dev-C++ creates
  5. Project > Add to Project: add main.cpp and CityEngine.cpp
  6. Tools > Compiler Options > General tab
     Add flag: -std=c++11
  7. Press F11 to Compile & Run

--------------------------------------------------------------------------------
METHOD 3 - Code::Blocks IDE
--------------------------------------------------------------------------------
  1. File > New > Project > Console Application > C++ > Next
  2. Save inside the neo_verse folder
  3. Delete the default main.cpp Code::Blocks creates
  4. Right-click project > Add Files: add main.cpp and CityEngine.cpp
  5. Project > Build Options > Compiler Settings > Other Options
     Add: -std=c++11
  6. Press F9 to Build and Run

--------------------------------------------------------------------------------
METHOD 4 - Visual Studio (MSVC)
--------------------------------------------------------------------------------
  1. File > New Project > Empty Project (C++)
  2. Right-click Source Files > Add > Existing Item
     Add: main.cpp and CityEngine.cpp
  3. Right-click Header Files > Add > Existing Item
     Add: CityEngine.h, CityComponents.h, CityModels.h
  4. Right-click project > Properties
     C/C++ > Language > C++ Language Standard: set to C++14 or C++17
  5. Ctrl+F5 to build and run

--------------------------------------------------------------------------------
METHOD 5 - Just run the pre-compiled .exe (Windows only)
--------------------------------------------------------------------------------
  If you only need to run it without compiling:
    Double-click NeoVerseEngine.exe
  The program launches directly in a console window.


--------------------------------------------------------------------------------
LOGIN CREDENTIALS
--------------------------------------------------------------------------------

  Username  | Password   | Clearance
  ----------|------------|----------
  admin     | admin123   | High
  tech      | tech123    | Medium
  viewer    | view123    | Low

  NOTE: Passwords are stored ROT-13 encrypted in engineers.dat


--------------------------------------------------------------------------------
MENU OPTIONS
--------------------------------------------------------------------------------

  [Data Management]
   1. Add Sensor Data
   2. Remove Outdated Sensor Data
   3. View Sensor Readings
   4. View Historical Logs

  [Event Processing]
   5. Trigger Traffic Accident       (Normal Event  | Severity 5 | FIFO)
   6. Trigger Power Failure          (EMERGENCY     | Severity 10| LIFO)
   7. Trigger Network Overload       (EMERGENCY     | Severity 8 | LIFO)
   8. Trigger Weather Alert          (Normal Event  | Severity 3 | FIFO)
   9. Process Next Event             (Emergency stack first, then queue)

  [Analytics & Reports]
  10. Run STL Analytics
  11. Search Event by ID
  12. Generate System Report

  [System]
  13. Save System State
  14. Load System State
   0. Exit (auto-saves before closing)


--------------------------------------------------------------------------------
SENSOR DATA INPUT GUIDE
--------------------------------------------------------------------------------

When adding sensor data (Option 1), enter:

  Sensor ID  : Any unique integer not already in use (e.g. 5, 6, 7)
  Type       : Energy / Traffic / Population / Water
  Value      : Decimal number between 0.0 and 100.0

Pre-loaded on startup (no need to re-add):
  ID  | Type        | Value
  ----|-------------|-------
  1   | Energy      | 85.5
  2   | Traffic     | 45.0
  3   | Population  | 95.2


--------------------------------------------------------------------------------
EVENT SYSTEM
--------------------------------------------------------------------------------

  Option | Event Type        | Event ID | Severity | Queue Type
  -------|-------------------|----------|----------|-----------
  5      | Traffic accident  | 10       | 5        | Normal FIFO queue
  6      | Power failure     | 20       | 10       | Emergency LIFO stack
  7      | Network overload  | 30       | 8        | Emergency LIFO stack
  8      | Weather alert     | 40       | 3        | Normal FIFO queue

  Emergency events (options 6 & 7) are ALWAYS processed before normal events.
  Within emergencies, the MOST RECENTLY triggered one is resolved first (LIFO).


--------------------------------------------------------------------------------
STL ALGORITHMS USED
--------------------------------------------------------------------------------

  Algorithm            | Used For                        | Complexity
  ---------------------|--------------------------------|-----------
  std::sort            | Sort sensor readings by value   | O(N log N)
  std::find            | Search event by ID              | O(N)
  std::find_if         | Login credential search         | O(N)
  std::binary_search   | Engineer ID existence demo      | O(log N)
  std::min_element     | Lowest sensor reading           | O(N)
  std::max_element     | Highest sensor reading          | O(N)
  std::count_if        | Count critical alerts (>90)     | O(N)
  std::remove_if       | Remove outdated sensor data     | O(N)
  std::accumulate      | Average event response time     | O(N)


--------------------------------------------------------------------------------
OOP CLASS HIERARCHY  (Section 4)
--------------------------------------------------------------------------------

  CityComponent  <-- abstract base class
  |  - componentID : int
  |  - name        : string
  |  - isActive    : bool
  |  + activate()
  |  + deactivate()
  |  + getStatus()  : string
  |  + processEvent(Event&) = 0   <-- pure virtual (polymorphism)
  |
  +-- PowerSystem       (powerLevel  : double)
  +-- TransportSystem   (trafficFlow : int)
  +-- HealthSystem      (hospitalCnt : int) + provideCare()
  +-- SecuritySystem    (threatLevel : int) + monitorCity()


--------------------------------------------------------------------------------
PERSISTENT FILES EXPLAINED
--------------------------------------------------------------------------------

  engineers.dat  - Saved engineer accounts with encrypted passwords.
                   Loaded on startup automatically.

  events.dat     - CSV log of every event that was triggered this session.
                   Restored on next startup so search history persists.

  city_logs.dat  - Full chronological action log (one entry per line).
                   Grows every time you trigger/process events or run analytics.

  city_logs.csv  - Same log exported in CSV format with an index column.
                   Human-readable, can be opened in Excel.

  config.txt     - Stores settings like totalEventsProcessed so the count
                   carries over between sessions.


--------------------------------------------------------------------------------
RECOMMENDED DEMO SEQUENCE (to show all features)
--------------------------------------------------------------------------------

  1.  Login      : admin / admin123
  2.  Option  3  : View pre-loaded sensor readings
  3.  Option  1  : Add sensor  ID=5, Type=Water, Value=99.0
  4.  Option  5  : Trigger Traffic Accident
  5.  Option  6  : Trigger Power Failure       [EMERGENCY]
  6.  Option  7  : Trigger Network Overload    [EMERGENCY]
  7.  Option  9  : Process -> Network Overload (newest emergency, LIFO)
  8.  Option  9  : Process -> Power Failure    (next emergency, LIFO)
  9.  Option  9  : Process -> Traffic Accident (normal queue, FIFO)
  10. Option 10  : Run STL Analytics
  11. Option 11  : Search Event ID: 10
  12. Option 12  : Generate System Report
  13. Option  2  : Remove sensors below threshold: 50.0
  14. Option  4  : View all historical logs
  15. Option  0  : Save and exit


--------------------------------------------------------------------------------
BIG-O ANALYSIS
--------------------------------------------------------------------------------

See BigO_Analysis.md for full complexity justifications covering:
  - Login search: linear (find_if) vs binary (binary_search) comparison
  - Container selection rationale (vector vs list vs queue vs stack)
  - Per-algorithm time complexity breakdown


================================================================================
  Neoverse - 402415267
  2nd Year | 2nd Semester | Programming Assignment
================================================================================


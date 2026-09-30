# NeoVerse AI City Survival System — Architectural & Big-O Analysis

---

## 1. AI Engineer Authentication (Section 1)

### Password Encryption
Passwords are stored using **ROT-13** (Caesar cipher, shift = 13).  
ROT-13 is its own inverse — `encrypt(encrypt(x)) == x` — so the same function
both encrypts and decrypts.  All output characters remain printable ASCII, which
is essential for clean file storage in `engineers.dat`.

> In production, use a one-way hash such as **bcrypt** or **Argon2**.

### Login Search — Linear vs Binary

| Approach | Algorithm | Time Complexity | Notes |
|---|---|---|---|
| **Used in login** | `std::find_if` | **O(N)** | Searches username + encrypted password simultaneously |
| **Demonstrated** | `std::binary_search` | **O(log N)** | Requires pre-sorted range; returns `bool` only (no object) |
| **Pre-sort** | `std::sort` | **O(N log N)** | Done once at startup in `loadEngineers()` |

**Verdict:** For a small, fixed set of engineers, `find_if` (O(N)) is the correct
choice — the overhead of maintaining a sorted structure for O(log N) lookup
is not justified.  If the system scaled to **thousands** of engineers,
`std::lower_bound` (O(log N), retrieves the object) would replace `find_if`.

---

## 2. City Data Management (Section 2)

### Container Selection Justification

**`std::vector<SensorReading>` — Daily Sensor Readings**
- **O(1)** random access by index (pointer arithmetic).
- **O(1)** amortised `push_back` — capacity doubles when full, so reallocation
  is rare.
- Excellent **cache locality** — elements stored contiguously in memory.
- Supports `std::sort`, `std::min/max_element`, `std::count_if` directly.
- *Suitable because* the number of daily readings is bounded and we need
  fast indexed access and STL algorithm support.

**`std::list<std::string>` — Historical City Logs**
- **O(1)** `push_back` with **no reallocation, ever** — critical for an
  append-only log with no upper bound on size.
- Nodes are individually heap-allocated, so the list can grow arbitrarily large
  without shifting or copying existing data (unlike `vector`).
- *Suitable because* the log is write-once, append-heavy, and unbounded.
  Random access is never needed, so the O(N) traversal cost is acceptable.

### Data Operation Complexities

| Operation | Container | Complexity | Reason |
|---|---|---|---|
| `push_back` (sensor) | `vector` | O(1) amortised | Amortised doubling strategy |
| `push_back` (log) | `list` | O(1) | Direct tail-pointer insertion |
| `erase-remove_if` | `vector` | O(N) | Linear scan + linear shift |
| Traversal / display | Both | O(N) | Must visit every element |
| `sort` | `vector` | O(N log N) | `std::sort` (introsort) |

---

## 3. Event Processing System (Section 3)

### Container Choice Justification

**`std::queue<Event>` — Normal Events (FIFO)**
- **Why?** Events should be handled in the **order they arrived** — fair,
  chronologically correct, and prevents starvation.
- `push` and `pop` are both **O(1)**.
- A queue enforces the FIFO discipline automatically; a vector or list would not.

**`std::stack<Event>` — Emergency Overrides (LIFO)**
- **Why?** The **most recently declared** emergency is the most urgent — it may
  be the result of cascading failures triggered by earlier events.  LIFO
  ensures the newest critical failure is resolved before older ones.
- `push` and `pop` are both **O(1)**.
- The emergency stack is **always drained first** before the normal queue is
  consulted — this provides the "override" semantics.

---

## 4. Object-Oriented City Architecture (Section 4)

| OOP Concept | Where Used |
|---|---|
| **Encapsulation** | Private members in each class; public interface only |
| **Inheritance** | `PowerSystem`, `TransportSystem`, `HealthSystem`, `SecuritySystem` all extend `CityComponent` |
| **Polymorphism** | `processEvent()` is `= 0` (pure virtual) in base; overridden in each derived class; called through `CityComponent*` at runtime |
| **Constructors** | All classes define parameterised constructors |
| **Virtual destructor** | `virtual ~CityComponent()` ensures correct cleanup when deleting through a base pointer |

**Polymorphism in action (processNextEvent):**
```cpp
for (auto* comp : cityComponents)   // cityComponents is vector<CityComponent*>
    comp->processEvent(current);     // runtime dispatch — correct override called
```

---

## 5. STL Algorithms & Performance (Section 5)

| Algorithm | Applied To | Time Complexity | Justification |
|---|---|---|---|
| `std::sort` | `vector<SensorReading>` | **O(N log N)** | Introsort; needed for full ordered output |
| `std::find_if` | `vector<Engineer>` | **O(N)** | Predicate-based credential search |
| `std::find` | `vector<int>` (event IDs) | **O(N)** | Direct equality search; satisfies §5.1 requirement |
| `std::binary_search` | `vector<Engineer>` | **O(log N)** | Existence check on sorted range; demo of faster search |
| `std::min_element` | `vector<SensorReading>` | **O(N)** | Single pass; optimal for unsorted/sorted range |
| `std::max_element` | `vector<SensorReading>` | **O(N)** | Single pass; optimal for unsorted/sorted range |
| `std::count_if` | `vector<SensorReading>` | **O(N)** | Counts critical readings without manual loop |
| `std::remove_if` + `erase` | `vector<SensorReading>` | **O(N)** | Erase-remove idiom; efficient in-place compaction |
| `std::accumulate` | `vector<double>` (response times) | **O(N)** | Sums all values; used for average response time |

---

## 6. System Reports & Analytics (Section 6)

- **Average Response Time** — computed with `std::accumulate` over `responseTimes`.
  Each event records `nowMs()` on enqueue and measures elapsed ms on process.
- **Most Common Event Type** — a `std::map<string,int>` tracks type frequencies.
  An **explicit iterator** traverses the map to find the maximum value entry.
- **Explicit iterators** used in:
  - `displayHistoricalLogs()` — `std::list<string>::const_iterator`
  - `generateReport()` — `std::map<string,int>::iterator` (for most-common logic)
  - `saveSystemState()` — `std::list<string>::const_iterator` (writing city_logs.dat)

---

## 7. File Handling & Persistence (Section 7)

| File | Content | Format |
|---|---|---|
| `engineers.dat` | Engineer registry | CSV — `ID,username,encrypted_password,clearance` |
| `events.dat` | Triggered event log | CSV — `eventID,type,severity,isEmergency` |
| `city_logs.dat` | Historical action log | Plain text, one entry per line |
| `city_logs.csv` | Human-readable log export | CSV with index column |
| `config.txt` | Engine configuration | `key=value` pairs |

**Load flow:** `loadSystemState()` is called automatically in the constructor.
Each file is opened with `std::ifstream`; comment lines (`#`) and blank lines
are skipped.  Malformed event lines are silently caught to prevent crashes.

**Save flow:** `saveSystemState()` writes all five files with `std::ofstream`.
It is called both from menu option 13 and automatically on exit (option 0).

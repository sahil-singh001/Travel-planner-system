# 🗺️ Travel Planner System

A terminal-based Travel Planner written in C that uses **dual B-Trees** to manage trip activities and navigation routes. Supports full CRUD operations, DFS-based pathfinding, cycle detection, cost sorting, and file persistence.

---

## ✨ Features

### 🧳 Trip Operations
- Create new trips with date, activity, start, destination, and cost
- Add / Insert activities into the B-Tree
- Delete activities by Trip ID
- Display full trip plan
- Count total activities
- Sort trips by cost
- Range search by cost

### 🧭 Navigation Operations
- Create routes with step-by-step directions
- Add / Insert / Delete / Update directions
- Search directions by text
- Display full route for any trip

### 🔍 Other Operations
- **Find Path** — DFS traversal to find a route between two locations
- **Detect Repeated Locations** — identifies locations visited more than once
- **Sort by Cost** — sorts all trips in ascending order of cost

### 💾 File Operations
- Save trip and route data to `.txt` files
- Load trip and route data from `.txt` files (persistent across sessions)

---

## 🛠️ Tech Stack

| Component | Details |
|-----------|---------|
| Language | C (C99) |
| Data Structure | B-Tree (Order 3) for Trips & Navigation |
| Pathfinding | DFS (Depth First Search) |
| Persistence | File I/O (tripdata.txt, routedata.txt) |
| Build Tool | GCC |

---

## 📁 Project Structure

```
ASS-2/
├── main.c          # Menu-driven entry point
├── trip.c          # All B-Tree logic and user-level functions
├── trip.h          # Structs, macros, and function declarations
├── tripdata.txt    # Saved trip records
├── routedata.txt   # Saved navigation/route records
└── .gitignore
```

---

## ⚙️ Getting Started

### Prerequisites
- GCC compiler installed

Check with:
```bash
gcc --version
```

Install if missing:
- **Windows** → Install [MinGW](https://www.mingw-w64.org/)
- **Linux** → `sudo apt install gcc`
- **Mac** → `xcode-select --install`

### Compile

```bash
gcc main.c trip.c -o trip
```

### Run

**Linux / Mac:**
```bash
./trip
```

**Windows:**
```bash
trip.exe
```

---

## 🖥️ Sample Menu

```
=====================================
       TRAVEL PLANNER SYSTEM
=====================================

Trip Operations
1.  Create New Trip
2.  Add Activity
3.  Insert Activity (by date)
4.  Delete Activity
5.  Display Trip Plan

Navigation Operations
6.  Create Route
7.  Add Direction
8.  Insert Direction
9.  Delete Direction
10. Search Direction
11. Update Direction
12. Display Route

Other Operations
13. Find Path
14. Detect Repeated Locations
15. Sort by Cost
16. Range Search
17. Count Activities

File Operations
18. Save Trip to File
19. Load Trip from File

0. Exit
```

---

## 📂 File Format

**tripdata.txt**
```
tripID|date|activity|start|destination|cost
1|2026-05-01|Flight|Delhi|Mumbai|5200.00
```

**routedata.txt**
```
tripID|stepID|direction|distance
1|1|Leave Home Delhi|2.00
```

---

## 🧠 Data Structure Design

```
TripNode (B-Tree)
├── key[]: Trip records (tripID, date, activity, start, dest, cost)
│           └── navRoot → NavNode (B-Tree)
│                         └── key[]: NavStep (stepID, direction, distance)
└── child[]: Pointers to child TripNodes
```

- **B-Tree order 3** — each node holds up to 3 keys and 4 children
- Trip B-Tree is keyed on `tripID`
- Navigation B-Tree (per trip) is keyed on `stepID`

---

## 📄 License

This project is for educational purposes as part of a Data Structures & Algorithms assignment.

---

## 🙋‍♂️ Author

**Sahil Kumar Singh**  
B.Tech CSE — VNIT Nagpur  
📧 nikumbhsahilsingh@gmail.com  
🔗 [LinkedIn](#) | [GitHub](https://github.com/sahil-singh001)
# SwiftStore IMS (Inventory Management System)

A clean, textbook Object-Oriented Programming (OOP) project built for a 2nd-semester university course. Built using **C++** and the **Qt 6 Framework** with an **SQLite** database.

## Project Overview

SwiftStore is an Inventory Management System divided into three distinct user roles, each with their own dedicated interface and functionality:

1. **Admin Dashboard:** Has full control over the inventory. Can add new products, search via real-time filtering, create restock requests, and register new users.
2. **Customer Portal:** A shopping interface where customers can browse available stock, add items to a dynamic shopping cart, and perform checkout transactions.
3. **Vendor Fulfillment Portal:** A dedicated dashboard for vendors to view pending restock requests sent by the Admin and fulfill them, automatically injecting stock back into the database.

## System Architecture & Design Methodologies

This project implements rigorous software engineering and Object-Oriented Programming methodologies:

* **Separation of Concerns:** Zero SQL queries exist in the UI code. All database logic (`SELECT`, `INSERT`, `UPDATE`) is completely encapsulated inside the `DatabaseManager` class.
* **Encapsulation (Data Hiding):** Data models (like `CartItem`) are strictly defined as `class` objects with `private` member variables, accessed only via secure `public` Getter and Setter methods.
* **Class Methods (Slots):** Bypasses advanced C++11 lambdas in favor of strict class-member slot methods for handling UI signals (button clicks, text changes).
* **Defensive Programming:** Implements SQL database transactions (`db.transaction()`, `db.commit()`, `db.rollback()`) and `try/catch` exception handling to prevent database corruption during checkout and fulfillment phases.

## How to Build and Run

### Prerequisites
* **C++ Compiler** (MSVC on Windows, GCC/Clang on Linux/Mac)
* **CMake** (Version 3.16 or higher)
* **Qt 6 Framework** (with the SQL module installed)

### Build Instructions

1. **Clone the repository:**
   ```bash
   git clone https://github.com/yokominaka/SwiftStore-IMS.git
   cd SwiftStore-IMS
   ```

2. **Create a build directory:**
   ```bash
   mkdir build
   cd build
   ```

3. **Run CMake:**
   ```bash
   cmake ..
   cmake --build .
   ```

4. **Run the executable:**
   * On Windows: `OOP_Project.exe`
   * On Linux/Mac: `./OOP_Project`

---
*Built for learning, re-engineered for simplicity.*

# 💾 Basic Database System in C++

## 📘 Overview
This **Database System**, developed in **C++**, is an educational project that simulates the foundational concepts of an **SQL-based database system**. It enables users to perform command-line operations such as table creation, data insertion, querying, and data persistence through serialization.

---

## 🚀 Features

- **📁 Database Creation**
  - Create new databases
  - Switch between existing databases

- **📊 Table Management**
  - Create tables with custom columns and datatypes
  - Describe table schemas
  - Drop existing tables

- **📥 Data Manipulation**
  - Insert records into tables
  - Retrieve all records from a table
  - Delete records using conditions

- **💽 Serialization**
  - Persist database data to disk
  - Load saved data for long-term use

---

## 🛠 Supported Commands

| Command Example | Description |
|-----------------|-------------|
| `CREATE DATABASE dbName;` | Create a new database |
| `USE dbName;` | Select a database to use |
| `SHOW TABLES;` | List all tables in the selected database |
| `CREATE TABLE tableName column1 datatype column2 ...;` | Create a table |
| `INSERT INTO tableName VALUES value1 value2 ...;` | Insert a new record |
| `SELECT * FROM tableName;` | Display all records from a table |
| `DELETE FROM tableName WHERE condition;` | Delete records matching a condition |

---

## 📚 Educational Value

This project demonstrates the following concepts:

- Command parsing and input handling
- File handling and data persistence in C++
- SQL-like query simulation
- Use of STL containers (maps, vectors)
- Basic logic for DBMS operations

---



#ifndef _DATABASE_H  // Check if the _DATABASE_H macro is not defined to prevent multiple inclusions of this header file.
#define _DATABASE_H  // Define the _DATABASE_H macro to indicate that the header file has been included, preventing multiple inclusions.
#include <iostream>  // Include the iostream library for input and output stream operations.
#include <string>  // Include the string library to handle string manipulation and use the std::string class.
#include <sqlite3.h>  // Include the sqlite3 library for SQLite database operations (e.g., creating, querying, and manipulating databases).

class database  // Define a class named 'database'
{
private:
    sqlite3* db;  // Pointer to the SQLite database connection.
    database();  // Constructor to initialize the database connection (private to prevent direct instantiation).
    ~database();  // Destructor to clean up the database connection.
public:
    database& getInstance();  // Return the singleton instance of the database class.
    void insert(float weight);  // Insert a new record with the specified weight into the database.
};




#endif
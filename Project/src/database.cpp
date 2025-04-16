#include "database.h"

// Singleton pattern to get the instance of the database class
database& database::getInstance()
{
    static database db;
    return db;
}

// Constructor: Opens or creates a SQLite database and a table
database::database()
{
    char* errMsg;
    int rc = sqlite3_open("./date.db", &db);
    if (rc) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return;
    }

    // Create table if it doesn't exist
    const char* create_table = "CREATE TABLE IF NOT EXISTS Record ("
        "weights FLOAT NOT NULL,created_at DATE NOT NULL DEFAULT CURRENT_DATE)";
    rc = sqlite3_exec(db, create_table, 0, 0, &errMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Insert error: %s\n", errMsg);
        sqlite3_free((void*)errMsg);
        return;
    }
}

// Destructor: Close the database connection
database::~database()
{
    if (db)
    {
        sqlite3_close(db);
    }
}

// Placeholder for insert function
void database::insert(float weight)
{
    const char* insertSql = "INSERT INTO Record (weights, created_at) VALUES (%f, 30);";
}

#ifndef _DATABASE_H
#define _DATABASE_H
#include <iostream>
#include <string>
#include <sqlite3.h>

class database
{
private:
    sqlite3* db;
    database();
    ~database();
public:
    database& getInstance();
    void insert(float weight);
};




#endif
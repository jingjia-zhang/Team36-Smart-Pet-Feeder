#include "Data/Database.hpp"
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <iostream>

Database::Database(const std::string& path) : path_(path), db_(nullptr) {
    if (sqlite3_open_v2(path.c_str(), &db_, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, nullptr) != SQLITE_OK) {
        throw std::runtime_error("Failed to open database: " + std::string(sqlite3_errmsg(db_)));
    }
    initializeSchema();
}

Database::~Database() {
    if (db_) {
        sqlite3_close(db_);
        db_ = nullptr;
    }
}

void Database::checkError(int rc, const std::string& context) const {
    if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) {
        std::stringstream ss;
        ss << "SQLite error (" << rc << "): " << sqlite3_errmsg(db_);
        if (!context.empty()) {
            ss << " | Context: " << context;
        }
        throw std::runtime_error(ss.str());
    }
}

void Database::initializeSchema() {
    const char* sql = R"(
        PRAGMA journal_mode = WAL;
        PRAGMA synchronous = NORMAL;

        CREATE TABLE IF NOT EXISTS feeding_records (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            timestamp DATETIME DEFAULT CURRENT_TIMESTAMP,
            weight_before REAL NOT NULL CHECK(weight_before >= 0),
            weight_after REAL NOT NULL CHECK(weight_after >= 0),
            amount REAL GENERATED ALWAYS AS (weight_before - weight_after) STORED
        );

        CREATE INDEX IF NOT EXISTS idx_timestamp ON feeding_records(timestamp);

        CREATE VIEW IF NOT EXISTS daily_summary AS
            SELECT date(timestamp) as date,
                   COUNT(*) as feed_count,
                   SUM(amount) as total_food,
                   AVG(amount) as avg_food
            FROM feeding_records
            GROUP BY date;
    )";

    char* errMsg = nullptr;
    int rc = sqlite3_exec(db_, sql, nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        std::string err = errMsg ? errMsg : "Unknown";
        sqlite3_free(errMsg);
        checkError(rc, err);
    }
}

void Database::logFeeding(double before, double after) {
    const char* sql = "INSERT INTO feeding_records (weight_before, weight_after) VALUES (?, ?);";
    sqlite3_stmt* stmt = nullptr;

    int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
    checkError(rc, "prepare logFeeding");

    sqlite3_bind_double(stmt, 1, before);
    sqlite3_bind_double(stmt, 2, after);

    rc = sqlite3_step(stmt);
    checkError(rc, "execute logFeeding");

    sqlite3_finalize(stmt);
}

std::vector<Database::FeedingRecord> Database::getFeedings(int limit) {
    const std::string sql = "SELECT timestamp, weight_before, weight_after, amount FROM feeding_records ORDER BY timestamp DESC LIMIT ?;";
    std::vector<FeedingRecord> records;
    sqlite3_stmt* stmt = nullptr;

    checkError(sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr), "prepare getFeedings");
    sqlite3_bind_int(stmt, 1, limit);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        records.push_back({
            reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)),
            sqlite3_column_double(stmt, 1),
            sqlite3_column_double(stmt, 2),
            sqlite3_column_double(stmt, 3)
        });
    }

    sqlite3_finalize(stmt);
    return records;
}

std::vector<Database::FeedingRecord> Database::getFeedingsBetween(
    const std::chrono::system_clock::time_point& start,
    const std::chrono::system_clock::time_point& end) {

    const char* sql = "SELECT timestamp, weight_before, weight_after, amount FROM feeding_records "
                      "WHERE timestamp BETWEEN ? AND ? ORDER BY timestamp DESC;";
    sqlite3_stmt* stmt = nullptr;
    std::vector<FeedingRecord> records;

    checkError(sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr), "prepare getFeedingsBetween");

    std::string start_str = timeToString(start);
    std::string end_str = timeToString(end);

    sqlite3_bind_text(stmt, 1, start_str.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, end_str.c_str(), -1, SQLITE_TRANSIENT);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        records.push_back({
            reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)),
            sqlite3_column_double(stmt, 1),
            sqlite3_column_double(stmt, 2),
            sqlite3_column_double(stmt, 3)
        });
    }

    sqlite3_finalize(stmt);
    return records;
}

Database::DailyStats Database::getDailyStats(const std::string& date) {
    const char* sql = "SELECT date, feed_count, total_food, avg_food FROM daily_summary WHERE date = ?;";
    sqlite3_stmt* stmt = nullptr;
    DailyStats stats{date, 0, 0.0, 0.0};

    checkError(sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr), "prepare getDailyStats");
    sqlite3_bind_text(stmt, 1, date.c_str(), -1, SQLITE_TRANSIENT);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        stats.feed_count = sqlite3_column_int(stmt, 1);
        stats.total_food = sqlite3_column_double(stmt, 2);
        stats.avg_food = sqlite3_column_double(stmt, 3);
    }

    sqlite3_finalize(stmt);
    return stats;
}

std::vector<Database::DailyStats> Database::getWeeklyStats() {
    const char* sql = "SELECT date, feed_count, total_food, avg_food FROM daily_summary "
                      "WHERE date >= date('now', '-7 days') ORDER BY date;";
    sqlite3_stmt* stmt = nullptr;
    std::vector<DailyStats> stats;

    checkError(sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr), "prepare getWeeklyStats");

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        stats.push_back({
            reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)),
            sqlite3_column_int(stmt, 1),
            sqlite3_column_double(stmt, 2),
            sqlite3_column_double(stmt, 3)
        });
    }

    sqlite3_finalize(stmt);
    return stats;
}

double Database::getTotalConsumed(const std::string& start_date, const std::string& end_date) {
    const char* sql = "SELECT SUM(amount) FROM feeding_records WHERE timestamp BETWEEN ? AND ?;";
    sqlite3_stmt* stmt = nullptr;
    double total = 0.0;

    checkError(sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr), "prepare getTotalConsumed");
    sqlite3_bind_text(stmt, 1, start_date.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, end_date.c_str(), -1, SQLITE_TRANSIENT);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        total = sqlite3_column_double(stmt, 0);
    }

    sqlite3_finalize(stmt);
    return total;
}

void Database::vacuum() {
    char* errMsg = nullptr;
    int rc = sqlite3_exec(db_, "VACUUM;", nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        std::string err = errMsg ? errMsg : "Unknown";
        sqlite3_free(errMsg);
        checkError(rc, err);
    }
}

void Database::backup(const std::string& output_path) {
    sqlite3* backup_db = nullptr;
    if (sqlite3_open(output_path.c_str(), &backup_db) != SQLITE_OK) {
        throw std::runtime_error("Failed to open backup database.");
    }

    sqlite3_backup* backup = sqlite3_backup_init(backup_db, "main", db_, "main");
    if (!backup) {
        sqlite3_close(backup_db);
        throw std::runtime_error("Failed to initialize backup.");
    }

    int rc;
    do {
        rc = sqlite3_backup_step(backup, 5);
        if (rc == SQLITE_BUSY || rc == SQLITE_LOCKED) {
            sqlite3_sleep(100);
        }
    } while (rc == SQLITE_OK || rc == SQLITE_BUSY || rc == SQLITE_LOCKED);

    sqlite3_backup_finish(backup);
    sqlite3_close(backup_db);

    checkError(rc, "backup failed");
}

std::string Database::timeToString(const std::chrono::system_clock::time_point& tp) {
    std::time_t t = std::chrono::system_clock::to_time_t(tp);
    std::tm* tm = std::localtime(&t);
    std::stringstream ss;
    ss << std::put_time(tm, "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

std::string Database::timeToDbString(const std::chrono::system_clock::time_point& tp) const {
    return timeToString(tp);
}

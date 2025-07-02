#pragma once
#include <sqlite3.h>
#include <string>
#include <vector>
#include <memory>
#include <chrono>

class Database final {
public:
    struct FeedingRecord {
        std::string timestamp;
        double weight_before;
        double weight_after;
        double amount;
    };

    struct DailyStats {
        std::string date;
        int feed_count;
        double total_food;
        double avg_food;
    };

    explicit Database(const std::string& path = "pet_feeder.db");
    ~Database();

    // Prohibit copying
    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;

    // Record Operations
    void logFeeding(double before, double after);
    std::vector<FeedingRecord> getFeedings(int limit = 100);
    std::vector<FeedingRecord> getFeedingsBetween(
        const std::chrono::system_clock::time_point& start,
        const std::chrono::system_clock::time_point& end);

    // statistical analysis
    DailyStats getDailyStats(const std::string& date);
    std::vector<DailyStats> getWeeklyStats();
    double getTotalConsumed(const std::string& start_date, const std::string& end_date);

    // maintenance operations
    void vacuum();
    void backup(const std::string& output_path);

private:
    sqlite3* db_;
    std::string path_;

    void checkError(int rc) const;
    void initializeSchema();
    std::string timeToDbString(const std::chrono::system_clock::time_point& tp) const;
};
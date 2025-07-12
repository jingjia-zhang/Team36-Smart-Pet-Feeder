#include "Data/DataApi.hpp"

DataApi::DataApi(const std::string& dbPath) : db_(std::make_unique<Database>(dbPath)) {}

DataApi::~DataApi() = default;

// Record feeding data to database
void DataApi::logFeeding(float weightBefore, float weightAfter) {
    db_->logFeeding(weightBefore, weightAfter);
}

// Get feeding records for a specified time range
std::vector<Database::FeedingRecord> DataApi::getFeedingsBetween(const std::string& start, const std::string& end) {
    return db_->getFeedingsBetween(start, end);
}

// Get daily summary information for a specified date
Database::DailyStats DataApi::getDailyStats(const std::string& date) {
    return db_->getDailyStats(date);
}

// Backup database
void DataApi::backupDatabase(const std::string& path) {
    db_->backup(path);
}

// Send HTTP request
requests::Response DataApi::sendHttpRequest(const std::string& reqStr) {
    return axios::axios(reqStr);
}
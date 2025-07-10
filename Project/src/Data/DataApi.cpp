#include "Data/DataApi.hpp"

DataApi::DataApi(const std::string& dbPath) : db_(std::make_unique<Database>(dbPath)) {}

DataApi::~DataApi() = default;

// 记录喂食数据到数据库
void DataApi::logFeeding(float weightBefore, float weightAfter) {
    db_->logFeeding(weightBefore, weightAfter);
}

// 获取指定时间范围内的喂食记录
std::vector<Database::FeedingRecord> DataApi::getFeedingsBetween(const std::string& start, const std::string& end) {
    return db_->getFeedingsBetween(start, end);
}

// 获取指定日期的每日汇总信息
Database::DailyStats DataApi::getDailyStats(const std::string& date) {
    return db_->getDailyStats(date);
}

// 备份数据库
void DataApi::backupDatabase(const std::string& path) {
    db_->backup(path);
}

// 发送HTTP请求
requests::Response DataApi::sendHttpRequest(const std::string& reqStr) {
    return axios::axios(reqStr);
}
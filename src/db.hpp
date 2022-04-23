#pragma once

#include <string>
#include <stdexcept>
#include <vector>
#include <map>
#include "sqlite3.h"
#include <SPI.h>
#include "SD.h"
#include <Arduino.h>

namespace {
    int defaultCallback(void *data, int argc, char **argv, char**azColName) {
        return 0;
    }
    int getRowCallback(void *data, int argc, char **argv, char**azColName) {
        std::vector<std::map<std::string, std::string>> *results = (std::vector<std::map<std::string, std::string>> *)data;
        std::map<std::string, std::string> result {};
        for (int i = 0; i < argc; i++) {
            result[std::string(azColName[i])] = argv[i] ? std::string(argv[i]) : "NULL";
            //Serial.printf("%s: %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
        }
        results->push_back(result);
        return 0;
    }
}

class DB {
public:
    ~DB() {
        sqlite3_close(DB::_db);
    }
    static bool initialize(std::string path) {
        // setup SD card
        SPI.begin(23, 33, 19, 22);
        SD.begin(22);
        // open database file
        int rc = sqlite3_open(path.c_str(), &DB::_db);
        if (rc) return false;
        exec_sql("PRAGMA foreign_keys=true");
        return true;
    }
    static std::vector<std::map<std::string, std::string>> select(std::string table, std::string cond = "") {
        std::string sql = "SELECT * FROM " + table;
        if (cond != "") {
            sql += " WHERE " + cond;
        }
        std::vector<std::map<std::string, std::string>> result {};
        void *data = (void *)&result;
        exec_sql(sql, getRowCallback, data);
        return result;
    }
    static int insert(std::string table, std::map<std::string, std::string> values) {
        std::string sql = "INSERT INTO " + table + " (";
        int index = 0;
        for (const auto& value : values) {
            sql += value.first;
            if (index != values.size() - 1) {
                sql += ",";
            }
            index++;
        }
        sql += ") VALUES (";
        index = 0;
        for (const auto& value : values) {
            sql += value.second;
            if (index != values.size() - 1) {
                sql += ",";
            }
            index++;
        }
        sql += ")";
        int rc = DB::exec_sql(sql);
        return rc;
    }
    static int updateById(std::string table, int id, std::map<std::string, std::string> values) {
        std::string sql = "UPDATE " + table + " SET ";
        int index = 0;
        for (const auto& value : values) {
            sql += value.first + " = " + value.second;
            if (index != values.size() - 1) {
                sql += ", ";
            }
            index++;
        }
        sql += " WHERE id = " + std::to_string(id);
        return DB::exec_sql(sql);
    }
    static bool deleteById(std::string table, int id) {
        std::string sql = "DELETE FROM " + table + " WHERE id = " + std::to_string(id);
        int rc = DB::exec_sql(sql);
        if (rc == SQLITE_OK) {
            return true;
        }
        return false;
    }
    static int exec_sql(std::string sql, int (*callback)(void *, int, char **, char **) = defaultCallback, void *data = 0) {
        Serial.printf("sql: %s\n", sql.c_str());
        char *errMsg = 0;
        int rc = sqlite3_exec(DB::_db, sql.c_str(), callback, data, &errMsg);
        if (rc != SQLITE_OK) {
            Serial.printf("SQL error: %s\n", errMsg);
            sqlite3_free(errMsg);
            return rc;
        }
        return rc;
    }
private:
    static sqlite3 *_db;
};
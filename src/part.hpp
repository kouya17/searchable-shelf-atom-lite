#pragma once

#include <vector>
#include <string>
#include <ArduinoJson.h>
#include "db.hpp"

class Part
{
public:
    int id;
    std::string code{};
    std::string name{};
    int count;
    int shelf_id;
    std::string memo{};
    std::string created_at{};
    std::string updated_at{};
    bool save() {
        std::map<std::string, std::string> values{};
        values["code"] = "'" + this->code + "'";
        values["name"] = "'" + this->name + "'";
        values["memo"] = "'" + this->memo + "'";
        values["count"] = std::to_string(count);
        values["shelf_id"] = std::to_string(shelf_id);
        DB::insert(Part::table_name, values);
        return true;
    }
    static bool updateByJson(int id, JsonVariant json) {
        //if (!json.containsKey("id")) return false;
        std::map<std::string, std::string> values{};
        // FIXME: 型の自動判定
        if (json.containsKey("code")) values["code"] = "'" + json["code"].as<std::string>() + "'";
        if (json.containsKey("name")) values["name"] = "'" + json["name"].as<std::string>() + "'";
        if (json.containsKey("memo")) values["memo"] = "'" + json["memo"].as<std::string>() + "'";
        if (json.containsKey("count")) values["count"] = std::to_string(json["count"].as<int>());
        if (json.containsKey("shelf_id")) values["shelf_id"] = std::to_string(json["shelf_id"].as<int>());
        DB::updateById(Part::table_name, id, values);
        return true;
    } 
    std::string toString() const {
        return "id: " + std::to_string(id)
            + ", code: " + code
            + ", name: " + name
            + ", memo: " + memo
            + ", count: " + std::to_string(count)
            + ", shelf_id: " + std::to_string(shelf_id)
            + ", created_at: " + created_at;
            + ", updated_at: " + updated_at;
    }
    JsonVariant toJsonVariant() const {
        JsonVariant json {};
        json["id"] = this->id;
        json["code"] = this->code;
        json["name"] = this->name;
        json["memo"] = this->memo;
        json["count"] = this->count;
        json["shelf_id"] = this->shelf_id;
        json["created_at"] = this->created_at;
        json["updated_at"] = this->updated_at;
        return json;
    }
    // FIXME: remove index from argument
    void setToJsonArrayAt(JsonArray json, int i) const {
        json[i]["id"] = this->id;
        json[i]["code"] = this->code;
        json[i]["name"] = this->name;
        json[i]["memo"] = this->memo;
        json[i]["count"] = this->count;
        json[i]["shelf_id"] = this->shelf_id;
        json[i]["created_at"] = this->created_at;
        json[i]["updated_at"] = this->updated_at;
    }
    static Part fromJsonVariant(JsonVariant json, bool has_id = true) {
        Part part{};
        if (has_id) {
            part.id = json["id"] | -1;
            part.created_at = json["created_at"] | "NULL";
            part.updated_at = json["updated_at"] | "NULL";
        }
        part.code = json["code"] | "NULL";
        part.name = json["name"] | "NULL";
        part.memo = json["memo"] | "NULL";
        part.count = json["count"] | -1;
        part.shelf_id = json["shelf_id"] | -1;
        return part;
    }
    static std::vector<Part> get(std::string cond = "", int limit = 0, int offset = 0) {
        return Part::MapsToParts(DB::select(Part::table_name, cond, "ORDER BY updated_at DESC LIMIT " + std::to_string(limit) + " OFFSET " + std::to_string(offset)));
    }
    static int counts(std::string cond = "") {
        return DB::count(Part::table_name, cond);
    }
    static bool deleteById(int id) {
        return DB::deleteById(Part::table_name, id);
    }
    static bool createTable() {
        DB::exec_sql("CREATE TABLE IF NOT EXISTS part ( \
            id INTEGER PRIMARY KEY, \
            code TEXT, name TEXT, \
            memo TEXT, count INTEGER, \
            shelf_id INTEGER, \
            created_at TIMESTAMP DEFAULT (datetime(CURRENT_TIMESTAMP,'localtime')), \
            updated_at TIMESTAMP DEFAULT (datetime(CURRENT_TIMESTAMP,'localtime')), \
            foreign key (shelf_id) references shelf(id))");
        DB::exec_sql("CREATE TRIGGER IF NOT EXISTS trigger_part_updated_at AFTER UPDATE ON part \
            BEGIN \
                UPDATE part SET updated_at = DATETIME('now', 'localtime') WHERE rowid == NEW.rowid; \
            END");
        return true;
    }
private:
    static const std::string table_name;
    static Part MapToPart(std::map<std::string, std::string> map) {
        Part part {};
        part.id = atoi(map.at("id").c_str());
        part.code = map.at("code");
        part.name = map.at("name");
        part.memo = map.at("memo");
        part.count = atoi(map.at("count").c_str());
        part.shelf_id = atoi(map.at("shelf_id").c_str());
        part.created_at = map.at("created_at");
        part.updated_at = map.at("updated_at");
        return part;
    }
    static std::vector<Part> MapsToParts(std::vector<std::map<std::string, std::string>> maps) {
        std::vector<Part> parts {};
        for (const auto& map : maps) {
            parts.push_back(Part::MapToPart(map));
        }
        return parts;
    }
};
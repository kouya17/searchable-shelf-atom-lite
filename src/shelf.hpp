#pragma once

#include <vector>
#include <string>
#include <ArduinoJson.h>
#include "db.hpp"

class Shelf
{
public:
    int id;
    std::string name{};
    std::string memo{};
    int port;
    std::string created_at{};
    bool save() {
        std::map<std::string, std::string> values{};
        values["name"] = "'" + this->name + "'";
        values["memo"] = "'" + this->memo + "'";
        values["port"] = std::to_string(port);
        DB::insert(Shelf::table_name, values);
        return true;
    }
    std::string toString() const {
        return "id: " + std::to_string(id)
            + ", name: " + name
            + ", memo: " + memo
            + ", port: " + std::to_string(port)
            + ", created_at: " + created_at;
    }
    JsonVariant toJsonVariant() const {
        JsonVariant json {};
        json["id"] = this->id;
        json["name"] = this->name;
        json["memo"] = this->memo;
        json["port"] = this->port;
        json["created_at"] = this->created_at;
        return json;
    }
    // FIXME: remove index from argument
    void setToJsonArrayAt(JsonVariant &json, int i) const {
        json[i]["id"] = this->id;
        json[i]["name"] = this->name;
        json[i]["memo"] = this->memo;
        json[i]["port"] = this->port;
        json[i]["created_at"] = this->created_at;
    }
    static Shelf fromJsonVariant(JsonVariant json, bool has_id = true) {
        Shelf shelf{};
        if (has_id) {
            shelf.id = json["id"] | -1;
            shelf.created_at = json["created_at"] | "NULL";
        }
        shelf.name = json["name"] | "NULL";
        shelf.memo = json["memo"] | "NULL";
        shelf.port = json["port"] | -1;
        return shelf;
    }
    static std::vector<Shelf> get() {
        return Shelf::MapsToShelfs(DB::select(Shelf::table_name));
    }
    static Shelf getById(int id) {
        return (Shelf::MapsToShelfs(DB::select(Shelf::table_name, "id =" + std::to_string(id))))[0];
    }
    static bool deleteById(int id) {
        return DB::deleteById(Shelf::table_name, id);
    }
    static bool createTable() {
        DB::exec_sql("CREATE TABLE IF NOT EXISTS shelf ( id INTEGER PRIMARY KEY, name TEXT, memo TEXT, port INTEGER, created_at TIMESTAMP DEFAULT (datetime(CURRENT_TIMESTAMP,'localtime')) )");
        return true;
    }
private:
    static const std::string table_name;
    static Shelf MapToShelf(std::map<std::string, std::string> map) {
        Shelf shelf {};
        shelf.id = atoi(map.at("id").c_str());
        shelf.name = map.at("name");
        shelf.memo = map.at("memo");
        shelf.port = atoi(map.at("port").c_str());
        shelf.created_at = map.at("created_at");
        return shelf;
    }
    static std::vector<Shelf> MapsToShelfs(std::vector<std::map<std::string, std::string>> maps) {
        std::vector<Shelf> shelfs {};
        for (const auto& map : maps) {
            shelfs.push_back(Shelf::MapToShelf(map));
        }
        return shelfs;
    }
};
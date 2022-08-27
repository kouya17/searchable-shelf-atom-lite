#pragma once

#include <memory>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#define FORMAT_SPIFFS_IF_FAILED true
#include <AsyncJson.h>
#include <ArduinoJson.h>
#include "shelf.hpp"
#include "leds.hpp"
#include "string.hpp"
#include <algorithm>
#include <iterator>

namespace
{
    void notFound(AsyncWebServerRequest *request)
    {
        if (request->method() == HTTP_OPTIONS) {
            request->send(200);
        } else {
            request->send(404);
        }
    }
}

class AppServer
{
public:
    AppServer(uint16_t port)
    {
        SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED);
        _leds.reset(new Leds());
        _server.reset(new AsyncWebServer(port));
        _server->serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");
        _server->onNotFound(notFound);
        _server->on("^\\/api\\/shelfs\\/([0-9]+)$", HTTP_GET, [](AsyncWebServerRequest *request) {
            int id = request->pathArg(0).toInt();
            auto shelf = Shelf::getById(id);
            AsyncJsonResponse *response = new AsyncJsonResponse(true, 2048);
            JsonVariant &root = response->getRoot();
            shelf.setToJsonArrayAt(root, 0);
            response->setLength();
            request->send(response);
        });
        _server->on("/api/shelfs", HTTP_GET, [](AsyncWebServerRequest *request) {
            auto shelfs = Shelf::get();
            AsyncJsonResponse *response = new AsyncJsonResponse(true, 2048);
            JsonVariant &root = response->getRoot();
            Serial.printf("shelfs.size() = %d\n", shelfs.size());
            for (int i = 0; i < shelfs.size(); i++) {
                // FIXME: remove index from argument
                shelfs[i].setToJsonArrayAt(root, i);
            }
            response->setLength();
            request->send(response);
        });
        _server->on("^\\/api\\/shelfs\\/([0-9]+)$", HTTP_DELETE, [](AsyncWebServerRequest *request) {
            int id = request->pathArg(0).toInt();
            Serial.printf("delete shelf id = %d\n", id);
            if (Shelf::deleteById(id)) {
                request->send(200, "application/json", "{}");
                return;
            }
            sendErrorResponse(request, "Failed to delete shelf", 500);
        });
        AsyncCallbackJsonWebHandler *post_handler = new AsyncCallbackJsonWebHandler("/api/shelfs", [](AsyncWebServerRequest *request, JsonVariant &json) {
            JsonObject request_json = json.as<JsonObject>();
            Shelf shelf = Shelf::fromJsonVariant(request_json);
            AsyncJsonResponse *response = new AsyncJsonResponse();
            JsonObject root = response->getRoot();
            if (shelf.save()) {
                root["shelf"] = request_json;
                response->setLength();
                request->send(response);
                return;
            }
            sendErrorResponse(request, "Failed to register shelf", 500);
        });
        _server->addHandler(post_handler);
        // FIXME: make common class for controller
        _server->on("/api/parts", HTTP_GET, [this](AsyncWebServerRequest *request) {
            std::string cond = "";
            std::vector<Part> parts{};
            std::vector<Part> parts_mb{};
            int item_count = 0;
            int item_count_mb = 0;
            if (request->hasArg("name")) {
                int page = 1;
                if (request->hasArg("page")) {
                    page = request->arg("page").toInt();
                }
                std::string name = std::string(request->arg("name").c_str());
                std::string name_mb = util::string::toMultiByte(name);
                item_count = Part::counts("name LIKE '%" + name + "%'");
                if (name_mb != name) {
                    item_count_mb = Part::counts("name LIKE '%" + name_mb + "%'");
                }
                Serial.printf("item_count: %d, item_count_mb: %d\n", item_count, item_count_mb);
                int item_offset_sum = parts_num_per_page * (page - 1);
                if (item_count > item_offset_sum) {
                    int remain_item_count = item_count - item_offset_sum;
                    if (remain_item_count >= parts_num_per_page) {
                        parts = Part::get("name LIKE '%" + name + "%'", parts_num_per_page, item_offset_sum);
                    } else {
                        parts = Part::get("name LIKE '%" + name + "%'", remain_item_count, item_offset_sum);
                        if (item_count_mb > 0) {
                            parts_mb = Part::get("name LIKE '%" + name_mb + "%'", parts_num_per_page - remain_item_count, 0);
                            parts.insert(parts.end(), parts_mb.begin(), parts_mb.end());
                        }
                    }
                } else {
                    if (item_count_mb > 0) {
                        parts = Part::get("name LIKE '%" + name_mb + "%'", parts_num_per_page, item_offset_sum - item_count);
                    }
                }
            } else {
                parts = Part::get("", parts_num_per_page);
            }
            AsyncJsonResponse *response = new AsyncJsonResponse(false, 2048);
            JsonVariant &root = response->getRoot();
            Serial.printf("parts.size() = %d\n", parts.size());
            JsonArray rootParts = root.createNestedArray("parts");
            for (int i = 0; i < parts.size(); i++) {
                // FIXME: remove index from argument
                parts[i].setToJsonArrayAt(rootParts, i);
            }
            root["maxPage"] = (item_count + item_count_mb) / parts_num_per_page + 1;
            response->setLength();
            request->send(response);
        });
        _server->on("^\\/api\\/parts\\/([0-9]+)$", HTTP_DELETE, [](AsyncWebServerRequest *request) {
            int id = request->pathArg(0).toInt();
            Serial.printf("delete part id = %d\n", id);
            if (Part::deleteById(id)) {
                request->send(200, "application/json", "{}");
                return;
            }
            sendErrorResponse(request, "Failed to delete part", 500);
        });
        AsyncCallbackJsonWebHandler *patch_part_handler = new AsyncCallbackJsonWebHandler("^\\/api\\/parts\\/([0-9]+)$", [](AsyncWebServerRequest *request, JsonVariant &json) {
            JsonObject request_json = json.as<JsonObject>();
            int id = request->pathArg(0).toInt();
            if (Part::updateByJson(id, request_json)) {
                request->send(200, "application/json", "{}");
            }
            sendErrorResponse(request, "Failed to register part", 500);
        });
        _server->addHandler(patch_part_handler);
        AsyncCallbackJsonWebHandler *post_part_handler = new AsyncCallbackJsonWebHandler("/api/parts", [](AsyncWebServerRequest *request, JsonVariant &json) {
            JsonObject request_json = json.as<JsonObject>();
            Part part = Part::fromJsonVariant(request_json);
            AsyncJsonResponse *response = new AsyncJsonResponse();
            JsonObject root = response->getRoot();
            if (part.save()) {
                root["part"] = request_json;
                response->setLength();
                request->send(response);
                return;
            }
            sendErrorResponse(request, "Failed to register part", 500);
        });
        _server->addHandler(post_part_handler);
        _server->on("^\\/api\\/ports\\/([0-9]+)\\/on$", HTTP_GET, [this](AsyncWebServerRequest *request) {
            int id = request->pathArg(0).toInt();
            Serial.printf("ON port id = %d\n", id);
            _leds->on(id);
            request->send(200, "application/json", "{}");
        });
        _server->on("^\\/api\\/ports\\/([0-9]+)\\/off$", HTTP_GET, [this](AsyncWebServerRequest *request) {
            int id = request->pathArg(0).toInt();
            Serial.printf("OFF port id = %d\n", id);
            _leds->off(id);
            request->send(200, "application/json", "{}");
        });
        _server->on("/api/ports/off", HTTP_GET, [this](AsyncWebServerRequest *request) {
            _leds->offAll();
            request->send(200, "application/json", "{}");
        });
        DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "http://127.0.0.1:5500");
        DefaultHeaders::Instance().addHeader("Access-Control-Allow-Methods", "POST, GET, OPTIONS, PATCH, DELETE");
        DefaultHeaders::Instance().addHeader("Access-Control-Allow-Headers", "Content-Type");
        _server->begin();
    }

private:
    std::unique_ptr<AsyncWebServer> _server{};
    static void sendErrorResponse(AsyncWebServerRequest *request, std::string message, int code) {
        AsyncJsonResponse *response = new AsyncJsonResponse();
        JsonObject root = response->getRoot();
        root["error"]["message"] = message;
        response->setLength();
        response->setCode(code);
        request->send(response);
    }
    std::unique_ptr<Leds> _leds{};
    int parts_num_per_page = 5;
};
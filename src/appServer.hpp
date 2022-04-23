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
            AsyncJsonResponse *response = new AsyncJsonResponse(true, 1024);
            JsonVariant &root = response->getRoot();
            shelf.setToJsonArrayAt(root, 0);
            response->setLength();
            request->send(response);
        });
        _server->on("/api/shelfs", HTTP_GET, [](AsyncWebServerRequest *request) {
            auto shelfs = Shelf::get();
            AsyncJsonResponse *response = new AsyncJsonResponse(true, 10240);
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
        _server->on("/api/parts", HTTP_GET, [](AsyncWebServerRequest *request) {
            std::string cond = "";
            if (request->hasArg("name")) {
                cond = "name LIKE '%" + std::string(request->arg("name").c_str()) + "%'";
            }
            auto parts = Part::get(cond);
            AsyncJsonResponse *response = new AsyncJsonResponse(true, 10240);
            JsonVariant &root = response->getRoot();
            Serial.printf("parts.size() = %d\n", parts.size());
            for (int i = 0; i < parts.size(); i++) {
                // FIXME: remove index from argument
                parts[i].setToJsonArrayAt(root, i);
            }
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
};
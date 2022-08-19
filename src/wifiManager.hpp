#pragma once

#include <SPIFFS.h>
#include <WiFi.h>

class WifiManager
{
public:
    static void beginWifiWithConfigFiles(const char *ssid_path, const char *pass_path)
    {
        SPIFFS.begin();
        String ssid = _read(SPIFFS, ssid_path);
        String pass = _read(SPIFFS, pass_path);
        if (ssid != "" && pass != "") {
            _connect(ssid, pass);
        } else {
            Serial.println("there are no wifi config files");
        }
    }
    static void setSoftAPWithConfigFiles(const char *ssid_path, const char *pass_path)
    {
        SPIFFS.begin();
        String ssid = _read(SPIFFS, ssid_path);
        String pass = _read(SPIFFS, pass_path);
        if (ssid != "" && pass != "") {
            WiFi.softAP(ssid.c_str(), pass.c_str());
            delay(100);
            WiFi.softAPConfig(IPAddress(192,168,4,1), IPAddress(192,168,4,1), IPAddress(255, 255, 255,0));
        } else {
            Serial.println("there are no softAP config files");
        }
    }

private:
    static String _read(fs::FS &fs, const char *path)
    {
        File file = fs.open(path);
        if (!file || file.isDirectory())
        {
            Serial.println("failed to open wifi config file.");
            return "";
        }

        String content;
        while (file.available())
        {
            content = file.readStringUntil('\n');
            break;
        }
        return content;
    }
    static void _connect(String ssid, String pass)
    {
        WiFi.begin(ssid.c_str(), pass.c_str());

        int retry_count = 0;
        while (WiFi.status() != WL_CONNECTED)
        {
            Serial.print(".");
            retry_count++;
            if (retry_count > 20)
            {
                Serial.println("failed to connect specified times, will reboot.");
                ESP.restart();
            }
            delay(500);
        }
        Serial.print("WiFi Connected : ");
        Serial.println(WiFi.localIP());
    }
};
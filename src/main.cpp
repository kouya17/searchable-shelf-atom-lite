#include <Arduino.h>
#include <M5Atom.h>
#include "db.hpp"
#include "shelf.hpp"
#include "part.hpp"
#include "appServer.hpp"
#include <memory>
#include <ESPmDNS.h>
#include <Wire.h>
#include "wifiManager.hpp"

#define HTTP_PORT 80
std::unique_ptr<AppServer> server{};

void setup()
{
  M5.begin(true, true, true);
  Wire.setClock(400000);
  WifiManager::beginWifiWithConfigFiles("/conf/ssid.txt", "/conf/pass.txt");
  WifiManager::setSoftAPWithConfigFiles("/conf/softap_ssid.txt", "/conf/softap_pass.txt");
  MDNS.begin("sshelf");
  configTime(9 * 3600L, 0, "ntp.nict.jp", "time.google.com", "ntp.jst.mfeed.ad.jp");
  server.reset(new AppServer(HTTP_PORT));
  DB::initialize("/sd/searchable_shelf.db");
  Shelf::createTable();
  Part::createTable();
  M5.dis.drawpix(0, 0x00ff00);
}

void loop()
{
  Serial.println("loop");
  delay(60000);
  //ESP.restart();
}
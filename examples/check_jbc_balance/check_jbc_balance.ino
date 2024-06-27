/*
  Example - Check JBC Balance using your ETH Address
  Created by Natthawat Raocharoensinp, June 27, 2024.
*/

#include <Arduino.h>
#if ESP32
#include <WiFi.h>
#else
#include <ESP8266WiFi.h>
#endif
#include <Web3JBC.h>
#include <Web3JBCUtil.h>

#define WIFI_SSID "WIFI_SSID"
#define WIFI_PWD "WIFI_PWD"
#define YOUR_ETH_ADDRESS "YOUR_ETH_ADDR"

Web3JBC web3;
uint32_t balanceUpdateTs;

void setup_wifi();

void get_jbc_balance();

void setup()
{
  Serial.begin(115200);
  setup_wifi();

  get_jbc_balance();

  balanceUpdateTs = millis();
}

void loop()
{
  if (millis() - balanceUpdateTs >= 10000) // every 10 sec
  {
    balanceUpdateTs = millis();
    get_jbc_balance();
  }
}

void setup_wifi()
{
  if (WiFi.status() == WL_CONNECTED)
  {
    return;
  }

  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);

  if (WiFi.status() != WL_CONNECTED)
  {
    WiFi.persistent(false);
    WiFi.mode(WIFI_OFF);
    WiFi.mode(WIFI_STA);

    WiFi.begin(WIFI_SSID, WIFI_PWD);
  }

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("WiFi connected.");
}

void get_jbc_balance()
{
  uint256_t balance = web3.eth_getBalance(YOUR_ETH_ADDRESS);
  String balanceStr = Web3JBCUtil::ConvertWeiToEthString(balance, 18);

  Serial.print("Balance: ");
  Serial.print(balanceStr.c_str());
  Serial.println(" JBC");
}
/*
  Example - Check CMJ Balance (CommuDAO) in JBC Chain using Web3JBCContract Class
  Created by Natthawat Raocharoensinp, June 27, 2024.
*/

#include <Arduino.h>
#if ESP32
#include <WiFi.h>
#else
#include <ESP8266WiFi.h>
#endif
#include <Web3JBC.h>
#include <Web3JBCContract.h>
#include <Web3JBCUtil.h>

#define WIFI_SSID "WIFI_SSID"
#define WIFI_PWD "WIFI_PWD"
#define YOUR_ETH_ADDRESS "YOUR_ETH_ADDR"

#define CMJ_CONTRACT_ADDRESS "0xE67E280f5a354B4AcA15fA7f0ccbF667CF74F97b"

Web3JBC web3;
Web3JBCContract cmjContract(&web3, CMJ_CONTRACT_ADDRESS);
uint32_t balanceUpdateTs;

void get_jdao_balance();

void setup()
{
  Serial.begin(115200);
  setup_wifi();

  get_jdao_balance();

  balanceUpdateTs = millis();
}

void loop()
{
  if (millis() - balanceUpdateTs >= 10000) // every 10 sec
  {
    balanceUpdateTs = millis();
    get_jdao_balance();
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

void get_jdao_balance()
{
  String param = cmjContract.SetupContractData("balanceOf(address)", YOUR_ETH_ADDRESS);
  String rawResult = cmjContract.ViewCall(param);

  if (web3.isOutputError(rawResult))
  {
    Serial.println("Read Contract Error");
  }
  else
  {
    String result = web3.getResult(rawResult);
    uint256_t balance = uint256_t(result.c_str());
    String balanceStr = Web3JBCUtil::ConvertWeiToEthString(balance, 18);

    Serial.print("Balance: ");
    Serial.print(balanceStr.c_str());
    Serial.println(" CMJ");
  }
}
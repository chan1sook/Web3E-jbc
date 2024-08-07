/*
  Example - Testing Contract Interaction with Coldsenses JBC Contract
  Created by Natthawat Raocharoensinp, June 27, 2024.
  Last Updated by Natthawat Raocharoensinp, August 7, 2024.
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
#define YOUR_ETH_PRIVATE_KEY "YOUR_ETH_PRIVATE_KEY"

#define COLDSENSES_CONTRACT_ADDRESS "0xC830F224956eD112B95b1Bbb90b5E32C05F0e77f"

Web3JBC web3;
Web3JBCContract iotContract(&web3, COLDSENSES_CONTRACT_ADDRESS);

uint32_t txLastestTs;

uint32_t balanceUpdateTs;
uint32_t dataUpdateTs;

void setup_wifi();

void get_jbc_balance();
void get_lastest_data();
void push_iot_data();

void setup()
{
    Serial.begin(115200);

    setup_wifi();

    iotContract.SetPrivateKey(YOUR_ETH_PRIVATE_KEY);
    strcpy(iotContract.options.from, YOUR_ETH_ADDRESS);

    get_jbc_balance();
    get_lastest_data();

    balanceUpdateTs = millis();
    dataUpdateTs = millis() - 30000;
}

void loop()
{
    if (millis() - dataUpdateTs >= 60000) // every 60 sec
    {
        dataUpdateTs = millis();
        push_iot_data();
    }
    if (millis() - balanceUpdateTs >= 10000) // every 10 sec
    {
        balanceUpdateTs = millis();
        get_jbc_balance();
        get_lastest_data();
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
    Serial.println("=================");
}

void get_jbc_balance()
{
    uint256_t balance = web3.eth_getBalance(YOUR_ETH_ADDRESS);
    String balanceStr = Web3JBCUtil::ConvertWeiToEthString(balance, 18);

    Serial.print("JBC: ");
    Serial.println(balanceStr.c_str());
    Serial.println("=================");
}

void get_lastest_data()
{
    uint256_t slot = uint256_0;
    String param = iotContract.SetupContractData("getDataLastest(address,uint256)", YOUR_ETH_ADDRESS, &slot);
    String rawResult = iotContract.ViewCall(param);
    String result = web3.getResult(rawResult);

    Serial.print("Lastest Data #");
    Serial.print(slot.str().c_str());
    Serial.println(":");

    if (web3.isOutputError(rawResult))
    {
        Serial.println("Read Contract Error");
    }
    else
    {
        uint256_t ts = uint256_t(Web3JBCUtil::Substr(result, 0, 64).c_str());
        _int256_t temp = _int256_t(Web3JBCUtil::Substr(result, 64, 64).c_str());
        _int256_t humid = _int256_t(Web3JBCUtil::Substr(result, 128, 64).c_str());
        _int256_t lat = _int256_t(Web3JBCUtil::Substr(result, 192, 64).c_str());
        _int256_t lng = _int256_t(Web3JBCUtil::Substr(result, 256, 64).c_str());

        String tempStr = Web3JBCUtil::ConvertInt256ToStringWithDecimal(temp, 18);
        String humidStr = Web3JBCUtil::ConvertInt256ToStringWithDecimal(humid, 18);
        String latStr = Web3JBCUtil::ConvertInt256ToStringWithDecimal(lat, 18);
        String lngStr = Web3JBCUtil::ConvertInt256ToStringWithDecimal(lng, 18);

        Serial.print("Temp: ");
        Serial.println(tempStr);
        Serial.print("Humidity: ");
        Serial.println(humidStr);
        Serial.print("Gps: (");
        Serial.print(latStr);
        Serial.print(",");
        Serial.print(lngStr);
        Serial.println(")");

        if (ts != txLastestTs)
        {
            if (txLastestTs != uint256_0)
            {
                Serial.println("|| TX Mined ||");
            }
            txLastestTs = ts;
        }
    }
    Serial.println("=================");
}

void push_iot_data()
{
    const char *fnsig = "pushData(uint256,int256,int256,int256,int256)";
    int slot = 0;
    double temp = random(2500, 3000) / 100.f;
    double humid = random(5500, 6000) / 100.f;
    double lat = 13.7563 + (random(0, 100) / 100000.f);
    double lng = 100.5018 + (random(0, 100) / 100000.f);

    Serial.print("Send Tx: ");
    Serial.println(fnsig);

    // gas price
    // basePrice + tip (2 gwei)
    uint64_t gasPriceVal = web3.eth_gasPrice() + (2 * 1e9);
    Serial.print("Gas Price:");
    Serial.println(gasPriceVal);

    // get nonce
    uint32_t nonceVal = web3.eth_getTransactionCount(YOUR_ETH_ADDRESS);

    // etup contract function call
    uint256_t slotParam = uint256_t(slot);
    uint256_t tempParam = Web3JBCUtil::ConvertToWei(temp, 18);
    uint256_t humidParam = Web3JBCUtil::ConvertToWei(humid, 18);
    uint256_t latParam = Web3JBCUtil::ConvertToWei(lat, 18);
    uint256_t lngParam = Web3JBCUtil::ConvertToWei(lng, 18);

    Serial.print("(slot, temp, humid, lat, lng) => (");
    Serial.print(slotParam.str().c_str());
    Serial.print(",");
    Serial.print(tempParam.str().c_str());
    Serial.print(",");
    Serial.print(humidParam.str().c_str());
    Serial.print(",");
    Serial.print(latParam.str().c_str());
    Serial.print(",");
    Serial.print(lngParam.str().c_str());
    Serial.println(")");

    String params = iotContract.SetupContractData(fnsig, &slotParam, &tempParam, &humidParam, &latParam, &lngParam);

    // Estimate gas here
    // 32000 x8
    const uint64_t baseGasUnit = 32000;
    uint64_t gasLimitVal = baseGasUnit * 8;
    Serial.print("Gas Limit:");
    Serial.println(gasLimitVal);

    // push transaction to ethereum
    uint256_t paymentValue = uint256_0;
    String result = iotContract.SendTransaction(nonceVal, gasPriceVal, gasLimitVal, COLDSENSES_CONTRACT_ADDRESS, paymentValue, params);
    if (web3.isOutputError(result))
    {
        Serial.println("Write Contract Error");
    }
    else
    {
        String tx = web3.getResult(result);
        Serial.print("TX Sent: [0x");
        Serial.print(tx);
        Serial.println("]");
    }
    Serial.println("=================");
}

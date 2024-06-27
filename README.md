# A fork Web3E for JBC Chain

Forked from https://github.com/AlphaWallet/Web3 for use in JBC Chain (https://exp-l1.jibchain.net/)

## Installation

- Using Web3E-JBC is a one step process:
    1. Create a new project in Platformio and edit the platformio.ini so it looks similar to:

```
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino

; Serial Monitor options
monitor_speed = 115200

lib_ldf_mode = deep
lib_deps =
  # Using a library name
  https://github.com/chan1sook/Web3E-jbc
```

**Please note: you may need to use 'lib_ldf_mode = deep' to locate some ESP32 libraries like 'EEPROM.h'**
**also: if you use many other libraries in your application you may need to add 'board_build.partitions = no_ota.csv' to fit your firmware**

## Node setup

``` C++
Web3JBC *web3 = new Web3JBC();
```

## Ethereum transaction (ie send ETH to address):

``` C++
// Setup Web3 and Web3JBCContract with Private Key
...
web3 = new Web3JBC();
Web3JBCContract contract(web3, "");
contract.SetPrivateKey(PRIVATE_KEY);
uint32_t nonceVal = (uint32_t)web3->EthGetTransactionCount(address); //obtain the next nonce
uint256_t weiValue = Web3JBCUtil::ConvertToWei(0.25, 18); //send 0.25 eth
uint64_t gasPriceVal = 1000000000ULL;
uint32_t gasLimitVal = 90000;
String toAddress = "0xC067A53c91258ba513059919E03B81CF93f57Ac7";
String result = contract.SendTransaction(nonceVal, gasPriceVal, gasLimitVal, toAddress, weiValue, "");
```

## Query ETH balance:
``` C++
uint256_t balance = web3->EthGetBalance(address); //obtain balance in Wei
String balanceStr = Web3JBCUtil::ConvertWeiToEthString(balance, 18); //get string balance as Eth (18 decimals)
```

## Query ERC20 Balance:
``` C++
String address = "0x007bee82bdd9e866b2bd114780a47f2261c684e3";
Web3JBCContract contract(web3, "0x20fe562d797a42dcb3399062ae9546cd06f63280"); //contract is on Ropsten

//Obtain decimals to correctly display ERC20 balance (if you already know this you can skip this step)
String param = contract.SetupContractData("decimals()", address.c_str());
String result = contract.ViewCall(param);
int decimals = web3->getInt(result);

//Fetch the balance in base units
param = contract.SetupContractData("balanceOf(address)", address.c_str());
result = contract.ViewCall(&param);

uint256_t baseBalance = web3->getUint256(result);
String balanceStr = Web3JBCUtil::ConvertWeiToEthString(baseBalance, decimals); //convert balance to double style using decimal places
```

## Send ERC20 Token:
``` C++
String contractAddr = "0x20fe562d797a42dcb3399062ae9546cd06f63280";
Web3JBCContract contract(web3, contractAddr.c_str());
contract.SetPrivateKey(<Your private key>);

//Get contract name
String param = contract.SetupContractData("name()", addr.c_str());
String result = contract.ViewCall(param);
String interpreted = Web3JBCUtil::InterpretStringResult(web3->getString(result).c_str());
Serial.println(interpreted);

//Get Contract decimals
param = contract.SetupContractData("decimals()", addr.c_str());
result = contract.ViewCall(param);
int decimals = web3->getInt(result);
Serial.println(decimals);

uint64_t gasPriceVal = 22000000000ULL;
uint32_t  gasLimitVal = 4300000;

//amount of erc20 token to send, note we use decimal value obtained earlier
uint256_t weiValue = Web3JBCUtil::ConvertToWei(0.1, decimals);

//get nonce
uint32_t nonceVal = (uint32_t)web3->EthGetTransactionCount(addr);
String toAddress = "0x007bee82bdd9e866b2bd114780a47f2261c684e3";
String valueStr = "0x00";

//Setup contract function call
String p = contract.SetupContractData("transfer(address,uint256)", toAddress.c_str(), weiValue.c_str()); //ERC20 function plus params

//push transaction to ethereum
result = contract.SendTransaction(nonceVal, gasPriceVal, gasLimitVal, contractAddr, valueStr, p);
String transactionHash = web3->getString(result);
```

# Donations
If you support the cause, we could certainly use donations to help fund development (JBC chain):

0xC70Edf3aB36E0D57eFD8f412A3460D225931A110

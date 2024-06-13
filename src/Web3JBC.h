// Web3E JBC
// by Natthawat Raocharoensinp (chan1sook)
// Based on Web3E by James Brown.

#ifndef ARDUINO_WEB3_WEB3JBC_H
#define ARDUINO_WEB3_WEB3JBC_H

typedef unsigned char BYTE;
#define ETHERS_PRIVATEKEY_LENGTH 32
#define ETHERS_PUBLICKEY_LENGTH 64
#define ETHERS_ADDRESS_LENGTH 20
#define ETHERS_KECCAK256_LENGTH 32
#define ETHERS_SIGNATURE_LENGTH 65

#define JBC_ID 8899
#define JBC_RPC_URL "rpc-l1.jibchain.net"

class Web3JBC;
class Crypto;
class KeyID;

#include "stdint.h"
#include "uint256/uint256_t.h"
#include <WiFiClientSecure.h>
#include <WiFiClient.h>
#include <Contract.h>
#include <Crypto.h>
#include <KeyID.h>
#include <Web3Util.h>
#include <string.h>
#include <string>

using namespace std;

enum ConnectionStage
{
    unconnected,
    handshake,
    have_token,
    confirmed
};

class Web3JBC
{
public:
    Web3JBC();
    string Web3ClientVersion();
    string Web3Sha3(const string *data);
    int NetVersion();
    bool NetListening();
    int NetPeerCount();
    double EthProtocolVersion();
    bool EthSyncing();
    bool EthMining();
    double EthHashrate();
    long long int EthGasPrice();
    void EthAccounts(char **array, int size);
    int EthBlockNumber();
    uint256_t EthGetBalance(const string *address);
    int EthGetTransactionCount(const string *address);
    string EthViewCall(const string *data, const char *to);

    string EthCall(const string *from, const char *to, long gas, long gasPrice, const string *value, const string *data);
    string EthSendSignedTransaction(const string *data, const uint32_t dataLen);

    long long int getLongLong(const string *json);
    string getString(const string *json);
    int getInt(const string *json);
    uint256_t getUint256(const string *json);
    long long int getChainId() const;
    string getResult(const string *json);

private:
    string exec(const string *data);
    string generateJson(const string *method, const string *params);
    void selectHost();
    void setupCert();

    long getLong(const string *json);
    double getDouble(const string *json);
    bool getBool(const string *json);

private:
    WiFiClientSecure *client;
    BYTE *mem;
    const char *host;
    const char *path;
    unsigned short port;
};

#endif // ARDUINO_WEB3_Web3JBC_H

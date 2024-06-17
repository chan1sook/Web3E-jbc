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

#include "stdint.h"
#include "uint256/uint256_t.h"
#include "TagReader/TagReader.h"
#include <WiFiClientSecure.h>
#include <Web3Contract.h>
#include <Crypto.h>
#include <Web3Util.h>
#include <string.h>
#include <string>

using namespace std;

// TODO better int256 definition
typedef uint256_t int256_t;
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
    string web3_clientVersion();
    string web3_sha3(const string *data);
    long long int net_version();
    bool net_listening();
    int net_peerCount();
    int eth_protocolVersion();
    bool eth_syncing();
    bool eth_mining();
    long long int eth_hashrate();
    long long int eth_gasPrice();
    void EthAccounts(char **array, int size);
    long long int eth_blockNumber();
    uint256_t eth_getBalance(const string *address);
    int eth_getTransactionCount(const string *address);
    string eth_call(const string *data, const char *to);

    string eth_call(const string *from, const char *to, long gas, long gasPrice, const string *value, const string *data);
    string eth_sendRawTransaction(const string *data, const uint32_t dataLen);

    long long int getLongLong(const string *json);
    string getString(const string *json);
    int getInt(const string *json);
    uint256_t getUint256(const string *json);
    string getResult(const string *json);

    bool isOutputError(const string *json);

    long long int getChainId() const;

private:
    string _exec(const string *data);
    string _generate_json(const string *method, const string *params);

    long _get_long(const string *json);
    double _get_double(const string *json);
    bool _get_bool(const string *json);

private:
    WiFiClientSecure _client;
    TagReader _tagreader;
};

#endif // ARDUINO_WEB3_Web3JBC_H

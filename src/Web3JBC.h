// Web3E JBC
// by Natthawat Raocharoensinp (chan1sook)
//
// Based on Web3E by James Brown.

#ifndef __WEB3JBC_H
#define __WEB3JBC_H

#define ETHERS_PRIVATEKEY_LENGTH 32
#define ETHERS_PUBLICKEY_LENGTH 64
#define ETHERS_ADDRESS_LENGTH 20
#define ETHERS_KECCAK256_LENGTH 32
#define ETHERS_SIGNATURE_LENGTH 65

#define JBC_ID 8899
#define JBC_RPC_URL "rpc-l1.jibchain.net"

#include <sstream>
#include <WiFiClientSecure.h>
#include "uint256/uint256_t.h"
#include "TagReader/TagReader.h"
#include "Web3JBCUtil.h"

using namespace std;

class Web3JBC
{
public:
    Web3JBC();
    String web3_clientVersion();
    String web3_sha3(const String &data);
    int64_t net_version();
    bool net_listening();
    int net_peerCount();
    int eth_protocolVersion();
    bool eth_syncing();
    bool eth_mining();
    int64_t eth_hashrate();
    int64_t eth_gasPrice();
    int64_t eth_blockNumber();
    uint256_t eth_getBalance(const String &address);
    uint32_t eth_getTransactionCount(const String &address);
    String eth_call(const String &data, const char *to);
    String eth_call(const String &from, const char *to, long gas, long gasPrice, const String &value, const String &data);
    String eth_sendRawTransaction(const String &data, const uint32_t dataLen);

    int64_t getLongLong(const String &json);
    String getString(const String &json);
    int getInt(const String &json);
    uint32_t getUint(const String &json);
    uint256_t getUint256(const String &json);
    String getResult(const String &json);

    bool isOutputError(const String &json);

    int64_t getChainId() const;

private:
    String _exec(const String &data);
    String _generate_json(const String &method, const String &params);

    long _get_long(const String &json);
    double _get_double(const String &json);
    bool _get_bool(const String &json);

private:
    WiFiClientSecure _client;
};

#endif // __WEB3JBC_H

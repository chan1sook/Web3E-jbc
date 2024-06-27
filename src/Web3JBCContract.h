// Web3E Contract handling code
// by Natthawat Raocharoensinp (chan1sook)
//
// Based on Web3E by James Brown.

#ifndef __WEB3JBC_CONTRACT_H
#define __WEB3JBC_CONTRACT_H

#include "Web3JBC.h"
#include "Web3JBCUtil.h"
#include "Crypto.h"
#include "uint256/uint256_t.h"

using namespace std;

#define SIGNATURE_LENGTH 64

class Web3JBCContract
{

public:
    typedef struct
    {
        char from[80];
        char to[80];
        char gasPrice[20];
        long gas;
    } Options;
    Options options;

public:
    Web3JBCContract(Web3JBC *_web3, const char *address);
    Web3JBCContract(Web3JBC *_web3, const String &address);
    void SetPrivateKey(const char *key);
    String SetupContractData(const char *func, ...);
    String Call(const String &param);
    String ViewCall(const String &param);
    String SendTransaction(const uint32_t nonceVal, const uint64_t gasPriceVal, const uint64_t gasLimitVal,
                           const String &toStr, const uint256_t &valueStr, const String &dataStr);
    String SignTransaction(const uint32_t nonceVal, const uint64_t gasPriceVal, const uint64_t gasLimitVal,
                           const String &toStr, const uint256_t &valueStr, const String &dataStr);

private:
    Web3JBC *web3;
    const char *contractAddress;
    Crypto crypto;

private:
    static String GenerateContractBytes(const char *func);
    string GenerateBytesForInt(const int32_t value);
    string GenerateBytesForUint(const uint256_t *value);
    string GenerateBytesForAddress(const char *value);
    string GenerateBytesForString(const char *value);
    string GenerateBytesForBytes(const char *value, const int len);
    string GenerateBytesForUIntArray(const vector<uint32_t> *v);
    string GenerateBytesForHexBytes(const char *value);
    string GenerateBytesForStruct(const char *value);

    void GenerateSignature(uint8_t *signature, int *recid, uint32_t nonceVal, const uint64_t gasPriceVal,
                           const uint64_t gasLimitVal, const String &toStr, const uint256_t &valueStr, const String &dataStr);
    vector<uint8_t> RlpEncode(
        const uint32_t nonceVal, const uint64_t gasPriceVal, const uint64_t gasLimitVal,
        const String &toStr, const uint256_t &valueStr, const String &dataStr);
    vector<uint8_t> RlpEncodeForRawTransaction(
        const uint32_t nonceVal, const uint64_t gasPriceVal, const uint64_t gasLimitVal,
        const String &toStr, const uint256_t &valueStr, const String &dataStr, const uint8_t *sig, const uint8_t recid);
    void Sign(uint8_t *hash, uint8_t *sig, int *recid);
};

#endif // __WEB3JBC_CONTRACT_H

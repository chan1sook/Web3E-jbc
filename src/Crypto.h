// Crypto.h
// by Natthawat Raocharoensinp (chan1sook)
//
// Based on Web3E by James Brown.

#ifndef ARDUINO_WEB3_CRYPTO_H
#define ARDUINO_WEB3_CRYPTO_H

#include "Trezor/secp256k1.h"
#include "Trezor/ecdsa.h"
#include "Web3JBC.h"
#include "Web3JBCUtil.h"

using namespace std;

extern const char *PERSONAL_MESSAGE_PREFIX;

class Crypto
{

public:
    Crypto(Web3JBC *_web3);
    bool Sign(uint8_t *digest, uint8_t *result);
    void SetPrivateKey(const char *key);

    static void ECRecover(uint8_t *signature, uint8_t *public_key, uint8_t *message_hash);
    static bool Verify(const uint8_t *public_key, const uint8_t *message_hash, const uint8_t *signature);
    static void PrivateKeyToPublic(const uint8_t *privateKey, uint8_t *publicKey);
    static void PublicKeyToAddress(const uint8_t *publicKey, uint8_t *address);
    static void Keccak256(const uint8_t *data, uint16_t length, uint8_t *result);
    static String ECRecoverFromPersonalMessage(const String &signature, const String &message);
    static String ECRecoverFromHexMessage(const String &signature, const String &hex);
    static String ECRecoverFromHash(const String &signature, uint8_t *digest);

    static String Keccak256(vector<uint8_t> *bytes);

private:
    Web3JBC *web3;
    uint8_t privateKey[ETHERS_PRIVATEKEY_LENGTH];
};

#endif // ARDUINO_WEB3_CRYPTO_H

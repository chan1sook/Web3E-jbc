// Web3E Utilities
// by Natthawat Raocharoensinp (chan1sook)
//
// Based on Web3E by James Brown.

#ifndef WEB3JBC_UTIL_H
#define WEB3JBC_UTIL_H

#include <Arduino.h>
#include <vector>
#include "uint256/uint256_t.h"
#include "TagReader/TagReader.h"

using namespace std;

#if !(__XSI_VISIBLE >= 4 && __POSIX_VISIBLE < 200112)
char *gcvt(double x, int ndigit, char *buf);
#endif

typedef uint256_t _int256_t;
extern _int256_t max_positive_val;

class Web3JBCUtil
{
public:
    // RLP implementation
    // reference:
    //     https://github.com/ethereum/wiki/wiki/%5BEnglish%5D-RLP
    static uint32_t RlpEncodeWholeHeader(uint8_t *header_output, uint32_t total_len);
    static vector<uint8_t> RlpEncodeWholeHeaderWithVector(uint32_t total_len);
    static uint32_t RlpEncodeItem(uint8_t *output, const uint8_t *input, uint32_t input_len);
    static vector<uint8_t> RlpEncodeItemWithVector(const vector<uint8_t> input);

    static uint32_t ConvertNumberToUintArray(uint8_t *str, uint32_t val);
    static vector<uint8_t> ConvertNumberToVector(uint32_t val);
    static vector<uint8_t> ConvertNumberToVector(uint64_t val);
    static uint32_t ConvertCharStrToUintArray(uint8_t *out, const uint8_t *in);
    static vector<uint8_t> ConvertHexToVector(const uint8_t *in);
    static vector<uint8_t> ConvertHexToVector(const String &str);

    static uint8_t HexToInt(const uint8_t s);
    static String VectorToString(const vector<uint8_t> *buf);
    static String PlainVectorToString(const vector<uint8_t> *buf);
    static String ConvertBytesToHex(const uint8_t *bytes, const int length);

    static void ConvertHexToBytes(uint8_t *_dst, const char *_src, const int length);
    static String ConvertBase(const int from, const int to, const char *s);
    static String ConvertBase(const int from, const int to, const String &s);
    static String ConvertDecimal(const int decimal, const String &s);
    static String ConvertDecimal(const int decimal, const char *s);
    static String ConvertHexToASCII(const char *result, const size_t length);
    static String ConvertHexToASCII(const String &result, const size_t length);
    static String InterpretStringResult(const char *result);
    static vector<String> *InterpretVectorResult(String &result);
    static String PadForward(const String &target, const int targetSize);
    static String PadForward(const char *target, const int targetSize);
    static uint256_t ConvertToWei(const double val, const int decimals);

    static String ConvertWeiToEthString(const uint256_t &weiVal);
    static String ConvertWeiToEthString(const uint256_t &weiVal, const int decimals);

    static String ConvertUint256ToString(const uint256_t &value);
    static String ConvertInt256ToString(const _int256_t &value);
    static String ConvertUint256ToStringWithDecimal(const uint256_t &value, const int decimals);
    static String ConvertInt256ToStringWithDecimal(const _int256_t &value, const int decimals);
    static uint256_t ConvertDecimalToUint256(const double val);
    static uint256_t ConvertDecimalToUint256(const double val, const int decimals);

    static vector<String> *ConvertCharStrToVector32(const char *resultPtr, const size_t resultSize, vector<String> *result);
    static vector<String> *ConvertResultToArray(const String &result);
    static vector<String> *ConvertStringHexToABIArray(const String &value);

    static String ConvertEthToWei(const double eth);

    static String ConvertIntegerToBytes(const int32_t value);

    static String Substr(const String &result, const size_t start, const size_t length);

    // signed int utils (temporary functions)
    static _int256_t Int256FromSigned(const int8_t value);
    static _int256_t Int256FromSigned(const int16_t value);
    static _int256_t Int256FromSigned(const int32_t value);
    static _int256_t Int256FromSigned(const int64_t value);
    static _int256_t Int256FromUint256(const uint256_t &value);

private:
    static uint8_t ConvertCharToByte(const uint8_t *ptr);
};

#endif // WEB3JBC_UTIL_H

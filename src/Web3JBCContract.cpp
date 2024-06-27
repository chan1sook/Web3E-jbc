// Web3E Contract handling code
// by Natthawat Raocharoensinp (chan1sook)
//
// Based on Web3E by James Brown.

#include "Web3JBCContract.h"

/**
 * Public functions
 * */

Web3JBCContract::Web3JBCContract(Web3JBC *_web3, const char *address) : crypto(_web3)
{
    web3 = _web3;
    // _bytesBuffers.reserve(256);
    contractAddress = address;
    options.gas = 0;
    strcpy(options.from, "");
    strcpy(options.to, "");
    strcpy(options.gasPrice, "0");
}

Web3JBCContract::Web3JBCContract(Web3JBC *_web3, const String &address) : Web3JBCContract(_web3, address.c_str())
{
}

void Web3JBCContract::SetPrivateKey(const char *key)
{
    crypto.SetPrivateKey(key);
}

String Web3JBCContract::SetupContractData(const char *func, ...)
{
    String ret = GenerateContractBytes(func);
    size_t paramCount = 0;
    vector<String> params;
    char *p;
    char tmp[strlen(func)];
    memset(tmp, 0, sizeof(tmp));
    strcpy(tmp, func);
    strtok(tmp, "(");
    p = strtok(0, "(");
    p = strtok(p, ")");
    p = strtok(p, ",");
    if (p != 0)
    {
        params.push_back(p);
        paramCount++;
    }
    while (p != 0)
    {
        p = strtok(0, ",");
        if (p != 0)
        {
            params.push_back(p);
            paramCount++;
        }
    }

    vector<string> abiBlocks;
    vector<bool> isDynamic;
    int dynamicStartPointer = 0;

    va_list args;
    va_start(args, func);
    for (int i = 0; i < paramCount; ++i)
    {
        if (strstr(params[i].c_str(), "uint") != NULL && strstr(params[i].c_str(), "[]") != NULL)
        {
            // value array
            string output = GenerateBytesForUIntArray(va_arg(args, vector<uint32_t> *));
            abiBlocks.push_back(output);
            isDynamic.push_back(false);
            dynamicStartPointer += 0x20;
        }
        else if (params[i].startsWith("uint"))
        {
            string output = GenerateBytesForUint(va_arg(args, uint256_t *));
            abiBlocks.push_back(output);
            isDynamic.push_back(false);
            dynamicStartPointer += 0x20;
        }
        else if (params[i].startsWith("int")) // use uint256
        {
            string output = GenerateBytesForUint(va_arg(args, uint256_t *));
            abiBlocks.push_back(output);
            isDynamic.push_back(false);
            dynamicStartPointer += 0x20;
        }
        // TODO handle uint/int others range than 256
        else if (params[i].startsWith("bool"))
        {
            string output = GenerateBytesForInt(va_arg(args, int32_t));
            abiBlocks.push_back(output);
            isDynamic.push_back(false);
            dynamicStartPointer += 0x20;
        }
        else if (params[i].startsWith("address"))
        {
            string output = GenerateBytesForAddress(va_arg(args, const char *));
            abiBlocks.push_back(output);
            isDynamic.push_back(false);
            dynamicStartPointer += 0x20;
        }
        else if (params[i].startsWith("string"))
        {
            string output = GenerateBytesForString(va_arg(args, const char *));
            abiBlocks.push_back(output);
            isDynamic.push_back(true);
            dynamicStartPointer += 0x20;
        }
        else if (params[i].startsWith("bytes")) // if sending bytes, take the value in hex
        {
            string output = GenerateBytesForHexBytes(va_arg(args, const char *));
            abiBlocks.push_back(output);
            isDynamic.push_back(true);
            dynamicStartPointer += 0x20;
        }
        else if (strncmp(params[i].c_str(), "struct", sizeof("struct")) == 0) // if sending bytes, take the value in hex
        {
            string output = GenerateBytesForStruct(va_arg(args, const char *));
            abiBlocks.push_back(output);
            isDynamic.push_back(true);
            dynamicStartPointer += 0x20;
        }
    }
    va_end(args);

    uint256_t abiOffet = uint256_t(dynamicStartPointer);
    // now build output - parse 1, standard params
    for (int i = 0; i < paramCount; ++i)
    {
        if (isDynamic[i])
        {
            ret += abiOffet.str(16, 64).c_str();
            string *outputHex = &abiBlocks[i];
            abiOffet += outputHex->size() / 2;
        }
        else
        {
            ret += abiBlocks[i].c_str();
        }
    }

    // parse 2: add dynamic params
    for (int i = 0; i < paramCount; ++i)
    {
        if (isDynamic[i])
        {
            ret += abiBlocks[i].c_str();
        }
    }

    return ret;
}

String Web3JBCContract::ViewCall(const String &param)
{
    return web3->eth_call(param, contractAddress);
}

String Web3JBCContract::Call(const String &param)
{
    const String from = options.from;
    const long gasPrice = strtol(options.gasPrice, nullptr, 10);
    const String value = "";

    return web3->eth_call(from, contractAddress, options.gas, gasPrice, value, param);
}

String Web3JBCContract::SendTransaction(const uint32_t nonceVal, const uint64_t gasPriceVal, const uint64_t gasLimitVal,
                                        const String &toStr, const uint256_t &valueStr, const String &dataStr)
{
    uint8_t _signature[SIGNATURE_LENGTH];
    memset(_signature, 0, SIGNATURE_LENGTH);
    int recid[1] = {0};
    GenerateSignature(_signature, recid, nonceVal, gasPriceVal, gasLimitVal, toStr, valueStr, dataStr);
    vector<uint8_t> param = RlpEncodeForRawTransaction(nonceVal, gasPriceVal, gasLimitVal,
                                                       toStr, valueStr, dataStr,
                                                       _signature, recid[0]);
    String paramStr = Web3JBCUtil::VectorToString(&param);
    return web3->eth_sendRawTransaction(paramStr, param.size());
}

String Web3JBCContract::SignTransaction(const uint32_t nonceVal, const uint64_t gasPriceVal, const uint64_t gasLimitVal, const String &toStr,
                                        const uint256_t &valueStr, const String &dataStr)
{
    uint8_t _signature[SIGNATURE_LENGTH];
    memset(_signature, 0, SIGNATURE_LENGTH);
    int recid[1] = {0};

    GenerateSignature(_signature, recid, nonceVal, gasPriceVal, gasLimitVal,
                      toStr, valueStr, dataStr);

    vector<uint8_t> param = RlpEncodeForRawTransaction(nonceVal, gasPriceVal, gasLimitVal,
                                                       toStr, valueStr, dataStr,
                                                       _signature, recid[0]);
    return Web3JBCUtil::VectorToString(&param);
}

/**
 * Private functions
 **/

void Web3JBCContract::GenerateSignature(uint8_t *signature, int *recid, const uint32_t nonceVal, const uint64_t gasPriceVal, const uint64_t gasLimitVal,
                                        const String &toStr, const uint256_t &valueStr, const String &dataStr)
{
    vector<uint8_t> encoded = RlpEncode(nonceVal, gasPriceVal, gasLimitVal, toStr, valueStr, dataStr);
    // hash
    String _buffer = Web3JBCUtil::VectorToString(&encoded);

    size_t encodedTxBytesLength = (_buffer.length() - 2) / 2;
    uint8_t _byteBuffer[encodedTxBytesLength];
    uint8_t _signHash[ETHERS_KECCAK256_LENGTH];
    Web3JBCUtil::ConvertHexToBytes(_byteBuffer, _buffer.c_str(), encodedTxBytesLength);
    Crypto::Keccak256(_byteBuffer, encodedTxBytesLength, _signHash);

    // sign
    Sign(_signHash, signature, recid);
}

String Web3JBCContract::GenerateContractBytes(const char *func)
{
    String in = Web3JBCUtil::ConvertBytesToHex((const uint8_t *)func, strlen(func));
    // get the hash of the input
    std::vector<uint8_t> contractBytes = Web3JBCUtil::ConvertHexToVector(in);
    String out = Crypto::Keccak256(&contractBytes);
    return out.substring(0, 10);
}

string Web3JBCContract::GenerateBytesForUint(const uint256_t *value)
{
    std::vector<uint8_t> bits = value->export_bits();
    return std::string(Web3JBCUtil::PlainVectorToString(&bits).c_str());
}

string Web3JBCContract::GenerateBytesForInt(const int32_t value)
{
    return string(56, '0') + std::string(Web3JBCUtil::ConvertIntegerToBytes(value).c_str());
}

string Web3JBCContract::GenerateBytesForUIntArray(const vector<uint32_t> *v)
{
    string dynamicMarker = std::string(64, '0');
    dynamicMarker.at(62) = '4'; // 0x000...40 Array Designator
    string arraySize = GenerateBytesForInt(v->size());
    string output = dynamicMarker + arraySize;
    for (auto itr = v->begin(); itr != v->end(); itr++)
    {
        output += GenerateBytesForInt(*itr);
    }

    return output;
}

string Web3JBCContract::GenerateBytesForAddress(const char *v)
{
    string cleaned = v;
    if (cleaned.at(0) == 'x')
        cleaned = cleaned.substr(1);
    else if (cleaned.at(1) == 'x')
        cleaned = cleaned.substr(2);
    size_t digits = cleaned.length();
    return string(64 - digits, '0') + cleaned;
}

string Web3JBCContract::GenerateBytesForString(const char *value)
{
    return GenerateBytesForBytes(value, strlen(value));
}

string Web3JBCContract::GenerateBytesForHexBytes(const char *value)
{
    string cleaned = value;
    if (cleaned.at(0) == 'x')
        cleaned = cleaned.substr(1);
    else if (cleaned.at(1) == 'x')
        cleaned = cleaned.substr(2);
    string digitsStr = String(cleaned.length() / 2, 16).c_str(); // bytes length will be hex length / 2
    string lengthDesignator = string(64 - digitsStr.length(), '0') + digitsStr;
    cleaned = lengthDesignator + cleaned;
    size_t digits = cleaned.length() % 64;
    return cleaned + (digits > 0 ? string(64 - digits, '0') : "");
}

string Web3JBCContract::GenerateBytesForStruct(const char *value)
{
    // struct has no length params: not required
    string cleaned = value;
    if (cleaned.at(0) == 'x')
        cleaned = cleaned.substr(1);
    else if (cleaned.at(1) == 'x')
        cleaned = cleaned.substr(2);
    size_t digits = cleaned.length() % 64;
    return cleaned + (digits > 0 ? string(64 - digits, '0') : "");
}

string Web3JBCContract::GenerateBytesForBytes(const char *value, const int len)
{
    String s = Web3JBCUtil::ConvertBytesToHex((const uint8_t *)value, len);
    string bytesStr = s.substring(2).c_str(); // clean hex prefix;
    size_t digits = bytesStr.length() % 64;
    return bytesStr + (digits > 0 ? string(64 - digits, '0') : "");
}

vector<uint8_t> Web3JBCContract::RlpEncode(
    const uint32_t nonceVal, const uint64_t gasPriceVal, const uint64_t gasLimitVal,
    const String &toStr, const uint256_t &val, const String &dataStr)
{
    vector<uint8_t> nonce = Web3JBCUtil::ConvertNumberToVector(nonceVal);
    vector<uint8_t> gasPrice = Web3JBCUtil::ConvertNumberToVector(gasPriceVal);
    vector<uint8_t> gasLimit = Web3JBCUtil::ConvertNumberToVector(gasLimitVal);
    vector<uint8_t> to = Web3JBCUtil::ConvertHexToVector(toStr);
    vector<uint8_t> value = val.export_bits_truncate();
    vector<uint8_t> data = Web3JBCUtil::ConvertHexToVector(dataStr);
    vector<uint8_t> chainId = Web3JBCUtil::ConvertNumberToVector(uint64_t(web3->getChainId()));

    vector<uint8_t> zero = Web3JBCUtil::ConvertHexToVector("0");
    vector<uint8_t> outputNonce = Web3JBCUtil::RlpEncodeItemWithVector(nonce);
    vector<uint8_t> outputGasPrice = Web3JBCUtil::RlpEncodeItemWithVector(gasPrice);
    vector<uint8_t> outputGasLimit = Web3JBCUtil::RlpEncodeItemWithVector(gasLimit);
    vector<uint8_t> outputTo = Web3JBCUtil::RlpEncodeItemWithVector(to);
    vector<uint8_t> outputValue = Web3JBCUtil::RlpEncodeItemWithVector(value);
    vector<uint8_t> outputData = Web3JBCUtil::RlpEncodeItemWithVector(data);

    vector<uint8_t> outputChainId = Web3JBCUtil::RlpEncodeItemWithVector(chainId);
    vector<uint8_t> outputZero = Web3JBCUtil::RlpEncodeItemWithVector(zero);

    vector<uint8_t> encoded = Web3JBCUtil::RlpEncodeWholeHeaderWithVector(
        outputNonce.size() +
        outputGasPrice.size() +
        outputGasLimit.size() +
        outputTo.size() +
        outputValue.size() +
        outputData.size() +
        outputChainId.size() +
        outputZero.size() +
        outputZero.size());

    encoded.insert(encoded.end(), outputNonce.begin(), outputNonce.end());
    encoded.insert(encoded.end(), outputGasPrice.begin(), outputGasPrice.end());
    encoded.insert(encoded.end(), outputGasLimit.begin(), outputGasLimit.end());
    encoded.insert(encoded.end(), outputTo.begin(), outputTo.end());
    encoded.insert(encoded.end(), outputValue.begin(), outputValue.end());
    encoded.insert(encoded.end(), outputData.begin(), outputData.end());

    encoded.insert(encoded.end(), outputChainId.begin(), outputChainId.end());
    encoded.insert(encoded.end(), outputZero.begin(), outputZero.end());
    encoded.insert(encoded.end(), outputZero.begin(), outputZero.end());

    return encoded;
}

void Web3JBCContract::Sign(uint8_t *hash, uint8_t *sig, int *recid)
{
    uint8_t fullSig[65];
    crypto.Sign(hash, fullSig);
    *recid = fullSig[64];
    memcpy(sig, fullSig, 64);
}

vector<uint8_t> Web3JBCContract::RlpEncodeForRawTransaction(
    const uint32_t nonceVal, const uint64_t gasPriceVal, const uint64_t gasLimitVal,
    const String &toStr, const uint256_t &val, const String &dataStr, const uint8_t *sig, const uint8_t recid)
{

    vector<uint8_t> signature;
    for (int i = 0; i < SIGNATURE_LENGTH; i++)
    {
        signature.push_back(sig[i]);
    }
    vector<uint8_t> nonce = Web3JBCUtil::ConvertNumberToVector(nonceVal);
    vector<uint8_t> gasPrice = Web3JBCUtil::ConvertNumberToVector(gasPriceVal);
    vector<uint8_t> gasLimit = Web3JBCUtil::ConvertNumberToVector(gasLimitVal);
    vector<uint8_t> to = Web3JBCUtil::ConvertHexToVector(toStr);
    vector<uint8_t> value = val.export_bits_truncate();
    vector<uint8_t> data = Web3JBCUtil::ConvertHexToVector(dataStr);

    vector<uint8_t> outputNonce = Web3JBCUtil::RlpEncodeItemWithVector(nonce);
    vector<uint8_t> outputGasPrice = Web3JBCUtil::RlpEncodeItemWithVector(gasPrice);
    vector<uint8_t> outputGasLimit = Web3JBCUtil::RlpEncodeItemWithVector(gasLimit);
    vector<uint8_t> outputTo = Web3JBCUtil::RlpEncodeItemWithVector(to);
    vector<uint8_t> outputValue = Web3JBCUtil::RlpEncodeItemWithVector(value);
    vector<uint8_t> outputData = Web3JBCUtil::RlpEncodeItemWithVector(data);

    vector<uint8_t> R;
    R.insert(R.end(), signature.begin(), signature.begin() + (SIGNATURE_LENGTH / 2));
    vector<uint8_t> S;
    S.insert(S.end(), signature.begin() + (SIGNATURE_LENGTH / 2), signature.end());
    // V.push_back((uint8_t)(recid + web3->getChainId() * 2 + 35)); // according to EIP-155
    uint256_t vv = recid + (web3->getChainId() * 2) + 35; // EIP-155 ensure long chainIds work correctly
    vector<uint8_t> V = vv.export_bits_truncate();        // convert to bytes
    vector<uint8_t> outputR = Web3JBCUtil::RlpEncodeItemWithVector(R);
    vector<uint8_t> outputS = Web3JBCUtil::RlpEncodeItemWithVector(S);
    vector<uint8_t> outputV = Web3JBCUtil::RlpEncodeItemWithVector(V);
    vector<uint8_t> encoded = Web3JBCUtil::RlpEncodeWholeHeaderWithVector(
        outputNonce.size() +
        outputGasPrice.size() +
        outputGasLimit.size() +
        outputTo.size() +
        outputValue.size() +
        outputData.size() +
        outputR.size() +
        outputS.size() +
        outputV.size());

    encoded.insert(encoded.end(), outputNonce.begin(), outputNonce.end());
    encoded.insert(encoded.end(), outputGasPrice.begin(), outputGasPrice.end());
    encoded.insert(encoded.end(), outputGasLimit.begin(), outputGasLimit.end());
    encoded.insert(encoded.end(), outputTo.begin(), outputTo.end());
    encoded.insert(encoded.end(), outputValue.begin(), outputValue.end());
    encoded.insert(encoded.end(), outputData.begin(), outputData.end());
    encoded.insert(encoded.end(), outputV.begin(), outputV.end());
    encoded.insert(encoded.end(), outputR.begin(), outputR.end());
    encoded.insert(encoded.end(), outputS.begin(), outputS.end());

    return encoded;
}

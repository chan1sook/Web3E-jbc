// Web3E JBC
// by Natthawat Raocharoensinp (chan1sook)
//
// Based on Web3E by James Brown.

#include "Web3JBC.h"

Web3JBC::Web3JBC() : _client()
{
    _client.setInsecure();
}

// web3_clientVersion
// return empty string if unsuccessful get
String Web3JBC::web3_clientVersion()
{
    String input = _generate_json("web3_clientVersion", "[]");
    String output = _exec(input);
    return isOutputError(output) ? "" : getString(output);
}

// web3_sha3
// return empty string if unsuccessful get
String Web3JBC::web3_sha3(const String &data)
{
    String p = "[\"";
    p += data;
    p += "\"]";
    String input = _generate_json("web3_sha3", p);
    String output = _exec(input);
    return isOutputError(output) ? "" : getString(output);
}

// net_version: get chain id
// return -1 if unsuccessful get
int64_t Web3JBC::net_version()
{
    String input = _generate_json("net_version", "[]");
    String output = _exec(input);
    return isOutputError(output) ? -1 : getLongLong(output);
}

// net_listening
// return false if unsuccessful get or return false
bool Web3JBC::net_listening()
{
    String input = _generate_json("net_listening", "[]");
    String output = _exec(input);
    return isOutputError(output) ? false : _get_bool(output);
}

// net_peerCount
// return -1 if unsuccessful get
int Web3JBC::net_peerCount()
{
    String input = _generate_json("net_peerCount", "[]");
    String output = _exec(input);
    return isOutputError(output) ? -1 : getInt(output);
}

// eth_protocolVersion
// return -1 if unsuccessful get
int Web3JBC::eth_protocolVersion()
{
    String input = _generate_json("eth_protocolVersion", "[]");
    String output = _exec(input);
    return isOutputError(output) ? -1 : getInt(output);
}

// eth_syncing
// return false if unsuccessful get or return false
bool Web3JBC::eth_syncing()
{
    String input = _generate_json("eth_syncing", "[]");
    String output = _exec(input);
    return isOutputError(output) ? false : _get_bool(output);
}

// eth_mining
// return false if unsuccessful get or return false
bool Web3JBC::eth_mining()
{
    String input = _generate_json("eth_mining", "[]");
    String output = _exec(input);
    return isOutputError(output) ? false : _get_bool(output);
}

int64_t Web3JBC::eth_hashrate()
{
    String input = _generate_json("eth_hashrate", "[]");
    String output = _exec(input);
    return isOutputError(output) ? -1 : getLongLong(output);
}

// eth_gasPrice
// return -1 if unsuccessful
int64_t Web3JBC::eth_gasPrice()
{
    String input = _generate_json("eth_gasPrice", "[]");
    String output = _exec(input);
    return isOutputError(output) ? -1 : getLongLong(output);
}

// eth_gasPrice
// return -1 if unsuccessful
int64_t Web3JBC::eth_blockNumber()
{
    String input = _generate_json("eth_blockNumber", "[]");
    String output = _exec(input);
    return isOutputError(output) ? -1 : getLongLong(output);
}

uint256_t Web3JBC::eth_getBalance(const String &address)
{
    String p = "[\"";
    p += address;
    p += "\",\"latest\"]";
    String input = _generate_json("eth_getBalance", p);
    String output = _exec(input);
    return getUint256(output);
}

String Web3JBC::eth_call(const String &data, const char *to)
{
    String p = "[{\"data\":\""; // + *data;
    p += data;
    p += "\",\"to\":\"";
    p += to;
    p += "\"}, \"latest\"]";
    String input = _generate_json("eth_call", p);
    return _exec(input);
}

uint32_t Web3JBC::eth_getTransactionCount(const String &address)
{
    String p = "[\"";
    p += address;
    p += "\",\"pending\"]"; // in case we need to push several transactions in a row
    String input = _generate_json("eth_getTransactionCount", p);
    String output = _exec(input);
    return getUint(output);
}

String Web3JBC::eth_call(const String &from, const char *to, long gas, long gasPrice,
                         const String &value, const String &data)
{
    // TODO use gas, gasprice and value
    String p = "[{\"from\":\"";
    p += from;
    p += "\",\"to\":\"";
    p += to;
    p += "\",\"data\":\"";
    p += data;
    p += "\"}, \"latest\"]";
    String input = _generate_json("eth_call", p);
    return _exec(input);
}

String Web3JBC::eth_sendRawTransaction(const String &data, const uint32_t dataLen)
{
    String p = "[\"";
    p += data;
    p += "\"]";
    String input = _generate_json("eth_sendRawTransaction", p);
    return _exec(input);
}

// -------------------------------
// Private

String Web3JBC::_generate_json(const String &method, const String &params)
{
    String result = "{\"jsonrpc\":\"2.0\",\"method\":\"";
    result += method;
    result += "\",\"params\":";
    result += params;
    result += ",\"id\":0}";

    return result;
}

String Web3JBC::_exec(const String &data)
{
    const char host[] = JBC_RPC_URL;
    const char path[] = "/";
    const unsigned int port = 443;

    String result = "";

    int connected = _client.connect(host, port);
    if (!connected)
    {
        Serial.print(F("Unable to connect to Host: "));
        Serial.println(host);
        return "";
    }

    // Make a HTTP request:
    _client.print(F("POST "));
    _client.print(path);
    _client.println(F(" HTTP/1.1"));

    _client.print(F("Host: "));
    _client.println(host);
    _client.println(F("Content-Type: application/json"));

    _client.print(F("Content-Length: "));
    _client.println(data.length());
    _client.println();
    _client.println(data);

    while (_client.connected())
    {
        String line = _client.readStringUntil('\n');
        if (line[0] == '{')
        {
            result += line;
            break;
        }
    }

    // if there are incoming bytes available
    // from the server, read them and print them:
    while (_client.available())
    {
        char c = _client.read();
        result += c;
        if (c == '\n' || c == '\r')
        {
            break;
        }
    }
    _client.flush();
    _client.stop();
    return result;
}

bool Web3JBC::isOutputError(const String &json)
{
    if (json.length() == 0)
    {
        return true;
    }
    TagReader tagreader;
    String errorVal = tagreader.getTag(json, "error");
    return errorVal.length() > 0;
}

int Web3JBC::getInt(const String &json)
{
    TagReader tagreader;
    String parseVal = tagreader.getTag(json, "result");
    return strtol(parseVal.c_str(), nullptr, 16);
}

uint32_t Web3JBC::getUint(const String &json)
{
    TagReader tagreader;
    String parseVal = tagreader.getTag(json, "result");
    return strtoul(parseVal.c_str(), nullptr, 16);
}

long Web3JBC::_get_long(const String &json)
{
    TagReader tagreader;
    String parseVal = tagreader.getTag(json, "result");
    return strtol(parseVal.c_str(), nullptr, 16);
}

int64_t Web3JBC::getLongLong(const String &json)
{
    TagReader tagreader;
    String parseVal = tagreader.getTag(json, "result");
    return strtoll(parseVal.c_str(), nullptr, 16);
}

uint256_t Web3JBC::getUint256(const String &json)
{
    TagReader tagreader;
    String parseVal = tagreader.getTag(json, "result");
    return uint256_t(parseVal.c_str());
}

double Web3JBC::_get_double(const String &json)
{
    TagReader tagreader;
    String parseVal = tagreader.getTag(json, "result");
    return strtof(parseVal.c_str(), nullptr);
}

bool Web3JBC::_get_bool(const String &json)
{
    TagReader tagreader;
    String parseVal = tagreader.getTag(json, "result");
    long v = strtol(parseVal.c_str(), nullptr, 16);
    return v != 0;
}

String Web3JBC::getResult(const String &json)
{
    TagReader tagreader;
    String res = tagreader.getTag(json, "result");
    if (res.length() == 0)
    {
        return "";
    }

    if (res[0] == 'x')
        res = res.substring(1);
    else if (res[1] == 'x')
        res = res.substring(2);
    return res;
}

// Currently only works for string return eg: function name() returns (string)
String Web3JBC::getString(const String &json)
{
    TagReader tagreader;
    String parseVal = tagreader.getTag(json, "result");
    if (parseVal.length() == 0)
    {
        return "";
    }

    vector<String> *v = Web3JBCUtil::ConvertStringHexToABIArray(parseVal);

    uint256_t length = uint256_t(v->at(1).c_str());
    uint32_t lengthIndex = length;

    String asciiHex;
    int index = 2;
    while (lengthIndex > 0)
    {
        Serial.println(index);
        asciiHex += v->at(index++);
        lengthIndex -= 32;
    }

    // convert ascii into string
    String text = Web3JBCUtil::ConvertHexToASCII(asciiHex.substring(0, length * 2), length * 2);
    delete v;

    return text;
}

int64_t Web3JBC::getChainId() const
{
    return JBC_ID;
}
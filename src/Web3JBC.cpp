// Web3E JBC
// by Natthawat Raocharoensinp (chan1sook)
// Based on Web3E by James Brown.

#include "Web3JBC.h"
#include "Web3Util.h"
#include <iostream>
#include <sstream>

Web3JBC::Web3JBC() : _client(), _tagreader()
{
    _client.setInsecure();
}

// web3_clientVersion
// return empty string if unsuccessful get
string Web3JBC::web3_clientVersion()
{
    string m = "web3_clientVersion";
    string p = "[]";
    string input = _generate_json(&m, &p);
    string output = _exec(&input);
    return isOutputError(&output) ? "" : getString(&output);
}

// web3_sha3
// return empty string if unsuccessful get
string Web3JBC::web3_sha3(const string *data)
{
    string m = "web3_sha3";
    string p = "[\"" + *data + "\"]";
    string input = _generate_json(&m, &p);
    string output = _exec(&input);
    return isOutputError(&output) ? "" : getString(&output);
}

// net_version: get chain id
// return -1 if unsuccessful get
long long int Web3JBC::net_version()
{
    string m = "net_version";
    string p = "[]";
    string input = _generate_json(&m, &p);
    string output = _exec(&input);
    return isOutputError(&output) ? -1 : getLongLong(&output);
}

// net_listening
// return false if unsuccessful get or return false
bool Web3JBC::net_listening()
{
    string m = "net_listening";
    string p = "[]";
    string input = _generate_json(&m, &p);
    string output = _exec(&input);
    return isOutputError(&output) ? false : _get_bool(&output);
}

// net_peerCount
// return -1 if unsuccessful get
int Web3JBC::net_peerCount()
{
    string m = "net_peerCount";
    string p = "[]";
    string input = _generate_json(&m, &p);
    string output = _exec(&input);
    return isOutputError(&output) ? -1 : getInt(&output);
}

// eth_protocolVersion
// return -1 if unsuccessful get
int Web3JBC::eth_protocolVersion()
{
    string m = "eth_protocolVersion";
    string p = "[]";
    string input = _generate_json(&m, &p);
    string output = _exec(&input);
    return isOutputError(&output) ? -1 : getInt(&output);
}

// eth_syncing
// return false if unsuccessful get or return false
bool Web3JBC::eth_syncing()
{
    string m = "eth_syncing";
    string p = "[]";
    string input = _generate_json(&m, &p);
    string output = _exec(&input);
    return isOutputError(&output) ? false : _get_bool(&output);
}

// eth_mining
// return false if unsuccessful get or return false
bool Web3JBC::eth_mining()
{
    string m = "eth_mining";
    string p = "[]";
    string input = _generate_json(&m, &p);
    string output = _exec(&input);
    return isOutputError(&output) ? false : _get_bool(&output);
}

long long int Web3JBC::eth_hashrate()
{
    string m = "eth_hashrate";
    string p = "[]";
    string input = _generate_json(&m, &p);
    string output = _exec(&input);
    return isOutputError(&output) ? -1 : getLongLong(&output);
}

// eth_gasPrice
// return -1 if unsuccessful
long long int Web3JBC::eth_gasPrice()
{
    string m = "eth_gasPrice";
    string p = "[]";
    string input = _generate_json(&m, &p);
    string output = _exec(&input);
    return isOutputError(&output) ? -1 : getLongLong(&output);
}

void Web3JBC::EthAccounts(char **array, int size)
{
    // JBC not store account then do nothings
}

// eth_gasPrice
// return -1 if unsuccessful
long long int Web3JBC::eth_blockNumber()
{
    string m = "eth_blockNumber";
    string p = "[]";
    string input = _generate_json(&m, &p);
    string output = _exec(&input);
    return isOutputError(&output) ? -1 : getLongLong(&output);
}

uint256_t Web3JBC::eth_getBalance(const string *address)
{
    string m = "eth_getBalance";
    string p = "[\"" + *address + "\",\"latest\"]";
    string input = _generate_json(&m, &p);
    string output = _exec(&input);
    return getUint256(&output);
}

string Web3JBC::eth_call(const string *data, const char *to)
{
    string m = "eth_call";
    string p = "[{\"data\":\""; // + *data;
    p += data->c_str();
    p += "\",\"to\":\"";
    p += to;
    p += "\"}, \"latest\"]";
    string input = _generate_json(&m, &p);
    return _exec(&input);
}

int Web3JBC::eth_getTransactionCount(const string *address)
{
    string m = "eth_getTransactionCount";
    string p = "[\"" + *address + "\",\"pending\"]"; // in case we need to push several transactions in a row
    string input = _generate_json(&m, &p);
    string output = _exec(&input);
    return getInt(&output);
}

string Web3JBC::eth_call(const string *from, const char *to, long gas, long gasPrice,
                         const string *value, const string *data)
{
    // TODO use gas, gasprice and value
    string m = "eth_call";
    string p = "[{\"from\":\"" + *from + "\",\"to\":\"" + *to + "\",\"data\":\"" + *data + "\"}, \"latest\"]";
    string input = _generate_json(&m, &p);
    return _exec(&input);
}

string Web3JBC::eth_sendRawTransaction(const string *data, const uint32_t dataLen)
{
    string m = "eth_sendRawTransaction";
    string p = "[\"" + *data + "\"]";
    string input = _generate_json(&m, &p);
#if 0
    LOG(input);
#endif
    return _exec(&input);
}

// -------------------------------
// Private

string Web3JBC::_generate_json(const string *method, const string *params)
{
    return "{\"jsonrpc\":\"2.0\",\"method\":\"" + *method + "\",\"params\":" + *params + ",\"id\":0}";
}

string Web3JBC::_exec(const string *data)
{
    const char host[] = JBC_RPC_URL;
    const char path[] = "/";
    const unsigned int port = 443;

    string result;

    int connected = _client.connect(host, port);
    if (!connected)
    {
        Serial.print("Unable to connect to Host: ");
        Serial.println(host);
        return "";
    }

    // Make a HTTP request:
    int l = data->size();
    stringstream ss;
    ss << l;
    string lstr = ss.str();

    string strPost = "POST " + string(path) + " HTTP/1.1";
    string strHost = "Host: " + string(host);
    string strContentLen = "Content-Length: " + lstr;

    _client.println(strPost.c_str());
    _client.println(strHost.c_str());
    _client.println("Content-Type: application/json");
    _client.println(strContentLen.c_str());
    _client.println();
    _client.println(data->c_str());

    while (_client.connected())
    {
        String line = _client.readStringUntil('\n');
        if (line == "\r")
        {
            break;
        }
    }

    // if there are incoming bytes available
    // from the server, read them and print them:
    while (_client.available())
    {
        char c = _client.read();
        result += c;
    }
    _client.flush();
    _client.stop();

    return result;
}

bool Web3JBC::isOutputError(const string *json)
{
    if (json->length() == 0)
    {
        return true;
    }
    string errorVal = _tagreader.getTag(json, "error");
    return errorVal.length() > 0;
}

int Web3JBC::getInt(const string *json)
{
    string parseVal = _tagreader.getTag(json, "result");
    return strtol(parseVal.c_str(), nullptr, 16);
}

long Web3JBC::_get_long(const string *json)
{
    string parseVal = _tagreader.getTag(json, "result");
    return strtol(parseVal.c_str(), nullptr, 16);
}

long long int Web3JBC::getLongLong(const string *json)
{
    string parseVal = _tagreader.getTag(json, "result");
    return strtoll(parseVal.c_str(), nullptr, 16);
}

uint256_t Web3JBC::getUint256(const string *json)
{
    string parseVal = _tagreader.getTag(json, "result");
    return uint256_t(parseVal.c_str());
}

double Web3JBC::_get_double(const string *json)
{
    string parseVal = _tagreader.getTag(json, "result");
    return strtof(parseVal.c_str(), nullptr);
}

bool Web3JBC::_get_bool(const string *json)
{
    string parseVal = _tagreader.getTag(json, "result");
    long v = strtol(parseVal.c_str(), nullptr, 16);
    return v != 0;
}

string Web3JBC::getResult(const string *json)
{
    string res = _tagreader.getTag(json, "result");
    if (res.length() == 0)
    {
        return string("");
    }

    if (res.at(0) == 'x')
        res = res.substr(1);
    else if (res.at(1) == 'x')
        res = res.substr(2);
    return res;
}

// Currently only works for string return eg: function name() returns (string)
string Web3JBC::getString(const string *json)
{
    string parseVal = _tagreader.getTag(json, "result");
    if (parseVal.length() == 0)
    {
        return string("");
    }

    vector<string> *v = Web3Util::ConvertStringHexToABIArray(&parseVal);

    uint256_t length = uint256_t(v->at(1));
    uint32_t lengthIndex = length;

    string asciiHex;
    int index = 2;
    while (lengthIndex > 0)
    {
        Serial.println(index);
        asciiHex += v->at(index++);
        lengthIndex -= 32;
    }

    // convert ascii into string
    string text = Web3Util::ConvertHexToASCII(asciiHex.substr(0, length * 2).c_str(), length * 2);
    delete v;

    return text;
}

long long Web3JBC::getChainId() const
{
    return JBC_ID;
}
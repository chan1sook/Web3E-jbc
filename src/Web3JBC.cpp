// Web3E JBC
// by Natthawat Raocharoensinp (chan1sook)
// Based on Web3E by James Brown.

#include "Web3JBC.h"
#include "Certificates.h"
#include "Util.h"
#include "TagReader/TagReader.h"
#include <iostream>
#include <sstream>
#include "nodes.h"

Web3JBC::Web3JBC()
{
    infura_key = INFURA_API_KEY;
    mem = new BYTE[sizeof(WiFiClientSecure)];
    chainId = 8899;
    selectHost();
}

string Web3JBC::Web3ClientVersion()
{
    string m = "web3_clientVersion";
    string p = "[]";
    string input = generateJson(&m, &p);
    string output = exec(&input);
    return getString(&output);
}

string Web3JBC::Web3Sha3(const string *data)
{
    string m = "web3_sha3";
    string p = "[\"" + *data + "\"]";
    string input = generateJson(&m, &p);
    string output = exec(&input);
    return getString(&output);
}

int Web3JBC::NetVersion()
{
    string m = "net_version";
    string p = "[]";
    string input = generateJson(&m, &p);
    string output = exec(&input);
    return getInt(&output);
}

bool Web3JBC::NetListening()
{
    string m = "net_listening";
    string p = "[]";
    string input = generateJson(&m, &p);
    string output = exec(&input);
    return getBool(&output);
}

int Web3JBC::NetPeerCount()
{
    string m = "net_peerCount";
    string p = "[]";
    string input = generateJson(&m, &p);
    string output = exec(&input);
    return getInt(&output);
}

double Web3JBC::EthProtocolVersion()
{
    string m = "eth_protocolVersion";
    string p = "[]";
    string input = generateJson(&m, &p);
    string output = exec(&input);
    return getDouble(&output);
}

bool Web3JBC::EthSyncing()
{
    string m = "eth_syncing";
    string p = "[]";
    string input = generateJson(&m, &p);
    string result = exec(&input);

    return getBool(&result);
}

bool Web3JBC::EthMining()
{
    string m = "eth_mining";
    string p = "[]";
    string input = generateJson(&m, &p);
    string output = exec(&input);
    return getBool(&output);
}

double Web3JBC::EthHashrate()
{
    string m = "eth_hashrate";
    string p = "[]";
    string input = generateJson(&m, &p);
    string output = exec(&input);
    return getDouble(&output);
}

long long int Web3JBC::EthGasPrice()
{
    string m = "eth_gasPrice";
    string p = "[]";
    string input = generateJson(&m, &p);
    string output = exec(&input);
    return getLongLong(&output);
}

void Web3JBC::EthAccounts(char **array, int size)
{
    // TODO
}

int Web3JBC::EthBlockNumber()
{
    string m = "eth_blockNumber";
    string p = "[]";
    string input = generateJson(&m, &p);
    string output = exec(&input);
    return getInt(&output);
}

uint256_t Web3JBC::EthGetBalance(const string *address)
{
    string m = "eth_getBalance";
    string p = "[\"" + *address + "\",\"latest\"]";
    string input = generateJson(&m, &p);
    string output = exec(&input);
    return getUint256(&output);
}

string Web3JBC::EthViewCall(const string *data, const char *to)
{
    string m = "eth_call";
    string p = "[{\"data\":\""; // + *data;
    p += data->c_str();
    p += "\",\"to\":\"";
    p += to;
    p += "\"}, \"latest\"]";
    string input = generateJson(&m, &p);
    return exec(&input);
}

int Web3JBC::EthGetTransactionCount(const string *address)
{
    string m = "eth_getTransactionCount";
    string p = "[\"" + *address + "\",\"pending\"]"; // in case we need to push several transactions in a row
    string input = generateJson(&m, &p);
    string output = exec(&input);
    return getInt(&output);
}

string Web3JBC::EthCall(const string *from, const char *to, long gas, long gasPrice,
                        const string *value, const string *data)
{
    // TODO use gas, gasprice and value
    string m = "eth_call";
    string p = "[{\"from\":\"" + *from + "\",\"to\":\"" + *to + "\",\"data\":\"" + *data + "\"}, \"latest\"]";
    string input = generateJson(&m, &p);
    return exec(&input);
}

string Web3JBC::EthSendSignedTransaction(const string *data, const uint32_t dataLen)
{
    string m = "eth_sendRawTransaction";
    string p = "[\"" + *data + "\"]";
    string input = generateJson(&m, &p);
#if 0
    LOG(input);
#endif
    return exec(&input);
}

// -------------------------------
// Private

string Web3JBC::generateJson(const string *method, const string *params)
{
    return "{\"jsonrpc\":\"2.0\",\"method\":\"" + *method + "\",\"params\":" + *params + ",\"id\":0}";
}

string Web3JBC::exec(const string *data)
{
    string result;

    client = new (mem) WiFiClientSecure();
    setupCert();

    int connected = client->connect(host, port);
    if (!connected)
    {
        Serial.print("Unable to connect to Host: ");
        Serial.println(host);
        delay(100);
        // trigger a reset of the device
        ESP.restart();
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

    client->println(strPost.c_str());
    client->println(strHost.c_str());
    client->println("Content-Type: application/json");
    client->println(strContentLen.c_str());
    client->println();
    client->println(data->c_str());

    while (client->connected())
    {
        String line = client->readStringUntil('\n');
        if (line == "\r")
        {
            break;
        }
    }

    // if there are incoming bytes available
    // from the server, read them and print them:
    while (client->available())
    {
        char c = client->read();
        result += c;
    }
    client->flush();
    client->stop();

    client->~WiFiClientSecure();

    return result;
}

int Web3JBC::getInt(const string *json)
{
    TagReader reader;
    string parseVal = reader.getTag(json, "result");
    return strtol(parseVal.c_str(), nullptr, 16);
}

long Web3JBC::getLong(const string *json)
{
    TagReader reader;
    string parseVal = reader.getTag(json, "result");
    return strtol(parseVal.c_str(), nullptr, 16);
}

long long int Web3JBC::getLongLong(const string *json)
{
    TagReader reader;
    string parseVal = reader.getTag(json, "result");
    return strtoll(parseVal.c_str(), nullptr, 16);
}

uint256_t Web3JBC::getUint256(const string *json)
{
    TagReader reader;
    string parseVal = reader.getTag(json, "result");
    return uint256_t(parseVal.c_str());
}

double Web3JBC::getDouble(const string *json)
{
    TagReader reader;
    string parseVal = reader.getTag(json, "result");
    return strtof(parseVal.c_str(), nullptr);
}

bool Web3JBC::getBool(const string *json)
{
    TagReader reader;
    string parseVal = reader.getTag(json, "result");
    long v = strtol(parseVal.c_str(), nullptr, 16);
    return v > 0;
}

string Web3JBC::getResult(const string *json)
{
    TagReader reader;
    string res = reader.getTag(json, "result");
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
    TagReader reader;
    string parseVal = reader.getTag(json, "result");
    if (parseVal.length() == 0)
    {
        return string("");
    }

    vector<string> *v = Util::ConvertStringHexToABIArray(&parseVal);

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
    string text = Util::ConvertHexToASCII(asciiHex.substr(0, length * 2).c_str(), length * 2);
    delete v;

    return text;
}

/**
 * @brief Fetch TLS certificate for the node
 *
 * TODO: Add remaining certificates as required
 *
 * @return const char*
 */
void Web3JBC::setupCert()
{
    const char *cert = getCertificate(chainId);
    if (cert != NULL)
    {
        client->setCACert(cert);
    }
    else
    {
        client->setInsecure();
    }
}

void Web3JBC::selectHost()
{
    std::string node = getNode(chainId);

    if (node.find("infura.io") != std::string::npos)
    {
        node += infura_key;
    }

    if (node.length() == 0)
    {
        Serial.print("ChainId: ");
        Serial.print(chainId);
        Serial.println("Is not yet supported, please add the RPC (and certificate if required) and submit a PR to the repo.");
        return;
    }

    int ppos = node.find(":");
    if (ppos > 0)
    {
        port = stoi(node.substr(ppos + 1));
        node = node.substr(0, ppos);
    }
    else
    {
        port = 443;
    }

    ppos = node.find("/");
    if (ppos > 0)
    {
        host = strdup(node.substr(0, ppos).c_str());
        path = strdup(node.substr(ppos).c_str());
    }
    else
    {
        host = strdup(node.c_str());
        path = "/";
    }
}

long long Web3JBC::getChainId() const
{
    return chainId;
}
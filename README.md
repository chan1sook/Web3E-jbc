# A fork Web3E for JBC Chain

Forked from https://github.com/AlphaWallet/Web3 for use in JBC Chain (https://exp-l1.jibchain.net/)

## Ethereum transaction (ie send ETH to address):

``` C++
// Setup Web3 and Web3JBCContract with Private Key
...
web3 = new Web3JBC();
Web3JBCContract contract(web3, "");
contract.SetPrivateKey(PRIVATE_KEY);
uint32_t nonceVal = (uint32_t)web3->EthGetTransactionCount(address); //obtain the next nonce
uint256_t weiValue = Web3JBCUtil::ConvertToWei(0.25, 18); //send 0.25 eth
uint64_t gasPriceVal = 1000000000ULL;
uint32_t gasLimitVal = 90000;
String toAddress = "0xC067A53c91258ba513059919E03B81CF93f57Ac7";
String result = contract.SendTransaction(nonceVal, gasPriceVal, gasLimitVal, toAddress, weiValue, "");
```

## Query ETH balance:
``` C++
uint256_t balance = web3->EthGetBalance(address); //obtain balance in Wei
String balanceStr = Web3JBCUtil::ConvertWeiToEthString(balance, 18); //get string balance as Eth (18 decimals)
```

# Donations
If you support the cause, we could certainly use donations to help fund development (JBC chain):

0xC70Edf3aB36E0D57eFD8f412A3460D225931A110

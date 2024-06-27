// Web3E Utilities
// by Natthawat Raocharoensinp (chan1sook)
//
// Based on Web3E by James Brown.

#include "Web3JBCUtil.h"

static const char *_web3e_hexStr = "0123456789ABCDEF";
_int256_t max_positive_val = _int256_t(0x7FFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF);

// returns output (header) length
uint32_t Web3JBCUtil::RlpEncodeWholeHeader(uint8_t *header_output, uint32_t total_len)
{
  if (total_len < 55)
  {
    header_output[0] = (uint8_t)0xc0 + (uint8_t)total_len;
    return 1;
  }
  else
  {
    uint8_t tmp_header[8];
    memset(tmp_header, 0, 8);
    uint32_t hexdigit = 1;
    uint32_t tmp = total_len;
    while ((uint32_t)(tmp / 256) > 0)
    {
      tmp_header[hexdigit] = (uint8_t)(tmp % 256);
      tmp = (uint32_t)(tmp / 256);
      hexdigit++;
    }
    tmp_header[hexdigit] = (uint8_t)(tmp);
    tmp_header[0] = (uint8_t)0xf7 + (uint8_t)hexdigit;

    // fix direction for header
    uint8_t header[8];
    memset(header, 0, 8);
    header[0] = tmp_header[0];
    for (int i = 0; i < hexdigit; i++)
    {
      header[i + 1] = tmp_header[hexdigit - i];
    }

    memcpy(header_output, header, (size_t)hexdigit + 1);
    return hexdigit + 1;
  }
}

vector<uint8_t> Web3JBCUtil::RlpEncodeWholeHeaderWithVector(uint32_t total_len)
{
  vector<uint8_t> header_output;
  if (total_len < 55)
  {
    header_output.push_back((uint8_t)0xc0 + (uint8_t)total_len);
  }
  else
  {
    vector<uint8_t> tmp_header;
    uint32_t hexdigit = 1;
    uint32_t tmp = total_len;
    while ((uint32_t)(tmp / 256) > 0)
    {
      tmp_header.push_back((uint8_t)(tmp % 256));
      tmp = (uint32_t)(tmp / 256);
      hexdigit++;
    }
    tmp_header.push_back((uint8_t)(tmp));
    tmp_header.insert(tmp_header.begin(), 0xf7 + (uint8_t)hexdigit);

    // fix direction for header
    vector<uint8_t> header;
    header.push_back(tmp_header[0]);
    for (int i = 0; i < tmp_header.size() - 1; i++)
    {
      header.push_back(tmp_header[tmp_header.size() - 1 - i]);
    }

    header_output.insert(header_output.end(), header.begin(), header.end());
  }
  return header_output;
}

// returns output length
uint32_t Web3JBCUtil::RlpEncodeItem(uint8_t *output, const uint8_t *input, uint32_t input_len)
{
  if (input_len == 1 && input[0] == 0x00)
  {
    uint8_t c[1] = {0x80};
    memcpy(output, c, 1);
    return 1;
  }
  else if (input_len == 1 && input[0] < 128)
  {
    memcpy(output, input, 1);
    return 1;
  }
  else if (input_len <= 55)
  {
    uint8_t _ = (uint8_t)0x80 + (uint8_t)input_len;
    uint8_t header[] = {_};
    memcpy(output, header, 1);
    memcpy(output + 1, input, (size_t)input_len);
    return input_len + 1;
  }
  else
  {
    uint8_t tmp_header[8];
    memset(tmp_header, 0, 8);
    uint32_t hexdigit = 1;
    uint32_t tmp = input_len;
    while ((uint32_t)(tmp / 256) > 0)
    {
      tmp_header[hexdigit] = (uint8_t)(tmp % 256);
      tmp = (uint32_t)(tmp / 256);
      hexdigit++;
    }
    tmp_header[hexdigit] = (uint8_t)(tmp);
    tmp_header[0] = (uint8_t)0xb7 + (uint8_t)hexdigit;

    // fix direction for header
    uint8_t header[8];
    memset(header, 0, 8);
    header[0] = tmp_header[0];
    for (int i = 0; i < hexdigit; i++)
    {
      header[i + 1] = tmp_header[hexdigit - i];
    }

    memcpy(output, header, hexdigit + 1);
    memcpy(output + hexdigit + 1, input, (size_t)input_len);
    return input_len + hexdigit + 1;
  }
}

vector<uint8_t> Web3JBCUtil::RlpEncodeItemWithVector(const vector<uint8_t> input)
{
  vector<uint8_t> output;
  uint16_t input_len = input.size();

  if (input_len == 1 && input[0] == 0x00)
  {
    output.push_back(0x80);
  }
  else if (input_len == 1 && input[0] < 128)
  {
    output.insert(output.end(), input.begin(), input.end());
  }
  else if (input_len <= 55)
  {
    uint8_t _ = (uint8_t)0x80 + (uint8_t)input_len;
    output.push_back(_);
    output.insert(output.end(), input.begin(), input.end());
  }
  else
  {
    vector<uint8_t> tmp_header;
    uint32_t tmp = input_len;
    while ((uint32_t)(tmp / 256) > 0)
    {
      tmp_header.push_back((uint8_t)(tmp % 256));
      tmp = (uint32_t)(tmp / 256);
    }
    tmp_header.push_back((uint8_t)(tmp));
    uint8_t len = tmp_header.size(); // + 1;
    tmp_header.insert(tmp_header.begin(), 0xb7 + len);

    // fix direction for header
    vector<uint8_t> header;
    header.push_back(tmp_header[0]);
    uint8_t hexdigit = tmp_header.size() - 1;
    for (int i = 0; i < hexdigit; i++)
    {
      header.push_back(tmp_header[hexdigit - i]);
    }

    output.insert(output.end(), header.begin(), header.end());
    output.insert(output.end(), input.begin(), input.end());
  }
  return output;
}

vector<uint8_t> Web3JBCUtil::ConvertNumberToVector(uint64_t val)
{
  vector<uint8_t> tmp;
  vector<uint8_t> ret;
  if ((uint64_t)(val / 256) >= 0)
  {
    while ((uint64_t)(val / 256) > 0)
    {
      tmp.push_back((uint8_t)(val % 256));
      val = (uint64_t)(val / 256);
    }
    tmp.push_back((uint8_t)(val % 256));
    uint8_t len = tmp.size();
    for (int i = 0; i < len; i++)
    {
      ret.push_back(tmp[len - i - 1]);
    }
  }
  else
  {
    ret.push_back((uint8_t)val);
  }
  return ret;
}

vector<uint8_t> Web3JBCUtil::ConvertNumberToVector(uint32_t val)
{
  return ConvertNumberToVector((uint64_t)val);
}

uint32_t Web3JBCUtil::ConvertNumberToUintArray(uint8_t *str, uint32_t val)
{
  uint32_t ret = 0;
  uint8_t tmp[8];
  memset(tmp, 0, 8);
  if ((uint32_t)(val / 256) >= 0)
  {
    while ((uint32_t)(val / 256) > 0)
    {
      tmp[ret] = (uint8_t)(val % 256);
      val = (uint32_t)(val / 256);
      ret++;
    }
    tmp[ret] = (uint8_t)(val % 256);
    for (int i = 0; i < ret + 1; i++)
    {
      str[i] = tmp[ret - i];
    }
  }
  else
  {
    str[0] = (uint8_t)val;
  }

  return ret + 1;
}

uint8_t Web3JBCUtil::ConvertCharToByte(const uint8_t *ptr)
{
  char c[3];
  c[0] = *(ptr);
  c[1] = *(ptr + 1);
  c[2] = 0x00;
  return strtol(c, nullptr, 16);
}

vector<uint8_t> Web3JBCUtil::ConvertHexToVector(const uint8_t *in)
{
  const uint8_t *ptr = in;
  vector<uint8_t> out;
  if (ptr[0] == '0' && ptr[1] == 'x')
    ptr += 2;

  size_t lenstr = strlen((const char *)ptr);
  int i = 0;
  if ((lenstr % 2) == 1) // deal with odd sized hex strings
  {
    char c[2];
    c[0] = *ptr;
    c[1] = 0;
    out.push_back(ConvertCharToByte((const uint8_t *)c));
    i = 1;
  }
  for (; i < lenstr; i += 2)
  {
    out.push_back(ConvertCharToByte(ptr + i));
  }
  return out;
}

vector<uint8_t> Web3JBCUtil::ConvertHexToVector(const String &str)
{
  return ConvertHexToVector((const uint8_t *)str.c_str());
}

uint32_t Web3JBCUtil::ConvertCharStrToUintArray(uint8_t *out, const uint8_t *in)
{
  uint32_t ret = 0;
  const uint8_t *ptr = in;
  // remove "0x"
  if (in[0] == '0' && in[1] == 'x')
    ptr += 2;

  size_t lenstr = strlen((const char *)ptr);
  for (int i = 0; i < lenstr; i += 2)
  {
    char c[3];
    c[0] = *(ptr + i);
    c[1] = *(ptr + i + 1);
    c[2] = 0x00;
    uint8_t val = strtol(c, nullptr, 16);
    out[ret] = val;
    ret++;
  }
  return ret;
};

uint8_t Web3JBCUtil::HexToInt(const uint8_t s)
{
  uint8_t ret = 0;
  if (s >= '0' && s <= '9')
  {
    ret = uint8_t(s - '0');
  }
  else if (s >= 'a' && s <= 'f')
  {
    ret = uint8_t(s - 'a' + 10);
  }
  else if (s >= 'A' && s <= 'F')
  {
    ret = uint8_t(s - 'A' + 10);
  }
  return ret;
}

String Web3JBCUtil::VectorToString(const vector<uint8_t> *buf)
{
  return ConvertBytesToHex((const uint8_t *)buf->data(), buf->size());
}

String Web3JBCUtil::ConvertIntegerToBytes(const int32_t value)
{
  size_t hex_len = 4 << 1;
  String rc(hex_len, '0');
  for (size_t i = 0, j = (hex_len - 1) * 4; i < hex_len; ++i, j -= 4)
  {
    rc[i] = _web3e_hexStr[(value >> j) & 0x0f];
  }
  return rc;
}

String Web3JBCUtil::PlainVectorToString(const vector<uint8_t> *buf)
{
  char *buffer = (char *)alloca(buf->size() * 2 + 1);
  char *pout = buffer;
  for (int i = 0; i < buf->size(); i++)
  {
    *pout++ = _web3e_hexStr[((*buf)[i] >> 4) & 0xF];
    *pout++ = _web3e_hexStr[(*buf)[i] & 0xF];
  }
  *pout = 0;
  return buffer;
}

String Web3JBCUtil::ConvertBytesToHex(const uint8_t *bytes, const int length)
{
  char *buffer = (char *)alloca(length * 2 + 3);
  char *pout = buffer;
  *pout++ = '0';
  *pout++ = 'x';
  for (int i = 0; i < length; i++)
  {
    *pout++ = _web3e_hexStr[((bytes)[i] >> 4) & 0xF];
    *pout++ = _web3e_hexStr[(bytes)[i] & 0xF];
  }
  *pout = 0;
  return buffer;
}

void Web3JBCUtil::ConvertHexToBytes(uint8_t *_dst, const char *_src, const int length)
{
  if (_src[0] == '0' && _src[1] == 'x')
    _src += 2; // chop off 0x

  for (int i = 0; i < length; i++)
  {
    char a = _src[2 * i];
    char b = _src[2 * i + 1];
    uint8_t extract = HexToInt(a) << 4 | HexToInt(b);
    _dst[i] = extract;
  }
}

String Web3JBCUtil::ConvertBase(const int from, const int to, const char *s)
{
  if (s == NULL)
    return "";

  if (from < 2 || from > 36 || to < 2 || to > 36)
  {
    return "";
  }

  if (s[0] == '0' && s[1] == 'x')
    s += 2;

  int il = strlen(s);

  int *fs = new (alloca(sizeof(int) * il)) int[il];
  int k = 0;
  int i, j;

  for (i = il - 1; i >= 0; i--)
  {
    if (s[i] >= '0' && s[i] <= '9')
    {
      fs[k] = (int)(s[i] - '0');
    }
    else
    {
      if (s[i] >= 'A' && s[i] <= 'Z')
      {
        fs[k] = 10 + (int)(s[i] - 'A');
      }
      else if (s[i] >= 'a' && s[i] <= 'z')
      {
        fs[k] = 10 + (int)(s[i] - 'a');
      }
      else
      {
        return "";
      } // only allow 0-9 A-Z characters
    }
    k++;
  }

  for (i = 0; i < il; i++)
  {
    if (fs[i] >= from)
      return "";
  }

  double x = ceil(log(from) / log(to));
  int ol = 1 + (il * x);

  int *ts = new (alloca(sizeof(int) * ol)) int[ol];
  int *cums = new (alloca(sizeof(int) * ol)) int[ol];

  for (i = 0; i < ol; i++)
  {
    ts[i] = 0;
    cums[i] = 0;
  }
  ts[0] = 1;

  // evaluate the output
  for (i = 0; i < il; i++) // for each input digit
  {
    for (j = 0; j < ol; j++) // add the input digit times (base:to from^i) to the output cumulator
    {
      cums[j] += ts[j] * fs[i];
      int temp = cums[j];
      int rem = 0;
      int ip = j;
      do // fix up any remainders in base:to
      {
        rem = temp / to;
        cums[ip] = temp - rem * to;
        ip++;
        if (ip >= ol)
        {
          if (rem > 0)
          {
            return "";
          }
          break;
        }
        cums[ip] += rem;
        temp = cums[ip];
      } while (temp >= to);
    }

    for (j = 0; j < ol; j++)
    {
      ts[j] = ts[j] * from;
    }

    for (j = 0; j < ol; j++) // check for any remainders
    {
      int temp = ts[j];
      int rem = 0;
      int ip = j;
      do // fix up any remainders
      {
        rem = temp / to;
        ts[ip] = temp - rem * to;
        ip++;
        if (ip >= ol)
        {
          if (rem > 0)
          {
            return "";
          }
          break;
        }
        ts[ip] += rem;
        temp = ts[ip];
      } while (temp >= to);
    }
  }

  char *out = (char *)alloca(sizeof(char) * (ol + 1));

  int spos = 0;
  bool first = false; // leading zero flag
  for (i = ol - 1; i >= 0; i--)
  {
    if (cums[i] != 0)
    {
      first = true;
    }
    if (!first)
    {
      continue;
    }

    if (cums[i] < 10)
    {
      out[spos] = (char)(cums[i] + '0');
    }
    else
    {
      out[spos] = (char)(cums[i] + 'A' - 10);
    }
    spos++;
  }
  out[spos] = 0;
  return out;
}

String Web3JBCUtil::ConvertBase(const int from, const int to, const String &s)
{
  return ConvertBase(from, to, s.c_str());
}

String Web3JBCUtil::ConvertDecimal(const int decimals, const String &result)
{
  int decimalLocation = result.length() - decimals;
  String newValue = "";
  if (decimalLocation <= 0)
  {
    newValue += "0.";
    for (; decimalLocation < 0; decimalLocation++)
    {
      newValue += "0";
    }
    newValue += result;
  }
  else
  {
    // need to insert the point within the string
    newValue = result.substring(0, decimalLocation);
    newValue += ".";
    newValue += result.substring(decimalLocation);
  }

  return newValue;
}

String Web3JBCUtil::ConvertDecimal(const int decimals, const char *result)
{
  return ConvertDecimal(decimals, String(result));
}

String Web3JBCUtil::ConvertHexToASCII(const char *result, const size_t length)
{
  // convert hex to string.
  // first trim all the zeros
  int index = 0;
  String converted = "";
  char reader;
  int state = 0;
  bool endOfString = false;

  // No ASCII is less than 16 so this is safe
  while (index < length && (result[index] == '0' || result[index] == 'x'))
    index++;

  while (index < length && endOfString == false)
  {
    // convert from hex to ascii
    char c = result[index];
    switch (state)
    {
    case 0:
      reader = (char)(Web3JBCUtil::HexToInt(c) * 16);
      state = 1;
      break;
    case 1:
      reader += (char)Web3JBCUtil::HexToInt(c);
      if (reader == 0)
      {
        endOfString = true;
      }
      else
      {
        converted += reader;
        state = 0;
      }
      break;
    }
    index++;
  }

  return converted;
}

String Web3JBCUtil::ConvertHexToASCII(const String &result, const size_t length)
{
  return ConvertHexToASCII(result.c_str(), length);
}
/**
 * Build a std::vector of bytes32 as hex strings
 **/
vector<String> *Web3JBCUtil::ConvertCharStrToVector32(const char *resultPtr, const size_t resultSize, vector<String> *result)
{
  if (resultSize < 64)
    return result;
  if (resultPtr[0] == '0' && resultPtr[1] == 'x')
    resultPtr += 2;
  // estimate size of return
  int returnSize = resultSize / 64;
  result->reserve(returnSize);
  int index = 0;
  char element[65];
  element[64] = 0;

  while (index <= (resultSize - 64))
  {
    memcpy(element, resultPtr, 64);
    result->push_back(element);
    resultPtr += 64;
    index += 64;
  }

  return result;
}

/**
 * @brief Only use to handle Ethereum results
 *
 * @param result
 * @return vector<string>*
 */
vector<String> *Web3JBCUtil::ConvertResultToArray(const String &value)
{
  TagReader reader;
  String result = reader.getTag(value, "result");
  return ConvertStringHexToABIArray(result);
}

vector<String> *Web3JBCUtil::ConvertStringHexToABIArray(const String &result)
{
  int startIndex = 0;
  vector<String> *retVal = new vector<String>();
  if (result[0] == 'x')
    startIndex = 1;
  else if (result[1] == 'x')
    startIndex = 2;

  int resultLength = result.length();

  while (startIndex < resultLength)
  {
    String subStr = result.substring(startIndex, startIndex + 64);
    // we should never need to pad, as this should always be multiple of 64 chars
    retVal->push_back(subStr);
    startIndex += 64;
  }

  return retVal;
}

String Web3JBCUtil::InterpretStringResult(const char *result)
{
  // convert to vector bytes32
  String retVal = "";

  if (result != NULL && strlen(result) > 0)
  {
    vector<String> breakDown;
    Web3JBCUtil::ConvertCharStrToVector32(result, strlen(result), &breakDown);

    if (breakDown.size() > 2)
    {
      // check first value
      auto itr = breakDown.begin();
      long dyn = strtol(itr++->c_str(), NULL, 16);
      if (dyn == 32) // array marker
      {
        long length = strtol(itr++->c_str(), NULL, 16);
        // now get a pointer to string immediately after the length marker
        const char *strPtr = result + 2 + (2 * 64);
        retVal = ConvertHexToASCII(strPtr, length * 2);
      }
    }
  }

  return retVal;
}

vector<String> *Web3JBCUtil::InterpretVectorResult(String &result)
{
  vector<String> *retVal = new vector<String>();
  TagReader reader;
  String value = reader.getTag(result, "result").c_str();

  if (value.length() > 0)
  {
    vector<String> breakDown;
    Web3JBCUtil::ConvertCharStrToVector32(value.c_str(), reader.length(), &breakDown);

    if (breakDown.size() > 2)
    {
      // check first value
      auto itr = breakDown.begin();
      long dyn = strtol(itr++->c_str(), NULL, 16);
      if (dyn == 32) // array marker
      {
        long length = strtol(itr++->c_str(), NULL, 16);

        // checksum
        if (breakDown.size() != (length + 2))
        {
          Serial.println("Bad array result data.");
          for (itr = breakDown.begin(); itr != breakDown.end(); itr++)
            Serial.println(*itr->c_str());
        }
        for (; itr != breakDown.end(); itr++)
        {
          retVal->push_back(*itr);
        }
      }
    }
  }

  return retVal;
}

String Web3JBCUtil::PadForward(const String &target, const int targetSize)
{
  int remain = (targetSize * 2) - (target.length() % targetSize);
  char buffer[remain + 1];
  memset(buffer, '0', remain);
  buffer[remain] = 0;
  String r = buffer;
  r += target;
  return r;
}
String Web3JBCUtil::PadForward(const char *target, const int targetSize)
{
  return PadForward(String(target), targetSize);
}

uint256_t Web3JBCUtil::ConvertToWei(const double val, const int decimals)
{
  return val > 0 ? ConvertDecimalToUint256(val, decimals) : uint256_0;
}

String Web3JBCUtil::ConvertWeiToEthString(const uint256_t &weiVal)
{
  return ConvertUint256ToStringWithDecimal(weiVal, 18);
}

String Web3JBCUtil::ConvertWeiToEthString(const uint256_t &weiVal, const int decimals)
{
  return ConvertUint256ToStringWithDecimal(weiVal, decimals);
}

String Web3JBCUtil::ConvertUint256ToString(const uint256_t &value)
{
  char buffer[65];
  vector<uint8_t> bytes = value.export_bits_truncate();
  String hex = ConvertBytesToHex(bytes.data(), bytes.size()).c_str();
  String amount = ConvertBase(16, 10, hex.c_str());
  int padLength = 0;
  if (amount.length() < 64)
  {
    padLength = 64 - amount.length();
    memset(buffer, '0', padLength);
  }
  memcpy(buffer + padLength, amount.c_str(), amount.length());
  buffer[64] = 0;

  amount = String(buffer);

  // crawl amount here
  while (amount.length() > 0)
  {
    if (amount[0] != '0')
    {
      break;
    }
    amount = amount.substring(1);
  }
  if (amount.length() == 0)
  {
    amount = "0";
  }
  return amount;
}

String Web3JBCUtil::ConvertInt256ToString(const _int256_t &value)
{
  if (value <= max_positive_val)
  {
    return ConvertUint256ToString((uint256_t)value);
  }
  else
  {
    uint256_t negVal = uint256_max - (uint256_t)value + 1;
    String res = "-";
    res += ConvertUint256ToString(negVal);
    return res;
  }
}

String Web3JBCUtil::ConvertUint256ToStringWithDecimal(const uint256_t &value, const int decimals)
{
  char buffer[65];
  vector<uint8_t> bytes = value.export_bits_truncate();
  String hex = ConvertBytesToHex(bytes.data(), bytes.size()).c_str();
  String amount = ConvertBase(16, 10, hex.c_str());
  int padLength = 0;
  if (amount.length() < 64)
  {
    padLength = 64 - amount.length();
    memset(buffer, '0', padLength);
  }
  memcpy(buffer + padLength, amount.c_str(), amount.length());
  buffer[64] = 0;

  amount = String(buffer);

  // now place the decimal at the right spot
  String left = amount.substring(0, 64 - decimals);
  // crawl left here
  while (left.length() > 0)
  {
    if (left[0] != '0')
    {
      break;
    }
    left = left.substring(1);
  }
  if (left.length() == 0)
  {
    left = "0";
  }

  String right = amount.substring(64 - decimals);
  // crawl right here
  while (right.length() > 0)
  {
    if (right[right.length() - 1] != '0')
    {
      break;
    }
    right.remove(right.length() - 1);
  }

  left += '.';
  if (right.length() > 0)
  {
    left += right;
  }
  else
  {
    left += '0';
  }

  return left;
}

String Web3JBCUtil::ConvertInt256ToStringWithDecimal(const _int256_t &value, const int decimals)
{
  if (value <= max_positive_val)
  {
    return ConvertUint256ToStringWithDecimal((uint256_t)value, decimals);
  }
  else
  {
    uint256_t negVal = uint256_max - (uint256_t)value + 1;
    String res = "-";
    res += ConvertUint256ToStringWithDecimal(negVal, decimals);
    return res;
  }
}

uint256_t Web3JBCUtil::ConvertDecimalToUint256(const double val)
{
  char buffer[36]; // allow extra 4 chars for gcvt
  sprintf(buffer, "%f", val);
  String weiStr = buffer;
  int index = weiStr.lastIndexOf('.');
  if (index != -1)
    weiStr = weiStr.substring(0, index);
  weiStr = ConvertBase(10, 16, weiStr.c_str());
  return uint256_t(weiStr.c_str());
}

uint256_t Web3JBCUtil::ConvertDecimalToUint256(const double val, const int decimals)
{
  return ConvertDecimalToUint256(val * pow(10, decimals));
}

String Web3JBCUtil::Substr(const String &result, const size_t start, const size_t length)
{
  return result.substring(start, start + length);
}

String Web3JBCUtil::ConvertEthToWei(const double eth)
{
  char buffer[36]; // allow extra 4 chars for gcvt
  double eth2 = eth;
  if (eth2 < 0)
    eth2 = 0;
  gcvt(eth2 * pow(10.0, 18), 32, buffer);
  String weiStr = buffer;
  int index = weiStr.lastIndexOf('.');
  if (index != -1)
    weiStr = weiStr.substring(0, index);
  return ConvertBase(10, 16, weiStr.c_str());
}

_int256_t Web3JBCUtil::Int256FromSigned(const int8_t value)
{
  return Int256FromSigned((int64_t)value);
}
_int256_t Web3JBCUtil::Int256FromSigned(const int16_t value)
{
  return Int256FromSigned((int64_t)value);
}
_int256_t Web3JBCUtil::Int256FromSigned(const int32_t value)
{
  return Int256FromSigned((int64_t)value);
}

_int256_t Web3JBCUtil::Int256FromSigned(const int64_t value)
{
  if (value < 0)
  {
    return (_int256_t)uint256_max - value + 1;
  }
  else
  {
    return _int256_t(value);
  }
}

_int256_t Web3JBCUtil::Int256FromUint256(const uint256_t &value)
{
  return _int256_t(value);
}
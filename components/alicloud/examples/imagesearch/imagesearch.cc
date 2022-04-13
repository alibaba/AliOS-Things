#include <map>
#include <sstream>
#include <iostream>
#include "../utils.h"
#include "alibabacloud/core/AlibabaCloud.h"
#include "alibabacloud/core/CommonClient.h"

using namespace std;
using namespace AlibabaCloud;

typedef unsigned char u8_t;


const string catId = "88888888"; // ref: https://help.aliyun.com/document_detail/66623.html
const string itemId = "1234";
string custContent = "{\"key\":\"value\"}";

map<string, string> picList;
map<int, string> mapStudent;

const string jpg1 = "1.jpg";
const string jpg2 = "2.jpg";



static const u8_t base64_table[65] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/**
* base64_encode - Base64 encode
* @src: Data to be encoded
* @len: Length of the data to be encoded
* @out_len: Pointer to output length variable, or %NULL if not used
* Returns: Allocated buffer of out_len bytes of encoded data,
* or empty string on failure
*/
string base64_encode(const unsigned char *src, size_t len) {
  unsigned char *out, *pos;
  const unsigned char *end, *in;
  size_t olen;

  olen = 4*((len + 2) / 3); /* 3-byte blocks to 4-byte */

  if (olen < len)
    return std::string(); /* integer overflow */

  std::string outStr;
  outStr.resize(olen);
  out = (unsigned char*)&outStr[0];

  end = src + len;
  in = src;
  pos = out;
  while (end - in >= 3) {
    *pos++ = base64_table[in[0] >> 2];
    *pos++ = base64_table[((in[0] & 0x03) << 4) | (in[1] >> 4)];
    *pos++ = base64_table[((in[1] & 0x0f) << 2) | (in[2] >> 6)];
    *pos++ = base64_table[in[2] & 0x3f];
    in += 3;
  }

  if (end - in) {
    *pos++ = base64_table[in[0] >> 2];
    if (end - in == 1) {
      *pos++ = base64_table[(in[0] & 0x03) << 4];
      *pos++ = '=';
    }
    else {
      *pos++ = base64_table[((in[0] & 0x03) << 4) | (in[1] >> 4)];
      *pos++ = base64_table[(in[1] & 0x0f) << 2];
    }
    *pos++ = '=';
  }
  return outStr;
}

string imageBase64(string file) {

  FILE* img = nullptr;
#ifdef _WIN32
  fopen_s(&img, file.c_str(), "rb");
#else
  img = fopen(file.c_str(), "rb");
#endif
  if (!img) {
    perror("open file error");
    return "";
  }

  fseek(img, 0L, SEEK_END);
  int size = ftell(img);
  fseek(img, 0L, SEEK_SET);
  u8_t* buf = (u8_t*)malloc(size);
  int bytes = fread(buf, 1, size, img);
  fclose(img);
  string encoded = base64_encode(buf, bytes);
  free(buf);
  return encoded;
}

string buildContent(map<string, string> params) {
  string meta = "";
  string body = "";
  int start = 0;

  for(auto item = params.begin(); item != params.end(); item++) {
    auto k = item->first;
    auto v = item->second;

    if (meta.size() > 0) {
      meta += "#";
    }

    std::stringstream ss1, ss2;
    ss1 << start;
    ss2 << start + v.size();

    meta += k + "," + ss1.str() + "," + ss2.str();
    body += v;
    start += v.size();

  }
  return meta + "^" + body;
}

string buildAddContent(map<string, string> picList) {
  if (itemId.empty() || catId.empty() ) {
    return "";
  }
  if (custContent.empty()) {
    custContent = "";
  }

  map<string, string> params;
  params["item_id"] = itemId;
  params["cat_id"] = catId;
  params["cust_content"] = custContent;

  string picListStr = "";
  for (auto item = picList.begin(); item != picList.end(); item++) {
    auto v = item->second;
    auto k = item->first;

    if (k.empty()) {
      continue;
    }
    picListStr += k + ",";
    params[k] = v;
  }

  params["pic_list"] = picListStr.substr(0, picListStr.size() - 1);
  return buildContent(params);
}

int main(int argc, char** argv) {
  utUtils utils;
  string key = utils.get_env("ENV_AccessKeyId");
  string secret = utils.get_env("ENV_AccessKeySecret");

  InitializeSdk();
  ClientConfiguration configuration("cn-shanghai");
  CommonClient client(key, secret, configuration);

  CommonRequest request(CommonRequest::RoaPattern);
  request.setScheme("http");
  request.setDomain("imagesearch.cn-shanghai.aliyuncs.com");
  request.setResourcePath("/item/add");

  request.setHeaderParameter("instanceName", "sdkimagetest1");
  request.setHttpMethod(HttpRequest::Post);

  char dir[1024];
  utils.get_dir_exec(dir, nullptr);
#ifdef _WIN32
  // windows binary has an extra Release/Debug/... dir
  string image1_path = string(dir) + "\\..\\1.jpg";
  string image2_path = string(dir) + "\\..\\2.jpg";
#else
  string image1_path = string(dir) + "1.jpg";
  string image2_path = string(dir) + "2.jpg";
#endif
  picList[base64_encode((u8_t*)jpg1.c_str(), jpg1.size())] = imageBase64(image1_path);
  picList[base64_encode((u8_t*)jpg2.c_str(), jpg2.size())] = imageBase64(image2_path);

  string cc = buildAddContent(picList);
  const char* data = cc.c_str();
  request.setContent(cc.c_str(), cc.size());
  request.setHeaderParameter("Content-Type", "application/octet-stream;charset=utf-8");
  request.setHeaderParameter("Accept", "application/json");
  request.setVersion("2018-01-20");

  auto out = client.commonResponse(request);
  if (!out.isSuccess()) {
    cout << "error code:      " << out.error().errorCode() << endl;
    cout << "error message:   " << out.error().errorMessage() << endl;
    cout << "error host:      " << out.error().host() << endl;
    cout << "error requestId: " << out.error().requestId() << endl;
    cout << "error detail:    " << out.error().detail() << endl;
    ShutdownSdk();
    return -1;
  }
  cout << endl << "add item returns: " << out.result().payload() << endl << endl;
  ShutdownSdk();
  return 0;
}

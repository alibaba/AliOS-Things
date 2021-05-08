#include "json/json.h"
#include <fstream>
#include <iostream>
#if AOS_COMP_CLI
#include "aos/cli.h"
#endif
/** \brief Parse from stream, collect comments and capture error info.
 * Example Usage:
 * $g++ readFromStream.cpp -ljsoncpp -std=c++11 -o readFromStream
 * $./readFromStream
 * // comment head
 * {
 *    // comment before
 *    "key" : "value"
 * }
 * // comment after
 * // comment tail
 */
// int main(int argc, char* argv[]) {
extern "C" {

int jsoncpp_comp_read_stream_example(int argc, char **argv)
{
  Json::Value root;
  std::ifstream ifs;
  ifs.open(argv[1]);

  Json::CharReaderBuilder builder;
  builder["collectComments"] = true;
  JSONCPP_STRING errs;
  if (!parseFromStream(builder, ifs, &root, &errs)) {
    // std::cout << errs << std::endl;
    printf("Read json error!\r\n");
    return EXIT_FAILURE;
  }
  // std::cout << root << std::endl;
  printf("josn read root = %s !\r\n", root);
  return EXIT_SUCCESS;
}

/** \brief Write the Value object to a stream.
 * Example Usage:
 * $g++ streamWrite.cpp -ljsoncpp -std=c++11 -o streamWrite
 * $./streamWrite
 * {
 *     "Age" : 20,
 *     "Name" : "robin"
 * }
 */
int jsoncpp_comp_write_stream_example(int argc, char **argv)
{
  Json::Value root;
  Json::StreamWriterBuilder builder;
  const std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());

  root["Name"] = "robin";
  root["Age"] = 20;
  writer->write(root, &std::cout);

  return EXIT_SUCCESS;
}

/**
 * \brief Parse a raw string into Value object using the CharReaderBuilder
 * class, or the legacy Reader class.
 * Example Usage:
 * $g++ readFromString.cpp -ljsoncpp -std=c++11 -o readFromString
 * $./readFromString
 * colin
 * 20
 */
int jsoncpp_comp_read_string_example(int argc, char **argv)
{
  const std::string rawJson = R"({"Age": 20, "Name": "colin"})";
  const auto rawJsonLength = static_cast<int>(rawJson.length());
  constexpr bool shouldUseOldWay = false;
  JSONCPP_STRING err;
  Json::Value root;

  if (shouldUseOldWay) {
    Json::Reader reader;
    reader.parse(rawJson, root);
  } else {
    Json::CharReaderBuilder builder;
    const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
    if (!reader->parse(rawJson.c_str(), rawJson.c_str() + rawJsonLength, &root,
                       &err)) {
      printf("Read json error!\r\n");
      // std::cout << "error" << std::endl;
      return EXIT_FAILURE;
    }
  }
  const std::string name = root["Name"].asString();
  const int age = root["Age"].asInt();

  // std::cout << name << std::endl;
  // std::cout << age << std::endl;
  printf("json read name = %s !\r\n", name.c_str());
  printf("josn read age = %d !\r\n", age);
  return EXIT_SUCCESS;
}

/** \brief Write a Value object to a string.
 * Example Usage:
 * $g++ stringWrite.cpp -ljsoncpp -std=c++11 -o stringWrite
 * $./stringWrite
 * {
 *     "action" : "run",
 *     "data" :
 *     {
 *         "number" : 1
 *     }
 * }
 */
int jsoncpp_comp_write_string_example(int argc, char **argv)
{
  Json::Value root;
  Json::Value data;
  constexpr bool shouldUseOldWay = false;
  root["action"] = "run";
  data["number"] = 1;
  root["data"] = data;

  if (shouldUseOldWay) {
    Json::FastWriter writer;
    const std::string json_file = writer.write(root);
    // std::cout << json_file << std::endl;
    printf("json write json_file = %s !\r\n", json_file.c_str());
  } else {
    Json::StreamWriterBuilder builder;
    const std::string json_file = Json::writeString(builder, root);
    // std::cout << json_file << std::endl;
    printf("json write json_file = %s !\r\n", json_file.c_str());
  }
  return EXIT_SUCCESS;
}

}

#if AOS_COMP_CLI
/* reg args: fun, cmd, description*/
// ALIOS_CLI_CMD_REGISTER(jsoncpp_comp_read_stream_example, jsoncpp_read_stream_example, jsoncpp component base example)
// ALIOS_CLI_CMD_REGISTER(jsoncpp_comp_write_stream_example, jsoncpp_write_stream_example, jsoncpp component base example)
ALIOS_CLI_CMD_REGISTER(jsoncpp_comp_read_string_example, jsoncpp_read_string_example, jsoncpp component base example)
ALIOS_CLI_CMD_REGISTER(jsoncpp_comp_write_string_example, jsoncpp_write_string_example, jsoncpp component base example)
#endif

#define HDT_VERSION "1"
#define INDEX_VERSION "1"
#define RELEASE_VERSION "1"
/*
using namespace std;

namespace hdt {
  // Parses version URI
  inline std::vector<int> parse_version_uri(std::string uri) {
    std::string version_base = HDTVocabulary::HDT_BASE+"HDTv";
    std::size_t found = uri.find(version_base);
    if (found==std::string::npos)
      throw "Invalid version URI";

    std::cout << "first 'needle' found at: " << found << '\n';

    uri.substr(version_base.length(), uri.length() - version_base.length() - 1); // remove trailing ">"

    // Parse version number
    // First number is HDT, second is Index, Third is release
    std::vector<int> result(3);
    std::string delimiter = "-";
    // Find first
    found = uri.find(delimiter);
    if (!found)
      throw "Invalid version URI";

    int hdt_version = stoi(uri.substr(0, found));

    found = uri.find(delimiter, found + 1);
    if (!found)
      return { hdt_version, 0, 0 };

    int index_version = stoi(uri.substr(0, found));

    found = uri.find(delimiter, found + 1);
    if (!found)
      return { hdt_version, index_version, 0 };

    int release_version = stoi(uri.substr(0, found));

    return { hdt_version, index_version, 0 };
  }
}
*/

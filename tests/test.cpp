#include <iostream>
#include <darmatch.h>

int main(int argc, char** argv) {
  std::vector<std::string> args(argv, argv + argc);
  std::string dict_path, user_dict_path;
  for (int i = 1; i < args.size(); i += 2) {
    if (args[i] == "--dict_path") {
      dict_path = std::string(args.at(i + 1));
    } else if (args[i] == "--user_dict_path") {
      user_dict_path = std::string(args.at(i + 1));
    } else {
      std::cout << "Unknown argument: " << args[i] << std::endl;
      std::cout << "Supported argument: --dict_path --user_dict_path" << std::endl;
      exit(EXIT_FAILURE);
    }
  }
  
  /*
    initialization methods:
    darmatch::DarMatch da;
    darmatch::DarMatch da(dict_path, user_dict_path = "");
  */
  darmatch::DarMatch da(dict_path, user_dict_path);

  std::string text = "俄罗斯联邦总统普京决定在顿巴斯地区开展特别军事行动。";
  
  /*
    maximum forward matching:
    std::vector<std::string> fwords = da.seg(text);
    ----------------------------------------------
    std::vector<std::pair<size_t, std::string>> fwords;
    da.seg(text, fwords);
  */
  std::vector<std::string> fwords = da.seg(text);
  std::cout << "The Chinese word segmentation based on Maximum Forward Matching: " << std::endl;
  for (size_t i = 0; i < fwords.size(); i++) {
    std::cout << fwords[i] << " ";
  }
  std::cout << std::endl;
  
  /*
    maximum backward matching:
    std::vector<std::string> bwords = da.seg(text, false);
    ------------------------------------------------------
    std::vector<std::pair<size_t, std::string>> bwords;
    da.seg(text, bwords, false);
  */
  std::vector<std::string> bwords = da.seg(text, false);
  std::cout << "The Chinese word segmentation based on Maximum Backward Matching: " << std::endl;
  for (size_t i = 0; i < bwords.size(); i++) {
    std::cout << bwords[i] << " ";
  }
  std::cout << std::endl;
  
  /*
    update the double-array trie by insert:
    da.insert(const std::string&);
    da.insert(const std::vector<std::string>&);
  */
  da.insert("俄罗斯联邦总统");
  
  // multi-pattern string matching
  std::vector<std::pair<size_t, std::string>> result = da.parse(text);
  std::cout << "The result of multi-pattern string matching: " << std::endl;
  for (size_t i = 0; i < result.size(); i++) {
    std::cout << result[i].first << "\t" << result[i].second << std::endl; 
  }
  return 0;
}

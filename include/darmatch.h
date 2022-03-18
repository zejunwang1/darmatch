/**
 * Copyright (c) 2021-present, Zejun Wang (wangzejunscut@126.com).
 * All rights reserved.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef DARMATCH_H
#define DARMATCH_H

#include <fstream>
#include <cctype>
#include <memory>
#include <vector>
#include <string>
#include <algorithm>

#ifdef USE_PREFIX_TRIE
#include "cedarpp.h"
#else
#include "cedar.h"
#endif

namespace darmatch {

typedef cedar::da<int> dar;

class DarMatch {
  protected:
    size_t _size;
    size_t _max_word_len;
    
    std::unique_ptr<dar> _da;
    std::vector<std::string> _keys;
    
    int build(const std::string& dict_path, const std::string& user_dict_path) {
      std::ifstream in(dict_path);
      if (!in.is_open()) {
        throw std::invalid_argument(dict_path + " can not be opened for loading.");
      }

      std::string word;
      while (std::getline(in, word)) {
        if (word.empty()) continue;
        _keys.emplace_back(word);
        _max_word_len = (_max_word_len > word.length()) ? _max_word_len : word.length();
      }
      if (!user_dict_path.empty()) {
        std::ifstream inuser(user_dict_path);
        if (!inuser.is_open()) {
          throw std::invalid_argument(user_dict_path + " can not be opened for loading.");
        }
        while (std::getline(inuser, word)) {
          if (word.empty()) continue;
          _keys.emplace_back(word);
          _max_word_len = (_max_word_len > word.length()) ? _max_word_len : word.length();
        }
      }

      std::sort(_keys.begin(), _keys.end());
      _keys.erase(std::unique(_keys.begin(), _keys.end()), _keys.end());
      _size = _keys.size();
      std::vector<const char*> keys;
      for (size_t i = 0; i < _keys.size(); i++) {
        keys.emplace_back(_keys[i].c_str());
      }
      
      _da = std::unique_ptr<dar>(new dar());
      return _da->build(_size, &(keys[0]), 0, 0);
    }
  
  public:
    DarMatch() : _size(0), _max_word_len(0) {
      _da = std::unique_ptr<dar>(new dar());
    }
    
    DarMatch(const std::string& dict_path, const std::string& user_dict_path = "")
    : _size(0), _max_word_len(0) {
      if (build(dict_path, user_dict_path) != 0) {
        throw std::invalid_argument("build double-array-trie failed.");
      }
    }
    
    size_t size() { return _size; }
    
    void insert(const std::vector<std::string>& words) {
      for (size_t i = 0; i < words.size(); i++) {
        insert(words[i]);
      }
    }
    
    void insert(const std::string& word) {
      insert(word.c_str());
    }  

    void insert(const char* word) {
      insert(word, std::strlen(word));
    }
    
    void insert(const char* word, size_t len) {
      dar::result_pair_type result_pair;
      _da->exactMatchSearch(word, len, result_pair);
      if (result_pair.value < 0) {
        _da->update(word, len, _size++);
        _keys.emplace_back(std::string(word, len));
        _max_word_len = (_max_word_len > len) ? _max_word_len : len;
      }
    }
    
    void maximum_forward_matching(const char* str, std::vector<int>& index,
      std::vector<std::pair<size_t, size_t>>& poslen, int max_prefix_matches = 1024) {
      if (index.size()) { index.clear(); }
      if (poslen.size())  { poslen.clear(); }
      int state = 1;
      size_t i = 0, bpos = 0, tlen = std::strlen(str);
      std::vector<dar::result_pair_type> result_pairs(max_prefix_matches);
      while (bpos < tlen) {
        size_t num = _da->commonPrefixSearch(str + bpos, &(result_pairs[0]), max_prefix_matches, tlen - bpos);
        if (num < 1) {
          if (state == 1) {
            i = bpos;
          }
          if (isascii(str[bpos])) {
            bpos++;
          } else {
            bpos++;
            while (bpos < tlen && (str[bpos] & 0xC0) == 0x80) {
              bpos++;
            }
          }
          state = 0;
        } else {
          if (state == 0) {
            poslen.emplace_back(i, bpos - i);
            index.emplace_back(-1);
          }
          num = (num < result_pairs.size()) ? num : result_pairs.size();
          const dar::result_pair_type& result_pair = result_pairs[num - 1];
          poslen.emplace_back(bpos, result_pair.length);
          index.emplace_back(result_pair.value);
          bpos += result_pair.length;
          state = 1;
        }
      }
      if (state == 0) {
        poslen.emplace_back(i, bpos - i);
        index.emplace_back(-1);
      }
    }
    
    bool single_word(const char* str, size_t len = 0) {
      size_t bpos = 0;
      size_t epos = (len == 0) ? std::strlen(str) : (bpos + len);
      epos = (epos < std::strlen(str)) ? epos : std::strlen(str);
      return single_word(str, bpos, epos);
    }
    
    bool single_word(const char* str, size_t bpos, size_t epos) {
      epos = (epos < std::strlen(str)) ? epos : std::strlen(str);
      if (bpos >= epos) { return false; }
      if (isascii(str[bpos])) {
        if (isalnum(str[bpos])) {
          while (bpos < epos && isalnum(str[bpos])) {
            bpos++;
          }
        } else if (isspace(str[bpos])) {
          while (bpos < epos && isspace(str[bpos])) {
            bpos++;
          }
        } else { bpos++; }
      } else {
        bpos++;
        while (bpos < epos && (str[bpos] & 0xC0) == 0x80) {
          bpos++;
        }
      }
      if (bpos == epos) { return true; }
      return false;
    }

    bool ascii_string(const std::string& word) {
      return ascii_string(word.c_str());
    }
    
    bool ascii_string(const char* str, size_t len = 0) {
      if (len == 0) { len = std::strlen(str); }
      for (size_t i = 0; i < len; i++) {
        if (!isascii(str[i])) { return false; }
      }
      return true;
    }

    void maximum_backward_matching(const char* str, std::vector<int>& index,
      std::vector<std::pair<size_t, size_t>>& poslen) {
      if (index.size()) { index.clear(); }
      if (poslen.size())  { poslen.clear(); }
      std::vector<int> index0;
      std::vector<std::pair<size_t, size_t>> poslen0;

      int epos = std::strlen(str);
      int bpos = epos - _max_word_len - 1;
      if (epos < 1) { return; }
      if (bpos >= 0) {
        if (isascii(str[bpos])) {
          bpos++;
        } else {
          bpos++;
          while (bpos < epos && (str[bpos] & 0xC0) == 0x80) {
            bpos++;
          }
        }
      } else {
        bpos = 0;
      }
      
      dar::result_pair_type result_pair;
      while (epos > 0) {
        _da->exactMatchSearch(str + bpos, epos - bpos, result_pair);
        if (result_pair.value == -1) {
          if (single_word(str, bpos, epos)) {
            poslen0.emplace_back(bpos, epos - bpos);
            index0.emplace_back(-1);
            epos = bpos;
            bpos = bpos - _max_word_len - 1;
          }
        } else {
          poslen0.emplace_back(bpos, epos - bpos);
          index0.emplace_back(result_pair.value);
          epos = bpos;
          bpos = bpos - _max_word_len - 1;
        }
        if (bpos >= 0) {
          if (isascii(str[bpos])) {
            bpos++;
          } else {
            bpos++;
            while (bpos < epos && (str[bpos] & 0xC0) == 0x80) {
              bpos++;
            }
          }
        } else {
          bpos = 0;
        }
      }

      std::reverse(poslen0.begin(), poslen0.end());
      std::reverse(index0.begin(), index0.end());
      
      size_t len = 0;
      for (size_t i = 0; i < poslen0.size(); ) {
        bpos = poslen0[i].first;
        len = poslen0[i].second;
        if (index0[i] != -1 || ascii_string(str + bpos, len)) {
          index.emplace_back(index0[i]);
          poslen.emplace_back(bpos, len);
          i++;
          continue;
        }
        i++;
        while (i < poslen0.size()) {
          if (index0[i] != -1 || ascii_string(str + poslen0[i].first, poslen0[i].second))  break;
          len += poslen0[i].second;
          i++;
        }
        index.emplace_back(-1);
        poslen.emplace_back(bpos, len);
      }
    }
    
    void seg(const std::string& text, std::vector<std::pair<size_t, std::string>>& words,
      bool forward = true, int max_prefix_matches = 1024) {
      if (words.size()) { words.clear(); }
      std::vector<int> index;
      std::vector<std::pair<size_t, size_t>> poslen;
      if (forward) {
        maximum_forward_matching(text.c_str(), index, poslen, max_prefix_matches);
      } else {
        maximum_backward_matching(text.c_str(), index, poslen);
      }
      for (size_t i = 0; i < poslen.size(); i++) {
        words.emplace_back(poslen[i].first, text.substr(poslen[i].first, poslen[i].second));
      }
    }
    
    std::vector<std::pair<size_t, std::string>> seg(const std::string& text, bool forward = true, int max_prefix_matches = 1024) {
      std::vector<std::pair<size_t, std::string>> words;
      seg(text, words, forward, max_prefix_matches);
      return words;
    }
    
    std::vector<std::pair<size_t, std::string>> parse(const std::string& text, int max_prefix_matches = 1024) {
      const char* str = text.c_str();
      size_t bpos = 0, tlen = std::strlen(str);
      std::vector<std::pair<size_t, std::string>> matches;
      std::vector<dar::result_pair_type> result_pairs(max_prefix_matches);
      while (bpos < tlen) {
        size_t num = _da->commonPrefixSearch(str + bpos, &(result_pairs[0]), max_prefix_matches, tlen - bpos);
        for (size_t i = 0; i < num && i < result_pairs.size(); i++) {
          const dar::result_pair_type& result_pair = result_pairs[i];
          matches.emplace_back(bpos, _keys[result_pair.value]);
        }
        if (isascii(str[bpos])) { bpos++; } 
        else {
          bpos++;
          while (bpos < tlen && (str[bpos] & 0xC0) == 0x80) {
            bpos++;
          }
        }
      }
      return matches;
    }
};

}
#endif



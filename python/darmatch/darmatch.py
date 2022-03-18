# Copyright (c) 2021-present, Zejun Wang (wangzejunscut@126.com).
# All rights reserved.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

from darmatch_pybind import _DarMatch
from typing import List

class DarMatch(object):
    def __init__(self, 
                 dict_path: str = None,
                 user_dict_path: str = None):
        if dict_path is None:
            self.dar = _DarMatch()
        else:
            self.dar = _DarMatch(dict_path) if user_dict_path is None else _DarMatch(dict_path, user_dict_path)
    
    def size(self):
        return self.dar.size()

    def insert(self, words: List[str]):
        self.dar.insert(words)
    
    def insert(self, word: str):
        self.dar.insert(word)

    def seg(self, 
            text: str, 
            forward: bool = True, 
            return_loc: bool = False,
            max_prefix_matches: int = 1024):

        word_list = self.dar.seg_text(text, forward=forward, max_prefix_matches=max_prefix_matches)

        if not return_loc:
            words = []
            for _, word in word_list:
                words.append(word)
            return words
        
        index = 0
        start = 0
        pos_map = {}
        pos_map[0] = 0
        for char in text:
            index += 1
            start += len(char.encode())
            pos_map[start] = index

        word_loc_list = []
        for pos, word in word_list:
            word_loc_list.append((pos_map[pos], word))
        return word_loc_list
    
    def parse(self,
              text: str,
              char_loc: bool = False,
              max_prefix_matches: int = 1024):
        
        word_list = self.dar.parse_text(text, max_prefix_matches=max_prefix_matches)
        
        if not char_loc:
            return word_list

        index = 0
        start = 0
        pos_map = {}
        pos_map[0] = 0
        for char in text:
            index += 1
            start += len(char.encode())
            pos_map[start] = index
        
        word_loc_list = []
        for pos, word in word_list:
            word_loc_list.append((pos_map[pos], word))
        return word_loc_list




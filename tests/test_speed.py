#!/usr/bin/env python
# -*- coding: utf-8 -*-

import time
import esm
from darmatch import DarMatch

def main():
    dict_path = "./string_match_dict.txt"
    text_path = "./check_text.txt"
    
    with open(text_path, 'r', encoding='utf-8') as f_text:
        text = f_text.read()
    
    words = []
    with open(dict_path, 'r', encoding='utf-8') as f_dict:
        for word in f_dict:
            word = word.rstrip()
            if word:
                words.append(word)
    
    # build esm index
    index = esm.Index()
    for word in words:
        index.enter(word)
    index.fix()
    
    # parse text by esm
    start = time.time()
    match1 = index.query(text)
    end = time.time()
    print("the number of matching results by esm: ", len(match1))
    print("esm time usage: {}s".format(end - start))
    
    print("----------------------------------------------------")
    
    # build double-array trie
    da = DarMatch(dict_path)
    
    # parse text by darmatch
    start = time.time()
    match2 = da.parse(text)
    end = time.time()
    print("the number of matching results by darmatch: ", len(match2))
    print("darmatch time usage: {}s".format(end - start)) 
    
if __name__ == '__main__':
    main()   

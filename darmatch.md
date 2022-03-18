# darmatch

darmatch 是一个非常高效的字符串匹配工具，支持正向/反向最大匹配分词和多模式字符串精确匹配：

- 仅包含头文件 (header-only)

- 基于双数组字典树 (double-array trie) 的模式匹配

## C++

使用示例可以参考 tests 文件夹中的 test.cpp：

```cpp
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
    std::vector<std::pair<size_t, std::string>> fwords = da.seg(text);
    ----------------------------------------------
    std::vector<std::pair<size_t, std::string>> fwords;
    da.seg(text, fwords);
  */
  std::vector<std::pair<size_t, std::string>> fwords = da.seg(text);
  std::cout << "The Chinese word segmentation based on Maximum Forward Matching: " << std::endl;
  for (size_t i = 0; i < fwords.size(); i++) {
    std::cout << fwords[i].second << " ";
  }
  std::cout << std::endl;
  
  /*
    maximum backward matching:
    std::vector<std::pair<size_t, std::string>> bwords = da.seg(text, false);
    ------------------------------------------------------
    std::vector<std::pair<size_t, std::string>> bwords;
    da.seg(text, bwords, false);
  */
  std::vector<std::pair<size_t, std::string>> bwords = da.seg(text, false);
  std::cout << "The Chinese word segmentation based on Maximum Backward Matching: " << std::endl;
  for (size_t i = 0; i < bwords.size(); i++) {
    std::cout << bwords[i].second << " ";
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
```

通过 cmake 进行编译：

```shell
git clone https://github.com/zejunwang1/darmatch
cd darmatch
mkdir build
cmake ..
# cmake -DUSE_PREFIX_TRIE=ON ..
make
```

执行上述命令后，会在 darmatch/build 文件夹中生成可执行文件 test。

```shell
./test --dict_path ../tests/dict.txt
```

运行后结果如下：

```
The Chinese word segmentation based on Maximum Forward Matching: 
俄罗斯联邦 总统 普京 决定 在 顿巴斯地区 开展 特别 军事行动 。 
The Chinese word segmentation based on Maximum Backward Matching: 
俄罗斯 联邦总统 普京 决定 在 顿巴斯地区 开展 特别 军事行动 。 
The result of multi-pattern string matching: 
0	俄罗斯联邦
0	俄罗斯联邦总统
9	联邦总统
21	普京
27	决定
36	顿巴斯地区
51	开展
63	军事行动
```

## Python

### Requirements

- Python version >= 3.6

- pybind11 >= 2.2

- setuptools >= 0.7.0

- typing

### Installation

通过 pip 命令直接安装：

```shell
pip install darmatch
```

或者从 github 仓库中获取最新版本安装：

```shell
git clone https://github.com/zejunwang1/darmatch
cd darmatch
pip install .
# or:
python setup.py install
```

### Demo

```python
from darmatch import DarMatch
da = DarMatch()
# da = DarMatch(dict_path, user_dict_path="")
words = ["俄罗斯联邦", "联邦总统", "普京", "决定", "顿巴斯地区", "开展", "军事行动"]
da.insert(words)
text = "俄罗斯联邦总统普京决定在顿巴斯地区开展特别军事行动。"

# maximum forward matching
word_list = da.seg(text, forward=True, return_loc=True)
print("The Chinese word segmentation based on Maximum Forward Matching:")
print(word_list)

# maximum backward matching
word_list = da.seg(text, forward=False, return_loc=True)
print("The Chinese word segmentation based on Maximum Backward Matching:")
print(word_list)

# multi-pattern string matching
da.insert("俄罗斯联邦总统")
word_list = da.parse(text, char_loc=True)
print("The result of multi-pattern string matching:")
print(word_list)
```

运行结果如下：

```
[(0, '俄罗斯联邦'), (5, '总统'), (7, '普京'), (9, '决定'), (11, '在'), (12, '顿巴斯地区'), (17, '开展'), (19, '特别'), (21, '军事行动'), (25, '。')]
The Chinese word segmentation based on Maximum Backward Matching:
[(0, '俄罗斯'), (3, '联邦总统'), (7, '普京'), (9, '决定'), (11, '在'), (12, '顿巴斯地区'), (17, '开展'), (19, '特别'), (21, '军事行动'), (25, '。')]
The result of multi-pattern string matching:
[(0, '俄罗斯联邦'), (0, '俄罗斯联邦总统'), (3, '联邦总统'), (7, '普京'), (9, '决定'), (12, '顿巴斯地区'), (17, '开展'), (21, '军事行动')]
```

### Speed

和基于 Aho-Corasick 的字符串匹配正则表达式工具 esmre 进行处理速度对比。

可以使用 pip 命令安装 esmre：

```shell
pip install esmre
```

在 tests 文件夹中包含字符串匹配需要用到的关键词词典文件 string_match_dict.txt，共计有 348982 个关键词。待进行匹配的文本字符串文件 check_text.txt，共计有 273864 个字符。

```shell
python test_speed.py
```

运行后结果如下：

```
the number of matching results by esm:  343623
esm time usage: 0.4515085220336914s
----------------------------------------------------
the number of matching results by darmatch:  343623
darmatch time usage: 0.1248319149017334s121s
```

可以看出，darmatch 比 esm 快 3~4 倍左右。

## Contact

邮箱： wangzejunscut@126.com

微信：autonlp







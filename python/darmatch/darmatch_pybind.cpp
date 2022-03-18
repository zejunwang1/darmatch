/**
 * Copyright (c) 2021-present, Zejun Wang (wangzejunscut@126.com).
 * All rights reserved.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <darmatch.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

PYBIND11_MODULE(darmatch_pybind, m) {

  m.doc() = "An efficient multi-pattern string matching toolkit.";
  
  py::class_<darmatch::DarMatch>(m, "_DarMatch") 

      .def(py::init()) 

      .def(py::init<const std::string&, const std::string&>(), 
            py::arg("dict_path"), py::arg("user_dict_path") = "")

      .def("size", &darmatch::DarMatch::size)

      .def("insert", 
            (void (darmatch::DarMatch::*)(const std::vector<std::string>&))(&darmatch::DarMatch::insert), 
            py::arg("words"))

      .def("insert", 
            (void (darmatch::DarMatch::*)(const std::string&))(&darmatch::DarMatch::insert), 
            py::arg("word"))

      .def("seg_text", 
            (std::vector<std::pair<size_t, std::string>>(darmatch::DarMatch::*)(const std::string&, bool, int))(&darmatch::DarMatch::seg),
            py::arg("text"), py::arg("forward") = true, py::arg("max_prefix_matches") = 1024)
      
      .def("parse_text",
            (std::vector<std::pair<size_t, std::string>>(darmatch::DarMatch::*)(const std::string&, int))(&darmatch::DarMatch::parse),
            py::arg("text"), py::arg("max_prefix_matches") = 1024); 
}



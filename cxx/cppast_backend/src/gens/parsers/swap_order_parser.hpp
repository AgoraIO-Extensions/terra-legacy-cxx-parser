//
// Created by LXH on 2022/5/17.
//

#ifndef AGORA_RTC_AST_SWAP_ORDER_PARSER_HPP
#define AGORA_RTC_AST_SWAP_ORDER_PARSER_HPP

#include "terra_parser.hpp"
#include <iterator>
#include <variant>
#include <vector>

class SwapOrderParser : public Parser {
public:
  bool Parse(const ParseConfig &parse_config,
             ParseResult &parse_result) override {
    for (auto &f : parse_result.cxx_files) {
      std::map<std::string, NodeType *> nodeMap;
      for (auto &node : f.nodes) {
        if (std::holds_alternative<Struct>(node)) {
          auto &aStruct = std::get<Struct>(node);
          nodeMap.emplace(std::make_pair(aStruct.name, &node));
        } else if (std::holds_alternative<Clazz>(node)) {
          auto &clazz = std::get<Clazz>(node);
          nodeMap.emplace(std::make_pair(clazz.name, &node));
        }
      }
      for (auto &node : f.nodes) {
        BaseNode baseNode;
        if (std::holds_alternative<Struct>(node)) {
          baseNode = std::get<Struct>(node);
        } else if (std::holds_alternative<Enumz>(node)) {
          baseNode = std::get<Enumz>(node);
        }
        if (nodeMap.find(baseNode.parent_name) != nodeMap.end()) {
          std::swap(node, *nodeMap[baseNode.parent_name]);
          nodeMap[baseNode.parent_name] = &node;
        }
      }
    }
  }
};

#endif // AGORA_RTC_AST_SWAP_ORDER_PARSER_HPP

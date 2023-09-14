#ifndef MERGE_RTC_EVENT_HANDLER_EX_PARSER_HPP
#define MERGE_RTC_EVENT_HANDLER_EX_PARSER_HPP

#include "terra_parser.hpp"
#include <iterator>
#include <variant>
#include <vector>
#include <map>

class MergeRtcEventHandlerExParser : public Parser
{
private:
    std::string rtc_event_handler_ex_ = "agora::rtc::IRtcEngineEventHandlerEx";
    std::string rtc_event_handler_ = "agora::rtc::IRtcEngineEventHandler";

public:
    bool Parse(const ParseConfig &parse_config, ParseResult &parse_result)
    {
        Clazz *rtc_event_handler_ex_clazz;
        bool found_rtc_event_handler_ex;
        bool found_rtc_event_handler;
        std::map<std::string, MemberFunction *> rtc_event_handler_ex_functions;
        std::map<std::string, bool> extra_info = {
            {
                "is_from_rtc_event_handler_ex",
                true,
            }};

        for (auto &f : parse_result.cxx_files)
        {
            // FilterClass(f.nodes);
            for (auto &node : f.nodes)
            {
                if (std::holds_alternative<Clazz>(node))
                {
                    Clazz &clazz = std::get<Clazz>(node);

                    std::string class_name = clazz.name;

                    std::string class_with_ns = BaseSyntaxRender::NameWithNamespace(clazz.name, clazz.namespaces);

                    if (class_with_ns == rtc_event_handler_ex_)
                    {
                        rtc_event_handler_ex_clazz = &clazz;
                        found_rtc_event_handler_ex = true;

                        for (auto &func : clazz.methods)
                        {
                            func.user_data = extra_info;
                            rtc_event_handler_ex_functions.emplace(std::make_pair(func.name, &func));
                        }

                        break;
                    }

                    // if (class_name[0] == 'I')
                    // {
                    //     class_name = std::string(class_name.begin() + 1, class_name.end());
                    //     clazz.name = class_name;
                    // }

                    // // for (auto &base_class : )

                    // for (auto &member_function : clazz.methods)
                    // {
                    //     for (auto &param : member_function.parameters)
                    //     {
                    //         std::string type_name = param.type.name;

                    //         std::vector<std::string> namespace_list = BaseSyntaxRender::Split(type_name, "::");
                    //         namespace_list = std::vector<std::string>(namespace_list.begin(), namespace_list.end() - 1);
                    //         std::string namespace_string = BaseSyntaxRender::JoinToString(namespace_list, "::");
                    //         std::string trim_namespaces_name = BaseSyntaxRender::TrimNamespaces(type_name);

                    //         if (!param.type.is_builtin_type && trim_namespaces_name[0] == 'I')
                    //         {
                    //             trim_namespaces_name = std::string(trim_namespaces_name.begin() + 1, trim_namespaces_name.end());
                    //             param.type.name = namespace_string + "::" + trim_namespaces_name;
                    //         }
                    //     }
                    // }
                }
            }

            if (found_rtc_event_handler_ex)
            {
                break;
            }
        }

        for (auto &f : parse_result.cxx_files)
        {
            for (auto &node : f.nodes)
            {
                if (std::holds_alternative<Clazz>(node))
                {
                    Clazz &clazz = std::get<Clazz>(node);

                    std::string class_name = clazz.name;

                    std::string class_with_ns = BaseSyntaxRender::NameWithNamespace(clazz.name, clazz.namespaces);

                    if (class_with_ns == rtc_event_handler_)
                    {
                        found_rtc_event_handler = true;
                        for (auto &func : clazz.methods)
                        {
                            if (rtc_event_handler_ex_functions.find(func.name) != rtc_event_handler_ex_functions.end())
                            {
                                func = *rtc_event_handler_ex_functions.find(func.name)->second;
                                func.parent_name = "IRtcEngineEventHandler";
                                rtc_event_handler_ex_functions.erase(func.name);
                                std::cout << "MergeRtcEventHandlerExParser " << class_with_ns << "::" << func.name << "\n";
                            }
                        }

                        if (!rtc_event_handler_ex_functions.empty())
                        {
                            for (auto const &it : rtc_event_handler_ex_functions)
                            {
                                clazz.methods.push_back(*it.second);
                            }
                        }

                        break;
                    }
                }
            }
        }

        for (auto &f : parse_result.cxx_files)
        {
            // FilterClass(f.nodes);
            for (auto &node : f.nodes)
            {
                f.nodes.erase(
                    std::remove_if(
                        f.nodes.begin(),
                        f.nodes.end(),
                        [this](NodeType &node)
                        {
                            bool shouldRemove = false;

                            if (std::holds_alternative<Clazz>(node))
                            {
                                Clazz &clazz = std::get<Clazz>(node);
                                std::string class_with_ns = BaseSyntaxRender::NameWithNamespace(clazz.name, clazz.namespaces);
                                if (class_with_ns == rtc_event_handler_ex_)
                                {
                                    std::cout << "MergeRtcEventHandlerExParser ShouldRemoveClass:" << class_with_ns << "\n";
                                    shouldRemove = true;
                                }
                            }

                            return shouldRemove;
                        }),
                    f.nodes.end());
            }
        }
    }
};

#endif // MERGE_RTC_EVENT_HANDLER_EX_PARSER_HPP

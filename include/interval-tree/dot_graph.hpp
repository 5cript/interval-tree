#pragma once

#include "interval_types.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <utility>

namespace lib_interval_tree
{
    struct dot_graph_draw_settings
    {
        bool digraph = true;
        std::string name = "G";
        std::vector<std::string> extra_node_attributes = {};
        std::vector<std::string> extra_statements = {};
        bool space_after_comma = false;
        char left_brace = '\0';
        char right_brace = '\0';
        std::vector<std::string> edge_attributes = {};
        std::string indent = "\t";
    };

    namespace detail
    {
        template <typename TreeT>
        class graph_painter
        {
          public:
            graph_painter(std::ostream& stream, TreeT const& tree, dot_graph_draw_settings settings)
                : stream_{stream}
                , tree_{tree}
                , settings_{std::move(settings)}
                , node_id_{"a"}
                , left_brace_{}
                , right_brace_{}
            {
                using ival_type = typename TreeT::interval_type;

                const auto determine_brace = []() {
                    if (std::is_same<typename ival_type::interval_kind, closed>::value)
                        return "[]";
                    else if (std::is_same<typename ival_type::interval_kind, left_open>::value)
                        return "(]";
                    else if (std::is_same<typename ival_type::interval_kind, right_open>::value)
                        return "[)";
                    else if (std::is_same<typename ival_type::interval_kind, open>::value)
                        return "()";
                    else if (std::is_same<typename ival_type::interval_kind, closed_adjacent>::value)
                        return "[]";
                    else
                        return "[]";
                };

                if (settings_.left_brace != '\0')
                    left_brace_ = settings_.left_brace;
                else
                    left_brace_ = determine_brace()[0];

                if (settings_.right_brace != '\0')
                    right_brace_ = settings_.right_brace;
                else
                    right_brace_ = determine_brace()[1];
            }

            void make_header()
            {
                stream_ << (settings_.digraph ? "digraph" : "graph") << " " << settings_.name << " {\n";
                for (auto const& statement : settings_.extra_statements)
                {
                    stream_ << settings_.indent << statement << ";\n";
                }
            }

            template <typename T>
            void make_label(T const& ival)
            {
#if __cplusplus >= 201703L
                if constexpr (std::is_same<typename T::interval_kind, dynamic>::value)
                {
                    stream_ << (ival.left_border() == interval_border::open ? '(' : '[') << ival.low()
                            << (settings_.space_after_comma ? ", " : ",") << ival.high()
                            << (ival.right_border() == interval_border::open ? ')' : ']');
                }
                else
                {
                    stream_ << left_brace_ << ival.low() << (settings_.space_after_comma ? ", " : ",") << ival.high()
                            << right_brace_;
                }
#else
                stream_ << left_brace_ << ival.low() << (settings_.space_after_comma ? ", " : ",") << ival.high()
                        << right_brace_;
#endif
            }

            template <typename interval_type>
            void specify_node(interval_type const& ival)
            {
                stream_ << settings_.indent << node_id_ << " [label=\"";
                increment_node_id();
                make_label(ival);
                stream_ << "\"";
                if (!settings_.extra_node_attributes.empty())
                {
                    for (auto const& attr : settings_.extra_node_attributes)
                    {
                        stream_ << ", " << attr;
                    }
                }
                stream_ << "];\n";
            }

            template <typename iterator_type>
            void specify_all_nodes(iterator_type const& node)
            {
                specify_node(*node);
                if (node.left() != tree_.end())
                    specify_all_nodes(node.left());
                if (node.right() != tree_.end())
                    specify_all_nodes(node.right());
            }

            void specify_edge(std::string const& from, std::string const& to)
            {
                stream_ << settings_.indent << from << (settings_.digraph ? " -> " : " -- ") << to;
                if (!settings_.edge_attributes.empty())
                {
                    stream_ << " [";
                    for (auto iter = settings_.edge_attributes.begin(); iter != settings_.edge_attributes.end(); ++iter)
                    {
                        stream_ << *iter;
                        if (iter + 1 != settings_.edge_attributes.end())
                            stream_ << ", ";
                    }
                    stream_ << "]";
                }
                stream_ << ";\n";
            }

            template <typename iterator_type>
            void specify_all_edges(iterator_type const& node)
            {
                auto previous_id = node_id_;
                if (node.left() != tree_.end())
                {
                    increment_node_id();
                    specify_edge(previous_id, node_id_);
                    specify_all_edges(node.left());
                }
                if (node.right() != tree_.end())
                {
                    increment_node_id();
                    specify_edge(previous_id, node_id_);
                    specify_all_edges(node.right());
                }
            }

            void close()
            {
                stream_ << "}";
            }

            void reset_node_id()
            {
                node_id_ = "a";
            }

          private:
            void increment_node_id()
            {
                const auto character = node_id_.begin();
                for (auto iter = character; iter != node_id_.end(); ++iter)
                {
                    if (*iter == 'z')
                    {
                        *iter = 'a';
                        if (iter + 1 == node_id_.end())
                        {
                            node_id_ += 'a';
                            break;
                        }
                    }
                    else
                    {
                        ++*iter;
                        break;
                    }
                }
            }

          private:
            std::ostream& stream_;
            TreeT const& tree_;
            dot_graph_draw_settings settings_;
            std::string node_id_;
            char left_brace_;
            char right_brace_;
        };
    }

    template <typename TreeT>
    void draw_dot_graph(std::ostream& stream, TreeT const& tree, dot_graph_draw_settings const& settings = {})
    {
        detail::graph_painter<TreeT> painter{stream, tree, settings};
        painter.make_header();
        if (tree.empty())
        {
            painter.close();
            return;
        }
        painter.specify_all_nodes(tree.root());
        painter.reset_node_id();
        painter.specify_all_edges(tree.root());
        painter.close();
    }
}
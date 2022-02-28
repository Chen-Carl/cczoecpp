#ifndef __ZOE_LEXICALCAST_H__
#define __ZOE_LEXICALCAST_H__

#include <string>
#include <vector>
#include <list>
#include <set>
#include <unordered_set>
#include <map>
#include <unordered_map>
#include <sstream>
#include <boost/lexical_cast.hpp>
#include <yaml-cpp/yaml.h>

namespace cczoe {
namespace config {

template <class F, class T>
class LexicalCast
{
public:
    T operator()(const F &v)
    {
        return boost::lexical_cast<T>(v);
    }
};

// vector to string
template <class T>
class LexicalCast<std::vector<T>, std::string>
{
public:
    std::string operator()(const std::vector<T> &vec)
    {
        YAML::Node node(YAML::NodeType::Sequence);
        for (auto &i : vec)
        {
            node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

// string to vector
template <class T>
class LexicalCast<std::string, std::vector<T>>
{
public:
    std::vector<T> operator()(const std::string &str)
    {
        std::vector<T> res;
        YAML::Node node = YAML::Load(str);
        std::stringstream ss;
        for (size_t i = 0; i < node.size(); i++)
        {
            ss.str("");
            ss << node[i];
            res.push_back(LexicalCast<std::string, T>()(ss.str()));
        }
        return res;
    }
};

// list to string
template <class T>
class LexicalCast<std::list<T>, std::string>
{
public:
    std::string operator()(const std::list<T> &li)
    {
        YAML::Node node(YAML::NodeType::Sequence);
        for (auto &i : li)
        {
            node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

// string to list
template <class T>
class LexicalCast<std::string, std::list<T>>
{
public:
    std::list<T> operator()(const std::string &str)
    {
        std::list<T> res;
        YAML::Node node = YAML::Load(str);
        std::stringstream ss;
        for (size_t i = 0; i < node.size(); i++)
        {
            ss.str("");
            ss << node[i];
            res.push_back(LexicalCast<std::string, T>()(ss.str()));
        }
        return res;
    }
};

// set to string
template <class T>
class LexicalCast<std::set<T>, std::string>
{
public:
    std::string operator()(const std::set<T> &s)
    {
        YAML::Node node(YAML::NodeType::Sequence);
        for (auto &i : s)
        {
            node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

// string to set
template <class T>
class LexicalCast<std::string, std::set<T>>
{
public:
    std::set<T> operator()(const std::string &str)
    {
        std::set<T> res;
        YAML::Node node = YAML::Load(str);
        std::stringstream ss;
        for (size_t i = 0; i < node.size(); i++)
        {
            ss.str("");
            ss << node[i];
            res.insert(LexicalCast<std::string, T>()(ss.str()));
        }
        return res;
    }
};

// unordered set to string
template <class T>
class LexicalCast<std::unordered_set<T>, std::string>
{
public:
    std::string operator()(const std::unordered_set<T> &s)
    {
        YAML::Node node(YAML::NodeType::Sequence);
        for (auto &i : s)
        {
            node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

// string to unordered set
template <class T>
class LexicalCast<std::string, std::unordered_set<T>>
{
public:
    std::unordered_set<T> operator()(const std::string &str)
    {
        std::unordered_set<T> res;
        YAML::Node node = YAML::Load(str);
        std::stringstream ss;
        for (size_t i = 0; i < node.size(); i++)
        {
            ss.str("");
            ss << node[i];
            res.insert(LexicalCast<std::string, T>()(ss.str()));
        }
        return res;
    }
};

// map to string
template <class T>
class LexicalCast<std::map<std::string, T>, std::string>
{
public:
    std::string operator()(const std::map<std::string, T> &mp)
    {
        YAML::Node node(YAML::NodeType::Map);
        for (auto it = mp.begin(); it != mp.end(); it++)
        {
            node[it->first] = YAML::Load(LexicalCast<T, std::string>()(it->second));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

// string to map
template <class T>
class LexicalCast<std::string, std::map<std::string, T>>
{
public:
    std::map<std::string, T> operator()(const std::string &str)
    {
        std::map<std::string, T> res;
        YAML::Node node = YAML::Load(str);
        std::stringstream ss;
        for (auto it = node.begin(); it != node.end(); it++)
        {
            ss.str("");
            ss << it->second;
            res.insert(std::make_pair(it->first.Scalar(), LexicalCast<std::string, T>()(ss.str())));
        }
        return res;
    }
};

// unordered map to string
template <class T>
class LexicalCast<std::unordered_map<std::string, T>, std::string>
{
public:
    std::string operator()(const std::unordered_map<std::string, T> &mp)
    {
        YAML::Node node(YAML::NodeType::Map);
        for (auto it = mp.begin(); it != mp.end(); it++)
        {
            node[it->first] = YAML::Load(LexicalCast<T, std::string>()(it->second));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

// string to unordered map
template <class T>
class LexicalCast<std::string, std::unordered_map<std::string, T>>
{
public:
    std::unordered_map<std::string, T> operator()(const std::string &str)
    {
        std::unordered_map<std::string, T> res;
        YAML::Node node = YAML::Load(str);
        std::stringstream ss;
        for (auto it = node.begin(); it != node.end(); it++)
        {
            ss.str("");
            ss << it->second;
            res.insert(std::make_pair(it->first.Scalar(), LexicalCast<std::string, T>()(ss.str())));
        }
        return res;
    }
};

}}

#endif
#include <algorithm>
#include "Config/Config.h"

namespace cczoe {
namespace config {

std::unordered_map<std::string, std::shared_ptr<ConfigVarBase>> s_datas;

/** 
 * @brief parse nodes
 * @param prefix    recurrence parameter, representing for the previous path
 * @param root      current node
 * @param res       recording the path
 */
void Config::ParseNodes(const std::string &prefix, const YAML::Node &root, std::list<std::pair<std::string, YAML::Node>> &res)
{
    if (root.IsMap())
    {
        for (auto it = root.begin(); it != root.end(); it++)
        {
            // recurrence
            ParseNodes(prefix.empty() ? it->first.Scalar() : prefix + "." + it->first.Scalar(), it->second, res);
        }
    }
    else
    {
        res.emplace_back(prefix, root);
    }
}


void Config::LoadFromYaml(const YAML::Node &root)
{
    std::list<std::pair<std::string, YAML::Node>> allNodes;
    ParseNodes("", root, allNodes);
    for (auto &node : allNodes)
    {
        std::string key = node.first;
        // insensitive to lowercase or uppercase
        std::transform(key.begin(), key.end(), key.begin(), ::tolower);
        std::shared_ptr<ConfigVarBase> var = LookupBase(key);
        if (var)
        {
            if (node.second.IsScalar())
            {
                var->fromString(node.second.Scalar());
            }
            else
            {
                std::stringstream ss;
                ss << node.second;
                var->fromString(ss.str());
            }
        }
        else
        {
            CCZOE_LOG_WARN(CCZOE_LOG_ROOT()) << std::format("config variable named \"{}\" does not exist, creating a new variable", key);
            if (node.second.IsScalar())
            {
                Lookup(key, node.second.Scalar());
            }
            else
            {
                std::stringstream ss;
                ss << node.second;
                Lookup(key, ss.str());
            }
        }
    }
}

void Config::LoadFromFile(const std::string &filename)
{
    YAML::Node node = YAML::LoadFile(filename);
    Config::LoadFromYaml(node);
}

}}

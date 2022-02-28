#include <algorithm>
#include "config.h"

namespace cczoe {
namespace config {

std::unordered_map<std::string, std::shared_ptr<ConfigVarBase>> s_datas;

/** 
 * @brief parse nodes
 * @param prefix    recurrence parameter, representing for the previous path
 * @param root      current node
 * @param res       recording the path
 */
void Config::parseNodes(const std::string &prefix, const YAML::Node &root, std::list<std::pair<std::string, YAML::Node>> &res)
{
    if (root.IsMap())
    {
        for (auto it = root.begin(); it != root.end(); it++)
        {
            // recurrence
            parseNodes(prefix.empty() ? it->first.Scalar() : prefix + "." + it->first.Scalar(), it->second, res);
        }
    }
    else
    {
        res.emplace_back(prefix, root);
    }
}


void Config::loadFromYaml(const YAML::Node &root)
{
    std::list<std::pair<std::string, YAML::Node>> allNodes;
    parseNodes("", root, allNodes);
    for (auto &node : allNodes)
    {
        std::string key = node.first;
        // insensitive to lowercase or uppercase
        std::transform(key.begin(), key.end(), key.begin(), ::tolower);
        std::shared_ptr<ConfigVarBase> var = lookupBase(key);
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
    }
}

void Config::loadFromFile(const std::string &filename)
{
    YAML::Node node = YAML::LoadFile(filename);
    Config::loadFromYaml(node);
}

}}

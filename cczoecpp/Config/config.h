#ifndef __CCZOE_CONFIG_H__
#define __CCZOE_CONFIG_H__

#include <format>
#include <string>
#include <unordered_map>
#include <memory>
#include <functional>
#include "lexicalCast.h"
#include "log.h"

namespace cczoe {
namespace config {

/* 
 * a config variable is formatted with yaml-cpp
 * @method: toString() - transfer a config variable to a yaml string
 * @method: fromString() - transfer a yaml string to a config variable
 * 
 * class ConfigVarBase - abstract class
 *  -> template ConfigVar<T>
 */
class ConfigVarBase
{
protected:
    std::string m_name;
    std::string m_description;

public:
    ConfigVarBase(const std::string &name, const std::string &description) :
        m_name(name), m_description(description)
    { }

    virtual ~ConfigVarBase() { }

    const std::string &getName() const { return m_name; }
    const std::string &getDescription() const { return m_description; }

    virtual std::string getTypeName() const = 0;
    virtual bool fromString(const std::string &str) = 0;
    virtual std::string toString() = 0;
};

template <class T, class FromStr = LexicalCast<std::string, T>, class ToStr = LexicalCast<T, std::string>>
class ConfigVar : public ConfigVarBase
{
private:
    T m_val;
    std::map<uint64_t, std::function<void(const T &old_value, const T &new_value)>> m_callbacks;

public:
    ConfigVar(const std::string &name, const T &default_value, const std::string &description = "") :
        ConfigVarBase(name, description), m_val(default_value)
    { }

    const T getValue() const { return m_val; }
    virtual std::string getTypeName() const override { return typeid(T).name(); }

    void setValue(const T &val)
    {
        if (m_val == val)
            return;
        for (auto &fun : m_callbacks)
        {
            fun.second(m_val, val);
        }
        m_val = val;
    }

    bool fromString(const std::string &str) override
    {
        try
        {
            setValue(FromStr()(str));
        }
        catch(std::exception& e)
        {
            CCZOE_LOG_ERROR(CCZOE_LOG_ROOT()) << std::format("ConfigVar::toString() exception {} convert: string to {} with string parameter: {}", e.what(), typeid(m_val).name(), str);
            return false;
        }
        return true;
    }

    std::string toString() override
    {
        try
        {
            return ToStr()(m_val);
        }
        catch(const std::exception& e)
        {
            CCZOE_LOG_ERROR(CCZOE_LOG_ROOT()) << std::format("ConfigVar::toString() exception {} convert: {} to string", e.what(), typeid(m_val).name());
        }
        return "";
    }

    void addListener(uint64_t key, std::function<void(const T &old_value, const T &new_value)> cb)
    {
        m_callbacks[key] = cb;
    }

    void delListener(uint64_t key)
    {
        m_callbacks.erase(key);
    }

    std::function<void(const T &old_value, const T &new_value)> getListener(uint64_t key)
    {
        auto it = m_callbacks.find(key);
        return it == m_callbacks.end() ? nullptr : it->second;
    }

    void clearListener()
    {
        m_callbacks.clear();
    }
};

class Config
{
private:
    static std::unordered_map<std::string, std::shared_ptr<ConfigVarBase>> s_datas;
    static std::unordered_map<std::string, std::shared_ptr<ConfigVarBase>> &getDatas()
    {
        static std::unordered_map<std::string, std::shared_ptr<ConfigVarBase>> s_datas;
        return s_datas;
    }

public:
    template <class T>
    static std::shared_ptr<ConfigVar<T>> lookup(const std::string &name, const T &default_value, const std::string &description = "")
    {
        auto it = getDatas().find(name);
        if (it != getDatas().end())
        {
            auto tmp = std::dynamic_pointer_cast<ConfigVar<T>>(it->second);
            if (tmp)
            {
                CCZOE_LOG_INFO(CCZOE_LOG_ROOT()) << std::format("config variable named \"{}\" exists", name);
                return tmp;
            }
            // tmp may be a null pointer
            else
            {
                CCZOE_LOG_INFO(CCZOE_LOG_ROOT()) << std::format("config variable named \"{}\" exists, which type is not \"{}\" but \"{}\"", name, typeid(T).name(), it->second->getTypeName());
                return nullptr;
            }
        }
        // create a new config variable
        else
        {
            std::shared_ptr<ConfigVar<T>> createVar = std::make_shared<ConfigVar<T>>(name, default_value, description);
            getDatas()[name] = createVar;
            return createVar;
        }
        return nullptr;
    }

    template <class T>
    static std::shared_ptr<ConfigVar<T>> lookup(const std::string &name)
    {
        auto it = getDatas().find(name);
        if (it == getDatas().end())
            return nullptr;
        return std::dynamic_pointer_cast<ConfigVar<T>>(it->second);
    }

    static std::shared_ptr<ConfigVarBase> lookupBase(const std::string &name)
    {
        auto it = getDatas().find(name);
        return it == getDatas().end() ? nullptr : it->second;
    }

    static void loadFromYaml(const YAML::Node &root);
    static void loadFromFile(const std::string &filename);
    static void parseNodes(const std::string &prefix, const YAML::Node &root, std::list<std::pair<std::string, YAML::Node>> &res);
};

}}

#endif
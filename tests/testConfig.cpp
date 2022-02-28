#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <sstream>
#include "Config/config.h"
#include "Config/lexicalCast.h"

using namespace cczoe;

void testLexicalCast();
void testConfig();
void testYamlFile();

int main()
{
    // testLexicalCast();
    // testConfig();
    testYamlFile();
    return 0;
}

void testLexicalCast()
{
    std::cout << "====== test[1/3]: test lexical cast ======" << std::endl;
    
    std::cout << "------ test1: string to int ------" << std::endl; 
    int a = config::LexicalCast<std::string, int>()("123");
    std::cout << a << std::endl;

    std::cout << "------ test2: string to vector ------" << std::endl; 
    std::vector<int> vec = config::LexicalCast<std::string, std::vector<int>>()("[1,2,3]");
    std::cout << "[1,2,3] -> ";
    for (int x : vec)
    {
        std::cout << x << " ";
    }
    std::cout << std::endl;

    std::vector<std::vector<int>> vv = config::LexicalCast<std::string, std::vector<std::vector<int>>>()("[[1,2,3],[4,5,6],[7,8,9]]");
    std::cout << "[[1,2,3],[4,5,6],[7,8,9]] -> " << std::endl;
    for (auto &x : vv)
    {
        for (int y : x)
        {
            std::cout << y << " ";
        }
        std::cout << std::endl;
    }

    std::cout << "------ test3: vector to string ------" << std::endl; 
    std::string ss = config::LexicalCast<std::vector<std::vector<int>>, std::string>()(vv);
    std::cout << ss << std::endl;
    
    std::cout << "------ test4: string to list ------" << std::endl; 
    std::list<int> str2li = config::LexicalCast<std::string, std::list<int>>()("[1,2,3,4,5,6]");
    for (int x : str2li)
    {
        std::cout << x << " ";
    }
    std::cout << std::endl;
    
    std::cout << "------ test5: list to string ------" << std::endl;
    std::string list2str = config::LexicalCast<std::list<std::string>, std::string>()(std::list<std::string>({"abc", "bcd", "cde"}));
    std::cout << list2str << std::endl;    

    std::cout << "------ test6: string to set ------" << std::endl; 
    std::set<int> str2set = config::LexicalCast<std::string, std::set<int>>()("[1,2,3,4,5,6]");
    for (int x : str2li)
    {
        std::cout << x << " ";
    }
    std::cout << std::endl;

    std::cout << "------ test6: set to string ------" << std::endl;
    std::string set2str = config::LexicalCast<std::set<std::string>, std::string>()(std::set<std::string>({"abc", "bcd", "cde"}));
    std::cout << set2str << std::endl;    

    std::cout << "====== test[2/3]: test config var ======" << std::endl;
    std::cout << "------ test toString() ------" << std::endl;
    std::vector<int> vec_config = {1, 2, 3};
    config::ConfigVar<std::vector<int>> configVar("test2", vec_config, "test vector config var");
    std::cout << configVar.toString() << std::endl;
    std::cout << "------ test fromString() ------" << std::endl;
    std::cout << "changing the content: [4,5,6] -> " << std::endl;
    configVar.fromString("[4,5,6]");
    std::cout << configVar.toString() << std::endl;

    std::cout << "====== test[3/3]: final test ======" << std::endl;
    std::unordered_map<std::string, std::vector<std::set<int>>> ump;
    ump["zoe"] = std::vector<std::set<int>>({{1, 2, 3}, {4, 5, 6}, {7, 8, 9}});
    ump["carl"] = std::vector<std::set<int>>({{9, 9, 9}, {8, 8, 8}, {7, 7, 7}});
    config::ConfigVar<std::unordered_map<std::string, std::vector<std::set<int>>>> cc("final test", ump, "final test");
    std::cout << cc.toString() << std::endl;
}

void testConfig()
{
    using config::ConfigVar;
    using config::Config;

    // the next line will intrigue a config change event
    std::shared_ptr<ConfigVar<int>> int_config(Config::lookup("system.port", (int)8080, "system port"));

    int_config->addListener(0x1f1f1f, [](const int &old_value, const int &new_value){
        CCZOE_LOG_INFO(CCZOE_LOG_ROOT()) << "int config changed from " << old_value << " to " << new_value;
    });

    // the next line will not intrigue a config change event
    std::shared_ptr<ConfigVar<float>> float_config(Config::lookup("system.value", (float)10.2, "system value"));

    // the next line will intrigue a config change event
    std::shared_ptr<config::ConfigVar<std::vector<int>>> int_vec_config(Config::lookup("system.int_vec", std::vector<int>{1, 2}, "system int vector"));

    int_vec_config->addListener(0x2f2f2f, [](const std::vector<int> &old_value, const std::vector<int> &new_value){
        std::stringstream from;
        for (size_t i = 0; i < old_value.size(); i++)
        {
            if (i != old_value.size() - 1)
                from << old_value[i] << ",";
            else
                from << old_value[i] << "]";
        }
        std::stringstream to;
        for (size_t i = 0; i < new_value.size(); i++)
        {
            if (i != new_value.size() - 1)
                to << new_value[i] << ",";
            else
                to << new_value[i] << "]";
        }
        CCZOE_LOG_INFO(CCZOE_LOG_ROOT()) << "int vector config changed from [" << from.str() << " to [" << to.str();
    });

    // the next line will not intrigue a config change event
    std::shared_ptr<ConfigVar<std::list<int>>> int_list_config(Config::lookup("system.int_list", std::list<int>{1, 2}, "system int vector"));

    // the next line will not intrigue a config change event
    std::shared_ptr<ConfigVar<std::set<int>>> int_set_config(Config::lookup("system.int_set", std::set<int>{1, 2}, "system int set"));

    // the next line will not intrigue a config change event
    std::shared_ptr<ConfigVar<std::unordered_set<int>>> int_uset_config(Config::lookup("system.int_uset", std::unordered_set<int>{1, 2}, "system int uset"));

    // the next line will not intrigue a config change event
    std::shared_ptr<ConfigVar<std::map<std::string, int>>> str_int_map_config(Config::lookup("system.str_int_map", std::map<std::string, int>{{"k", 2}}, "system str int map"));

    // the next line will output a error log
    std::shared_ptr<ConfigVar<std::vector<int>>> int_vector_config2(Config::lookup("system.int_list", std::vector<int>{1, 2}, "system int vector"));

    Config::loadFromFile("../tests/testYamlFile.yml");

}

void testYamlFile()
{
    YAML::Node node = YAML::LoadFile("../tests/testConfig.yml");
    std::list<std::pair<std::string, YAML::Node>> output;
    config::Config::parseNodes("", node, output);
    for (auto &i : output)
    {
        std::cout << i.first << ": " << i.second << std::endl;
    }
}
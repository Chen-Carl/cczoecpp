#include <iostream>
#include <string>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <yaml-cpp/yaml.h>

void testBasicParsing();
void testYamlType();

int main()
{
    testBasicParsing();
    testYamlType();
    return 0;
}

void testBasicParsing()
{
    std::cout << "------ test1: test basic parsing ------" << std::endl;
    YAML::Node node = YAML::Load("[1, 2, 3]");
    assert(node.Type() == YAML::NodeType::Sequence);
    std::cout << "load " << node.size() << " numbers: ";
    for (YAML::const_iterator it = node.begin(); it != node.end(); it++)
    {
        std::cout << it->as<int>() << " ";
    }
    std::cout << std::endl;
}

void testYamlType()
{
    std::cout << "------ test2: test yaml type ------" << std::endl;
    char *buffer = getcwd(NULL, 0);
    std::cout << "current pwd: " << buffer << std::endl;
    free(buffer);
    YAML::Node node = YAML::LoadFile("../tests/testYaml.yml");
    // std::cout << node << std::endl;
    std::cout << "name: " << node["name"].as<std::string>() << std::endl;
    std::cout << "sex: " << node["sex"].as<std::string>() << std::endl;
    std::cout << "age: " << node["age"].as<int>() << std::endl;
    std::cout << "skills: " << node["skills"]["c++"].as<std::string>() << std::endl;
    std::cout << "skills: " << node["skills"]["java"].as<std::string>() << std::endl;
}
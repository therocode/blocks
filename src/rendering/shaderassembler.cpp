#include "shaderassembler.hpp"
#include <algorithm>
#include <iostream>

std::string ShaderAssembler::assemble(const std::string& source, const std::vector<std::string>& moduleSources)
{
    mOriginalMappings.clear();

    std::vector<ShaderModule> modules = parseModules(moduleSources);
    std::string result = source;

    if(source.find("~%~") != std::string::npos)
    {
        std::unordered_map<std::string, ShaderVariable> shaderVariables;

        size_t variablePos;

        while((variablePos = result.find("~~")) != std::string::npos)
        {
            ShaderVariable shaderVariable;
            std::string variableLine = result.substr(variablePos, result.find("~~", variablePos + 2) - variablePos + 2);

            if((std::count(variableLine.begin(), variableLine.end(), '|') < 2) || (variableLine.find("||") != std::string::npos))
            {
                std::cout << "Error, shader variable line malformatted! Should be: ~~registerName|type|variable~\n";
                exit(0); //TODO: exception
            }

            result.erase(variablePos, result.find("~~", variablePos + 2) - variablePos + 2);

            size_t sectionStart = 2;
            size_t sectionEnd = variableLine.find_first_of('|');

            shaderVariable.registerName = variableLine.substr(sectionStart, sectionEnd - sectionStart);

            sectionStart = sectionEnd + 1;
            sectionEnd = variableLine.find_first_of('|', sectionStart);

            shaderVariable.type = variableLine.substr(sectionStart, sectionEnd - sectionStart);

            sectionStart = sectionEnd + 1;
            sectionEnd = variableLine.find_first_of('~', sectionStart);

            shaderVariable.currentChainer = variableLine.substr(sectionStart, sectionEnd - sectionStart);
            mOriginalMappings.emplace(shaderVariable.registerName, shaderVariable.currentChainer);

            shaderVariables.emplace(shaderVariable.registerName, shaderVariable);
        }

        size_t insertLocation = result.find("~%~");
        result.erase(insertLocation, 3);
        std::string shaderModuleSource;
        shaderModuleSource += "/////////START GENERATED CODE////////\n";

        //for(uint32_t i = 0; i < shaderVariables.size(); i++)
        //{
        //    const ShaderVariable& variable = shaderVariables[i];
        //    shaderModuleSource +=
        //}

        for(const auto& module : modules)
        {
            for(const auto& variableMapping : module.variableMappings)
            {
                auto shaderVariableIter = shaderVariables.find(variableMapping.first);
                const std::string& uniqueName = variableMapping.second;

                if(shaderVariableIter != shaderVariables.end())
                {
                    auto& shaderVariable = shaderVariableIter->second;
                    shaderModuleSource += shaderVariable.type + " " + uniqueName + " = " + shaderVariable.currentChainer + ";\n\n";
                    shaderVariable.currentChainer = uniqueName;
                    shaderModuleSource += module.source + "\n";
                }
                else
                {
                    std::cout << "Error, shader variable " << variableMapping.first << " doesn't exist!\n";
                    exit(0); //TODO: exception
                }
            }
        }

        if(modules.size() > 0)
        {
            const auto& lastModule = modules.back();

            for(const auto& variableMapping : lastModule.variableMappings)
            {

                shaderModuleSource += mOriginalMappings.at(variableMapping.first) + " = " + variableMapping.second + ";\n";
            }

            shaderModuleSource += "/////////END GENERATED CODE////////\n\n";

            result.insert(insertLocation, shaderModuleSource);
        }
    }

    return result;
}

std::vector<ShaderAssembler::ShaderModule> ShaderAssembler::parseModules(const std::vector<std::string>& moduleSources) const
{
    std::vector<ShaderModule> result;
    size_t iterator = 0;

    for(auto source : moduleSources)
    {
        ShaderModule module;
        module.source += "  //module " + std::to_string(iterator) + " content";
        size_t variablePos;
        while((variablePos = source.find("~~")) != std::string::npos)
        {
            std::string variableLine = source.substr(variablePos, source.find("~~", variablePos + 2) - variablePos + 2);

            source.erase(variablePos, source.find("~~", variablePos + 2) - variablePos + 2);

            std::string variableName = variableLine.substr(2, variableLine.size() - 4);

            std::string uniqueName = "gen_" + variableName + std::to_string(iterator);
            std::string occurrenceString = "~" + variableName + "~";

            module.variableMappings.emplace(variableName, uniqueName);

            source = replaceAll(source, occurrenceString, uniqueName);
        }
        
        module.source += source;
        module.source += "  //end module " + std::to_string(iterator) + " content\n";
        result.push_back(module);
        iterator++;
    }

    return result;
}

std::string ShaderAssembler::replaceAll(std::string input, const std::string& from, const std::string& to) const
{
    std::string::size_type n = 0;
    while((n = input.find(from, n)) != std::string::npos)
    {
        input.replace(n, from.size(), to);
        n += to.size();
    }

    return input;
}

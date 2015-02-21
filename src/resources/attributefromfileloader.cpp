#include "attributefromfileloader.hpp"
#include <json/json.hpp>
#include <fstream>
#include "resourceexception.hpp"
#include <fea/entity/jsonentityloader.hpp>

std::unordered_map<std::string, std::string> AttributeFromFileLoader::load(const std::string& filename)
{
	fea::JsonEntityLoader loader;
	return loader.loadEntityAttributes(filename);
}

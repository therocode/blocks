#include "entityfromfileloader.hpp"
#include <json/json.hpp>
#include <fstream>
#include "resourceexception.hpp"
#include <fea/entity/jsonentityloader.hpp>

std::vector<std::pair<std::string, fea::EntityTemplate>> EntityFromFileLoader::load(const std::string& filename)
{
	fea::JsonEntityLoader loader;
	return loader.loadEntityTemplates(filename);
}

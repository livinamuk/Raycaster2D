#include "Scene.h"

std::vector<Light> Scene::s_lights;

void Scene::AddLight(int x, int y, glm::vec3 lightColor, float lightScale, LightType lightType, float lightStrength)
{
	s_lights.push_back(Light(glm::vec2(x, y), lightColor, lightScale, lightType, lightStrength));
}

void Scene::SaveScene(std::string filename)
{
	rapidjson::Document document;
	rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
	rapidjson::Value savefileArray(rapidjson::kArrayType);
	document.SetObject();
	rapidjson::Value object(rapidjson::kObjectType);

	// Lights
	rapidjson::Value lightsArray;
	lightsArray.SetArray();
	for (Light& light : s_lights) {
		rapidjson::Value flagObject;
		flagObject.SetObject();
		SaveVec2(&flagObject, "Position", light.m_position, allocator);
		SaveVec3(&flagObject, "Color", light.m_color, allocator);
		SaveInt(&flagObject, "Type", light.m_type, allocator);
		SaveFloat(&flagObject, "Strength", light.m_strength, allocator);
		SaveFloat(&flagObject, "Scale", light.m_scale, allocator);
		lightsArray.PushBack(flagObject, allocator);
	}
	object.AddMember("Lights", lightsArray, allocator);


	savefileArray.PushBack(object, allocator);
	document.AddMember("Savefile", savefileArray, allocator);

	// Convert JSON document to string
	rapidjson::StringBuffer strbuf;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(strbuf);
	document.Accept(writer);

	// Save it
	std::string data = strbuf.GetString();
	std::ofstream out(filename);
	out << data;
	out.close();
}
void Scene::SaveString(rapidjson::Value* object, std::string elementName, std::string string, rapidjson::Document::AllocatorType& allocator)
{
	rapidjson::Value name(elementName.c_str(), allocator);
	rapidjson::Value value(rapidjson::kObjectType);
	value.SetString(string.c_str(), static_cast<rapidjson::SizeType>(string.length()), allocator);
	object->AddMember(name, value, allocator);
}
void Scene::SaveFloat(rapidjson::Value* object, std::string elementName, float number, rapidjson::Document::AllocatorType& allocator)
{
	rapidjson::Value name(elementName.c_str(), allocator);
	rapidjson::Value value(rapidjson::kObjectType);
	value.SetFloat(number);
	object->AddMember(name, value, allocator);
}
void Scene::SaveInt(rapidjson::Value* object, std::string elementName, int number, rapidjson::Document::AllocatorType& allocator)
{
	rapidjson::Value name(elementName.c_str(), allocator);
	rapidjson::Value value(rapidjson::kObjectType);
	value.SetInt(number);
	object->AddMember(name, value, allocator);
}
void Scene::SaveVec3(rapidjson::Value* object, std::string elementName, glm::vec3 vector, rapidjson::Document::AllocatorType& allocator)
{
	rapidjson::Value array(rapidjson::kArrayType);
	rapidjson::Value name(elementName.c_str(), allocator);
	array.PushBack(rapidjson::Value().SetFloat(vector.x), allocator);
	array.PushBack(rapidjson::Value().SetFloat(vector.y), allocator);
	array.PushBack(rapidjson::Value().SetFloat(vector.z), allocator);
	object->AddMember(name, array, allocator);
}
void Scene::SaveVec2(rapidjson::Value* object, std::string elementName, glm::vec2 vector, rapidjson::Document::AllocatorType& allocator)
{
	rapidjson::Value array(rapidjson::kArrayType);
	rapidjson::Value name(elementName.c_str(), allocator);
	array.PushBack(rapidjson::Value().SetFloat(vector.x), allocator);
	array.PushBack(rapidjson::Value().SetFloat(vector.y), allocator);
	object->AddMember(name, array, allocator);
}


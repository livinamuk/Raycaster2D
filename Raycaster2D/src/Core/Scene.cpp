#include "Scene.h"

std::vector<Light> Scene::s_lights;

void Scene::AddLight(int x, int y, glm::vec3 lightColor, float lightScale, int lightType, float strength, int rotate)
{
	s_lights.push_back(Light(glm::vec2(x, y), lightColor, lightScale, lightType, strength, rotate));
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
		SaveInt(&flagObject, "X", light.m_position.x, allocator);
		SaveInt(&flagObject, "Y", light.m_position.y, allocator);
		SaveFloat(&flagObject, "ColorR", light.m_color.x, allocator);
		SaveFloat(&flagObject, "ColorG", light.m_color.y, allocator);
		SaveFloat(&flagObject, "ColorB", light.m_color.z, allocator);
		SaveInt(&flagObject, "Type", light.m_type, allocator);
		SaveInt(&flagObject, "Rotate", light.m_rotate, allocator);
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
void Scene::LoadScene(std::string filename)
{
	std::string fileName = filename;
	FILE* pFile = fopen(fileName.c_str(), "rb");
	char buffer[65536];
	rapidjson::FileReadStream is(pFile, buffer, sizeof(buffer));
	rapidjson::Document document;
	document.ParseStream<0, rapidjson::UTF8<>, rapidjson::FileReadStream>(is);

	// Check for errors
	if (document.HasParseError())
		std::cout << "Error  : " << document.GetParseError() << '\n' << "Offset : " << document.GetErrorOffset() << '\n';

	const rapidjson::Value& a = document["Savefile"];
	assert(a.IsArray());

	for (rapidjson::SizeType i = 0; i < a.Size(); i++)
	{
		auto element = a[i].GetObject();

		// Quests
		auto lights = element["Lights"].GetArray();
		for (rapidjson::SizeType j = 0; j < lights.Size(); j++) {
			auto light = lights[j].GetObject();

			int x = light["X"].GetInt();
			int y = light["Y"].GetInt();
			float r = light["ColorR"].GetFloat();
			float g = light["ColorG"].GetFloat();
			float b = light["ColorB"].GetFloat();
			int type = light["Type"].GetInt();
			int rotate = light["Rotate"].GetInt();
			float strength = light["Strength"].GetFloat();
			float scale = light["Scale"].GetFloat();

			Scene::AddLight(x, y, glm::vec3(r,g,b), scale, type, strength, rotate);
		}
	}
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


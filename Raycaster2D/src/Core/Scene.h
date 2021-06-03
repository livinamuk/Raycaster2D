#pragma once
#include "Header.h"
#include "Core/Light.h"
#include "Helpers/Util.h"

class Scene
{
public: // static functions
	static void AddLight(int x, int y, glm::vec3 color, float scale, int type, float strength, int rotate);
	static void SaveScene(std::string filename);
	static void LoadScene(std::string filename);

public: // static class varaibles
	static std::vector<Light> s_lights;

private:
	static void SaveString(rapidjson::Value* object, std::string elementName, std::string string, rapidjson::Document::AllocatorType& allocator);
	static void SaveFloat(rapidjson::Value* object, std::string elementName, float number, rapidjson::Document::AllocatorType& allocator);
	static void SaveInt(rapidjson::Value* object, std::string elementName, int number, rapidjson::Document::AllocatorType& allocator);
	static void SaveVec3(rapidjson::Value* object, std::string elementName, glm::vec3 vector, rapidjson::Document::AllocatorType& allocator);
	static void SaveVec2(rapidjson::Value* object, std::string elementName, glm::vec2 vector, rapidjson::Document::AllocatorType& allocator);
};

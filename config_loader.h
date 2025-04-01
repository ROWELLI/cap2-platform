#ifndef CONFIG_LOADER_H
#define CONFIG_LOADER_H

#include <string>
#include <map>
#include <fstream>
#include <jsoncpp/json/json.h>
#include <iostream>

#define MAX_ITEMS 50

struct Config
{
    int SCREEN_WIDTH;
    int SCREEN_HEIGHT;
    int RECT_COUNT;
    int RECT_WIDTH;
    int RECT_HEIGHT;
    int RECT_GAP;
    int BOTTOM_MARGIN;
    float SELECTED_SCALE;
    float ANIMATION_SPEED;
    float SCALE_SPEED;
    std::string FONT_PATH;
    int FONT_SIZE;
    std::string IMAGE_FILES[MAX_ITEMS];
    int IMAGE_FILE_COUNT;
    std::string BACKGROUND_IMAGE;
    std::map<std::string, std::map<std::string, std::pair<std::string, std::string>>> secondScreenContent;
    std::string secondScreenMapping[MAX_ITEMS];
    int MAPPING_COUNT;
};

inline bool loadConfig(Config &config, const std::string &filename = "config.json")
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "설정 파일을 열 수 없습니다: " << filename << std::endl;
        return false;
    }

    Json::Value root;
    Json::CharReaderBuilder builder;
    std::string errors;

    if (!Json::parseFromStream(builder, file, &root, &errors))
    {
        std::cerr << "JSON 파싱 오류: " << errors << std::endl;
        return false;
    }

    try
    {
        config.SCREEN_WIDTH = root["SCREEN_WIDTH"].asInt();
        config.SCREEN_HEIGHT = root["SCREEN_HEIGHT"].asInt();
        config.RECT_COUNT = root["RECT_COUNT"].asInt();
        config.RECT_WIDTH = root["RECT_WIDTH"].asInt();
        config.RECT_HEIGHT = root["RECT_HEIGHT"].asInt();
        config.RECT_GAP = root["RECT_GAP"].asInt();
        config.BOTTOM_MARGIN = root["BOTTOM_MARGIN"].asInt();
        config.SELECTED_SCALE = root["SELECTED_SCALE"].asFloat();
        config.ANIMATION_SPEED = root["ANIMATION_SPEED"].asFloat();
        config.SCALE_SPEED = root["SCALE_SPEED"].asFloat();
        config.FONT_PATH = root["FONT_PATH"].asString();
        config.FONT_SIZE = root["FONT_SIZE"].asInt();
        config.BACKGROUND_IMAGE = root["BACKGROUND_IMAGE"].asString();

        config.IMAGE_FILE_COUNT = 0;
        for (const auto &f : root["IMAGE_FILES"])
        {
            if (config.IMAGE_FILE_COUNT >= MAX_ITEMS)
                break;
            config.IMAGE_FILES[config.IMAGE_FILE_COUNT++] = f.asString();
        }

        config.MAPPING_COUNT = 0;
        for (const auto &f : root["SECOND_SCREEN_MAPPING"])
        {
            if (config.MAPPING_COUNT >= MAX_ITEMS)
                break;
            config.secondScreenMapping[config.MAPPING_COUNT++] = f.asString();
        }

        for (const auto &category : root["SECOND_SCREEN_CONTENT"].getMemberNames())
        {
            const Json::Value &items = root["SECOND_SCREEN_CONTENT"][category];
            for (const auto &itemName : items.getMemberNames())
            {
                std::string imagePath = items[itemName]["CONTENT_IMAGE"].asString();
                std::string command = items[itemName].get("COMMAND", "").asString();
                config.secondScreenContent[category][itemName] = std::make_pair(imagePath, command);
            }
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "설정값 변환 오류: " << e.what() << std::endl;
        return false;
    }

    return true;
}

#endif // CONFIG_LOADER_H
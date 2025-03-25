// config_loader.h + cpp 통합 파일
#ifndef CONFIG_LOADER_H
#define CONFIG_LOADER_H

#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <jsoncpp/json/json.h>
#include <iostream>

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
    std::vector<std::string> IMAGE_FILES;
    std::string BACKGROUND_IMAGE;

    // second screen
    std::map<std::string, std::vector<std::string>> secondScreenContent;
    std::vector<std::string> secondScreenMapping;
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

        for (const auto &f : root["IMAGE_FILES"])
            config.IMAGE_FILES.push_back(f.asString());

        for (const auto &f : root["SECOND_SCREEN_MAPPING"])
            config.secondScreenMapping.push_back(f.asString());

        for (const auto &key : root["SECOND_SCREEN_CONTENT"].getMemberNames())
        {
            for (const auto &val : root["SECOND_SCREEN_CONTENT"][key])
            {
                config.secondScreenContent[key].push_back(val.asString());
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
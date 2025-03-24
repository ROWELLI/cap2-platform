#ifndef CONFIG_LOADER_H
#define CONFIG_LOADER_H

#include <iostream>
#include <fstream>
#include <jsoncpp/json/json.h>
#include <vector>

using namespace std;

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
    string FONT_PATH;
    int FONT_SIZE;
    vector<string> IMAGE_FILES;
    string BACKGROUND_IMAGE;
};

bool loadConfig(Config &config, const string &filename = "config.json")
{
    ifstream file(filename, ifstream::binary);
    if (!file.is_open())
    {
        cerr << "설정 파일을 열 수 없습니다: " << filename << endl;
        return false;
    }

    Json::Value root;
    Json::CharReaderBuilder builder;
    string errors;

    if (!Json::parseFromStream(builder, file, &root, &errors))
    {
        cerr << "JSON 파싱 오류: " << errors << endl;
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

        for (const auto &file : root["IMAGE_FILES"])
        {
            config.IMAGE_FILES.push_back(file.asString());
        }

        config.BACKGROUND_IMAGE = root["BACKGROUND_IMAGE"].asString();
    }
    catch (exception &e)
    {
        cerr << "설정값 변환 오류: " << e.what() << endl;
        return false;
    }

    return true;
}

#endif // CONFIG_LOADER_H
#ifndef CONFIG_LOADER_H
#define CONFIG_LOADER_H

#include <string>
#include <map>
#include <fstream>
#include <jsoncpp/json/json.h>
#include <iostream>

#define MAX_ITEMS 50

// Key definitions for input handling
#define KEY_LEFT SDLK_LEFT
#define KEY_RIGHT SDLK_RIGHT
#define KEY_SELECT SDLK_RETURN
#define KEY_BACK SDLK_ESCAPE

using namespace std;

// Structure to store configuration settings
struct Config
{
    int SCREEN_WIDTH;              // Width of the screen
    int SCREEN_HEIGHT;             // Height of the screen
    int RECT_COUNT;                // Number of selectable rectangles
    int RECT_WIDTH;                // Width of each rectangle
    int RECT_HEIGHT;               // Height of each rectangle
    int RECT_GAP;                  // Gap between rectangles
    int BOTTOM_MARGIN;             // Margin at the bottom
    float SELECTED_SCALE;          // Scale factor when rectangle is selected
    float ANIMATION_SPEED;         // Speed of the animation
    float SCALE_SPEED;             // Speed of the scaling animation
    string FONT_PATH;              // Path to the font file
    int FONT_SIZE;                 // Font size
    string IMAGE_FILES[MAX_ITEMS]; // List of image file paths
    int IMAGE_FILE_COUNT;          // Number of image files
    string BACKGROUND_IMAGE;       // Background image path

    // Second screen content structure: category -> item -> (image, command)
    map<string, map<string, pair<string, string>>> secondScreenContent;

    // Mapping between main screen items and second screen content
    string secondScreenMapping[MAX_ITEMS];
    int MAPPING_COUNT;
};

// Function to load configuration from a JSON file
inline bool loadConfig(Config &config, const string &filename = "config.json")
{
    ifstream file(filename);
    if (!file.is_open())
    {
        cerr << "Failed to open config file: " << filename << endl;
        return false;
    }

    Json::Value root;
    Json::CharReaderBuilder builder;
    string errors;

    // Parse JSON content
    if (!Json::parseFromStream(builder, file, &root, &errors))
    {
        cerr << "Failed to parse JSON: " << errors << endl;
        return false;
    }

    try
    {
        // Load basic configuration values
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

        // Load image file list
        config.IMAGE_FILE_COUNT = 0;
        for (const auto &f : root["IMAGE_FILES"])
        {
            if (config.IMAGE_FILE_COUNT >= MAX_ITEMS)
                break;
            config.IMAGE_FILES[config.IMAGE_FILE_COUNT++] = f.asString();
        }

        // Load second screen mapping
        config.MAPPING_COUNT = 0;
        for (const auto &f : root["SECOND_SCREEN_MAPPING"])
        {
            if (config.MAPPING_COUNT >= MAX_ITEMS)
                break;
            config.secondScreenMapping[config.MAPPING_COUNT++] = f.asString();
        }

        // Load second screen content
        for (const auto &category : root["SECOND_SCREEN_CONTENT"].getMemberNames())
        {
            const Json::Value &items = root["SECOND_SCREEN_CONTENT"][category];
            for (const auto &itemName : items.getMemberNames())
            {
                string imagePath = items[itemName]["CONTENT_IMAGE"].asString();
                string command = items[itemName].get("COMMAND", "").asString();
                config.secondScreenContent[category][itemName] = make_pair(imagePath, command);
            }
        }
    }
    catch (const exception &e)
    {
        cerr << "Error parsing config values: " << e.what() << endl;
        return false;
    }

    return true;
}

#endif // CONFIG_LOADER_H

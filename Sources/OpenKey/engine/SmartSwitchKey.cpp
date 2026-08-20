//
//  SmartSwitchKey.cpp
//  OpenKey
//
//  Created by Tuyen on 8/13/19.
//  Copyright © 2019 Tuyen Mai. All rights reserved.
//

#include "SmartSwitchKey.h"
#include <map>
#include <iostream>
#include <memory.h>

//main data, i use `map` because it has O(Log(n))
static map<string, Int8> _smartSwitchKeyData;
static string _cacheKey = ""; //use cache for faster
static Int8 _cacheData = 0; //use cache for faster

void initSmartSwitchKey(const Byte* pData, const int& size) {
    _smartSwitchKeyData.clear();
    if (pData == NULL) return;
    Uint16 count = 0;
    Uint32 cursor = 0;
    if (size >= 2) {
        memcpy(&count, pData + cursor, 2);
        cursor+=2;
    }
    Uint8 bundleIdSize;
    Uint8 value;
    for (int i = 0; i < count; i++) {
        bundleIdSize = pData[cursor++];
        string bundleId((char*)pData + cursor, bundleIdSize);
        cursor += bundleIdSize;
        value = pData[cursor++];
        _smartSwitchKeyData[bundleId] = value;
    }
}

void getSmartSwitchKeySaveData(vector<Byte>& outData) {
    outData.clear();
    Uint16 count = (Uint16)_smartSwitchKeyData.size();
    outData.push_back((Byte)count);
    outData.push_back((Byte)(count>>8));
    
    for (std::map<string, Int8>::iterator it = _smartSwitchKeyData.begin(); it != _smartSwitchKeyData.end(); ++it) {
        outData.push_back((Byte)it->first.length());
        for (int j = 0; j < it->first.length(); j++) {
            outData.push_back(it->first[j]);
        }
        outData.push_back(it->second);
    }
}

#include <algorithm>

bool isGameApp(string name) {
    std::transform(name.begin(), name.end(), name.begin(), [](unsigned char ch) { return std::tolower(ch); });
    static const vector<string> gameList = {
        "league of legends.exe", "leagueclientux.exe", "leagueclient.exe", "valorant.exe", "valorant-win64-shipping.exe",
        "csgo.exe", "cs2.exe", "dota2.exe", "gta5.exe", "overwatch.exe", "genshinimpact.exe", "starrail.exe"
    };
    for (size_t i = 0; i < gameList.size(); i++) {
        if (name == gameList[i]) return true;
    }
    return false;
}

static bool isExcludedApp(string name) {
    std::transform(name.begin(), name.end(), name.begin(), [](unsigned char ch) { return std::tolower(ch); });
    static const vector<string> excludedList = {
        "cmd.exe", "powershell.exe", "pwsh.exe", "windowsterminal.exe", "wt.exe",
        "mintty.exe", "bash.exe", "git-bash.exe", "conhost.exe", "alacritty.exe", "wezterm-gui.exe",
        "code.exe", "devenv.exe", "clion64.exe", "idea64.exe", "pycharm64.exe",
        "webstorm64.exe", "rider64.exe", "sublime_text.exe", "notepad++.exe",
        "steam.exe", "epicgameslauncher.exe", "league of legends.exe", "valorant.exe",
        "csgo.exe", "cs2.exe", "dota2.exe", "gta5.exe", "overwatch.exe"
    };
    for (size_t i = 0; i < excludedList.size(); i++) {
        if (name == excludedList[i]) return true;
    }
    return false;
}

int getAppInputMethodStatus(const string& bundleId, const int& currentInputMethod) {
    if (_cacheKey.compare(bundleId) == 0) {
        return _cacheData;
    }
    if (_smartSwitchKeyData.find(bundleId) != _smartSwitchKeyData.end()) {
        _cacheKey = bundleId;
        _cacheData = _smartSwitchKeyData[bundleId];
        return _cacheData;
    }
    _cacheKey = bundleId;
    _cacheData = isExcludedApp(bundleId) ? 0 : currentInputMethod;
    _smartSwitchKeyData[bundleId] = _cacheData;
    return _cacheData;
}

void setAppInputMethodStatus(const string& bundleId, const int& language) {
    _smartSwitchKeyData[bundleId] = language;
    _cacheKey = bundleId;
    _cacheData = language;
}


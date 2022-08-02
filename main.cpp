#define WINVER 0x0500
#pragma region includes
#include <windows.h>
#include <chrono>
#include <string>
#include <thread>
#include <iostream>
#include <vector>
#include <fstream>
#include <random>
#include <regex>
#include <stdio.h>
#pragma region CIncludes
extern "C" {
    #include "lua.h"
    #include "lauxlib.h"
    #include "lualib.h"
    #include <ctime>
    #include "duktape.h"
}
#pragma endregion

#pragma endregion

#pragma region using
using std::string;
using std::vector;
using std::to_string;
#pragma endregion

#pragma region globaldata
bool logIntoFile = false;
string fileLocation = "log{mon}{day}{year} {hour} {min} {sec}.txt";
struct timePoint {
    string sec;
    string min;
    string hour;
    string day;
    string month;
    string year;
};
timePoint start;
string finalOutputFilename;
struct luaListener {
    //todo impliment
};
struct jsListener {
    string hotkey;
    void* functionptr;
};
struct listener {
    bool isLua;
    luaListener luaListen;
    jsListener jsListen;
    string hotkey;
};
std::vector<listener> hotkeys = {};
void callListener(string hotkey) {
    //todo impliment
}
#pragma endregion

#pragma region cppfuncs

#pragma region randomizer
int numCalls = 0;
double random(double min, double max) {
    const auto p1 = std::chrono::system_clock::now();
    auto sinc = (double)std::chrono::duration_cast<std::chrono::milliseconds>(p1.time_since_epoch()).count();
    std::random_device dev;
    std::mt19937 rng(dev() + sinc * numCalls);
    std::uniform_real_distribution<double> rang(min, max);
    double ret = rang(rng);
    numCalls++;
    return ret;
}
#pragma endregion

#pragma region fileHandling
bool fileExists(string& path) {
    FILE* file;
    fopen_s(&file, path.c_str(), "r");
    if (file) {
        fclose(file);
        return true;
    }
    else {
        return false;
    }
}
string readFileSTR(string& filepath) {
    if (fileExists(filepath)) {
        std::ifstream input_file{};
        input_file.open(filepath);
        if (!input_file.is_open()) {
            std::cerr << "Could not open the file - '" << filepath << "'\n";
            exit(EXIT_FAILURE);
        }
        return string((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());
    }
}
#pragma endregion

#pragma region keyboard
void typeChar(char c, int delayMillis = 0) {
    INPUT ip;
    // Set up a generic keyboard event.
    ip.type = INPUT_KEYBOARD;
    ip.ki.wScan = 0; // hardware scan code for key
    ip.ki.time = 0;
    ip.ki.dwExtraInfo = 0;

    // Press the "A" ke
    int num = int(c);
    ip.ki.wVk = num; // virtual-key code for the "a" key
    ip.ki.dwFlags = 0; // 0 for key press
    SendInput(1, &ip, sizeof(INPUT));
    //if delay, add delay
    if (delayMillis != 0) {
        auto dur = std::chrono::milliseconds(delayMillis);
        std::this_thread::sleep_for(dur);
    }
    // Release the "A" key
    ip.ki.dwFlags = KEYEVENTF_KEYUP; // KEYEVENTF_KEYUP for key release
    SendInput(1, &ip, sizeof(INPUT));
}
bool needsShift(char c) {
    string chars = ")!@#$%^&*(ABCDEFGHIJKLMNOPQRSTUVWXYZ<>?:\"{}|_+~";
    for (int i = 0; i < chars.size(); ++i) {
        char c2 = chars[i];
        if (c == c2) {
            return true;
        }
    }
    return false;
}
//defining a function that takes in a string and types the values on my keyboard
void typeString(string& str, int ButtonHoldTime = 0, int ButtonSpaceTime = 0) {
    bool needsInbetweenTime = (ButtonSpaceTime != 0);
    INPUT ip;
    ip.type = INPUT_KEYBOARD;
    ip.ki.wScan = 0;
    ip.ki.time = 0;
    ip.ki.dwExtraInfo = 0;
    HKL kbl = GetKeyboardLayout(0);
    for (unsigned int i = 0; i < str.length(); ++i)
    {
        char c = str[i];
        bool needsS = needsShift(c);
        if (needsS) {
            ip.ki.wVk = VK_SHIFT;
            ip.ki.dwFlags = 0; //<= Add this to indicate key-down
            SendInput(1, &ip, sizeof(ip));
        }
        ip.ki.wVk = VkKeyScanExA(c, kbl); //<== don't mix ANSI with UNICODE
        ip.ki.dwFlags = 0; //<= Add this to indicate key-down
        SendInput(1, &ip, sizeof(ip));
        if (ButtonHoldTime != 0) {
            auto dur = std::chrono::milliseconds(ButtonHoldTime);
            std::this_thread::sleep_for(dur);
        }
        if (needsS) {
            ip.ki.wVk = VK_SHIFT;
            ip.ki.dwFlags = KEYEVENTF_KEYUP; //<= Add this to indicate key-down
            SendInput(1, &ip, sizeof(ip));
        }
        ip.ki.dwFlags = KEYEVENTF_KEYUP;
        SendInput(1, &ip, sizeof(ip));
        if (needsInbetweenTime) {
            auto dur = std::chrono::milliseconds(ButtonSpaceTime);
            std::this_thread::sleep_for(dur);
        }
    }
}
void typeStringNatural(string& str, double minButtonSpaceDelay, double maxButtonSpaceDelay, double minButtonHoldTime, double maxButtonHoldTime) {
    INPUT ip;
    ip.type = INPUT_KEYBOARD;
    ip.ki.wScan = 0;
    ip.ki.time = 0;
    ip.ki.dwExtraInfo = 0;
    HKL kbl = GetKeyboardLayout(0);
    for (unsigned int i = 0; i < str.length(); ++i)
    {
        char c = str[i];
        bool needsS = needsShift(c);
        if (needsS) {
            ip.ki.wVk = VK_SHIFT;
            ip.ki.dwFlags = 0; //<= Add this to indicate key-down
            SendInput(1, &ip, sizeof(ip));
            auto ButtonHoldTime = random(minButtonSpaceDelay, maxButtonSpaceDelay);
            auto dur = std::chrono::milliseconds(int(floor(ButtonHoldTime)));
            std::this_thread::sleep_for(dur);
        }
        ip.ki.wVk = VkKeyScanExA(c, kbl);
        ip.ki.dwFlags = 0; //<= Add this to indicate key-down
        SendInput(1, &ip, sizeof(ip));
        auto ButtonHoldTime = random(minButtonHoldTime, maxButtonHoldTime);
        auto dur = std::chrono::milliseconds(int(floor(ButtonHoldTime)));
        std::this_thread::sleep_for(dur);
        if (needsS) {
            ip.ki.wVk = VK_SHIFT;
            ip.ki.dwFlags = KEYEVENTF_KEYUP;
            SendInput(1, &ip, sizeof(ip));
            auto ButtonHoldTime3 = random(minButtonSpaceDelay, maxButtonSpaceDelay);
            auto dur3 = std::chrono::milliseconds(int(floor(ButtonHoldTime3)));
            std::this_thread::sleep_for(dur3);
        }
        ip.ki.wVk = VkKeyScanExA(c, kbl);
        ip.ki.dwFlags = KEYEVENTF_KEYUP;
        SendInput(1, &ip, sizeof(ip));
        auto ButtonHoldTime2 = random(minButtonSpaceDelay, maxButtonSpaceDelay);
        auto dur2 = std::chrono::milliseconds(int(floor(ButtonHoldTime2)));
        std::this_thread::sleep_for(dur2);
    }
}
void pressKey(string& cha) {
    INPUT ip;
    ip.type = INPUT_KEYBOARD;
    ip.ki.wScan = 0;
    ip.ki.time = 0;
    ip.ki.dwExtraInfo = 0;
    HKL kbl = GetKeyboardLayout(0);
    auto vkp = VkKeyScanExA(cha[0], kbl);
    ip.ki.wVk = vkp;
    vector<string> compareStrings = { "f1", "f2", "f3", "f4", "f5", "f6", "f7", "f8", "f9", "f10", "f11", "f12", "f13", "f14", "f15", "f16", "f17", "f18", "f19", "f20", "f21", "f22", "f23", "f24", "esc", "control", "shift", "insert", "delete", "start", "home", "end", "alt", "printscreen", "scrolllock", "numlock", "pause", "num0", "num1", "num2", "num3", "num4", "num5", "num6", "num7", "num8", "num9" };
    vector<short> keys = { VK_F1, VK_F2, VK_F3, VK_F4, VK_F5, VK_F6, VK_F7, VK_F8, VK_F9, VK_F10, VK_F11, VK_F12, VK_F13, VK_F14, VK_F15, VK_F16, VK_F17, VK_F18, VK_F19, VK_F20, VK_F21, VK_F22, VK_F23, VK_F24, VK_ESCAPE, VK_CONTROL, VK_SHIFT, VK_INSERT, VK_DELETE, VK_LWIN, VK_HOME, VK_END, VK_MENU, VK_SNAPSHOT, VK_SCROLL, VK_NUMLOCK, VK_PAUSE, VK_NUMPAD0, VK_NUMPAD1, VK_NUMPAD2, VK_NUMPAD3, VK_NUMPAD4, VK_NUMPAD5, VK_NUMPAD6, VK_NUMPAD7, VK_NUMPAD8, VK_NUMPAD9 };
    bool foundNonCharCode = false;
    size_t id = 0;
    for (size_t i = 0; i < compareStrings.size(); ++i) {
        if (cha == compareStrings[i]) {
            foundNonCharCode = true;
            id = i;
            vkp = keys[i];
            break;
        }
    }
    ip.ki.dwFlags = 0; //<= Add this to indicate key-down
    SendInput(1, &ip, sizeof(ip));
}
void releaseKey(string& cha) {
    INPUT ip;
    ip.type = INPUT_KEYBOARD;
    ip.ki.wScan = 0;
    ip.ki.time = 0;
    ip.ki.dwExtraInfo = 0;
    HKL kbl = GetKeyboardLayout(0);
    auto vkp = VkKeyScanExA(cha[0], kbl);
    vector<string> compareStrings = { "f1", "f2", "f3", "f4", "f5", "f6", "f7", "f8", "f9", "f10", "f11", "f12", "f13", "f14", "f15", "f16", "f17", "f18", "f19", "f20", "f21", "f22", "f23", "f24", "esc", "control", "shift", "insert", "delete", "start", "home", "end", "alt", "printscreen", "scrolllock", "numlock", "pause", "num0", "num1", "num2", "num3", "num4", "num5", "num6", "num7", "num8", "num9" };
    vector<short> keys = { VK_F1, VK_F2, VK_F3, VK_F4, VK_F5, VK_F6, VK_F7, VK_F8, VK_F9, VK_F10, VK_F11, VK_F12, VK_F13, VK_F14, VK_F15, VK_F16, VK_F17, VK_F18, VK_F19, VK_F20, VK_F21, VK_F22, VK_F23, VK_F24, VK_ESCAPE, VK_CONTROL, VK_SHIFT, VK_INSERT, VK_DELETE, VK_LWIN, VK_HOME, VK_END, VK_MENU, VK_SNAPSHOT, VK_SCROLL, VK_NUMLOCK, VK_PAUSE, VK_NUMPAD0, VK_NUMPAD1, VK_NUMPAD2, VK_NUMPAD3, VK_NUMPAD4, VK_NUMPAD5, VK_NUMPAD6, VK_NUMPAD7, VK_NUMPAD8, VK_NUMPAD9 };
    bool foundNonCharCode = false;
    size_t id = 0;
    for (size_t i = 0; i < compareStrings.size(); ++i) {
        if (cha == compareStrings[i]) {
            foundNonCharCode = true;
            id = i;
            vkp = keys[i];
            break;
        }
    }
    ip.ki.wVk = vkp;
    ip.ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(1, &ip, sizeof(ip));
}
#pragma endregion

#pragma region mouse
void moveMouse(float x, float y, int moveFlags) {
    INPUT input;
    input.type = INPUT_MOUSE;
    input.mi.mouseData = 0;
    input.mi.dx = x * (65536.0f / GetSystemMetrics(SM_CXSCREEN));
    input.mi.dy = y * (65536.0f / GetSystemMetrics(SM_CYSCREEN));
    input.mi.dwFlags = moveFlags | MOUSEEVENTF_MOVE;
    SendInput(1, &input, sizeof(input));
}
void moveMouseRelative(float x, float y) {
    moveMouse(x, y, 0);
}
void moveMouseAbsolute(float x, float y) {
    moveMouse(x, y, MOUSEEVENTF_ABSOLUTE);
}
void moveMouseTime(float x, float y, float timeS, int frequencyPerSec) {
    size_t numIters = timeS * frequencyPerSec;
    double sec = timeS / (numIters);
    for (size_t i = 0; i < numIters; ++i) {
        moveMouseRelative(x / numIters, y / numIters);
        std::this_thread::sleep_for(std::chrono::nanoseconds((int64_t)floor((double)(sec) * 1000000000)));
    }
}
enum mouseButtons {
    M1 = MK_LBUTTON,
    M2 = MK_RBUTTON,
    M3 = MK_XBUTTON1,
    M4 = MK_XBUTTON2
};
enum mouseButtonEvents {
    M1Down = MOUSEEVENTF_LEFTDOWN,
    M1Up = MOUSEEVENTF_LEFTUP,
    M2Down = MOUSEEVENTF_RIGHTDOWN,
    M2Up = MOUSEEVENTF_RIGHTUP,
    M3Down = MOUSEEVENTF_XDOWN,
    M3Up = MOUSEEVENTF_XUP,
    M4Down = MOUSEEVENTF_XDOWN,
    M4Up = MOUSEEVENTF_XUP
};
void mouseEventDispatch(mouseButtonEvents me) {
    INPUT input;
    input.type = INPUT_MOUSE;
    input.mi.dwFlags = me;
    input.mi.dx = 0;
    input.mi.dy = 0;
    SendInput(1, &input, sizeof(INPUT));
}
void actionMouseButton(string& mb, int holdTime = 0) {
    string m1 = "mouse1";
    string m2 = "mouse2";
    string m3 = "mouse3";
    string m4 = "mouse4";
    if (mb == m1) {
        mouseEventDispatch(mouseButtonEvents::M1Down);
        if (holdTime != 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(holdTime));
        }
        mouseEventDispatch(mouseButtonEvents::M1Up);
    }
    else if (mb == m2) {
        mouseEventDispatch(mouseButtonEvents::M2Down);
        if (holdTime != 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(holdTime));
        }
        mouseEventDispatch(mouseButtonEvents::M2Up);
    } else if (mb == m3) {
        mouseEventDispatch(mouseButtonEvents::M3Down);
        if (holdTime != 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(holdTime));
        }
        mouseEventDispatch(mouseButtonEvents::M3Up);
    }
    else if(mb == m4) {
        mouseEventDispatch(mouseButtonEvents::M4Down);
        if (holdTime != 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(holdTime));
        }
        mouseEventDispatch(mouseButtonEvents::M4Up);
    }
}
void pressMouseButton(string& mb) {
    string m1 = "mouse1";
    string m2 = "mouse2";
    string m3 = "mouse3";
    string m4 = "mouse4";
    if (mb == m1) {
        mouseEventDispatch(mouseButtonEvents::M1Down);
        //mouseEventDispatch(mouseButtonEvents::M1Up);
    }
    else if (mb == m2) {
        mouseEventDispatch(mouseButtonEvents::M2Down);
        //mouseEventDispatch(mouseButtonEvents::M2Up);
    }
    else if (mb == m3) {
        mouseEventDispatch(mouseButtonEvents::M3Down);
        //mouseEventDispatch(mouseButtonEvents::M3Up);
    }
    else if (mb == m4) {
        mouseEventDispatch(mouseButtonEvents::M4Down);
        //mouseEventDispatch(mouseButtonEvents::M4Up);
    }
}
void releaseMouseButton(string& mb) {
    string m1 = "mouse1";
    string m2 = "mouse2";
    string m3 = "mouse3";
    string m4 = "mouse4";
    if (mb == m1) {
        //mouseEventDispatch(mouseButtonEvents::M1Down);
        mouseEventDispatch(mouseButtonEvents::M1Up);
    }
    else if (mb == m2) {
        //mouseEventDispatch(mouseButtonEvents::M2Down);
        mouseEventDispatch(mouseButtonEvents::M2Up);
    }
    else if (mb == m3) {
        //mouseEventDispatch(mouseButtonEvents::M3Down);
        mouseEventDispatch(mouseButtonEvents::M3Up);
    }
    else if (mb == m4) {
        //mouseEventDispatch(mouseButtonEvents::M4Down);
        mouseEventDispatch(mouseButtonEvents::M4Up);
    }
}
#pragma endregion

#pragma endregion

#pragma region universalFuncs
struct argsList {
    vector<size_t> usedArgs;
    vector<string> args;
    string codePath;
};
void findCodePath(argsList& inOut) {
    string luafile = "default";
    vector<size_t>& usedArgs = inOut.usedArgs;
    vector<string>& args = inOut.args;
    for (size_t i = 0; i < args.size(); ++i) {
        string& str = args[i];
        string fl = "--File";
        if (str == fl) {
            usedArgs.push_back(i);
            if (i + 1 < args.size()) {
                usedArgs.push_back(i + 1);
                luafile = string(args[i + 1].c_str());
                inOut.codePath = luafile;
            }
        }
    }
    if (luafile == "default") {
        if (args.size() != 0 and fileExists(args[0])) {
            luafile = string(args[0].c_str());
            usedArgs.push_back(0);
        }
    }
}
#pragma endregion

#pragma region lua

#pragma region luautil
string getLuaString(lua_State* LuaState, int id) {
    string str = "";
    if (lua_isstring(LuaState, id)) {
        str = string((char*)lua_tostring(LuaState, id));
    }
    return str;
}
bool checkLua(lua_State* LuaState, int exitCode) {
    if (exitCode != LUA_OK) {
        string err = lua_tostring(LuaState, -1);
        std::cout << "Lua program exited with error code: " << std::to_string(exitCode) << ", Error Message: " << err << "\n";
        return false;
    }
    return true;
}
string replaceTimeConsts(string& filelocation) {
    string stage1 = std::regex_replace(fileLocation, std::regex("{sec}"), start.sec);
    string stage2 = std::regex_replace(stage1, std::regex("{min}"), start.min);
    string stage3 = std::regex_replace(stage2, std::regex("{hour}"), start.hour);
    string stage4 = std::regex_replace(stage3, std::regex("{day}"), start.day);
    string stage5 = std::regex_replace(stage4, std::regex("{mon}"), start.month);
    string stage6 = std::regex_replace(stage5, std::regex("{year}"), start.year);
    return stage6;
}
#pragma endregion
//this region below defines all the lua functions that eventually call the cpp functions above
#pragma region LuaBindingFunctions
namespace lua {
    int lua_typeString(lua_State* L) {
        int numArgs = lua_gettop(L);
        if (numArgs < 3) {
            return 0;
        }
        string toType;
        int time1;
        int time2;
        if (lua_isstring(L, 1)) {
            toType = string((char*)lua_tostring(L, 1));
        }
        else {
            return 0;
        }
        if (lua_isnumber(L, 2)) {
            time1 = int(lua_tonumber(L, 2));
        }
        else {
            return 0;
        }
        if (lua_isnumber(L, 3)) {
            time2 = int(lua_tonumber(L, 3));
        }
        else {
            return 0;
        }
        typeString(toType, time1, time2);
        return 0;
    } //lua callable version of typeString
    int lua_sleep(lua_State* L) {
        if (lua_isnumber(L, 1)) {
            float sec = (float)lua_tonumber(L, 1);
            std::this_thread::sleep_for(std::chrono::nanoseconds((int64_t)floor((double)(sec) * 1000000000)));
        }
        return 0;
    }
    int lua_exit(lua_State* L) {
        int exitCode = 0;
        if (lua_isnumber(L, 1)) {
            exitCode = (int)lua_tonumber(L, 1);
        }
        exit(exitCode);
    }
    int lua_typeStringNatural(lua_State* L) {
        string str;
        double minButtonSpaceDelay; double maxButtonSpaceDelay; double minButtonHoldTime; double maxButtonHoldTime;
        if (lua_isstring(L, 1)) {
            str = string((char*)(lua_tostring(L, 1)));
        }
        else {
            return 0;
        }
        if (lua_isnumber(L, 2)) {
            minButtonSpaceDelay = (double)lua_tonumber(L, 2);
        }
        else {
            return 0;
        }
        if (lua_isnumber(L, 3)) {
            maxButtonSpaceDelay = (double)lua_tonumber(L, 3);
        }
        else {
            return 0;
        }
        if (lua_isnumber(L, 4)) {
            minButtonHoldTime = (double)lua_tonumber(L, 4);
        }
        else {
            return 0;
        }
        if (lua_isnumber(L, 5)) {
            maxButtonHoldTime = (double)lua_tonumber(L, 5);
        }
        else {
            return 0;
        }
        typeStringNatural(str, minButtonSpaceDelay, maxButtonSpaceDelay, minButtonHoldTime, maxButtonHoldTime);
        return 0;
    }
    int lua_pressKey(lua_State* L) {
        if (lua_isstring(L, 1)) {
            string str = string((char*)lua_tostring(L, 1));
            pressKey(str);
            return 0;
        }
        return 0;
    }
    int lua_releaseKey(lua_State* L) {
        if (lua_isstring(L, 1)) {
            string str = string((char*)lua_tostring(L, 1));
            releaseKey(str);
            return 0;
        }
        return 0;
    }
    int lua_readFile(lua_State* L) {
        if (lua_isstring(L, 1)) {
            string str = string((char*)(lua_tostring(L, 1)));
            if (fileExists(str)) {
                string content = readFileSTR(str);
                lua_pushstring(L, content.c_str());
                return 1;
            }
            lua_pushstring(L, "");
            return 1;
        }
        lua_pushstring(L, "");
        return 1;
    }
    int lua_exec(lua_State *L) {
        if (lua_isstring(L, 1)) {
            string val = getLuaString(L, 1);
            int ret = luaL_dostring(L, val.c_str());
            checkLua(L, ret);
            lua_pushnumber(L, ret);
            return 1;
        }
        return 0;
    }
    int lua_pause(lua_State* L) {
        system("pause");
        return 0;
    }
    int lua_print(lua_State* L) {
        string str = getLuaString(L, 1);
        std::cout << str;
        if (logIntoFile) {
            string str2 = fileLocation;
            std::ofstream outFile;
            outFile.open(finalOutputFilename);
            outFile << str;
            outFile.close();
        }
        return 0;
    }
    int lua_changeLogLocation(lua_State* L) {
        string str = getLuaString(L, 1);
        if (str != string("")) {
            fileLocation = str;
            finalOutputFilename = replaceTimeConsts(str);
            return 0;
        }
        return 0;
    }
    int lua_setLog(lua_State* L) {
        if (lua_isboolean(L, 1)) {
            bool logNow = (bool)lua_toboolean(L, 1);
            logIntoFile = logNow;
        }
        return 0;
    }

    //TODO: Make js bind equavilants
    int lua_moveMouseRelative(lua_State* L) {
        if (!lua_isnumber(L, 1) or !lua_isnumber(L, 2)) return 0;
        moveMouseRelative((float)lua_tonumber(L, 1), (float)lua_tonumber(L, 2));
        return 0;
    }
    int lua_moveMouseAbsolute(lua_State* L) {
        if (!lua_isnumber(L, 1) or !lua_isnumber(L, 2)) return 0;
        moveMouseAbsolute((float)lua_tonumber(L, 1), (float)lua_tonumber(L, 2));
        return 0;
    }
    int lua_moveMouseTime(lua_State* L) {
        if (!lua_isnumber(L, 1) or !lua_isnumber(L, 2) or !lua_isnumber(L, 3) or lua_isnumber(L, 4)) return 0;
        moveMouseTime(lua_tonumber(L, 1), lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4));
        return 0;
    }
    int lua_actionMouseButton(lua_State* L) {
        if (!lua_isstring(L, 1)) return 0;
        if (lua_isnumber(L, 2)) {
            string str = getLuaString(L, 1);
            actionMouseButton(str, lua_tonumber(L, 2));
        }
        else {
            string str = getLuaString(L, 1);
            actionMouseButton(str);
            return 0;
        }
    }
    int lua_pressMouseButton(lua_State* L) {
        if (!lua_isstring(L, 1)) return 0;
        string str = getLuaString(L, 1);
        pressMouseButton(str);
        return 0;
    }
    int lua_releaseMouseButton(lua_State* L) {
        if (!lua_isstring(L, 1)) return 0;
        string str = getLuaString(L, 1);
        releaseMouseButton(str);
        return 0;
    }
}
#pragma endregion
//the region below is everything needed to setup our custom lua enviornment with all of the unique functions that make the program useful
#pragma region LuaEnv
namespace luaEnv {
    void setupLuaEnviornment(lua_State* L) {
        luaL_openlibs(L);
        lua_register(L, "typeString", lua::lua_typeString);
        lua_register(L, "sleep", lua::lua_sleep);
        lua_register(L, "exit", lua::lua_exit);
        lua_register(L, "typeStringNatural", lua::lua_typeStringNatural);
        lua_register(L, "pressKey", lua::lua_pressKey);
        lua_register(L, "releaseKey", lua::lua_releaseKey);
        lua_register(L, "readFile", lua::lua_readFile);
        lua_register(L, "exec", lua::lua_exec);
        lua_register(L, "pause", lua::lua_pause);
        lua_register(L, "print", lua::lua_print);
        lua_register(L, "changeLocLocation", lua::lua_changeLogLocation);
        lua_register(L, "setLogging", lua::lua_setLog);
        lua_register(L, "moveMouseRel", lua::lua_moveMouseRelative);
        lua_register(L, "moveMouseAbs", lua::lua_moveMouseAbsolute);
        lua_register(L, "moveMouseTimeRel", lua::lua_moveMouseTime);
        lua_register(L, "actionMouseButton", lua::lua_actionMouseButton);
        lua_register(L, "pressMouseButton", lua::lua_actionMouseButton);
        lua_register(L, "releaseMouseButton", lua::lua_releaseMouseButton);
    }
}
#pragma endregion

//this region defines the codepath for everything, and executes out lua file in the end
#pragma region luarun
void luaexec(lua_State* L, argsList& aList) {
    string cmd;
    if (aList.codePath != "default") {
        if (!fileExists(aList.codePath)) {
            std::cout << "Error: Lua file not found: " << aList.codePath << ", please check your command and try again.\n";
            lua_close(L);
            exit(0);
        }
        cmd = readFileSTR(aList.codePath);
        cmd = cmd + "\nexit(0)";
        int r = luaL_dostring(L, cmd.c_str());
        checkLua(L, r);
    }
    else {
        std::cout << "Error: no file loaded to be used, please add it to the command line arguments following \"--File <filename>\" or just use \"<filename>\" as your only argument\n";
    }
}
int luarun(vector<string>& args) {
    argsList aList = { {}, args, "default" };
    findCodePath(aList);
    lua_State* L = luaL_newstate();
    luaEnv::setupLuaEnviornment(L);
    luaexec(L, aList);
    lua_close(L);
    return 0;
}
#pragma endregion

#pragma endregion

#pragma region launchArgs
vector<string> split(string s, string delimiter) {
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    string token;
    vector<string> res;

    while ((pos_end = s.find(delimiter, pos_start)) != string::npos) {
        token = s.substr(pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back(token);
    }

    res.push_back(s.substr(pos_start));
    return res;
}
void setStartTime() {
    time_t t = time(NULL);
    tm time = {0};
    tm* timePtr = &time;
    localtime_s(timePtr, &t);

    string sec = to_string(timePtr->tm_sec);
    string min = to_string(timePtr->tm_min);
    string hour = to_string(timePtr->tm_hour);
    string day = to_string(timePtr->tm_mday);
    string month = to_string(timePtr->tm_mon);
    string year = to_string(timePtr->tm_year);
    start = { sec, min, hour, day, month, year };
}
vector<string> interpretArgs(int argc, char** argv) {
    vector<string> args;
    if (argc >= 1) {
        for (size_t i = 1; i < argc; ++i) {
            char* val = argv[i];
            string str(val);
            args.push_back(str);
        }
    }
    return args;
}

#pragma endregion

#pragma region js
duk_context* ctx;
#pragma region jsCBinds
static duk_ret_t native_print(duk_context* ctx) {
    printf("%s\n", duk_to_string(ctx, 0));
    return 0;  /* no return value (= undefined) */
}
static duk_ret_t native_sleep(duk_context* ctx) {
    if (!duk_is_number(ctx, 0)) return 0;
    std::this_thread::sleep_for(std::chrono::nanoseconds((int64_t)floor((double)(duk_get_number(ctx, 0)) * 1000000000)));
    return 0;
}
static duk_ret_t js_typeString(duk_context* ctx) {
    if (!duk_is_string(ctx, 0)) return 0;
    if (!duk_is_number(ctx, 1)) return 0;
    if (!duk_is_number(ctx, 2)) return 0;
    string str = duk_get_string(ctx, 0);
    int holdTime = duk_get_number(ctx, 1);
    int spaceTime = duk_get_number(ctx, 2);
    typeString(str, holdTime, spaceTime);
    return 0;
}
static duk_ret_t js_typeStringNatural(duk_context* ctx) {
    if (!duk_is_string(ctx, 0) or !duk_is_number(ctx, 1) or !duk_is_number(ctx, 2) or !duk_is_number(ctx, 3) or !duk_is_number(ctx, 4)) return 0; //make sure we have a string and four numbers
    int nums[4] = { round(duk_get_number(ctx, 1)), round(duk_get_number(ctx, 2)), round(duk_get_number(ctx, 3)), round(duk_get_number(ctx, 4))};
    string str = duk_get_string(ctx, 0);
    int minButtonSpace = min(nums[0], nums[1]);
    int maxButtonSpace = max(nums[0], nums[1]);
    int minButtonHold = min(nums[2], nums[3]);
    int maxButtonHold = max(nums[2], nums[3]);
    typeStringNatural(str, minButtonSpace, maxButtonSpace, minButtonHold, maxButtonHold);
    return 0;
}
static duk_ret_t js_pressKey(duk_context* ctx) {
    if (!duk_is_string(ctx, 0)) return 0;
    string cha = duk_get_string(ctx, 0);
    pressKey(cha);
}
static duk_ret_t js_releaseKey(duk_context* ctx) {
    if (!duk_is_string(ctx, 0)) return 0;
    string cha = duk_get_string(ctx, 0);
    releaseKey(cha);
}
static duk_ret_t js_random(duk_context* ctx) {
    //this takes an optional two argumants, but it either takes 0 or 2, not 1
    if (duk_is_number(ctx, 0) and duk_is_number(ctx, 1)) {

        duk_push_number(ctx, random(duk_get_number(ctx, 0), duk_get_number(ctx, 1)));
        return 1;
    }
    else if (duk_is_number(ctx, 0)) {
        duk_push_number(ctx, random(0, duk_get_number(ctx, 0)));
        return 1;
    }
    else {
        duk_push_number(ctx, random(0, 1));
        return 1;
    }
}
static duk_ret_t js_readFile(duk_context* ctx) {
    if (!duk_is_string(ctx, 0)) return 0;
    string fp = duk_get_string(ctx, 0);
    if (!fileExists(fp)) return 0;
    duk_push_string(ctx, readFileSTR(fp).c_str());
    return 1;
}

static duk_ret_t js_moveMouseRelative(duk_context* ctx) {
    if (!duk_is_number(ctx, 0) or !duk_is_number(ctx, 1)) return 0;
    moveMouseRelative((float)duk_to_number(ctx, 0), (float)duk_to_number(ctx, 1));
    return 0;
}
static duk_ret_t js_moveMouseAbsolute(duk_context* ctx) {
    if (!duk_is_number(ctx, 0) or !duk_is_number(ctx, 1)) return 0;
    moveMouseAbsolute(duk_to_number(ctx, 0), duk_to_number(ctx, 1));
    return 0;
}
static duk_ret_t js_moveMouseTime(duk_context* ctx) {
    if (!duk_is_number(ctx, 0) or !duk_is_number(ctx, 1) or !duk_is_number(ctx, 2) or !duk_is_number(ctx, 3)) return 0;
    moveMouseTime(duk_to_number(ctx, 0), duk_to_number(ctx, 1), duk_to_number(ctx, 2), duk_to_number(ctx, 3));
    return 0;
}
static duk_ret_t js_actionMouseButton(duk_context* ctx) {
    if (!duk_is_string(ctx, 0) or duk_is_number(ctx, 0)) return 0;
    double num = duk_to_number(ctx, 1);
    string str = duk_to_string(ctx, 0);
    actionMouseButton(str, round(num));
    return 0;
}
static duk_ret_t js_pressMouseButton(duk_context* ctx) {
    if (!duk_is_string(ctx, 0)) return 0;
    string str = duk_to_string(ctx, 0);
    pressMouseButton(str);
    return 0;
}
static duk_ret_t js_releaseMouseButton(duk_context* ctx) {
    if (!duk_is_string(ctx, 0)) return 0;
    string str = duk_to_string(ctx, 0);
    releaseMouseButton(str);
    return 0;
}
#pragma endregion
inline void bindJSFunc(duk_context* ctx, duk_c_function func, duk_idx_t numArgs, std::string& name) {
    duk_push_c_function(ctx, func, numArgs);
    duk_put_global_string(ctx, name.c_str());
}
inline void bindJSFunc(duk_context* ctx, duk_c_function func, duk_idx_t numArgs, std::string name) {
    duk_push_c_function(ctx, func, numArgs);
    duk_put_global_string(ctx, name.c_str());
}

void createCallbackFuncMemory(duk_context* ctx) {
    int stackLoc = duk_push_array(ctx);
    duk_push_string(ctx, "__eventFunctions"); //we are allocating memory specifically for the purpose of making sure callback functions do not get culled by the garbage collector
}
void setCallbackArrayValue(duk_context* ctx, int index, void* address) {
    duk_get_global_string(ctx, "__eventFunctions");
    size_t len = duk_get_length(ctx, -1);
    if (index <= len) {
        duk_push_pointer(ctx, address);
        duk_put_prop_index(ctx, -2, index);
    }
    duk_pop(ctx);
}
void jssetup(duk_context* ctx) {
    createCallbackFuncMemory(ctx);
    bindJSFunc(ctx, native_print, 1, "print");
    bindJSFunc(ctx, native_sleep, 1, "sleep");
    bindJSFunc(ctx, js_typeString, 3, "typeString");
    bindJSFunc(ctx, js_typeStringNatural, 5, "typeStringNatural");
    bindJSFunc(ctx, js_pressKey, 1, "pressKey");
    bindJSFunc(ctx, js_releaseKey, 1, "releaseKey");
    bindJSFunc(ctx, js_random, DUK_VARARGS, "random");
    bindJSFunc(ctx, js_readFile, 1, "readFile");
    bindJSFunc(ctx, js_moveMouseRelative, 2, "moveMouseRel");
    bindJSFunc(ctx, js_moveMouseAbsolute, 2, "moveMouseAbs");
    bindJSFunc(ctx, js_moveMouseTime, 4, "moveMouseTimeRel");
    bindJSFunc(ctx, js_actionMouseButton, 2, "actionMouseButton");
    bindJSFunc(ctx, js_pressMouseButton, 1, "pressMouseButton");
    bindJSFunc(ctx, js_releaseMouseButton, 1, "releaseMouseButton");
}

int jsrun(vector<string>& args) {
    argsList arg = { {}, args, "default" };
    findCodePath(arg);
    if (arg.codePath == string("defailt") or not fileExists(arg.codePath)) {
        std::cout << "Error: no file loaded to be used, please add it to the command line arguments following \"--File < filename>\" or just use \" < filename > \" as your only argument" << "\n";
        return 0;
    }
    ctx = duk_create_heap_default();
    jssetup(ctx);
    duk_eval_string(ctx, readFileSTR(arg.codePath).c_str());
    duk_destroy_heap(ctx);
    return 0;
}
#pragma endregion

#pragma region docs
/*

<Insert program name here> Documentation:
[NOTICE]: All functions will require all parameters unless otherwise specified, these functions will return/and or do nothing if nothing can be done
typeString(string str, number ButtonHoldTime, number ButtonSpaceTime): will press keys individually with the specified hard values to delay by, will probably get you flagged by anti-cheat
typeStringNatural(string str, number minButtonSpaceDelay, number maxButtonSpaceDelay, number minButtonHoldTime, number maxButtonHoldTime): will attempt to type a string with a random delays and button hold times, this is the most likely to bypass anti-cheat, may still cause a flag idk until someone else tests, will update
pressKey(string cha): will simulate a key being pressed and not released
releaseKey(string cha): will simulate a key up and nothing else
print(any value): will print its one argument to the standard output of the application
sleep(number seconds): will delay by the specified number of seconds accurate down to about five nanoseconds
readFile(string filepath): will attempt to read the file at filepath, if nothing exists there will return nothing, otherwise will return whole file
mouseMoveRel(number x, number y): will move the mouse relative to its current position instantly, due to limitations, only accurate to 32 bits
mouseMoveAbs(number x, number y): will move the mouse to a absolute position on your monitor, to find coords I would recommend using printscreen and paint, due to limitations only accurate to 32 bits
mouseMoveTimeRel(number x, number y, number timeSeconds, number iterationsPerSecond): will move the mouse smoothly from where it is to the final position, may be detected by anticheat and will look EXTREMELY wrong to onlookers, iterationsPerSecond is rounded for obvious reasons
pressMouseButton(string str): presses the specified mouse button, use mouse1, mouse2, mouse3, or mouse4 to specify
releaseMouseButton(string str): releases the specified mouse button, use mouse1, mouse2, mouse3, or mouse4 to specify

Javascript specific funcitons:
random(optional number lower, optional number higher): generates a random number between lower and higher, if only lower is specified the lower bounds is assumed to be 0 and lower is the higher bounds, if no arguments will return some number between 0 and 1, if more than two arguments the first two are the only ones considered, uses the c++ standard library mt19937 randomizer using time, hardware(if availible), and access count as a seed

Lua Specific Functions:
exit(number code): exit the lua interpreter with code TODO: create a javascript binding
lua has all of the standard lua libraries like math and others included by default, enjoy.
NOTICE: if you can't code, this is not the program for you to be using, you can learn all oof the interpreted languages from codecademy.com for free if you wish, that's where the main dev of this program got his start!


to switch from the default lua to javascript, please use the launch argument --js

GLOBAL TODO HIGH PRIORITY: create a event based hotkey system that works system-wide

*/
#pragma endregion

#pragma region entrypoint

HINSTANCE ins;

inline int run(vector<string>& args) {
    bool lua = true;
    for (auto& i : args) {
        if (i == string("--js")) {
            lua = false;
            break;
        }
    }
    if (lua) return luarun(args);
    return jsrun(args);
}

int main(int argc, char** argv)
{
    setStartTime();
    auto args = interpretArgs(argc, argv);
    return run(args);
}
int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    setStartTime();
    logIntoFile = true;
    const char* c = reinterpret_cast<const char*>(lpCmdLine);
    std::string argStr = string(c);
    vector<string> args = split(argStr, " ");
    int ret = run(args);
    MessageBox(NULL, L"code has exited!", L"WARNING", MB_ICONEXCLAMATION | MB_OK);
    return 0;
}

#pragma endregion
//
// Created by DasDarki on 25.06.2020.
//

#include <iostream>
#include <cstring>

#include <filesystem>
namespace fs = std::filesystem;

#ifdef _WIN32
#include <windows.h>
#endif

#include "Plugin.hpp"

#ifdef _WIN32
# define EXPORTED extern "C" __declspec(dllexport)
#else
# define EXPORTED
#endif

#ifdef LUA_DEFINE
# undef LUA_DEFINE
#endif
#define LUA_DEFINE(name) Define(#name, [](lua_State *L) -> int

Lua::LuaArgs_t Plugin::CallLuaFunction(const char* LuaFunctionName, Lua::LuaArgs_t* Arguments) {
    Lua::LuaArgs_t ReturnValues;
    int ArgCount = lua_gettop(Plugin::MainScriptVM);
    lua_getglobal(Plugin::MainScriptVM, LuaFunctionName);
    int argc = 0;
    if (Arguments) {
        for (auto const& e : *Arguments) {
            Lua::PushValueToLua(e, Plugin::MainScriptVM);
            argc++;
        }
    }
    int Status = lua_pcall(Plugin::MainScriptVM, argc, LUA_MULTRET, 0);
    ArgCount = lua_gettop(Plugin::MainScriptVM) - ArgCount;
    if (Status == LUA_OK) {
        Lua::ParseArguments(Plugin::MainScriptVM, ReturnValues);
        lua_pop(Plugin::MainScriptVM, ArgCount);
    }
    return ReturnValues;
}

Plugin::Plugin()
{
}

//region Native Bridge Functions

EXPORTED void SetPlayerName(long player, const char* name)
{
    Lua::LuaArgs_t argValues = Lua::BuildArgumentList(player, name);
    Plugin::Get()->CallLuaFunction("SetPlayerName", &argValues);
}

EXPORTED const char* GetPlayerName(long player)
{
    Lua::LuaArgs_t argValues = Lua::BuildArgumentList(player);
    Lua::LuaArgs_t returnValues = Plugin::Get()->CallLuaFunction("GetPlayerName", &argValues);
    auto name = returnValues.at(0).GetValue<std::string>();
    const char* namePtr = name.c_str();
    return namePtr;
}

EXPORTED void SendPlayerChatMessage(long player, const char* message)
{
    Lua::LuaArgs_t argValues = Lua::BuildArgumentList(player, message);
    Plugin::Get()->CallLuaFunction("AddPlayerChat", &argValues);
}

EXPORTED int* GetEntities(const char* entityName, int* len)
{
    std::string sFuncName = "GetAll" + std::string(entityName);
    Lua::LuaArgs_t argValues = Lua::BuildArgumentList();
    Lua::LuaArgs_t returnValues = Plugin::Get()->CallLuaFunction(sFuncName.c_str(), &argValues);
    auto entityTable = returnValues.at(0).GetValue<Lua::LuaTable_t>();
    *len = entityTable->Count();
    int* entities = new int[entityTable->Count()];
    int idx = 0;
    entityTable->ForEach([entities, &idx](Lua::LuaValue k, Lua::LuaValue v) {
        entities[idx] = (int)v.GetValue<float>();
        idx++;
    });
    return entities;
}

EXPORTED int ReleaseLongArray(long* lArray)
{
    delete[] lArray;
    return 0;
}

EXPORTED void ForceRuntimeRestart(bool complete)
{
    (void*) complete;
    Plugin::Get()->GetBridge().Restart();
}

EXPORTED bool IsEntityValid(long id, const char* entityName)
{
    std::string sFuncName = "IsValid" + std::string(entityName);
    Lua::LuaArgs_t argValues = Lua::BuildArgumentList(id);
    Lua::LuaArgs_t returnValues = Plugin::Get()->CallLuaFunction(sFuncName.c_str(), &argValues);
    return returnValues.at(0).GetValue<bool>();
}

//endregion
#include "PluginManager.hpp"
#include<iostream>
#include <chrono>

//#include"NameMangling/NameMangler.hpp"

namespace fs = std::filesystem;


Plugin::Plugin(std::string_view path) {
    this->m_handle = LoadLibraryA(path.data());
    
    if (this->m_handle == nullptr) {
        std::cout << ("loading a plugin failed") << std::endl;
        return;
    }
    std::string initializePlugin("initializePlugin");
    std::string getPluginName("getPluginName");
    std::string GetPluginAuthor("getPluginAuthor");
    std::string GetPluginDescription("getPluginDescription");
    this->m_initializePluginFunction = getPluginFunction<InitializePluginFunc>(initializePlugin);
    this->m_getPluginNameFunction = getPluginFunction<GetPluginNameFunc>(getPluginName);
    this->m_getPluginAuthorFunction = getPluginFunction<GetPluginAuthorFunc>(GetPluginAuthor);
    this->m_getPluginDescriptionFunction = getPluginFunction<GetPluginDescriptionFunc>(GetPluginDescription);
}

Plugin::Plugin(Plugin&& other) noexcept {
    this->m_handle = other.m_handle;
    this->m_initializePluginFunction = other.m_initializePluginFunction;
    this->m_getPluginNameFunction = other.m_getPluginNameFunction;
    this->m_getPluginAuthorFunction = other.m_getPluginAuthorFunction;
    this->m_getPluginDescriptionFunction = other.m_getPluginDescriptionFunction;

    other.m_handle = nullptr;
    other.m_initializePluginFunction = nullptr;
    other.m_getPluginNameFunction = nullptr;
    other.m_getPluginAuthorFunction = nullptr;
    other.m_getPluginDescriptionFunction = nullptr;
}

Plugin::~Plugin() {
    if (this->m_handle != nullptr)
        FreeLibrary(this->m_handle);
}

void Plugin::initializePlugin(Window& window) const {
    if (this->m_initializePluginFunction != nullptr) {
        this->m_initializePluginFunction((void *)&window);
    }
    else {
        std::cout << ":( couldnt initialize" << std::endl;
    }
}

std::string Plugin::getPluginName() const {
    if (this->m_getPluginNameFunction != nullptr)
        return this->m_getPluginNameFunction();
    else
        return ("Unknown Plugin");
}

std::string Plugin::getPluginAuthor() const {
    if (this->m_getPluginAuthorFunction != nullptr)
        return this->m_getPluginAuthorFunction();
    else
        return "Unknown";
}

std::string Plugin::getPluginDescription() const {
    if (this->m_getPluginDescriptionFunction != nullptr)
        return this->m_getPluginDescriptionFunction();
    else
        return "Unknown Description";
}


bool PluginManager::load(std::string_view pluginFolder) {
    if (!std::filesystem::exists(pluginFolder))
    {
        throw std::runtime_error("Failed to find plugin folder");
        return false;
    }

    PluginManager::s_pluginFolder = pluginFolder;

    for (auto& pluginPath : std::filesystem::directory_iterator(pluginFolder)) {
        if (pluginPath.is_regular_file() && pluginPath.path().extension() == ".tetris")
            PluginManager::s_plugins.emplace_back(pluginPath.path().string());
    }

    if (PluginManager::s_plugins.empty())
    {
        throw std::runtime_error("Plugin Folder Empty");
        return false;
    }

    return true;
}

void PluginManager::unload() {
    PluginManager::s_plugins.clear();
    PluginManager::s_pluginFolder.clear();
}

void PluginManager::reload() {
    PluginManager::unload();
    PluginManager::load(PluginManager::s_pluginFolder);
}

std::vector<std::string> getPath() {
#if defined(WIN32)
    std::string exePath(MAX_PATH, '\0');
    GetModuleFileNameA(nullptr, exePath.data(), exePath.length());
    auto parentDir = std::filesystem::path(exePath).parent_path();

    std::filesystem::path appDataDir;
    {
        LPWSTR wAppDataPath = nullptr;
        if (!SUCCEEDED(SHGetKnownFolderPath(FOLDERID_LocalAppData, KF_FLAG_CREATE, nullptr, &wAppDataPath)))
            throw std::runtime_error("Failed to get APPDATA folder path");

        appDataDir = wAppDataPath;
        CoTaskMemFree(wAppDataPath);
    }
        return { (parentDir / "plugins").string()};
#elif defined(OS_MACOS)
    return { getPathForMac(path) };
#else
    std::vector<std::filesystem::path> configDirs = xdg::ConfigDirs();
    std::vector<std::filesystem::path> dataDirs = xdg::DataDirs();

    configDirs.insert(configDirs.begin(), xdg::ConfigHomeDir());
    dataDirs.insert(dataDirs.begin(), xdg::DataHomeDir());

    std::vector<std::string> result;

        std::transform(dataDirs.begin(), dataDirs.end(), std::back_inserter(result),
            [](auto p) { return (p / "bin/plugins").string(); });
        return result;
    }
#endif
}

bool PluginManager::loadPlugins(Window &window) {
    printf("loading Plugins");
    for (const auto& dir : getPath()) {
        PluginManager::load(dir);
    }

    if (PluginManager::getPlugins().empty()) {
        return false;
    }

    for (const auto& plugin : PluginManager::getPlugins()) {
        plugin.initializePlugin(window);
        Shakkar::Plugins::getEntries().back()->m_unlocalizedTetrisAuth = plugin.getPluginAuthor();
        Shakkar::Plugins::getEntries().back()->m_unlocalizedTetrisDesc = plugin.getPluginDescription();
        Shakkar::Plugins::getEntries().back()->m_unlocalizedTetrisName = plugin.getPluginName();

    }

    return true;
}

bool unloadPlugins() {
    PluginManager::unload();

    return true;
}
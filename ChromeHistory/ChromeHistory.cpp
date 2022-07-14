#include <iostream>
#include <windows.h>
#include <vector>
#include <filesystem>
#include <shlobj.h> 
#include <shlwapi.h>
#include "CppSQLite3.h"
#pragma comment(lib,"Shlwapi.lib")
#pragma comment(lib,"sqlite3.lib")

int main() {
    std::vector<std::string> Profiles;
    char Path[MAX_PATH];
    SHGetSpecialFolderPathA(NULL, Path, CSIDL_LOCAL_APPDATA, FALSE);
    strcat(Path, "\\Google\\Chrome\\User Data\\");
    for (auto& Folder : std::filesystem::directory_iterator(Path)) {
        std::string FolderPath = std::filesystem::absolute(Folder.path()).string();
        if ((FolderPath.find("Profile") != std::string::npos) || (FolderPath.find("Default") != std::string::npos)) {
            std::string HistoryFile = FolderPath + "\\History";
            if ((PathFileExists(HistoryFile.c_str())) && (HistoryFile.find("System") == std::string::npos) && (HistoryFile.find("Guest") == std::string::npos)) {
                Profiles.push_back(HistoryFile);               
            }
        }       
    }

    std::vector<std::vector<std::string>> ProfileUrls;
    for (int i = 0; i < Profiles.size(); i++) {
        std::vector<std::string> Urls;
        CppSQLite3DB Database;
        Database.open(Profiles[i].c_str());
        CppSQLite3Query Query = Database.execQuery("SELECT url FROM urls ORDER BY id ASC");
        while (!Query.eof()) {
            Urls.push_back(Query.fieldValue("url"));
            Query.nextRow();
        }
        ProfileUrls.push_back(Urls);
    }

    std::vector<std::vector<std::string>> ProfileHistory;
    for (int i = 0; i < Profiles.size(); i++) {
        std::vector<std::string> History;
        CppSQLite3DB Database;
        Database.open(Profiles[i].c_str());
        CppSQLite3Query Query = Database.execQuery("SELECT url FROM visits ORDER BY id ASC");
        while (!Query.eof()) {
            History.push_back(ProfileUrls[0][atoi(Query.fieldValue("url")) - 1]);
            Query.nextRow();
        }
        ProfileHistory.push_back(History);
    }

    //List all history
    for (int i = 0; i < Profiles.size(); i++) {
        if (ProfileHistory[i].size() > 0) {
            std::vector<std::string> History = ProfileHistory[i];
            for (int j = 0; j < History.size(); j++) {
                std::cout << "[ProfileID: " << (i + 1) << " | HistoryID: " << (j + 1) << "] " << History[j] << std::endl;
            }
        }
    }
    return 0;
}
// This is a first version (test)
// Now it just records which app you're using and
// writes it into log.txt with app name and duration m:s

#include <iostream>
#include <chrono>
#include <thread>
#include <string>
#include <windows.h>
#include <psapi.h>
#include <fstream>
#include <filesystem>
#include <map>
#include <utility>
#include <csignal>

using namespace std;

// I need to add elements to map (minutes, seconds) and values for those el.
map<string, long long> processData;

string GetActiveProcessName()
{
    HWND hwnd = GetForegroundWindow(); // active window
    if (!hwnd)
        return "";

    DWORD pid;
    GetWindowThreadProcessId(hwnd, &pid);

    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
    if (!hProcess)
        return "";

    char processName[MAX_PATH];
    if (GetModuleFileNameExA(hProcess, NULL, processName, MAX_PATH))
    {
        CloseHandle(hProcess);
        return string(processName);
    }

    CloseHandle(hProcess);
    return "";
}

void loadFile()
{
    ifstream logFile("log.txt");
    if (!logFile.is_open())
    {
        cerr << "Error opening log.txt file!\n";
        return;
    }

    string line;

    while(getline(logFile, line))
    {
        stringstream ss(line);
        string pName, pDuration;

        getline(ss, pName, '(');
        getline(ss, pDuration, ')');

        // Remove any trailing spaces from pName
        if (!pName.empty())
            pName.erase(pName.find_last_not_of(" \n\r\t") + 1);

        // Parse duration in m:s format
        int minutes = 0, seconds = 0;
        size_t colonPos = pDuration.find(':');

        if (colonPos != string::npos)
        {
            try {
                minutes = stoi(pDuration.substr(0, colonPos));
                seconds = stoi(pDuration.substr(colonPos + 1));
            } catch (...) {
                minutes = 0;
                seconds = 0;
            }
        }

        long long totalSeconds = minutes * 60 + seconds;
        processData[pName] = totalSeconds;
    }

    logFile.close();
}

void saveFile()
{
    ofstream logFile("log.txt");
    if (!logFile.is_open())
    {
        cerr << "Error opening log.txt file!\n";
        return;
    }

    for (auto &p : processData)
    {
        long long totalSeconds = p.second;
        int minutes = totalSeconds / 60;
        int seconds = totalSeconds % 60;

        logFile << p.first << " (" << minutes << ":" << seconds << ")\n";
        logFile.flush();
    }

    logFile.close();
}

void signalHandler(int signum)
{
    saveFile();
    exit(signum);
}

int main()
{
    signal(SIGINT, signalHandler);
    loadFile();

    cout << "----------------------------------\n";
    for (auto &p : processData)
    {
        cout << p.first << " - " << p.second << "\n";
    }
    cout << "----------------------------------\n";

    string lastProcess;
    auto startTime = chrono::steady_clock::now();
    
    while (true)
    {
        string current = GetActiveProcessName();
        if (current != "" && current != lastProcess)
        {
            if (lastProcess != "")
            {
                auto endTime = chrono::steady_clock::now();
                long long duration = chrono::duration_cast<chrono::seconds>(endTime - startTime).count();

                string fileName = filesystem::path(lastProcess).filename().string();
                processData[fileName] += duration;
            }

            startTime = chrono::steady_clock::now();

            cout << "You're using: " << current << "\n";
            lastProcess = current;
        }

        static auto lastSave = chrono::steady_clock::now();
        auto now = chrono::steady_clock::now();
        if (chrono::duration_cast<chrono::seconds>(now - lastSave).count() >= 20)
        {
            saveFile();
            lastSave = now;
        }

        Sleep(1000);
    }
    return 0;
}
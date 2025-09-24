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

int main()
{
    ofstream logFile("log.txt", ios::app);
    if (!logFile.is_open())
    {
        cerr << "Error opening log.txt file!\n";
        return 1;
    }

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
                auto duration = chrono::duration_cast<chrono::seconds>(endTime - startTime).count();
            
                int minutes = duration / 60;
                int seconds = duration % 60;

                string fileName = filesystem::path(lastProcess).filename().string();

                logFile << fileName << " (" << minutes << ":" << seconds << ")\n";
                logFile.flush();
                
                cout << "Closed " << fileName << " - " << minutes << ":" << seconds << "\n";
            }

            startTime = chrono::steady_clock::now();

            cout << "You're using: " << current << "\n";
            lastProcess = current;
        }
        Sleep(1000);
    }

    logFile.close();
    return 0;
}
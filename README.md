# ⏱️ Time Tracker (C++)

Time Tracker is a C++ console application that measures how much time you spend in different applications on Windows.  
All sessions are logged into **log.txt**, and total usage time for each app is accumulated.

---

## 🚀 Features
- Tracks the currently active window in real-time  
- Stores **total usage time** for each application  
- Saves data periodically and also on exit (Ctrl + C)  
- Skips system/ignored processes (e.g. `explorer.exe`, `svchost.exe`, `dwm.exe`)  
- Logs data into `log.txt` in the format:



---

## 🛠️ Tech Details
- Language: **C++17**  
- Platform: **Windows**  
- APIs used:  
- `WinAPI` (`GetForegroundWindow`, `GetWindowThreadProcessId`, `OpenProcess`)  
- `psapi.h` (`GetModuleFileNameExA`)  
- `chrono` (for time measurement)  
- `map` (to store app usage times)  

---

## 📦 How to Build
1. Clone this repository:
 ```bash
 git clone https://github.com/cyrelonx/time-tracker-cpp.git
```

2. Open the project in Visual Studio Code or build with g++:
 ```bash
 g++ -std=c++17 -o timetracker main.cpp -lpsapi
```

3.Run the executable
 ```bash
\.timetracker.exe
```

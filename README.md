
# 🛠️ CrashHandler for Windows (C++)

A lightweight C++ class for capturing and logging unhandled exceptions on Windows systems, complete with detailed stack traces and exception metadata. Ideal for debugging and post-mortem crash analysis in native Windows applications.

## 📦 Features

- Automatically captures unhandled exceptions via `SetUnhandledExceptionFilter`.
- Logs:
  - Timestamp of the crash
  - Exception code and human-readable description
  - Full stack trace with function names and source code line info (if available)
- Generates a detailed crash log (`crash_YYYY-MM-DD_HH-MM-SS.log`)
- Uses Windows APIs: `DbgHelp`, `SymFromAddr`, `StackWalk64`, etc.

## 💻 Dependencies

- Windows OS
- `DbgHelp.lib` (linked via pragma)
- Requires `/Zi` compiler flag and `.pdb` files for symbol resolution

## 🧩 How to Use

1. Include the `CrashHandler.h` file in your project.
2. Initialize the crash handler early in your application:

```cpp
int main() {
    CrashHandler::Initialize();
    // Your application code
}
```

3. Upon a crash (e.g., access violation, divide by zero), a crash log will be created in the working directory.

## 📝 Example Log Output

```
=== Crash Report ===
Timestamp: 2025-05-08_15-42-31
Exception Code: 0xc0000005
Exception Description: Access Violation

Stack Trace:
[0] MyFunction (0x00007FF612341000)
    File: main.cpp
    Line: 42
...
```

## 📄 License

MIT License – free to use, modify, and distribute.

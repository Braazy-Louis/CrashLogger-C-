#pragma once
#include <Windows.h>
#include <DbgHelp.h>
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <sstream>
#pragma warning(disable : 4996)
#pragma comment(lib, "DbgHelp.lib")

class CrashHandler {
private:
    static LONG WINAPI TopLevelExceptionHandler(EXCEPTION_POINTERS* pExceptionInfo) {
   
        DWORD exceptionCode = pExceptionInfo->ExceptionRecord->ExceptionCode;        
      
        auto now = std::time(nullptr);
        auto tm = *std::localtime(&now);
        std::ostringstream oss;
        oss << std::put_time(&tm, "%Y-%m-%d_%H-%M-%S");
        std::string timestamp = oss.str();
        
        std::string crashLogName = "crash_" + timestamp + ".log";
        std::ofstream crashLog(crashLogName);
      
        crashLog << "=== Crash Report ===\n";
        crashLog << "Timestamp: " << timestamp << "\n";
        crashLog << "Exception Code: 0x" << std::hex << exceptionCode << std::dec << "\n";
        
        // Get description
        std::string exceptionDesc;
        switch (exceptionCode) {
            case EXCEPTION_ACCESS_VIOLATION:
                exceptionDesc = "Access Violation";
                break;
            case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
                exceptionDesc = "Array Bounds Exceeded";
                break;
            case EXCEPTION_BREAKPOINT:
                exceptionDesc = "Breakpoint";
                break;
            case EXCEPTION_DATATYPE_MISALIGNMENT:
                exceptionDesc = "Data Type Misalignment";
                break;
            case EXCEPTION_FLT_DENORMAL_OPERAND:
                exceptionDesc = "Floating Point Denormal Operand";
                break;
            case EXCEPTION_FLT_DIVIDE_BY_ZERO:
                exceptionDesc = "Floating Point Divide By Zero";
                break;
            case EXCEPTION_FLT_INEXACT_RESULT:
                exceptionDesc = "Floating Point Inexact Result";
                break;
            case EXCEPTION_FLT_INVALID_OPERATION:
                exceptionDesc = "Floating Point Invalid Operation";
                break;
            case EXCEPTION_FLT_OVERFLOW:
                exceptionDesc = "Floating Point Overflow";
                break;
            case EXCEPTION_FLT_STACK_CHECK:
                exceptionDesc = "Floating Point Stack Check";
                break;
            case EXCEPTION_FLT_UNDERFLOW:
                exceptionDesc = "Floating Point Underflow";
                break;
            case EXCEPTION_ILLEGAL_INSTRUCTION:
                exceptionDesc = "Illegal Instruction";
                break;
            case EXCEPTION_IN_PAGE_ERROR:
                exceptionDesc = "In Page Error";
                break;
            case EXCEPTION_INT_DIVIDE_BY_ZERO:
                exceptionDesc = "Integer Divide By Zero";
                break;
            case EXCEPTION_INT_OVERFLOW:
                exceptionDesc = "Integer Overflow";
                break;
            case EXCEPTION_INVALID_DISPOSITION:
                exceptionDesc = "Invalid Disposition";
                break;
            case EXCEPTION_NONCONTINUABLE_EXCEPTION:
                exceptionDesc = "Noncontinuable Exception";
                break;
            case EXCEPTION_PRIV_INSTRUCTION:
                exceptionDesc = "Privileged Instruction";
                break;
            case EXCEPTION_SINGLE_STEP:
                exceptionDesc = "Single Step";
                break;
            case EXCEPTION_STACK_OVERFLOW:
                exceptionDesc = "Stack Overflow";
                break;
            default:
                exceptionDesc = "Unknown Exception";
        }
        
        crashLog << "Exception Description: " << exceptionDesc << "\n";        
      
        HANDLE process = GetCurrentProcess();
        HANDLE thread = GetCurrentThread();
        
        CONTEXT* context = pExceptionInfo->ContextRecord;
        STACKFRAME64 frame = { 0 };
        frame.AddrPC.Offset = context->Rip;
        frame.AddrPC.Mode = AddrModeFlat;
        frame.AddrFrame.Offset = context->Rbp;
        frame.AddrFrame.Mode = AddrModeFlat;
        frame.AddrStack.Offset = context->Rsp;
        frame.AddrStack.Mode = AddrModeFlat;

        crashLog << "\nStack Trace:\n";
        for (int i = 0; i < 25; i++) {
            if (!StackWalk64(IMAGE_FILE_MACHINE_AMD64, process, thread, &frame, context, NULL, SymFunctionTableAccess64, SymGetModuleBase64, NULL)) {
                break;
            }
            
            char buffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)];
            PSYMBOL_INFO symbol = (PSYMBOL_INFO)buffer;
            symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
            symbol->MaxNameLen = MAX_SYM_NAME;
            
            DWORD64 displacement = 0;
            if (SymFromAddr(process, frame.AddrPC.Offset, &displacement, symbol)) {                
                IMAGEHLP_LINE64 line = { 0 };
                line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
                DWORD lineDisplacement = 0;
                
                if (SymGetLineFromAddr64(process, frame.AddrPC.Offset, &lineDisplacement, &line)) {
                    crashLog << "[" << i << "] " << symbol->Name << " (0x" << std::hex << frame.AddrPC.Offset << ")\n";
                    crashLog << "    File: " << line.FileName << "\n";
                    crashLog << "    Line: " << std::dec << line.LineNumber << "\n";
                } else {
                    crashLog << "[" << i << "] " << symbol->Name << " (0x" << std::hex << frame.AddrPC.Offset << ")\n";
                    crashLog << "    File: Unknown\n";
                    crashLog << "    Line: Unknown\n";
                }
            }
        }

        crashLog.close();        
   
        std::cout << "\n=== CRASH DETECTED ===\n";
        std::cout << "Crash log saved to: " << crashLogName << "\n";
        std::cout << "Exception: " << exceptionDesc << " (0x" << std::hex << exceptionCode << ")\n";
        
        return EXCEPTION_CONTINUE_SEARCH;
    }

public:
    static void Initialize() {    
        SymSetOptions(SYMOPT_LOAD_LINES | SYMOPT_UNDNAME | SYMOPT_DEBUG);
        SymInitialize(GetCurrentProcess(), NULL, TRUE);        
      
        SetUnhandledExceptionFilter(TopLevelExceptionHandler);
    }
}; 
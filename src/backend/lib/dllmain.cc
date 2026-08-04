#include <filesystem>
namespace fs = std::filesystem;
#include <format>

#include "base/windows_shit.h"
#define EAT_SHIT_FIRST  // prevent linter move windows shit down
#include "backend/config.h"
#include "backend/hook/game_loop.h"
#include "backend/hook/hook.h"
#include "base/logging.h"
#include "version.h"

namespace {
std::string GetModule(HINSTANCE hInst) {
  char path[MAX_PATH] = {0};
  DWORD nSize = GetModuleFileNameA(hInst, path, _countof(path));
  if (nSize > 0) {
    return std::string(path);
  } else {
    return "";
  }
}
}  // namespace

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason,
                    LPVOID lpvReserved) {
  switch (fdwReason) {
    case DLL_PROCESS_ATTACH: {
      OutputDebugStringA(std::format("[YRTR] Process attach base={:08X}",
                                     reinterpret_cast<uint32_t>(hinstDLL))
                             .c_str());
      yrtr::logging::InitLogging(yrtr::logging::LogSink::kFile,
                                 yrtr::backend::Config::kLogFileName);
      std::string dll_path = GetModule(hinstDLL);
      if (!dll_path.empty()) {
        yrtr::backend::hook::SetDllPath(dll_path);
        // Only install hooks inside game executable. Maybe gamemd.exe,
        // gameares.exe, gamemd-spawn.exe...
        std::string exe_path = GetModule(NULL);
        if (fs::path(exe_path).filename().string().find("game") !=
            std::string::npos) {
          LOG_F(INFO, "Ra2 trainer version=" YRTR_VERSION);
          InstallHooks();
        }
      } else {
        DWORD err = GetLastError();
        std::string message = std::system_category().message(err);
        MessageBoxA(NULL,
                    std::format("Failed to get module file name, err=[{}]{}",
                                err, message)
                        .c_str(),
                    "Ra2 trainer module symbol loading error", 0);
        // Allow carry on without symbol hints.
      }
      break;
    }
    case DLL_THREAD_ATTACH:
      break;
    case DLL_THREAD_DETACH:
      break;
    case DLL_PROCESS_DETACH: {
      OutputDebugStringA("[YRTR] Process detach");
      yrtr::backend::hook::ReclaimResourceOnce();
      if (lpvReserved != nullptr) {
        break;
      }
      break;
    }
  }
  return TRUE;
}

#pragma once
#include <string_view>

#include "base/windows_shit.h"
#define EAT_SHIT_FIRST  // prevent linter move windows shit down

namespace yrtr {
namespace backend {
namespace hook {

void SetDllPath(std::string_view path);
void ReclaimResourceOnce();

void HookUpdate();
void HookExitGame();

}  // namespace hook
}  // namespace backend
}  // namespace yrtr

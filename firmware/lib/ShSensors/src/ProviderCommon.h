#pragma once

namespace SmartHome {
namespace ShSensors {

using LogFn = void (*)(const char* level, const char* format, ...);

struct ProviderUpdate {
    bool changed;
    bool fault;
};

}  // namespace ShSensors
}  // namespace SmartHome

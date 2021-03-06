#define NODE_WANT_INTERNALS 1
#include <node_main_instance.h>
#include <node_native_module_env.h>

namespace node
{
v8::StartupData *NodeMainInstance::GetEmbeddedSnapshotBlob() { return nullptr; }

const std::vector<size_t> *NodeMainInstance::GetIsolateDataIndexes() { return nullptr; }

namespace native_module
{
const bool has_code_cache = false;

void NativeModuleEnv::InitializeCodeCache() {}

}  // namespace native_module

}  // namespace node

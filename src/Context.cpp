#include "Context.hpp"

namespace BIGGEngine {
namespace Context {
namespace {
    bx::AllocatorI* m_allocator;

}   // anonymous namespace

    void init() {
        m_allocator = new bx::DefaultAllocator();
    }
    void shutdown() {
        delete m_allocator;
    }
    bx::AllocatorI* getAllocator() {
        return m_allocator;
    }
}   // namespace Context
}   // namespace BIGGEngine

//
// Created by Stefan Antoszko on 2022-06-23.
//

#include "RenderUtils.hpp"

// for reading shader from disk
#include <bx/file.h>
#include <bx/readerwriter.h>

namespace BIGGEngine {
namespace RenderUtils {

    const bgfx::Memory* loadMem(bx::AllocatorI* allocator, const char* filepath) {
        bx::FileReaderI* fileReader = BX_NEW(allocator, bx::FileReader);
        BIGG_ASSERT(bx::open(fileReader, filepath), "Failed to load {}", filepath);
        uint32_t size = (uint32_t)bx::getSize(fileReader);
        const bgfx::Memory* mem = bgfx::alloc(size+1);
        bx::read(fileReader, mem->data, size, bx::ErrorAssert{});
        bx::close(fileReader);
        mem->data[mem->size-1] = '\0';
        BX_DELETE(allocator, fileReader);
        return mem;
    }
} // namespace RenderBase
} // namespace BIGGEngine
#pragma once

#include "Luna/Core/Core.h"

namespace Luna {

    struct FrameBufferSpecification
    {
        uint32_t Width, Height;
        // FrameBufferFormat format =
        uint32_t Samples = 1;

        bool SwapChainTarget = false;
    };

    class Framebuffer
    {
        public:
            virtual ~Framebuffer() = default;

            virtual const uint32_t GetRendererID() const = 0;
            virtual const FrameBufferSpecification& GetSpecification() const = 0;

            virtual uint32_t GetColorAttachmentRendererID() const = 0;

            virtual void Bind() const = 0;
            virtual void Unbind() const = 0;
            virtual void Resize(uint32_t width, uint32_t height) = 0;
            virtual void SetSpecification(const FrameBufferSpecification& spec) = 0;

            static Ref<Framebuffer> Create(const FrameBufferSpecification& spec);
    };

}

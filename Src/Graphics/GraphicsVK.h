#ifndef PGEINTERNAL_GRAPHICS_VK_H_INCLUDED
#define PGEINTERNAL_GRAPHICS_VK_H_INCLUDED

#include <vulkan/vulkan.hpp>

#include <Graphics/Graphics.h>

namespace PGE {

class GraphicsVK : public Graphics {
    public:
        GraphicsVK(String name, int w = 1280, int h = 720, bool fs = false);
        ~GraphicsVK();

        virtual Renderer getRenderer() override;

        void clear(Color color) override;

        virtual void setDepthTest(bool enabled) override;
        virtual bool getDepthTest() const override;

        void setRenderTarget(Texture* renderTarget) override;
        void setRenderTargets(std::vector<Texture*> renderTargets) override;
        void resetRenderTarget() override;

        void setViewport(Rectanglei vp) override;

        vk::Device getDevice() const;
        vk::CommandBuffer getCurrentCommandBuffer();

    private:
        void throwException(String func, String details) override;
        void cleanup() override;

        bool depthTestEnabled;
};

}

#endif // PGEINTERNAL_GRAPHICS_VK_H_INCLUDED

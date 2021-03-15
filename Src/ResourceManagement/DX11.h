#ifndef PGEINTERNAL_RESOURCEMANAGEMENT_DX11
#define PGEINTERNAL_RESOURCEMANAGEMENT_DX11

#include <d3d11.h>
#include <Exception/Exception.h>
#include <ResourceManagement/ResourceManager.h>
#include <ResourceManagement/ResourceOwner.h>
#include <ResourceManagement/ResourceReference.h>
#include <inttypes.h>

namespace PGE {

typedef ResourceReference<ID3D11Device*> D3D11DeviceRef;
typedef ResourceReference<ID3D11RenderTargetView*> D3D11RenderTargetViewRef;
typedef ResourceReference<ID3D11DepthStencilView*> D3D11DepthStencilViewRef;
typedef ResourceReference<IDXGIFactory1*> DXGIFactory1Ref;
typedef ResourceReference<IDXGISwapChain*> DXGISwapChainRef;
typedef ResourceReference<ID3D11DepthStencilView*> D3D11DepthStencilViewRef;
typedef ResourceReference<ID3D11Texture2D*> D3D11Texture2DRef;
typedef ResourceReference<ID3D11DepthStencilState*> D3D11DepthStencilStateRef;
typedef ResourceReference<ID3D11RasterizerState*> D3D11RasterizerStateRef;
typedef ResourceReference<ID3D11BlendState*> D3D11BlendStateRef;
typedef ResourceReference<ID3D11SamplerState*> D3D11SamplerStateRef;
typedef ResourceReference<ID3D11RenderTargetView*> D3D11RenderTargetViewRef;
typedef ResourceReference<ID3D11DeviceContext*> D3D11DeviceContextRef;
typedef ResourceReference<ID3D11Buffer*> D3D11BufferRef;
typedef ResourceReference<ID3D11VertexShader*> D3D11VertexShaderRef;
typedef ResourceReference<ID3D11PixelShader*> D3D11PixelShaderRef;
typedef ResourceReference<ID3D11InputLayout*> D3D11InputLayoutRef;
typedef ResourceReference<ID3D11ShaderResourceView*> D3D11ShaderResourceViewRef;

template <class T>
class DX11ResourceOwner : public ResourceOwner<T> {
    protected:
        virtual ~DX11ResourceOwner() override {
            static_assert(std::is_pointer<T>::value);
            static_assert(std::is_convertible<T, IUnknown*>::value);
            if (this->holdsResource) { ((IUnknown*)this->resource)->Release(); }
        }
};

class DXGIFactory1Owner : public DX11ResourceOwner<IDXGIFactory1*> {
    protected:
        virtual void initInternal() override {
            HRESULT hResult = CreateDXGIFactory1(__uuidof(IDXGIFactory1), (LPVOID*)(&resource));
            if (FAILED(hResult)) {
                throw Exception("DXGIFactory1Owner", "Failed to create DXGI factory (HRESULT " + String::fromInt(hResult) + ")");
            }
        }
    public:

        static DXGIFactory1Ref createRef(ResourceManager& resourceManager) {
            DXGIFactory1Owner* owner = new DXGIFactory1Owner();
            resourceManager.addResource(owner);
            owner->init();
            return DXGIFactory1Ref(owner->get());
        }
};

class D3D11DeviceOwner : public DX11ResourceOwner<ID3D11Device*> {
    protected:
        virtual void initInternal() override {
            D3D_FEATURE_LEVEL dxFeatureLevel[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_9_3 };

            HRESULT hResult = D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, dxFeatureLevel, 2, D3D11_SDK_VERSION,
                    &resource, NULL, NULL);
            if (FAILED(hResult)) {
                resource = nullptr;
                throw Exception("D3D11DeviceOwner", "Failed to create D3D11 device (HRESULT " + String::fromInt(hResult) + ")");
            }
        }
    public:

        static D3D11DeviceRef createRef(ResourceManager& resourceManager) {
            D3D11DeviceOwner* owner = new D3D11DeviceOwner();
            resourceManager.addResource(owner);
            owner->init();
            return D3D11DeviceRef(owner->get());
        }
};

class D3D11ImmediateContextOwner : public DX11ResourceOwner<ID3D11DeviceContext*> {
    private:
        D3D11DeviceRef dxDevice;
    protected:
        virtual void initInternal() override {
            dxDevice->GetImmediateContext(&resource);
        }
    public:
        D3D11ImmediateContextOwner(D3D11DeviceRef device) {
            dxDevice = device;
        }

        static D3D11DeviceContextRef createRef(D3D11DeviceRef device, ResourceManager& resourceManager) {
            D3D11ImmediateContextOwner* owner = new D3D11ImmediateContextOwner(device);
            resourceManager.addResource(owner);
            owner->init();
            return D3D11DeviceContextRef(owner->get());
        }
};

class DXGISwapChainOwner : public DX11ResourceOwner<IDXGISwapChain*> {
    private:
        DXGIFactory1Ref dxgiFactory;
        D3D11DeviceRef dxDevice;
        DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
    protected:
        virtual void initInternal() override {
            IDXGIDevice1* dxgiDevice = nullptr;
            HRESULT hResult = dxDevice->QueryInterface(__uuidof(IDXGIDevice1), (LPVOID*)(&dxgiDevice));
            if (FAILED(hResult)) {
                throw Exception("DXGISwapChainOwner", "Failed to initialize DXGI device (HRESULT " + String::fromInt(hResult) + ")");
            }

            hResult = dxgiFactory->CreateSwapChain(dxgiDevice, &dxgiSwapChainDesc, &resource);
            dxgiDevice->Release();
            if (FAILED(hResult)) {
                throw Exception("DXGISwapChainOwner", "Failed to create DXGI swapchain (HRESULT " + String::fromInt(hResult) + ")");
            }
        }
    public:
        DXGISwapChainOwner(DXGIFactory1Ref factory, D3D11DeviceRef device, DXGI_SWAP_CHAIN_DESC swapChainDesc) {
            dxgiFactory = factory;
            dxDevice = device;
            dxgiSwapChainDesc = swapChainDesc;
        }

        static DXGISwapChainRef createRef(DXGIFactory1Ref factory, D3D11DeviceRef device, DXGI_SWAP_CHAIN_DESC swapChainDesc, ResourceManager& resourceManager) {
            DXGISwapChainOwner* owner = new DXGISwapChainOwner(factory, device, swapChainDesc);
            resourceManager.addResource(owner);
            owner->init();
            return DXGISwapChainRef(owner->get());
        }
};

class D3D11RenderTargetViewOwner : public DX11ResourceOwner<ID3D11RenderTargetView*> {
    private:
        D3D11DeviceRef dxDevice;
        D3D11Texture2DRef dxTexture;
        bool hasRtvDesc;
        D3D11_RENDER_TARGET_VIEW_DESC dxRenderTargetViewDesc;
    protected:
        virtual void initInternal() override {
            HRESULT hResult = dxDevice->CreateRenderTargetView(dxTexture.get(), hasRtvDesc ? (&dxRenderTargetViewDesc) : nullptr, &resource);
            if (FAILED(hResult)) {
                throw Exception("D3D11RenderTargetViewOwner", "Failed to create render target view (HRESULT " + String::fromInt(hResult) + ")");
            }
        }
    public:
        D3D11RenderTargetViewOwner(D3D11DeviceRef device, D3D11Texture2DRef texture, D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc) {
            dxDevice = device;
            dxTexture = texture;
            dxRenderTargetViewDesc = renderTargetViewDesc;
            hasRtvDesc = true;
        }

        D3D11RenderTargetViewOwner(D3D11DeviceRef device, D3D11Texture2DRef texture) {
            dxDevice = device;
            dxTexture = texture;
            hasRtvDesc = false;
        }

        static D3D11RenderTargetViewRef createRef(D3D11DeviceRef device, D3D11Texture2DRef texture, D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc, ResourceManager& resourceManager) {
            D3D11RenderTargetViewOwner* owner = new D3D11RenderTargetViewOwner(device, texture, renderTargetViewDesc);
            resourceManager.addResource(owner);
            owner->init();
            return D3D11RenderTargetViewRef(owner->get());
        }

        static D3D11RenderTargetViewRef createRef(D3D11DeviceRef device, D3D11Texture2DRef texture, ResourceManager& resourceManager) {
            D3D11RenderTargetViewOwner* owner = new D3D11RenderTargetViewOwner(device, texture);
            resourceManager.addResource(owner);
            owner->init();
            return D3D11RenderTargetViewRef(owner->get());
        }
};

class D3D11BackBufferRtvOwner : public DX11ResourceOwner<ID3D11RenderTargetView*> {
    private:
        D3D11DeviceRef dxDevice;
        DXGISwapChainRef dxgiSwapChain;
    protected:
        virtual void initInternal() override {
            ID3D11Texture2D* backBuffer;
            HRESULT hResult = dxgiSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);
            if (FAILED(hResult)) {
                throw Exception("D3D11BackBufferRtvOwner", "Failed to retrieve back buffer (HRESULT " + String::fromInt(hResult) + ")");
            }
            hResult = dxDevice->CreateRenderTargetView(backBuffer, NULL, &resource);
            backBuffer->Release();
            if (FAILED(hResult)) {
                throw Exception("D3D11BackBufferRtvOwner", "Failed to create back buffer target view (HRESULT " + String::fromInt(hResult) + ")");
            }
        }
    public:
        D3D11BackBufferRtvOwner(D3D11DeviceRef device, DXGISwapChainRef swapChain) {
            dxDevice = device;
            dxgiSwapChain = swapChain;
        }

        static D3D11RenderTargetViewRef createRef(D3D11DeviceRef device, DXGISwapChainRef swapChain, ResourceManager& resourceManager) {
            D3D11BackBufferRtvOwner* owner = new D3D11BackBufferRtvOwner(device, swapChain);
            resourceManager.addResource(owner);
            owner->init();
            return D3D11RenderTargetViewRef(owner->get());
        }
};

class D3D11Texture2DOwner : public DX11ResourceOwner<ID3D11Texture2D*> {
    private:
        D3D11DeviceRef dxDevice;
        D3D11_TEXTURE2D_DESC dxTextureDesc;
    protected:
        virtual void initInternal() override {
            HRESULT hResult = dxDevice->CreateTexture2D(&dxTextureDesc, NULL, &resource);
            if (FAILED(hResult)) {
                throw Exception("D3D11Texture2DOwner", "Failed to create texture (HRESULT " + String::fromInt(hResult) + ")");
            }
        }
    public:
        D3D11Texture2DOwner(D3D11DeviceRef device, D3D11_TEXTURE2D_DESC textureDesc) {
            dxDevice = device;
            dxTextureDesc = textureDesc;
        }

        static D3D11Texture2DRef createRef(D3D11DeviceRef device, D3D11_TEXTURE2D_DESC textureDesc, ResourceManager& resourceManager) {
            D3D11Texture2DOwner* owner = new D3D11Texture2DOwner(device, textureDesc);
            resourceManager.addResource(owner);
            owner->init();
            return D3D11Texture2DRef(owner->get());
        }
};

class D3D11DepthStencilViewOwner : public DX11ResourceOwner<ID3D11DepthStencilView*> {
    private:
        D3D11DeviceRef dxDevice;
        D3D11Texture2DRef dxTexture;
        D3D11_DEPTH_STENCIL_VIEW_DESC dxDsvDesc;
    protected:
        virtual void initInternal() override {
            HRESULT hResult = dxDevice->CreateDepthStencilView(dxTexture.get(), &dxDsvDesc, &resource);
            if (FAILED(hResult)) {
                throw Exception("D3D11DepthStenciViewOwner", "Failed to create main depth stencil view (HRESULT " + String::fromInt(hResult) + ")");
            }
        }
    public:
        D3D11DepthStencilViewOwner(D3D11DeviceRef device, D3D11Texture2DRef texture, D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc) {
            dxDevice = device;
            dxTexture = texture;
            dxDsvDesc = dsvDesc;
        }

        static D3D11DepthStencilViewRef createRef(D3D11DeviceRef device, D3D11Texture2DRef texture, D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc, ResourceManager& resourceManager) {
            D3D11DepthStencilViewOwner* owner = new D3D11DepthStencilViewOwner(device, texture, dsvDesc);
            resourceManager.addResource(owner);
            owner->init();
            return D3D11DepthStencilViewRef(owner->get());
        }
};

class D3D11ShaderResourceViewOwner : public DX11ResourceOwner<ID3D11ShaderResourceView*> {
    private:
        D3D11DeviceRef dxDevice;
        D3D11Texture2DRef dxTexture;
        D3D11_SHADER_RESOURCE_VIEW_DESC dxShaderResourceViewDesc;
    protected:
        virtual void initInternal() override {
            HRESULT hResult = dxDevice->CreateShaderResourceView(dxTexture.get(), &dxShaderResourceViewDesc, &resource);
            if (FAILED(hResult)) {
                throw Exception("D3D11ShaderResourceViewOwner", "Failed to create shader resource view (HRESULT " + String::fromInt(hResult) + ")");
            }
        }
    public:
        D3D11ShaderResourceViewOwner(D3D11DeviceRef device, D3D11Texture2DRef texture, D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc) {
            dxDevice = device;
            dxTexture = texture;
            dxShaderResourceViewDesc = srvDesc;
        }

        static D3D11ShaderResourceViewRef createRef(D3D11DeviceRef device, D3D11Texture2DRef texture, D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc, ResourceManager& resourceManager) {
            D3D11ShaderResourceViewOwner* owner = new D3D11ShaderResourceViewOwner(device, texture, srvDesc);
            resourceManager.addResource(owner);
            owner->init();
            return D3D11ShaderResourceViewRef(owner->get());
        }
};

class D3D11RasterizerStateOwner : public DX11ResourceOwner<ID3D11RasterizerState*> {
    private:
        D3D11DeviceRef dxDevice;
        D3D11_RASTERIZER_DESC dxRasterizerDesc;
    protected:
        virtual void initInternal() override {
            HRESULT hResult = dxDevice->CreateRasterizerState(&dxRasterizerDesc, &resource);
            if (FAILED(hResult)) {
                throw Exception("D3D11RasterizerStateOwner", "Failed to create rasterizer state (HRESULT " + String::fromInt(hResult) + ")");
            }
        }
    public:
        D3D11RasterizerStateOwner(D3D11DeviceRef device, D3D11_RASTERIZER_DESC rasterizerDesc) {
            dxDevice = device;
            dxRasterizerDesc = rasterizerDesc;
        }

        static D3D11RasterizerStateRef createRef(D3D11DeviceRef device, D3D11_RASTERIZER_DESC rasterizerDesc, ResourceManager& resourceManager) {
            D3D11RasterizerStateOwner* owner = new D3D11RasterizerStateOwner(device, rasterizerDesc);
            resourceManager.addResource(owner);
            owner->init();
            return D3D11RasterizerStateRef(owner->get());
        }
};

class D3D11BlendStateOwner : public DX11ResourceOwner<ID3D11BlendState*> {
    private:
        D3D11DeviceRef dxDevice;
        D3D11_BLEND_DESC dxBlendDesc;
    protected:
        virtual void initInternal() override {
            HRESULT hResult = dxDevice->CreateBlendState(&dxBlendDesc, &resource);
            if (FAILED(hResult)) {
                throw Exception("D3D11BlendStateOwner", "Failed to create blend state (HRESULT " + String::fromInt(hResult) + ")");
            }
        }
    public:
        D3D11BlendStateOwner(D3D11DeviceRef device, D3D11_BLEND_DESC blendDesc) {
            dxDevice = device;
            dxBlendDesc = blendDesc;
        }

        static D3D11BlendStateRef createRef(D3D11DeviceRef device, D3D11_BLEND_DESC blendDesc, ResourceManager& resourceManager) {
            D3D11BlendStateOwner* owner = new D3D11BlendStateOwner(device, blendDesc);
            resourceManager.addResource(owner);
            owner->init();
            return D3D11BlendStateRef(owner->get());
        }
};

class D3D11SamplerStateOwner : public DX11ResourceOwner<ID3D11SamplerState*> {
    private:
        D3D11DeviceRef dxDevice;
        D3D11_SAMPLER_DESC dxSamplerDesc;
    protected:
        virtual void initInternal() override {
            HRESULT hResult = dxDevice->CreateSamplerState(&dxSamplerDesc, &resource);
            if (FAILED(hResult)) {
                throw Exception("D3D11SamplerStateOwner", "Failed to create sampler state (HRESULT " + String::fromInt(hResult) + ")");
            }
        }
    public:
        D3D11SamplerStateOwner() {}

        D3D11SamplerStateOwner(D3D11DeviceRef device, D3D11_SAMPLER_DESC samplerDesc) {
            dxDevice = device;
            dxSamplerDesc = samplerDesc;
        }

        static D3D11SamplerStateRef createRef(D3D11DeviceRef device, D3D11_SAMPLER_DESC samplerDesc, ResourceManager& resourceManager) {
            D3D11SamplerStateOwner* owner = new D3D11SamplerStateOwner(device, samplerDesc);
            resourceManager.addResource(owner);
            owner->init();
            return D3D11SamplerStateRef(owner->get());
        }
};

class D3D11DepthStencilStateOwner : public DX11ResourceOwner<ID3D11DepthStencilState*> {
    private:
        D3D11DeviceRef dxDevice;
        D3D11_DEPTH_STENCIL_DESC dxDepthStencilDesc;
    protected:
        virtual void initInternal() override {
            HRESULT hResult = dxDevice->CreateDepthStencilState(&dxDepthStencilDesc, &resource);
            if (FAILED(hResult)) {
                throw Exception("D3D11DepthStencilStateOwner", "Failed to create depth stencil state (HRESULT " + String::fromInt(hResult) + ")");
            }
        }
    public:
        D3D11DepthStencilStateOwner(D3D11DeviceRef device, D3D11_DEPTH_STENCIL_DESC stencilDesc) {
            dxDevice = device;
            dxDepthStencilDesc = stencilDesc;
        }

        static D3D11DepthStencilStateRef createRef(D3D11DeviceRef device, D3D11_DEPTH_STENCIL_DESC stencilDesc, ResourceManager& resourceManager) {
            D3D11DepthStencilStateOwner* owner = new D3D11DepthStencilStateOwner(device, stencilDesc);
            resourceManager.addResource(owner);
            owner->init();
            return D3D11DepthStencilStateRef(owner->get());
        }
};

class D3D11BufferOwner : public DX11ResourceOwner<ID3D11Buffer*> {
    private:
        D3D11DeviceRef dxDevice;
        D3D11_BUFFER_DESC dxBufferDesc;
        D3D11_SUBRESOURCE_DATA dxSubresourceData;
    protected:
        virtual void initInternal() override {
            HRESULT hResult = dxDevice->CreateBuffer(&dxBufferDesc, &dxSubresourceData, &resource);
            if (FAILED(hResult)) {
                throw Exception("D3D11BufferOwner", "Failed to create buffer (HRESULT " + String::fromInt(hResult) + ")");
            }
        }
    public:
        D3D11BufferOwner() { }

        D3D11BufferOwner(D3D11DeviceRef device, D3D11_BUFFER_DESC bufferDesc, D3D11_SUBRESOURCE_DATA subresourceData) {
            dxDevice = device;
            dxBufferDesc = bufferDesc;
            dxSubresourceData = subresourceData;
        }

        static D3D11BufferRef createRef(D3D11DeviceRef device, D3D11_BUFFER_DESC bufferDesc, D3D11_SUBRESOURCE_DATA subresourceData, ResourceManager& resourceManager) {
            D3D11BufferOwner* owner = new D3D11BufferOwner(device, bufferDesc, subresourceData);
            resourceManager.addResource(owner);
            owner->init();
            return D3D11BufferRef(owner->get());
        }
};

class D3D11VertexShaderOwner : public DX11ResourceOwner<ID3D11VertexShader*> {
    private:
        D3D11DeviceRef dxDevice;
        std::vector<uint8_t> vertexShaderBytecode;
    protected:
        virtual void initInternal() override {
            HRESULT hResult = dxDevice->CreateVertexShader(vertexShaderBytecode.data(), sizeof(uint8_t) * vertexShaderBytecode.size(), NULL, &resource);
            if (FAILED(hResult)) {
                throw Exception("D3D11VertexShaderOwner", "Failed to create vertex shader (HRESULT " + String::fromInt(hResult) + ")");
            }
        }
    public:
        D3D11VertexShaderOwner() {}

        D3D11VertexShaderOwner(D3D11DeviceRef device, const std::vector<uint8_t>& bytecode) {
            dxDevice = device;
            vertexShaderBytecode = bytecode;
        }

        static D3D11VertexShaderRef createRef(D3D11DeviceRef device, const std::vector<uint8_t>& bytecode, ResourceManager& resourceManager) {
            D3D11VertexShaderOwner* owner = new D3D11VertexShaderOwner(device, bytecode);
            resourceManager.addResource(owner);
            owner->init();
            return D3D11VertexShaderRef(owner->get());
        }
};

class D3D11PixelShaderOwner : public DX11ResourceOwner<ID3D11PixelShader*> {
    private:
        D3D11DeviceRef dxDevice;
        std::vector<uint8_t> fragmentShaderBytecode;
    protected:
        virtual void initInternal() override {
            HRESULT hResult = dxDevice->CreatePixelShader(fragmentShaderBytecode.data(), sizeof(uint8_t) * fragmentShaderBytecode.size(), NULL, &resource);
            if (FAILED(hResult)) {
                throw Exception("D3D11PixelShaderOwner", "Failed to create fragment shader (HRESULT " + String::fromInt(hResult) + ")");
            }
        }
    public:
        D3D11PixelShaderOwner() {}

        D3D11PixelShaderOwner(D3D11DeviceRef device, const std::vector<uint8_t>& bytecode) {
            dxDevice = device;
            fragmentShaderBytecode = bytecode;
        }

        static D3D11PixelShaderRef createRef(D3D11DeviceRef device, const std::vector<uint8_t>& bytecode, ResourceManager& resourceManager) {
            D3D11PixelShaderOwner* owner = new D3D11PixelShaderOwner(device, bytecode);
            resourceManager.addResource(owner);
            owner->init();
            return D3D11PixelShaderRef(owner->get());
        }
};

class D3D11InputLayoutOwner : public DX11ResourceOwner<ID3D11InputLayout*> {
    private:
        D3D11DeviceRef dxDevice;
        std::vector<D3D11_INPUT_ELEMENT_DESC> dxVertexInputElemDesc;
        std::vector<uint8_t> vertexShaderBytecode;
    protected:
        virtual void initInternal() override {
            HRESULT hResult = dxDevice->CreateInputLayout(dxVertexInputElemDesc.data(), (UINT)dxVertexInputElemDesc.size(), vertexShaderBytecode.data(), vertexShaderBytecode.size() * sizeof(uint8_t), &resource);
            if (FAILED(hResult)) {
                throw Exception("ShaderDX11", "Failed to create input layout (HRESULT " + String::fromInt(hResult) + ")");
            }
        }
    public:
        D3D11InputLayoutOwner() {}

        D3D11InputLayoutOwner(D3D11DeviceRef device, const std::vector<D3D11_INPUT_ELEMENT_DESC> vertexInputElemDesc, const std::vector<uint8_t>& bytecode) {
            dxDevice = device;
            dxVertexInputElemDesc = vertexInputElemDesc;
            vertexShaderBytecode = bytecode;
        }

        static D3D11InputLayoutRef createRef(D3D11DeviceRef device, const std::vector<D3D11_INPUT_ELEMENT_DESC> vertexInputElemDesc, const std::vector<uint8_t>& bytecode, ResourceManager& resourceManager) {
            D3D11InputLayoutOwner* owner = new D3D11InputLayoutOwner(device, vertexInputElemDesc, bytecode);
            resourceManager.addResource(owner);
            owner->init();
            return D3D11InputLayoutRef(owner->get());
        }
};

}

#endif

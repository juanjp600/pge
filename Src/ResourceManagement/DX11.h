#ifndef PGEINTERNAL_RESOURCEMANAGEMENT_DX11
#define PGEINTERNAL_RESOURCEMANAGEMENT_DX11

#include <d3d11.h>
#include <Exception/Exception.h>
#include <ResourceManagement/ResourceManager.h>
#include <ResourceManagement/ResourceOwner.h>
#include <ResourceManagement/ResourceReference.h>

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
    public:
        DXGIFactory1Owner() {
            HRESULT hResult = CreateDXGIFactory1(__uuidof(IDXGIFactory1), (LPVOID*)(&resource));
            if (FAILED(hResult)) {
                throw Exception("DXGIFactory1Owner", "Failed to create DXGI factory (HRESULT " + String::fromInt(hResult) + ")");
            }
        }

        __RES_MNGMT__REF_METH__NO_DFT_CTOR(DXGIFactory1Owner, DXGIFactory1Ref)
};

class D3D11DeviceOwner : public DX11ResourceOwner<ID3D11Device*> {
    public:
        D3D11DeviceOwner() {
            D3D_FEATURE_LEVEL dxFeatureLevel[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_9_3 };

            HRESULT hResult = D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, dxFeatureLevel, 2, D3D11_SDK_VERSION,
                &resource, NULL, NULL);
            if (FAILED(hResult)) {
                resource = nullptr;
                throw Exception("D3D11DeviceOwner", "Failed to create D3D11 device (HRESULT " + String::fromInt(hResult) + ")");
            }
        }

        __RES_MNGMT__REF_METH__NO_DFT_CTOR(D3D11DeviceOwner, D3D11DeviceRef)
};

class D3D11ImmediateContextOwner : public DX11ResourceOwner<ID3D11DeviceContext*> {
    public:
        D3D11ImmediateContextOwner(D3D11DeviceRef device) {
            device->GetImmediateContext(&resource);
        }

        __RES_MNGMT__REF_METH(D3D11ImmediateContextOwner, D3D11DeviceContextRef)
};

class DXGISwapChainOwner : public DX11ResourceOwner<IDXGISwapChain*> {
    public:
        DXGISwapChainOwner(DXGIFactory1Ref factory, D3D11DeviceRef device, DXGI_SWAP_CHAIN_DESC swapChainDesc) {
            IDXGIDevice1* dxgiDevice = nullptr;
            HRESULT hResult = device->QueryInterface(__uuidof(IDXGIDevice1), (LPVOID*)(&dxgiDevice));
            if (FAILED(hResult)) {
                throw Exception("DXGISwapChainOwner", "Failed to initialize DXGI device (HRESULT " + String::fromInt(hResult) + ")");
            }

            hResult = factory->CreateSwapChain(dxgiDevice, &swapChainDesc, &resource);
            dxgiDevice->Release();
            if (FAILED(hResult)) {
                throw Exception("DXGISwapChainOwner", "Failed to create DXGI swapchain (HRESULT " + String::fromInt(hResult) + ")");
            }
        }

        __RES_MNGMT__REF_METH(DXGISwapChainOwner, DXGISwapChainRef)
};

class D3D11RenderTargetViewOwner : public DX11ResourceOwner<ID3D11RenderTargetView*> {
    public:
        D3D11RenderTargetViewOwner(D3D11DeviceRef device, D3D11Texture2DRef texture, D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc) {
            HRESULT hResult = device->CreateRenderTargetView(texture, &renderTargetViewDesc, &resource);
            if (FAILED(hResult)) {
                throw Exception("D3D11RenderTargetViewOwner", "Failed to create render target view (HRESULT " + String::fromInt(hResult) + ")");
            }
        }

        D3D11RenderTargetViewOwner(D3D11DeviceRef device, D3D11Texture2DRef texture) {
            HRESULT hResult = device->CreateRenderTargetView(texture, nullptr, &resource);
            if (FAILED(hResult)) {
                throw Exception("D3D11RenderTargetViewOwner", "Failed to create render target view (HRESULT " + String::fromInt(hResult) + ")");
            }
        }

        __RES_MNGMT__REF_METH(D3D11RenderTargetViewOwner, D3D11RenderTargetViewRef)
};

class D3D11BackBufferRtvOwner : public DX11ResourceOwner<ID3D11RenderTargetView*> {
    public:
        D3D11BackBufferRtvOwner(D3D11DeviceRef device, DXGISwapChainRef swapChain) {
            ID3D11Texture2D* backBuffer;
            HRESULT hResult = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);
            if (FAILED(hResult)) {
                throw Exception("D3D11BackBufferRtvOwner", "Failed to retrieve back buffer (HRESULT " + String::fromInt(hResult) + ")");
            }
            hResult = device->CreateRenderTargetView(backBuffer, NULL, &resource);
            backBuffer->Release();
            if (FAILED(hResult)) {
                throw Exception("D3D11BackBufferRtvOwner", "Failed to create back buffer target view (HRESULT " + String::fromInt(hResult) + ")");
            }
        }

        __RES_MNGMT__REF_METH(D3D11BackBufferRtvOwner, D3D11RenderTargetViewRef)
};

class D3D11Texture2DOwner : public DX11ResourceOwner<ID3D11Texture2D*> {
    public:
        D3D11Texture2DOwner(D3D11DeviceRef device, D3D11_TEXTURE2D_DESC textureDesc) {
            HRESULT hResult = device->CreateTexture2D(&textureDesc, NULL, &resource);
            if (FAILED(hResult)) {
                throw Exception("D3D11Texture2DOwner", "Failed to create texture (HRESULT " + String::fromInt(hResult) + ")");
            }
        }

        __RES_MNGMT__REF_METH(D3D11Texture2DOwner, D3D11Texture2DRef)
};

class D3D11DepthStencilViewOwner : public DX11ResourceOwner<ID3D11DepthStencilView*> {
    public:
        D3D11DepthStencilViewOwner(D3D11DeviceRef device, D3D11Texture2DRef texture, D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc) {
            HRESULT hResult = device->CreateDepthStencilView(texture, &dsvDesc, &resource);
            if (FAILED(hResult)) {
                throw Exception("D3D11DepthStenciViewOwner", "Failed to create main depth stencil view (HRESULT " + String::fromInt(hResult) + ")");
            }
        }

        __RES_MNGMT__REF_METH(D3D11DepthStencilViewOwner, D3D11DepthStencilViewRef)
};

class D3D11ShaderResourceViewOwner : public DX11ResourceOwner<ID3D11ShaderResourceView*> {
    public:
        D3D11ShaderResourceViewOwner(D3D11DeviceRef device, D3D11Texture2DRef texture, D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc) {
            HRESULT hResult = device->CreateShaderResourceView(texture, &srvDesc, &resource);
            if (FAILED(hResult)) {
                throw Exception("D3D11ShaderResourceViewOwner", "Failed to create shader resource view (HRESULT " + String::fromInt(hResult) + ")");
            }
        }

        __RES_MNGMT__REF_METH(D3D11ShaderResourceViewOwner, D3D11ShaderResourceViewRef)
};

class D3D11RasterizerStateOwner : public DX11ResourceOwner<ID3D11RasterizerState*> {
    public:
        D3D11RasterizerStateOwner(D3D11DeviceRef device, D3D11_RASTERIZER_DESC rasterizerDesc) {
            HRESULT hResult = device->CreateRasterizerState(&rasterizerDesc, &resource);
            if (FAILED(hResult)) {
                throw Exception("D3D11RasterizerStateOwner", "Failed to create rasterizer state (HRESULT " + String::fromInt(hResult) + ")");
            }
        }

        __RES_MNGMT__REF_METH(D3D11RasterizerStateOwner, D3D11RasterizerStateRef)
};

class D3D11BlendStateOwner : public DX11ResourceOwner<ID3D11BlendState*> {
    public:
        D3D11BlendStateOwner(D3D11DeviceRef device, D3D11_BLEND_DESC blendDesc) {
            HRESULT hResult = device->CreateBlendState(&blendDesc, &resource);
            if (FAILED(hResult)) {
                throw Exception("D3D11BlendStateOwner", "Failed to create blend state (HRESULT " + String::fromInt(hResult) + ")");
            }
        }

        __RES_MNGMT__REF_METH(D3D11BlendStateOwner, D3D11BlendStateRef)
};

class D3D11SamplerStateOwner : public DX11ResourceOwner<ID3D11SamplerState*> {
    public:
        D3D11SamplerStateOwner(D3D11DeviceRef device, D3D11_SAMPLER_DESC samplerDesc) {
            HRESULT hResult = device->CreateSamplerState(&samplerDesc, &resource);
            if (FAILED(hResult)) {
                throw Exception("D3D11SamplerStateOwner", "Failed to create sampler state (HRESULT " + String::fromInt(hResult) + ")");
            }
        }

        __RES_MNGMT__REF_METH(D3D11SamplerStateOwner, D3D11SamplerStateRef)
};

class D3D11DepthStencilStateOwner : public DX11ResourceOwner<ID3D11DepthStencilState*> {
    public:
        D3D11DepthStencilStateOwner(D3D11DeviceRef device, D3D11_DEPTH_STENCIL_DESC stencilDesc) {
            HRESULT hResult = device->CreateDepthStencilState(&stencilDesc, &resource);
            if (FAILED(hResult)) {
                throw Exception("D3D11DepthStencilStateOwner", "Failed to create depth stencil state (HRESULT " + String::fromInt(hResult) + ")");
            }
        }

        __RES_MNGMT__REF_METH(D3D11DepthStencilStateOwner, D3D11DepthStencilStateRef)
};

class D3D11BufferOwner : public DX11ResourceOwner<ID3D11Buffer*> {
    public:
        D3D11BufferOwner(D3D11DeviceRef device, D3D11_BUFFER_DESC bufferDesc, D3D11_SUBRESOURCE_DATA subresourceData) {
            HRESULT hResult = device->CreateBuffer(&bufferDesc, &subresourceData, &resource);
            if (FAILED(hResult)) {
                throw Exception("D3D11BufferOwner", "Failed to create buffer (HRESULT " + String::fromInt(hResult) + ")");
            }
        }

        __RES_MNGMT__REF_METH(D3D11BufferOwner, D3D11BufferRef)
};

class D3D11VertexShaderOwner : public DX11ResourceOwner<ID3D11VertexShader*> {
    public:
        D3D11VertexShaderOwner(D3D11DeviceRef device, const std::vector<uint8_t>& bytecode) {
            HRESULT hResult = device->CreateVertexShader(bytecode.data(), sizeof(uint8_t) * bytecode.size(), NULL, &resource);
            if (FAILED(hResult)) {
                throw Exception("D3D11VertexShaderOwner", "Failed to create vertex shader (HRESULT " + String::fromInt(hResult) + ")");
            }
        }

        __RES_MNGMT__REF_METH(D3D11VertexShaderOwner, D3D11VertexShaderRef)
};

class D3D11PixelShaderOwner : public DX11ResourceOwner<ID3D11PixelShader*> {
    public:
        D3D11PixelShaderOwner(D3D11DeviceRef device, const std::vector<uint8_t>& bytecode) {
            HRESULT hResult = device->CreatePixelShader(bytecode.data(), sizeof(uint8_t) * bytecode.size(), NULL, &resource);
            if (FAILED(hResult)) {
                throw Exception("D3D11PixelShaderOwner", "Failed to create fragment shader (HRESULT " + String::fromInt(hResult) + ")");
            }
        }

        __RES_MNGMT__REF_METH(D3D11PixelShaderOwner, D3D11PixelShaderRef)
};

class D3D11InputLayoutOwner : public DX11ResourceOwner<ID3D11InputLayout*> {
    public:
        D3D11InputLayoutOwner(D3D11DeviceRef device, const std::vector<D3D11_INPUT_ELEMENT_DESC> vertexInputElemDesc, const std::vector<uint8_t>& bytecode) {
            HRESULT hResult = device->CreateInputLayout(vertexInputElemDesc.data(), (UINT)vertexInputElemDesc.size(), bytecode.data(), bytecode.size() * sizeof(uint8_t), &resource);
            if (FAILED(hResult)) {
                throw Exception("ShaderDX11", "Failed to create input layout (HRESULT " + String::fromInt(hResult) + ")");
            }
        }

        __RES_MNGMT__REF_METH(D3D11InputLayoutOwner, D3D11InputLayoutRef)
};

}

#endif // PGEINTERNAL_RESOURCEMANAGEMENT_DX11

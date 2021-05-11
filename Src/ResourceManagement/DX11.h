#ifndef PGEINTERNAL_RESOURCEMANAGEMENT_DX11
#define PGEINTERNAL_RESOURCEMANAGEMENT_DX11

#include <ResourceManagement/ResourceManager.h>

#include <d3d11.h>

namespace PGE {

template <class T>
class DX11Resource : public Resource<T> {
    protected:
        ~DX11Resource() override {
            static_assert(std::is_pointer<T>::value);
            static_assert(std::is_convertible<T, IUnknown*>::value);
            ((IUnknown*)this->resource)->Release();
        }
};

class DXGIFactory1 : public DX11Resource<IDXGIFactory1*> {
    public:
        DXGIFactory1() {
            HRESULT hResult = CreateDXGIFactory1(__uuidof(IDXGIFactory1), (LPVOID*)(&resource));
            PGE_ASSERT(!FAILED(hResult), "Failed to create DXGI factory (HRESULT: " + String::fromInt(hResult) + ")");
        }
};

class D3D11Device : public DX11Resource<ID3D11Device*> {
    public:
        D3D11Device() {
            D3D_FEATURE_LEVEL dxFeatureLevel[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_9_3 };
            UINT creationFlags = 0;
#ifdef DEBUG
            creationFlags |= D3D11_CREATE_DEVICE_FLAG::D3D11_CREATE_DEVICE_DEBUG;
#endif
            HRESULT hResult = D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, creationFlags, dxFeatureLevel, 2, D3D11_SDK_VERSION,
                &resource, NULL, NULL);
            PGE_ASSERT(!FAILED(hResult), "Failed to create D3D11 device (HRESULT: " + String::fromInt(hResult) + ")");
        }
};

class D3D11ImmediateContext : public DX11Resource<ID3D11DeviceContext*> {
    public:
        D3D11ImmediateContext(ID3D11Device* device) {
            device->GetImmediateContext(&resource);
        }
};

class DXGISwapChain : public DX11Resource<IDXGISwapChain*> {
    public:
        DXGISwapChain(IDXGIFactory1* factory, ID3D11Device* device, DXGI_SWAP_CHAIN_DESC swapChainDesc) {
            IDXGIDevice1* dxgiDevice = nullptr;
            HRESULT hResult = device->QueryInterface(__uuidof(IDXGIDevice1), (LPVOID*)(&dxgiDevice));
            PGE_ASSERT(!FAILED(hResult), "Failed to initialize DXGI device (HRESULT: " + String::fromInt(hResult) + ")");

            hResult = factory->CreateSwapChain(dxgiDevice, &swapChainDesc, &resource);
            dxgiDevice->Release();
            PGE_ASSERT(!FAILED(hResult), "Failed to create DXGI swapchain (HRESULT: " + String::fromInt(hResult) + ")");
        }
};

class D3D11RenderTargetView : public DX11Resource<ID3D11RenderTargetView*> {
    public:
        D3D11RenderTargetView(ID3D11Device* device, ID3D11Texture2D* texture, D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc) {
            HRESULT hResult = device->CreateRenderTargetView(texture, &renderTargetViewDesc, &resource);
            PGE_ASSERT(!FAILED(hResult), "Failed to create render target view (HRESULT: " + String::fromInt(hResult) + ")");
        }

        D3D11RenderTargetView(ID3D11Device* device, ID3D11Texture2D* texture) {
            HRESULT hResult = device->CreateRenderTargetView(texture, nullptr, &resource);
            PGE_ASSERT(!FAILED(hResult), "Failed to create render target view (HRESULT: " + String::fromInt(hResult) + ")");
        }
};

class D3D11BackBufferRtv : public DX11Resource<ID3D11RenderTargetView*> {
    public:
        D3D11BackBufferRtv(ID3D11Device* device, IDXGISwapChain* swapChain) {
            ID3D11Texture2D* backBuffer;
            HRESULT hResult = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);
            PGE_ASSERT(!FAILED(hResult), "Failed to retrieve back buffer (HRESULT: " + String::fromInt(hResult) + ")");
            hResult = device->CreateRenderTargetView(backBuffer, NULL, &resource);
            backBuffer->Release();
            PGE_ASSERT(!FAILED(hResult), "Failed to create back buffer target view (HRESULT: " + String::fromInt(hResult) + ")");
        }
};

class D3D11Texture2D : public DX11Resource<ID3D11Texture2D*> {
    public:
        enum class TYPE {
            RENDER_TARGET,
            DEPTH_STENCIL,
            NORMAL
        };
        
        D3D11Texture2D(ID3D11Device* device, TYPE type, int width, int height, DXGI_FORMAT format) {
            D3D11_TEXTURE2D_DESC textureDesc;
            ZeroMemory(&textureDesc, sizeof(D3D11_TEXTURE2D_DESC));
            textureDesc.Width = (UINT)width;
            textureDesc.Height = (UINT)height;
            if (type == TYPE::DEPTH_STENCIL) {
                textureDesc.MipLevels = 1;
            }
            textureDesc.ArraySize = 1;
            textureDesc.Format = format;
            textureDesc.SampleDesc.Count = 1;
            textureDesc.Usage = D3D11_USAGE_DEFAULT;
            if (type == TYPE::DEPTH_STENCIL) {
                textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
            } else {
                textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
            }
            if (type == TYPE::NORMAL) {
                textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
            }
            HRESULT hResult = device->CreateTexture2D(&textureDesc, NULL, &resource);
            PGE_ASSERT(!FAILED(hResult), "Failed to create texture (HRESULT: " + String::fromInt(hResult) + ")");
        }
};

class D3D11DepthStencilView : public DX11Resource<ID3D11DepthStencilView*> {
    public:
        D3D11DepthStencilView(ID3D11Device* device, ID3D11Texture2D* texture, DXGI_FORMAT format) {
            D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
            ZeroMemory(&descDSV, sizeof(descDSV));
            descDSV.Format = format;
            descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
            descDSV.Texture2D.MipSlice = 0;
            HRESULT hResult = device->CreateDepthStencilView(texture, &descDSV, &resource);
            PGE_ASSERT(!FAILED(hResult), "Failed to create main depth stencil view (HRESULT: " + String::fromInt(hResult) + ")");
        }
};

class D3D11ShaderResourceView : public DX11Resource<ID3D11ShaderResourceView*> {
    public:
        D3D11ShaderResourceView(ID3D11Device* device, ID3D11Texture2D* texture, DXGI_FORMAT format, bool rt) {
            D3D11_SHADER_RESOURCE_VIEW_DESC dxShaderResourceViewDesc;
            ZeroMemory(&dxShaderResourceViewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
            dxShaderResourceViewDesc.Format = format;
            dxShaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
            dxShaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
            dxShaderResourceViewDesc.Texture2D.MipLevels = rt ? 1 : -1;

            HRESULT hResult = device->CreateShaderResourceView(texture, &dxShaderResourceViewDesc, &resource);
            PGE_ASSERT(!FAILED(hResult), "Failed to create shader resource view (HRESULT: " + String::fromInt(hResult) + ")");
        }
};

class D3D11RasterizerState : public DX11Resource<ID3D11RasterizerState*> {
    public:
        D3D11RasterizerState(ID3D11Device* device, D3D11_RASTERIZER_DESC rasterizerDesc) {
            HRESULT hResult = device->CreateRasterizerState(&rasterizerDesc, &resource);
            PGE_ASSERT(!FAILED(hResult), "Failed to create rasterizer state (HRESULT: " + String::fromInt(hResult) + ")");
        }
};

class D3D11BlendState : public DX11Resource<ID3D11BlendState*> {
    public:
        D3D11BlendState(ID3D11Device* device, D3D11_BLEND_DESC blendDesc) {
            HRESULT hResult = device->CreateBlendState(&blendDesc, &resource);
            PGE_ASSERT(!FAILED(hResult), "Failed to create blend state (HRESULT: " + String::fromInt(hResult) + ")");
        }
};

class D3D11SamplerState : public DX11Resource<ID3D11SamplerState*> {
    public:
        D3D11SamplerState(ID3D11Device* device, D3D11_SAMPLER_DESC samplerDesc) {
            HRESULT hResult = device->CreateSamplerState(&samplerDesc, &resource);
            PGE_ASSERT(!FAILED(hResult), "Failed to create sampler state (HRESULT: " + String::fromInt(hResult) + ")");
        }
};

class D3D11DepthStencilState : public DX11Resource<ID3D11DepthStencilState*> {
    public:
        D3D11DepthStencilState(ID3D11Device* device, D3D11_DEPTH_STENCIL_DESC stencilDesc) {
            HRESULT hResult = device->CreateDepthStencilState(&stencilDesc, &resource);
            PGE_ASSERT(!FAILED(hResult), "Failed to create depth stencil state (HRESULT: " + String::fromInt(hResult) + ")");
        }
};

class D3D11Buffer : public DX11Resource<ID3D11Buffer*> {
    public:
        D3D11Buffer(ID3D11Device* device, D3D11_BUFFER_DESC bufferDesc, D3D11_SUBRESOURCE_DATA subresourceData) {
            HRESULT hResult = device->CreateBuffer(&bufferDesc, &subresourceData, &resource);
            PGE_ASSERT(!FAILED(hResult), "Failed to create buffer (HRESULT: " + String::fromInt(hResult) + ")");
        }
};

class D3D11VertexShader : public DX11Resource<ID3D11VertexShader*> {
    public:
        D3D11VertexShader(ID3D11Device* device, const std::vector<byte>& bytecode) {
            HRESULT hResult = device->CreateVertexShader(bytecode.data(), bytecode.size(), NULL, &resource);
            PGE_ASSERT(!FAILED(hResult), "Failed to create vertex shader (HRESULT: " + String::fromInt(hResult) + ")");
        }
};

class D3D11PixelShader : public DX11Resource<ID3D11PixelShader*> {
    public:
        D3D11PixelShader(ID3D11Device* device, const std::vector<byte>& bytecode) {
            HRESULT hResult = device->CreatePixelShader(bytecode.data(), bytecode.size(), NULL, &resource);
            PGE_ASSERT(!FAILED(hResult), "Failed to create fragment shader (HRESULT: " + String::fromInt(hResult) + ")");
        }
};

class D3D11InputLayout : public DX11Resource<ID3D11InputLayout*> {
    public:
        D3D11InputLayout(ID3D11Device* device, const std::vector<D3D11_INPUT_ELEMENT_DESC> vertexInputElemDesc, const std::vector<byte>& bytecode) {
            HRESULT hResult = device->CreateInputLayout(vertexInputElemDesc.data(), (UINT)vertexInputElemDesc.size(), bytecode.data(), bytecode.size(), &resource);
            PGE_ASSERT(!FAILED(hResult), "Failed to create input layout (HRESULT: " + String::fromInt(hResult) + ")");
        }
};

}

#endif // PGEINTERNAL_RESOURCEMANAGEMENT_DX11

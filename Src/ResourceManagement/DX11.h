#ifndef PGEINTERNAL_RESOURCEMANAGEMENT_DX11
#define PGEINTERNAL_RESOURCEMANAGEMENT_DX11

#include <PGE/ResourceManagement/ResourceManager.h>

#include <SDL_syswm.h>
#include <d3d11.h>

#define PGE_ASSERT_DX(CALL, ACTION) { HRESULT hResult = CALL; PGE_ASSERT(!FAILED(hResult), "DX11 Operation failed: " ACTION " (HRESULT: " + String::hexFromInt<unsigned long>(hResult) + ")"); } (void)0

namespace PGE {

template <typename T>
class DX11Resource : public Resource<T> {
    static_assert(std::is_pointer<T>::value);
    static_assert(std::is_convertible<T, IUnknown*>::value);

    protected:
        ~DX11Resource() override {
            ((IUnknown*)this->resource)->Release();
        }
};

class DXGIFactory1 : public DX11Resource<IDXGIFactory1*> {
    public:
        DXGIFactory1() {
            PGE_ASSERT_DX(CreateDXGIFactory1(IID_PPV_ARGS(&resource)), "Create DXGI factory");
        }
};

class D3D11Device : public DX11Resource<ID3D11Device*> {
    public:
        D3D11Device() {
            UINT creationFlags = 0;
#ifdef DEBUG
            creationFlags |= D3D11_CREATE_DEVICE_FLAG::D3D11_CREATE_DEVICE_DEBUG;
#endif
            PGE_ASSERT_DX(D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, creationFlags, NULL, 0, D3D11_SDK_VERSION,
                &resource, NULL, NULL), "Create D3D11 device");
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
        DXGISwapChain(ID3D11Device* device, IDXGIFactory1* factory, int width, int height, SDL_Window* window) {
            IDXGIDevice1* dxgiDevice;
            PGE_ASSERT_DX(device->QueryInterface(IID_PPV_ARGS(&dxgiDevice)), "Initialize DXGI device");

            SDL_SysWMinfo sysWMinfo;
            SDL_VERSION(&sysWMinfo.version); // REMINDER: THIS LINE IS VERY IMPORTANT!
            bool validInfo = SDL_GetWindowWMInfo(window, &sysWMinfo);
            PGE_ASSERT(validInfo, "Failed to initialize SDL version info (SDLERROR: " + String(SDL_GetError()) + ")");

            DXGI_SWAP_CHAIN_DESC desc;
            ZeroMemory(&desc, sizeof(desc));
            desc.BufferCount = 1;
            desc.BufferDesc.Width = width;
            desc.BufferDesc.Height = height;
            desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            desc.BufferDesc.RefreshRate.Numerator = 60;
            desc.BufferDesc.RefreshRate.Denominator = 1;
            desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            desc.OutputWindow = sysWMinfo.info.win.window;
            desc.SampleDesc.Count = 1;
            desc.SampleDesc.Quality = 0;
            desc.Windowed = TRUE;

            HRESULT hRes = factory->CreateSwapChain(dxgiDevice, &desc, &resource);
            dxgiDevice->Release();
            PGE_ASSERT_DX(hRes, "Create DXGI swapchain");
        }
};

class D3D11RenderTargetView : public DX11Resource<ID3D11RenderTargetView*> {
    public:
        D3D11RenderTargetView(ID3D11Device* device, ID3D11Texture2D* texture) {
            PGE_ASSERT_DX(device->CreateRenderTargetView(texture, nullptr, &resource), "Create render target view");
        }
};

class D3D11BackBufferRtv : public DX11Resource<ID3D11RenderTargetView*> {
    public:
        D3D11BackBufferRtv(ID3D11Device* device, IDXGISwapChain* swapChain) {
            ID3D11Texture2D* backBuffer;
            PGE_ASSERT_DX(swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer)), "Retrieve back buffer");
            
            HRESULT hRes = device->CreateRenderTargetView(backBuffer, NULL, &resource);
            backBuffer->Release();
            PGE_ASSERT_DX(hRes, "Create back buffer target view");
        }
};

class D3D11Texture2D : public DX11Resource<ID3D11Texture2D*> {
    public:
        enum class Type {
            RENDER_TARGET,
            DEPTH_STENCIL,
            NO_MIPMAPS,
            NORMAL,
            COMPRESSED,
        };
        
        D3D11Texture2D(ID3D11Device* device, Type type, int width, int height, DXGI_FORMAT format) {
            D3D11_TEXTURE2D_DESC textureDesc;
            ZeroMemory(&textureDesc, sizeof(textureDesc));
            textureDesc.Width = (UINT)width;
            textureDesc.Height = (UINT)height;
            if (type == Type::DEPTH_STENCIL || type == Type::NO_MIPMAPS || type == Type::RENDER_TARGET) {
                textureDesc.MipLevels = 1;
            }
            textureDesc.ArraySize = 1;
            textureDesc.Format = format;
            textureDesc.SampleDesc.Count = 1;
            textureDesc.Usage = D3D11_USAGE_DEFAULT;
            if (type == Type::DEPTH_STENCIL) {
                textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
            } else {
                textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
                if (type != Type::COMPRESSED) {
                    textureDesc.BindFlags |= D3D11_BIND_RENDER_TARGET;
                }
            }
            if (type == Type::NORMAL) {
                textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
            }

            PGE_ASSERT_DX(device->CreateTexture2D(&textureDesc, NULL, &resource), "Create texture");
        }
};

class D3D11DepthStencilView : public DX11Resource<ID3D11DepthStencilView*> {
    public:
        D3D11DepthStencilView(ID3D11Device* device, ID3D11Texture2D* texture) {
            D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
            ZeroMemory(&descDSV, sizeof(descDSV));
            descDSV.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
            descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
            descDSV.Texture2D.MipSlice = 0;

            PGE_ASSERT_DX(device->CreateDepthStencilView(texture, &descDSV, &resource), "Create main depth stencil view");
        }
};

class D3D11ShaderResourceView : public DX11Resource<ID3D11ShaderResourceView*> {
    public:
        D3D11ShaderResourceView(ID3D11Device* device, ID3D11Texture2D* texture, DXGI_FORMAT format) {
            D3D11_SHADER_RESOURCE_VIEW_DESC dxShaderResourceViewDesc;
            ZeroMemory(&dxShaderResourceViewDesc, sizeof(dxShaderResourceViewDesc));
            dxShaderResourceViewDesc.Format = format;
            dxShaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
            dxShaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
            dxShaderResourceViewDesc.Texture2D.MipLevels = (UINT)-1;

            PGE_ASSERT_DX(device->CreateShaderResourceView(texture, &dxShaderResourceViewDesc, &resource), "Create shader resource view");
        }
};

class D3D11RasterizerState : public DX11Resource<ID3D11RasterizerState*> {
    public:
        D3D11RasterizerState(ID3D11Device* device, Graphics::Culling cullMode) {
            D3D11_RASTERIZER_DESC desc;
            ZeroMemory(&desc, sizeof(desc));
            desc.AntialiasedLineEnable = false;
            D3D11_CULL_MODE dxMode;
            switch (cullMode) {
                default:
                case Graphics::Culling::BACK: {
                    dxMode = D3D11_CULL_BACK;
                } break;
                case Graphics::Culling::FRONT: {
                    dxMode = D3D11_CULL_FRONT;
                } break;
                case Graphics::Culling::NONE: {
                    dxMode = D3D11_CULL_NONE;
                } break;
            }
            desc.CullMode = dxMode;
            desc.DepthClipEnable = true;
            desc.FillMode = D3D11_FILL_SOLID;
            desc.ScissorEnable = false;
            desc.MultisampleEnable = false;
            desc.FrontCounterClockwise = true;

            PGE_ASSERT_DX(device->CreateRasterizerState(&desc, &resource), "Create rasterizer state");
        }
};

class D3D11BlendState : public DX11Resource<ID3D11BlendState*> {
    public:
        D3D11BlendState(ID3D11Device* device) {
            D3D11_BLEND_DESC desc;
            ZeroMemory(&desc, sizeof(desc));
            desc.RenderTarget[0].BlendEnable = true;
            desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
            desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
            desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
            desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
            desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
            desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
            desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

            PGE_ASSERT_DX(device->CreateBlendState(&desc, &resource), "Create blend state");
        }
};

class D3D11SamplerState : public DX11Resource<ID3D11SamplerState*> {
    public:
        D3D11SamplerState(ID3D11Device* device) {
            D3D11_SAMPLER_DESC desc;
            ZeroMemory(&desc, sizeof(desc));
            desc.Filter = D3D11_FILTER_ANISOTROPIC;
            desc.MaxAnisotropy = 8;
            desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
            desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
            desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
            desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
            desc.MinLOD = 0;
            desc.MaxLOD = D3D11_FLOAT32_MAX;
            desc.MipLODBias = -0.1f;

            PGE_ASSERT_DX(device->CreateSamplerState(&desc, &resource), "Create sampler state");
        }
};

class D3D11DepthStencilState : public DX11Resource<ID3D11DepthStencilState*> {
    public:
        D3D11DepthStencilState(ID3D11Device* device, D3D11_DEPTH_STENCIL_DESC stencilDesc) {
            PGE_ASSERT_DX(device->CreateDepthStencilState(&stencilDesc, &resource), "Create depth stencil state");
        }
};

class D3D11Buffer : public DX11Resource<ID3D11Buffer*> {
    public:
        enum class Type {
            CONSTANT,
            VERTEX,
            INDEX,
        };

        D3D11Buffer(ID3D11Device* device, Type type, void* data, size_t size) {
            D3D11_BUFFER_DESC desc;
            ZeroMemory(&desc, sizeof(desc));
            desc.Usage = D3D11_USAGE_DEFAULT;
            desc.ByteWidth = (UINT)size;
            switch (type) {
                case Type::CONSTANT: {
                    desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
                } break;
                case Type::VERTEX: {
                    desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
                } break;
                case Type::INDEX: {
                    desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
                } break;
            }

            D3D11_SUBRESOURCE_DATA subresourceData;
            ZeroMemory(&subresourceData, sizeof(subresourceData));
            subresourceData.pSysMem = data;

            PGE_ASSERT_DX(device->CreateBuffer(&desc, &subresourceData, &resource), "Create buffer");
        }
};

class D3D11VertexShader : public DX11Resource<ID3D11VertexShader*> {
    public:
        D3D11VertexShader(ID3D11Device* device, const std::vector<byte>& bytecode) {
            PGE_ASSERT_DX(device->CreateVertexShader(bytecode.data(), bytecode.size(), NULL, &resource), "Create vertex shader");
        }
};

class D3D11PixelShader : public DX11Resource<ID3D11PixelShader*> {
    public:
        D3D11PixelShader(ID3D11Device* device, const std::vector<byte>& bytecode) {
            PGE_ASSERT_DX(device->CreatePixelShader(bytecode.data(), bytecode.size(), NULL, &resource), "Create fragment shader");
        }
};

class D3D11InputLayout : public DX11Resource<ID3D11InputLayout*> {
    public:
        D3D11InputLayout(ID3D11Device* device, const std::vector<D3D11_INPUT_ELEMENT_DESC> vertexInputElemDesc, const std::vector<byte>& bytecode) {
            PGE_ASSERT_DX(device->CreateInputLayout(vertexInputElemDesc.data(), (UINT)vertexInputElemDesc.size(), bytecode.data(), bytecode.size(), &resource),
                "Create input layout");
        }
};

}

#endif // PGEINTERNAL_RESOURCEMANAGEMENT_DX11

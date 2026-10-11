#include <lunapch.h>
#include "Luna/Renderer/Renderer2D.h"

#include "Luna/Renderer/RenderCommand.h"

#include "Luna/Renderer/Shader.h"
#include "Luna/Renderer/VertexArray.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Luna {

    struct QuadVertex
    {
        glm::vec3 Position;
        glm::vec4 Color;
        glm::vec2 TexCoord;
        float TilingFactor;
        float TexID;
    };

    struct Renderer2DData
    {
        static const uint32_t MaxQuads = 40000;
        static const uint32_t MaxVertices = MaxQuads * 4;
        static const uint32_t MaxIndices = MaxQuads * 6;
        static const uint32_t MaxTextureBound = 32; // TODO: Render Capabilities

        Ref<VertexArray> QuadVertexArray;
        Ref<VertexBuffer> QuadVertexBuffer;
        Ref<Shader> ColorTextureShader;
        Ref<Texture2D> WhiteTexture;
        Ref<SubTexture2D> WhiteSubTexture;

        uint32_t QuadIndexCount = 0;
        QuadVertex* QuadVertexBufferBase = nullptr;
        QuadVertex* QuadVertexBufferPtr = nullptr;

        std::array<Ref<Texture2D>, MaxTextureBound> TextureSlot;
        uint32_t TextureSlotIndex = 1; // 0 is white texture

        glm::vec4 QuadVertexPos[4];

        Renderer2D::Statistics Stats;
    };

    static Renderer2DData* s_Data;

    void Renderer2D::Init()
    {
        LUNA_PROFILE_FUNCTION();

        if (s_Data)
            return;

        s_Data = new Renderer2DData();

        s_Data->QuadVertexArray = VertexArray::Create();
        s_Data->QuadVertexBuffer = VertexBuffer::Create(s_Data->MaxVertices * sizeof(QuadVertex));
        BufferLayout sqLayout = {
            { ShaderDataType::Float3, "a_Pos" },
            { ShaderDataType::Float4, "a_Color" },
            { ShaderDataType::Float2, "a_TexCoord" },
            { ShaderDataType::Float,  "a_TilingFactor" },
            { ShaderDataType::Float,  "a_TexID" }
        };
        s_Data->QuadVertexBuffer->SetLayout(sqLayout);
        s_Data->QuadVertexArray->AddVertexBuffer(s_Data->QuadVertexBuffer);

        s_Data->QuadVertexBufferBase = new QuadVertex[s_Data->MaxVertices];

        uint32_t* quadIndices = new uint32_t[s_Data->MaxIndices];
        uint32_t offset = 0;
        for (uint32_t i = 0; i < s_Data->MaxIndices; i+=6)
        {
            quadIndices[i + 0] = offset + 0;
            quadIndices[i + 1] = offset + 1;
            quadIndices[i + 2] = offset + 2;

            quadIndices[i + 3] = offset + 2;
            quadIndices[i + 4] = offset + 3;
            quadIndices[i + 5] = offset + 0;
            offset += 4;
        }
        Ref<IndexBuffer> squareIB = IndexBuffer::Create(quadIndices, s_Data->MaxIndices);
        s_Data->QuadVertexArray->SetIndexBuffer(squareIB);
        delete[] quadIndices;

        s_Data->WhiteTexture = Texture2D::Create(1, 1);
        uint32_t texData = 0xffffffff;
        s_Data->WhiteTexture->SetData(&texData, sizeof(texData));
        s_Data->WhiteSubTexture = SubTexture2D::CreateFromCoords(s_Data->WhiteTexture, glm::vec2{0.0f, 0.0f}, glm::vec2{1.0f, 1.0f});

        int32_t samplers[s_Data->MaxTextureBound];
        for (uint32_t i =0; i < s_Data->MaxTextureBound; i++)
            samplers[i] = i;

        s_Data->ColorTextureShader = Shader::Create("Assets/Shader/ColorTextureShader.glsl");
        s_Data->ColorTextureShader->Bind();
        s_Data->ColorTextureShader->SetIntArray(samplers, s_Data->MaxTextureBound, "u_Texture");

        s_Data->TextureSlot[0] = s_Data->WhiteTexture;

        s_Data->QuadVertexPos[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
        s_Data->QuadVertexPos[1] = {  0.5f, -0.5f, 0.0f, 1.0f };
        s_Data->QuadVertexPos[2] = {  0.5f,  0.5f, 0.0f, 1.0f };
        s_Data->QuadVertexPos[3] = { -0.5f,  0.5f, 0.0f, 1.0f };
    }

    void Renderer2D::Shutdown()
    {
        LUNA_PROFILE_FUNCTION();

        delete[] s_Data->QuadVertexBufferBase;
        delete s_Data;
        s_Data = nullptr;
    }

    void Renderer2D::BeginScene(const CameraOrtho& camera)
    {
        LUNA_PROFILE_FUNCTION();

        s_Data->ColorTextureShader->Bind();
        s_Data->ColorTextureShader->SetMat4(camera.GetViewProjectionMat(), "u_ViewProjection");

        s_Data->QuadIndexCount = 0;
        s_Data->QuadVertexBufferPtr = s_Data->QuadVertexBufferBase;
        s_Data->TextureSlotIndex = 1;
    }

    void Renderer2D::EndScene()
    {
        LUNA_PROFILE_FUNCTION();

        uint32_t dataSize = (uint8_t*)s_Data->QuadVertexBufferPtr - (uint8_t*)s_Data->QuadVertexBufferBase;
        s_Data->QuadVertexBuffer->SetData(s_Data->QuadVertexBufferBase, dataSize);
        Flush();
    }

    void Renderer2D::Flush()
    {
        LUNA_PROFILE_FUNCTION();
        
        for (uint32_t i = 0; i < s_Data->TextureSlotIndex; i++)
            s_Data->TextureSlot[i]->Bind(i);

        RenderCommand::DrawIndexed(s_Data->QuadVertexArray, s_Data->QuadIndexCount);
        s_Data->Stats.DrawCalls++;
    }

    void Renderer2D::FlushAndReset()
    {
        EndScene();
        s_Data->QuadIndexCount = 0;
        s_Data->QuadVertexBufferPtr = s_Data->QuadVertexBufferBase;
        s_Data->TextureSlotIndex = 1;
    }

    // Texture
    void Renderer2D::DrawQuad(const glm::mat4 transform, const Ref<Texture2D>& texture, const float tilingFactor)
    {
        DrawQuad(transform, glm::vec4{1.0f, 1.0f, 1.0f, 1.0f}, texture, tilingFactor);
    }

    void Renderer2D::DrawQuad(const glm::mat4 transform, const glm::vec4& color, const Ref<Texture2D>& texture, const float tilingFactor)
    {
        LUNA_PROFILE_FUNCTION();

        if (s_Data->QuadIndexCount >= Renderer2DData::MaxIndices)
            FlushAndReset();

        float textureIndex = 0.0f;
        if (texture)
        {
            for (uint32_t i = 1; i < s_Data->TextureSlotIndex; i ++)
            {
                if (s_Data->TextureSlot[i].get() == texture.get())
                {
                    textureIndex = (float)i;
                    break;
                }
            }
            
            if (textureIndex == 0.0f)
            {
                if (s_Data->TextureSlotIndex >= Renderer2DData::MaxTextureBound)
                    FlushAndReset();

                textureIndex = (float)s_Data->TextureSlotIndex;
                s_Data->TextureSlot[s_Data->TextureSlotIndex] = texture;
                s_Data->TextureSlotIndex++;
            }
        }

        s_Data->QuadVertexBufferPtr->Position = transform * s_Data->QuadVertexPos[0];
        s_Data->QuadVertexBufferPtr->Color = color;
        s_Data->QuadVertexBufferPtr->TexCoord = {0.0f, 0.0f};
        s_Data->QuadVertexBufferPtr->TilingFactor = tilingFactor;
        s_Data->QuadVertexBufferPtr->TexID = textureIndex;
        s_Data->QuadVertexBufferPtr++;

        s_Data->QuadVertexBufferPtr->Position = transform * s_Data->QuadVertexPos[1];
        s_Data->QuadVertexBufferPtr->Color = color;
        s_Data->QuadVertexBufferPtr->TexCoord = {1.0f, 0.0f};
        s_Data->QuadVertexBufferPtr->TilingFactor = tilingFactor;
        s_Data->QuadVertexBufferPtr->TexID = textureIndex;
        s_Data->QuadVertexBufferPtr++;

        s_Data->QuadVertexBufferPtr->Position = transform * s_Data->QuadVertexPos[2];
        s_Data->QuadVertexBufferPtr->Color = color;
        s_Data->QuadVertexBufferPtr->TexCoord = {1.0f, 1.0f};
        s_Data->QuadVertexBufferPtr->TilingFactor = tilingFactor;
        s_Data->QuadVertexBufferPtr->TexID = textureIndex;
        s_Data->QuadVertexBufferPtr++;

        s_Data->QuadVertexBufferPtr->Position = transform * s_Data->QuadVertexPos[3];
        s_Data->QuadVertexBufferPtr->Color = color;
        s_Data->QuadVertexBufferPtr->TexCoord = {0.0f, 1.0f};
        s_Data->QuadVertexBufferPtr->TilingFactor = tilingFactor;
        s_Data->QuadVertexBufferPtr->TexID = textureIndex;
        s_Data->QuadVertexBufferPtr++;

        s_Data->QuadIndexCount += 6;

        s_Data->Stats.QuadCount++;
    }

    // SubTexture2D
    void Renderer2D::DrawQuad(const glm::mat4 transform, const Ref<SubTexture2D>& texture, const float tilingFactor)
    {
        DrawQuad(transform, glm::vec4{1.0f, 1.0f, 1.0f, 1.0f}, texture, tilingFactor);
    }

    void Renderer2D::DrawQuad(const glm::mat4 transform, const glm::vec4& color, const Ref<SubTexture2D>& subTexture, const float tilingFactor)
    {
        LUNA_PROFILE_FUNCTION();

        if (s_Data->QuadIndexCount >= Renderer2DData::MaxIndices)
            FlushAndReset();

        float textureIndex = 0.0f;
        Ref<SubTexture2D> texture = subTexture;
        if (texture == nullptr)
            texture = s_Data->WhiteSubTexture;

        if (texture)
        {
            for (uint32_t i = 1; i < s_Data->TextureSlotIndex; i ++)
            {
                if (s_Data->TextureSlot[i].get() == texture->GetTexture().get())
                {
                    textureIndex = (float)i;
                    break;
                }
            }
            
            if (textureIndex == 0.0f)
            {
                if (s_Data->TextureSlotIndex >= Renderer2DData::MaxTextureBound)
                    FlushAndReset();

                textureIndex = (float)s_Data->TextureSlotIndex;
                s_Data->TextureSlot[s_Data->TextureSlotIndex] = texture->GetTexture();
                s_Data->TextureSlotIndex++;
            }
        }

        s_Data->QuadVertexBufferPtr->Position = transform * s_Data->QuadVertexPos[0];
        s_Data->QuadVertexBufferPtr->Color = color;
        s_Data->QuadVertexBufferPtr->TexCoord = texture->GetTexCoords()[0];
        s_Data->QuadVertexBufferPtr->TilingFactor = tilingFactor;
        s_Data->QuadVertexBufferPtr->TexID = textureIndex;
        s_Data->QuadVertexBufferPtr++;

        s_Data->QuadVertexBufferPtr->Position = transform * s_Data->QuadVertexPos[1];
        s_Data->QuadVertexBufferPtr->Color = color;
        s_Data->QuadVertexBufferPtr->TexCoord = texture->GetTexCoords()[1];
        s_Data->QuadVertexBufferPtr->TilingFactor = tilingFactor;
        s_Data->QuadVertexBufferPtr->TexID = textureIndex;
        s_Data->QuadVertexBufferPtr++;

        s_Data->QuadVertexBufferPtr->Position = transform * s_Data->QuadVertexPos[2];
        s_Data->QuadVertexBufferPtr->Color = color;
        s_Data->QuadVertexBufferPtr->TexCoord = texture->GetTexCoords()[2];
        s_Data->QuadVertexBufferPtr->TilingFactor = tilingFactor;
        s_Data->QuadVertexBufferPtr->TexID = textureIndex;
        s_Data->QuadVertexBufferPtr++;

        s_Data->QuadVertexBufferPtr->Position = transform * s_Data->QuadVertexPos[3];
        s_Data->QuadVertexBufferPtr->Color = color;
        s_Data->QuadVertexBufferPtr->TexCoord = texture->GetTexCoords()[3];
        s_Data->QuadVertexBufferPtr->TilingFactor = tilingFactor;
        s_Data->QuadVertexBufferPtr->TexID = textureIndex;
        s_Data->QuadVertexBufferPtr++;

        s_Data->QuadIndexCount += 6;

        s_Data->Stats.QuadCount++;
    }

    // Overloads
    void Renderer2D::DrawQuad(const glm::vec2& position,const glm::vec2& scale, const Ref<Texture2D>& texture, const float tilingFactor)
    {
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3{position.x, position.y, 0.0f})
            * glm::scale(glm::mat4(1.0f), {scale.x, scale.y, 1.0f});

        DrawQuad(transform, glm::vec4{1.0f, 1.0f, 1.0f, 1.0f}, texture, tilingFactor);
    }

    void Renderer2D::DrawQuad(const glm::vec3& position,const glm::vec2& scale, const Ref<Texture2D>& texture, const float tilingFactor)
    {
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
            * glm::scale(glm::mat4(1.0f), {scale.x, scale.y, 1.0f});

        DrawQuad(transform, glm::vec4{1.0f, 1.0f, 1.0f, 1.0f}, texture, tilingFactor);
    }

    void Renderer2D::DrawQuad(const glm::vec2& position,const glm::vec2& scale, const glm::vec4& color, const Ref<Texture2D>& texture, const float tilingFactor)
    {
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3{position.x, position.y, 0.0f})
            * glm::scale(glm::mat4(1.0f), {scale.x, scale.y, 1.0f});

        DrawQuad(transform, color, texture, tilingFactor);
    }

    void Renderer2D::DrawQuad(const glm::vec3& position,const glm::vec2& scale, const glm::vec4& color, const Ref<Texture2D>& texture, const float tilingFactor)
    {
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
            * glm::scale(glm::mat4(1.0f), {scale.x, scale.y, 1.0f});

        DrawQuad(transform, color, texture, tilingFactor);
    }

    void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& scale, const float rotation, const Ref<Texture2D>& texture, const float tilingFactor)
    {
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3{position.x, position.y, 0.0f})
            * glm::rotate(glm::mat4(1.0f), rotation, {0.0f, 0.0f, 1.0f})
            * glm::scale(glm::mat4(1.0f), {scale.x, scale.y, 1.0f});

        DrawQuad(transform , glm::vec4{1.0f, 1.0f, 1.0f, 1.0f}, texture, tilingFactor);
    }

    void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& scale, const float rotation, const Ref<Texture2D>& texture, const float tilingFactor)
    {
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3{position.x, position.y, 0.0f})
            * glm::rotate(glm::mat4(1.0f), rotation, {0.0f, 0.0f, 1.0f})
            * glm::scale(glm::mat4(1.0f), {scale.x, scale.y, 1.0f});

        DrawQuad(transform , glm::vec4{1.0f, 1.0f, 1.0f, 1.0f}, texture, tilingFactor);
    }

    void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& scale, const float rotation, const glm::vec4& color, const Ref<Texture2D>& texture, const float tilingFactor)
    {
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3{position.x, position.y, 0.0f})
            * glm::rotate(glm::mat4(1.0f), rotation, {0.0f, 0.0f, 1.0f})
            * glm::scale(glm::mat4(1.0f), {scale.x, scale.y, 1.0f});

        DrawQuad(transform, color, texture, tilingFactor);
    }

    void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& scale, const float rotation, const glm::vec4& color, const Ref<Texture2D>& texture, const float tilingFactor)
    {
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
            * glm::rotate(glm::mat4(1.0f), rotation, {0.0f, 0.0f, 1.0f})
            * glm::scale(glm::mat4(1.0f), {scale.x, scale.y, 1.0f});

        DrawQuad(transform, color, texture, tilingFactor);
    }

    void Renderer2D::DrawQuad(const glm::vec2& position,const glm::vec2& scale, const Ref<SubTexture2D>& texture, const float tilingFactor)
    {
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3{position.x, position.y, 0.0f})
            * glm::scale(glm::mat4(1.0f), {scale.x, scale.y, 1.0f});

        DrawQuad(transform, glm::vec4{1.0f, 1.0f, 1.0f, 1.0f}, texture, tilingFactor);
    }

    void Renderer2D::DrawQuad(const glm::vec3& position,const glm::vec2& scale, const Ref<SubTexture2D>& texture, const float tilingFactor)
    {
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3{position.x, position.y, 0.0f})
            * glm::scale(glm::mat4(1.0f), {scale.x, scale.y, 1.0f});

        DrawQuad(transform, glm::vec4{1.0f, 1.0f, 1.0f, 1.0f}, texture, tilingFactor);
    }

    void Renderer2D::DrawQuad(const glm::vec2& position,const glm::vec2& scale, const glm::vec4& color, const Ref<SubTexture2D>& texture, const float tilingFactor)
    {
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3{position.x, position.y, 0.0f})
            * glm::scale(glm::mat4(1.0f), {scale.x, scale.y, 1.0f});

        DrawQuad(transform, color, texture, tilingFactor);
    }

    void Renderer2D::DrawQuad(const glm::vec3& position,const glm::vec2& scale, const glm::vec4& color, const Ref<SubTexture2D>& texture, const float tilingFactor)
    {
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
            * glm::scale(glm::mat4(1.0f), {scale.x, scale.y, 1.0f});

        DrawQuad(transform, color, texture, tilingFactor);
    }

    void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& scale, const float rotation, const Ref<SubTexture2D>& texture, const float tilingFactor)
    {
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3{position.x, position.y, 0.0f})
            * glm::rotate(glm::mat4(1.0f), rotation, {0.0f, 0.0f, 1.0f})
            * glm::scale(glm::mat4(1.0f), {scale.x, scale.y, 1.0f});

        DrawQuad(transform, glm::vec4{1.0f, 1.0f, 1.0f, 1.0f}, texture, tilingFactor);
    }

    void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& scale, const float rotation, const Ref<SubTexture2D>& texture, const float tilingFactor)
    {
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
            * glm::rotate(glm::mat4(1.0f), rotation, {0.0f, 0.0f, 1.0f})
            * glm::scale(glm::mat4(1.0f), {scale.x, scale.y, 1.0f});

        DrawQuad(transform, glm::vec4{1.0f, 1.0f, 1.0f, 1.0f}, texture, tilingFactor);
    }

    void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& scale, const float rotation, const glm::vec4& color, const Ref<SubTexture2D>& texture, const float tilingFactor)
    {
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3{position.x, position.y, 0.0f})
            * glm::rotate(glm::mat4(1.0f), rotation, {0.0f, 0.0f, 1.0f})
            * glm::scale(glm::mat4(1.0f), {scale.x, scale.y, 1.0f});

        DrawQuad(transform, color, texture, tilingFactor);
    }

    void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& scale, const float rotation, const glm::vec4& color, const Ref<SubTexture2D>& texture, const float tilingFactor)
    {
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
            * glm::rotate(glm::mat4(1.0f), rotation, {0.0f, 0.0f, 1.0f})
            * glm::scale(glm::mat4(1.0f), {scale.x, scale.y, 1.0f});
        
        DrawQuad(transform, color, texture, tilingFactor);
    }


    void Renderer2D::OnWindowResize(int width, int height)
    {
        RenderCommand::SetViewport(0, 0, width, height);
    }

    void Renderer2D::ResetStats()
    {
        memset(&s_Data->Stats, 0, sizeof(Renderer2D::Statistics));
    }

    Renderer2D::Statistics Renderer2D::GetStats()
    {
        return s_Data->Stats;
    }
}

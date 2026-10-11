#pragma once
#include <lunapch.h>

#include "Luna/Renderer/Camera.h"
#include "Luna/Renderer/Texture.h"
#include "Luna/Renderer/SubTexture.h"

namespace Luna {

    class Renderer2D
    {
        public:
            static void Init();
            static void Shutdown();
            static void OnWindowResize(int width, int height);

            static void BeginScene(const CameraOrtho& camera);
            static void Flush();
            static void EndScene();

            // Primitives

            // Textures
            static void DrawQuad(const glm::mat4 transform, const Ref<Texture2D>& texture = nullptr, const float tilingFactor = 1.0f);
            static void DrawQuad(const glm::mat4 transform, const glm::vec4& color, const Ref<Texture2D>& texture = nullptr, const float tilingFactor = 1.0f);

            static void DrawQuad(const glm::vec2& position,const glm::vec2& scale, const Ref<Texture2D>& texture = nullptr, const float tilingFactor = 1.0f);
            static void DrawQuad(const glm::vec3& position,const glm::vec2& scale, const Ref<Texture2D>& texture = nullptr, const float tilingFactor = 1.0f);
            static void DrawQuad(const glm::vec2& position,const glm::vec2& scale, const glm::vec4& color, const Ref<Texture2D>& texture = nullptr, const float tilingFactor = 1.0f);
            static void DrawQuad(const glm::vec3& position,const glm::vec2& scale, const glm::vec4& color, const Ref<Texture2D>& texture = nullptr, const float tilingFactor = 1.0f);

            static void DrawRotatedQuad(const glm::vec2& position, const glm::vec2& scale, const float rotation, const Ref<Texture2D>& texture = nullptr, const float tilingFactor = 1.0f);
            static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2& scale, const float rotation, const Ref<Texture2D>& texture = nullptr, const float tilingFactor = 1.0f);
            static void DrawRotatedQuad(const glm::vec2& position, const glm::vec2& scale, const float rotation, const glm::vec4& color, const Ref<Texture2D>& texture = nullptr, const float tilingFactor = 1.0f);
            static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2& scale, const float rotation, const glm::vec4& color, const Ref<Texture2D>& texture = nullptr, const float tilingFactor = 1.0f);

            // Sub Textures
            static void DrawQuad(const glm::mat4 transform, const Ref<SubTexture2D>& texture = nullptr, const float tilingFactor = 1.0f);
            static void DrawQuad(const glm::mat4 transform, const glm::vec4& color, const Ref<SubTexture2D>& texture, const float tilingFactor = 1.0f);

            static void DrawQuad(const glm::vec2& position,const glm::vec2& scale, const Ref<SubTexture2D>& texture, const float tilingFactor = 1.0f);
            static void DrawQuad(const glm::vec3& position,const glm::vec2& scale, const Ref<SubTexture2D>& texture, const float tilingFactor = 1.0f);
            static void DrawQuad(const glm::vec2& position,const glm::vec2& scale, const glm::vec4& color, const Ref<SubTexture2D>& texture, const float tilingFactor = 1.0f);
            static void DrawQuad(const glm::vec3& position,const glm::vec2& scale, const glm::vec4& color, const Ref<SubTexture2D>& texture, const float tilingFactor = 1.0f);

            static void DrawRotatedQuad(const glm::vec2& position, const glm::vec2& scale, const float rotation, const Ref<SubTexture2D>& texture, const float tilingFactor = 1.0f);
            static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2& scale, const float rotation, const Ref<SubTexture2D>& texture, const float tilingFactor = 1.0f);
            static void DrawRotatedQuad(const glm::vec2& position, const glm::vec2& scale, const float rotation, const glm::vec4& color, const Ref<SubTexture2D>& texture, const float tilingFactor = 1.0f);
            static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2& scale, const float rotation, const glm::vec4& color, const Ref<SubTexture2D>& texture, const float tilingFactor = 1.0f);

            struct Statistics
            {
                uint32_t DrawCalls = 0;
                uint32_t QuadCount = 0;

                uint32_t GetTotalVertexCount() { return QuadCount * 4; }
                uint32_t GetTotalIndexCount() { return QuadCount * 6; }
            };
            static void ResetStats();
            static Statistics GetStats();
        private:
            static void FlushAndReset();
    };

}

#include "LightingLayer.h"
#include "..\Gui\EditorGUI.h"

namespace Lighting
{
    Lighting::Lighting(EditorCamera& eCam)
        : m_EditorCamera(eCam)
    {
        m_CubeShader = Shader::Create("Assets/Shaders/Cube Shader.glsl");
        m_LightingShader = Shader::Create("Assets/Shaders/Lighting.glsl");
        m_LampShader = Shader::Create("Assets/Shaders/Lamp.glsl");

        m_CubeVA = VertexArray::Create();

        float vertices[] =
        {
            // Positions            // Normals              // Texture Coords
            -0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,     0.0f,  0.0f,
            0.5f, -0.5f, -0.5f,     0.0f,  0.0f, -1.0f,     1.0f,  0.0f,
            0.5f,  0.5f, -0.5f,     0.0f,  0.0f, -1.0f,     1.0f,  1.0f,
            0.5f,  0.5f, -0.5f,     0.0f,  0.0f, -1.0f,     1.0f,  1.0f,
            -0.5f,  0.5f, -0.5f,    0.0f,  0.0f, -1.0f,     0.0f,  1.0f,
            -0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,     0.0f,  0.0f,

            -0.5f, -0.5f,  0.5f,    0.0f,  0.0f,  1.0f,     0.0f,  0.0f,
            0.5f, -0.5f,  0.5f,     0.0f,  0.0f,  1.0f,     1.0f,  0.0f,
            0.5f,  0.5f,  0.5f,     0.0f,  0.0f,  1.0f,     1.0f,  1.0f,
            0.5f,  0.5f,  0.5f,     0.0f,  0.0f,  1.0f,  	1.0f,  1.0f,
            -0.5f,  0.5f,  0.5f,    0.0f,  0.0f,  1.0f,     0.0f,  1.0f,
            -0.5f, -0.5f,  0.5f,    0.0f,  0.0f,  1.0f,     0.0f,  0.0f,

            -0.5f,  0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,    1.0f,  0.0f,
            -0.5f,  0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,    1.0f,  1.0f,
            -0.5f, -0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,    0.0f,  1.0f,
            -0.5f, -0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,    0.0f,  1.0f,
            -0.5f, -0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,    0.0f,  0.0f,
            -0.5f,  0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,    1.0f,  0.0f,

            0.5f,  0.5f,  0.5f,     1.0f,  0.0f,  0.0f,     1.0f,  0.0f,
            0.5f,  0.5f, -0.5f,     1.0f,  0.0f,  0.0f,     1.0f,  1.0f,
            0.5f, -0.5f, -0.5f,     1.0f,  0.0f,  0.0f,     0.0f,  1.0f,
            0.5f, -0.5f, -0.5f,     1.0f,  0.0f,  0.0f,     0.0f,  1.0f,
            0.5f, -0.5f,  0.5f,     1.0f,  0.0f,  0.0f,     0.0f,  0.0f,
            0.5f,  0.5f,  0.5f,     1.0f,  0.0f,  0.0f,     1.0f,  0.0f,

            -0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,     0.0f,  1.0f,
            0.5f, -0.5f, -0.5f,     0.0f, -1.0f,  0.0f,     1.0f,  1.0f,
            0.5f, -0.5f,  0.5f,     0.0f, -1.0f,  0.0f,     1.0f,  0.0f,
            0.5f, -0.5f,  0.5f,     0.0f, -1.0f,  0.0f,     1.0f,  0.0f,
            -0.5f, -0.5f,  0.5f,    0.0f, -1.0f,  0.0f,     0.0f,  0.0f,
            -0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,     0.0f,  1.0f,

            -0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,     0.0f,  1.0f,
            0.5f,  0.5f, -0.5f,     0.0f,  1.0f,  0.0f,     1.0f,  1.0f,
            0.5f,  0.5f,  0.5f,     0.0f,  1.0f,  0.0f,     1.0f,  0.0f,
            0.5f,  0.5f,  0.5f,     0.0f,  1.0f,  0.0f,     1.0f,  0.0f,
            -0.5f,  0.5f,  0.5f,    0.0f,  1.0f,  0.0f,     0.0f,  0.0f,
            -0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,     0.0f,  1.0f
        };

        const Ref<VertexBuffer> cubeVB = VertexBuffer::Create(vertices, sizeof(vertices));

        cubeVB->SetLayout({
            { ShaderDataType::Vector3, "a_Position" },
            { ShaderDataType::Vector3, "a_Normal" },
            { ShaderDataType::Vector2, "a_TexCoords" }
        });

        m_CubeVA->AddVertexBuffer(cubeVB);

        uint32_t squareIndices[36];
        for (uint32_t i = 0; i < 36; i++)
            squareIndices[i] = i;

        const Ref<IndexBuffer> cubeIB = IndexBuffer::Create(squareIndices, 36);

        m_CubeVA->SetIndexBuffer(cubeIB);

        m_LampVA = VertexArray::Create();
        m_LampVA->AddVertexBuffer(cubeVB);
        m_LampVA->SetIndexBuffer(cubeIB);

        m_ViewPosition = UniformBuffer::Create(sizeof(Vector3), 2);
        m_MaterialUB = UniformBuffer::Create(sizeof(Material), 3);
        m_LightUB = UniformBuffer::Create(sizeof(Light), 4);

        m_Container = AssetsManager::GetTexture("Textures\\Container.png");
        m_ContainerSpecular = AssetsManager::GetTexture("Textures\\Container_Specular.png");
    }

    Vector3 cubePositions[] = {
        Vector3(0.0f,   0.0f,   0.0f),
        Vector3(2.0f,   5.0f,   -15.0f),
        Vector3(-1.5f,  -2.2f,  -2.5f),
        Vector3(-3.8f,  -2.0f,  -12.3f),
        Vector3(2.4f,   -0.4f,  -3.5f),
        Vector3(-1.7f,  3.0f,   -7.5f),
        Vector3(1.3f,   -2.0f,  -2.5f),
        Vector3(1.5f,   2.0f,   -2.5f),
        Vector3(1.5f,   0.2f,   -1.5f),
        Vector3(-1.3f,  1.0f,   -1.5f)
    };

    void Lighting::OnUpdate(TimeStep ts)
    {
        Renderer::BeginScene(m_EditorCamera.GetViewProjection());

        m_ViewPosition->SetData(glm::value_ptr(Vector3{ m_EditorCamera.GetPosition() }), sizeof(Vector3));

        m_Light.Position = Vector4(m_LightPosition, 1.0f);

        m_Light.Ambient = Vector4{ 0.2f };
        m_Light.Diffuse = Vector4{ 0.5f };
        m_Light.Specular = Vector4{ 1.0f };

        m_LightUB->SetData(&m_Light, sizeof(Light));

        m_Material.Shininess = 32.0f;

        m_Container->Bind(0);
        m_ContainerSpecular->Bind(1);

        m_MaterialUB->SetData(&m_Material, sizeof(Material));

        for (uint32_t i = 0; i < 10; i++)
        {
            m_Transform = glm::translate(Matrix4(1.0f), cubePositions[i]) * glm::rotate(Matrix4(1.0f), 20.0f * i, {1.0f, 0.3f, 0.5f});
            Renderer::Submit(m_LightingShader, m_CubeVA, m_Transform);
        }

        Renderer::EndScene();
    }

	void Lighting::OnImGuiRender()
	{
        if (ImGui::Begin("Lighting"))
        {
            EditorGUI::InputVector3("Light Position", m_LightPosition);

            ImGui::End();
        } else ImGui::End();
	}

    Lighting::~Lighting()
    {
    }
}

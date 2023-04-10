#include "EditorLayer.h"

#include "Quelos/Scene/SceneSerializer.h"
#include "Quelos/Utiles/PlatformUtils.h"
#include "Quelos/Core/Application.h"
#include "Quelos/Scripting/ScriptEngine.h"

#include "Gui/EditorGUI.h"

#include <ImGuizmo.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Quelos
{
    extern std::filesystem::path g_AssetsPath;

    EditorLayer::EditorLayer()
        : Layer("SandboxLayer"), m_HierarchyPanel(nullptr, m_InspectorPanel)
    {
        m_ViewportBounds[0] = { 0, 0 };
        m_ViewportBounds[1] = { 0, 0 };

        m_ViewportSize = { 0, 0 };
    }

    void EditorLayer::OnAttach()
    {
        QS_PROFILE_FUNCTION();

        m_IconPlay = Texture2D::Create("Resources/Icons/PlayButton.png");
        m_IconPause = Texture2D::Create("Resources/Icons/PauseButton.png");
        m_IconStop = Texture2D::Create("Resources/Icons/StopButton.png");
        m_IconStep = Texture2D::Create("Resources/Icons/StepButton.png");
        m_IconSimStart = Texture2D::Create("Resources/Icons/SimulationButton.png");
        m_IconSimStop = Texture2D::Create("Resources/Icons/StopSimulationButton.png");

        NewScene();

        FrameBufferSpecification fbSpec;
        fbSpec.Attachments = { FrameBufferTextureFormat::RGBA8,FrameBufferTextureFormat::RED_INT, FrameBufferTextureFormat::Depth };
        fbSpec.Width = 1280;
        fbSpec.Height = 720;
        m_FrameBuffer = FrameBuffer::Create(fbSpec);

        auto commandLineArgs = Application::Get().GetSpecification().CommandLineArgs;
        if (commandLineArgs.Count > 1)
        {
            auto projFilepath = commandLineArgs[1];
            OpenProject(projFilepath);
        }
        else
        {
            // TODO: Propmt user to select directory
            NewProject();
        }

        m_EditorCamera = EditorCamera(30.0f, 1.778f, 0.1f, 1000.0f);
        
        m_SpriteEditorWindow.DisplayTexture(m_Container);
    }

    void EditorLayer::OnUpdate(TimeStep ts)
    {
        QS_PROFILE_FUNCTION();

        //Update

        if (const FrameBufferSpecification spec = m_FrameBuffer->GetSpecification();
            m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f &&
            (spec.Width != m_ViewportSize.x || spec.Height != m_ViewportSize.y))
        {
            m_FrameBuffer->Resize(static_cast<uint32_t>(m_ViewportSize.x), static_cast<uint32_t>(m_ViewportSize.y));
            m_ActiveScene->OnViewportResize(static_cast<uint32_t>(m_ViewportSize.x), static_cast<uint32_t>(m_ViewportSize.y));
        }

        //Render
        Renderer2D::ResetStats();
        
        m_FrameBuffer->Bind();

        RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
        RenderCommand::Clear();

        // Clear The EntityID Attachment to -1.
        m_FrameBuffer->ClearColorAttachment(1, -1);

        switch (m_SceneState)
        {
            case SceneState::Edit:
            {
                if (m_ViewportFocused)
                    m_EditorCamera.OnUpdate(ts);

                m_ActiveScene->OnUpdateEditor(ts);
                m_ActiveScene->OnRenderEditor(m_EditorCamera);
                break;
            }
            case SceneState::Simulate:
            {
                if (m_ViewportFocused)
                    m_EditorCamera.OnUpdate(ts);

                if (!m_ScenePaused || m_SceneStep)
                    m_ActiveScene->OnUpdateSimulation(ts);
                
                m_ActiveScene->OnRenderSimulation(m_EditorCamera);
                break;
            }
            case SceneState::Play:
            {
                if (!m_ScenePaused || m_SceneStep)
                    m_ActiveScene->OnUpdateRuntime(ts);
                
                m_ActiveScene->OnRenderRuntime();
                break;
            }
        }

        m_SceneStep = false;

        auto [mx, my] = ImGui::GetMousePos();
        mx -= m_ViewportBounds[0].x;
        my -= m_ViewportBounds[0].y;
        my = m_ViewportSize.y - my;

        const int mouseX = static_cast<int>(mx);
        const int mouseY = static_cast<int>(my);

        if (mouseX >= 0 && mouseY >= 0 && mouseX < static_cast<int>(m_ViewportSize.x) && mouseY < static_cast<int>(m_ViewportSize.y))
        {
            int pixelData = m_FrameBuffer->ReadPixel(1, mouseX, mouseY);
            m_HoveredEntity = pixelData == -1 ?  Entity() : Entity(static_cast<entt::entity>(pixelData), m_ActiveScene.get());
        }

        //m_Lighting.OnUpdate(ts);

        OnOverlayRender();

        m_FrameBuffer->Unbind();

        const bool control = Input::GetKey(KeyCode::LeftControl) || Input::GetKey(KeyCode::RightControl);
        const bool shift = Input::GetKey(KeyCode::LeftShift) || Input::GetKey(KeyCode::RightShift);

        m_SpriteEditorWindow.OnUpdate(ts);
    }

    void EditorLayer::OnImGuiRender()
    {
        // If you strip some features of, this demo is pretty much equivalent to calling DockSpaceOverViewport()!
        // In most cases you should be able to just call DockSpaceOverViewport() and ignore all the code below!
        // In this specific demo, we are not using DockSpaceOverViewport() because:
        // - we allow the host window to be floating/moveable instead of filling the viewport (when opt_fullscreen == false)
        // - we allow the host window to have padding (when opt_padding == true)
        // - we have a local menu bar in the host window (vs. you could use BeginMainMenuBar() + DockSpaceOverViewport() in your code!)
        // TL;DR; this demo is more complicated than what you would normally use.
        // If we removed all the options we are showcasing, this demo would become:
        //     void ShowExampleAppDockSpace()
        //     {
        //         ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
        //     }

        static bool dockspaceOpen = true;
        static bool opt_fullscreen = true;
        static bool opt_padding = false;
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

        // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
        // because it would be confusing to have two docking targets within each others.
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        if (opt_fullscreen)
        {
            const ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->WorkPos);
            ImGui::SetNextWindowSize(viewport->WorkSize);
            ImGui::SetNextWindowViewport(viewport->ID);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
            window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        }
        else
        {
            dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
        }

        // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
        // and handle the pass-thru hole, so we ask Begin() to not render a background.
        if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
            window_flags |= ImGuiWindowFlags_NoBackground;

        // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
        // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
        // all active windows docked into it will lose their parent and become undocked.
        // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
        // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
        if (!opt_padding)
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("DockSpace", &dockspaceOpen, window_flags);
        if (!opt_padding)
            ImGui::PopStyleVar();

        if (opt_fullscreen)
            ImGui::PopStyleVar(2);

        // Submit the DockSpace
        ImGuiIO& io = ImGui::GetIO();
        ImGuiStyle& style = ImGui::GetStyle();
        const float minWinSize = style.WindowMinSize.x;
        style.WindowMinSize.x = 370.0f;
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
        {
	        const ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
        }

        style.WindowMinSize.x = minWinSize;
        static bool show_app_style_editor = false;
        static bool open = false;
        static bool show_console = true;
        static bool showViewport = true;

        if (show_app_style_editor)
        {
            ImGui::Begin("StyleEditor", &show_app_style_editor);
            ImGui::ShowStyleEditor();
            ImGui::End();
        }

        if (show_console)
            m_ConsolePanel.Draw("Console", &show_console);

        if (Input::GetKeyDown(KeyCode::Space))
            m_ConsolePanel.AddLog("# This is an error");

        static bool show_settings = true;

        if (show_settings)
        {
            if (!ImGui::Begin("Settings", &show_settings))
                ImGui::End();
            else
            {
                EditorGUI::CheckBox("Show Physics Colliders", m_ShowPhysicsColliders);
                ImGui::End();
            }
        }

        if (open)
            ImGui::ShowDemoWindow(&open);

        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Open Project...", "Ctrl+O"))
                    OpenProject();

                ImGui::Separator();

                if (ImGui::MenuItem("New Scene", "Ctrl+N"))
                    NewScene();

                if (ImGui::MenuItem("Save Scene", "Ctrl+S"))
                    SaveScene();

                if (ImGui::MenuItem("Save Scene as...", "Ctrl+Shift+S"))
                    SaveSceneAs();

                ImGui::Separator();

                if (ImGui::MenuItem("Close")) { Application::Close(); }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Editor"))
            {
                ImGui::MenuItem("Style Editor", nullptr, &show_app_style_editor);
                ImGui::MenuItem("ImGui Demo Window", nullptr, &open);
                if (ImGui::BeginMenu("Windows"))
                {
                    ImGui::MenuItem("Settings", nullptr, &show_settings);
                    ImGui::MenuItem("Viewport", nullptr, &showViewport);
                    ImGui::EndMenu();
                }

                ImGui::Separator();

                if (ImGui::MenuItem("Reload Mono Assembly", "Ctrl+R"))
                {
                    if (m_SceneState == SceneState::Edit)
                        ScriptEngine::ReloadAssembly();
                }

                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }

        m_HierarchyPanel.OnImGuiRender();
        m_InspectorPanel.OnImGuiRender();
        m_ContentBrowserPanel.OnImGuiRender();
        m_SpriteEditorWindow.OnImGuiRender();
        //m_Lighting.OnImGuiRender();

        if (showViewport)
        {
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.0f, 0.0f });
            if (!ImGui::Begin("Viewport", &showViewport))
            {
                ImGui::PopStyleVar(1);
                ImGui::End();
            }
            else
            {
                const auto viewportOffset = ImGui::GetCursorPos(); // Includes the tab bar

                ImVec2 viewPortPanelSize = ImGui::GetContentRegionAvail();

                if (m_ViewportSize != *reinterpret_cast<Vector2*>(&viewPortPanelSize))
                {
                    m_ViewportSize = { viewPortPanelSize.x, viewPortPanelSize.y };
                    m_FrameBuffer->Resize(static_cast<uint32_t>(m_ViewportSize.x), static_cast<uint32_t>(m_ViewportSize.y));
                    m_EditorCamera.SetViewportSize(m_ViewportSize.x, m_ViewportSize.y);
                    m_ActiveScene->OnViewportResize(static_cast<uint32_t>(m_ViewportSize.x), static_cast<uint32_t>(m_ViewportSize.y));
                }

                ImGui::Image(reinterpret_cast<void*>(static_cast<uint64_t>(m_FrameBuffer->GetColorAttachmentRendererID())),
                             { m_ViewportSize.x, m_ViewportSize.y }, { 0.0f, 1.0f }, { 1.0f, 0.0f });

                const auto windowSize = ImGui::GetContentRegionAvail();
                ImVec2 minBound = ImGui::GetWindowPos();
                minBound.x += viewportOffset.x;
                minBound.y += viewportOffset.y;

                ImVec2 maxBound = { minBound.x + windowSize.x, minBound.y + windowSize.y };
                m_ViewportBounds[0] = { minBound.x, minBound.y };
                m_ViewportBounds[1] = { maxBound.x, maxBound.y };

                m_ViewportFocused = ImGui::IsWindowFocused();
                m_ViewportHovered = ImGui::IsWindowHovered();

                if (ImGui::BeginDragDropTarget())
                {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
                    {
                        const wchar_t* wpath = static_cast<const wchar_t*>(payload->Data);
                        const std::filesystem::path path(wpath);

                        if (path.extension() == ".quelos")
                            OpenScene(Project::GetAssetFileSystemPath(path));
                        else if (path.extension() == ".png" || path.extension() == ".jpg")
                        {
                            Entity e = m_ActiveScene->CreateEntity(path.stem().string());
                            auto& src = e.AddComponent<SpriteRendererComponent>();
                            src.Texture = CreateRef<SubTexture2D>(AssetsManager::GetTexture(path.string()));
                        }

                        ImGui::EndDragDropTarget();
                    }
                }

                // Gizmos

                Entity selectedEntity = m_InspectorPanel.GetSelectedEntity();
                if (selectedEntity && m_GizmoType != -1)
                {
                    ImGuizmo::SetOrthographic(false);
                    ImGuizmo::SetDrawlist();

                    //Editor Camera
                    const float windowWidth = ImGui::GetWindowWidth();
                    const float windowHeight = ImGui::GetWindowHeight();

                    const Matrix4& cameraProjection = m_EditorCamera.GetProjection();
                    Matrix4 cameraView = m_EditorCamera.GetViewMatrix();

                    ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);

                    // Entity transform
                    auto& tc = selectedEntity.GetComponent<TransformComponent>();
                    Matrix4 transform = tc.GetTransformMatrix();

                    const bool snap = Input::GetKey(KeyCode::LeftControl);
                    float snapValue = 0.5f;
                    if (m_GizmoType == ImGuizmo::OPERATION::ROTATE)
                        snapValue = 45.0f;

                    float snapValues[3]{ snapValue, snapValue, snapValue };

                    ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection), static_cast<ImGuizmo::OPERATION>(m_GizmoType),
                        ImGuizmo::LOCAL, glm::value_ptr(transform),nullptr, snap ? snapValues : nullptr);
                    
                    if (ImGuizmo::IsUsing())
                    {
                        Vector3 position, rotation, scale;
                        Math::DecomposeTransform(transform, position, rotation, scale);

                        const Vector3 deltaRotation = rotation - tc.Rotation;
                        tc.Position = position;
                        tc.Rotation += deltaRotation;
                        tc.Scale = scale;
                    }
                }

                ImGui::End();
                ImGui::PopStyleVar();
            }
        }

        if (!ImGui::Begin("Stats"))
            ImGui::End();
        else
        {
            //ImGui::Text("Light:");
            //EditorGUI::InputVector3("   Position", m_LightPosition);
            //EditorGUI::InputColor4("Object Color: ", m_Material.Ambient);

            //std::string name = "None";
            //if (m_HoveredEntity)
            //    name = m_HoveredEntity.GetComponent<TagComponent>().Tag;
            //ImGui::Text("\nHovered Entity: %s", name.c_str());

            auto stats = Renderer2D::GetStats();
            ImGui::Text("Renderer2D Statistics:");
            ImGui::Text("   Draw Calls: %d", stats.DrawCalls);
            ImGui::Text("   Quad Count: %d", stats.QuadCount);
            ImGui::Text("   Textures Count: %d", stats.Textures);
            ImGui::Text("   Vertices: %d", stats.GetTotalVertexCount());
            ImGui::Text("   Indices: %d", stats.GetTotalIndexCount());

            if (ImGui::Button("Reload Renderer"))
                Renderer2D::Reload();

            ImGui::End();
        }

        UI_Toolbar();

        ImGui::End();
    }

    void EditorLayer::UI_Toolbar()
    {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));

        const auto& colors = ImGui::GetStyle().Colors;
        const ImVec4 hovered = colors[ImGuiCol_ButtonHovered];
        const ImVec4 active = colors[ImGuiCol_ButtonActive];

        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(hovered.x, hovered.y, hovered.z, 0.5f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(active.x, active.y, active.z, 0.5f));

        if (ImGui::Begin("##toolbar", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar
                         | ImGuiWindowFlags_NoScrollWithMouse))
        {
            float size = ImGui::GetWindowHeight() - 4.0f;

            Ref<Texture2D> icon = m_SceneState == SceneState::Edit || m_SceneState == SceneState::Simulate ? m_IconPlay : m_IconStop;
            ImGui::SetCursorPosX((ImGui::GetContentRegionMax().x * 0.5f) - size * 2.0f);
            if (ImGui::ImageButton((ImTextureID)(uint64_t)icon->GetRendererID(), { size, size }, { 0, 1 }, {1, 0}, 0))
            {
                if (m_SceneState == SceneState::Edit || m_SceneState == SceneState::Simulate)
                    OnScenePlay();
                else if (m_SceneState == SceneState::Play)
                    OnSceneStop();
            }

            ImGui::SameLine();

            icon = m_SceneState == SceneState::Edit || m_SceneState == SceneState::Play ? m_IconSimStart : m_IconSimStop;
            if (ImGui::ImageButton((ImTextureID)(uint64_t)icon->GetRendererID(), { size, size }, { 0, 1 }, {1, 0}, 0))
            {
                if (m_SceneState == SceneState::Edit || m_SceneState == SceneState::Play)
                    OnSimulationStart();
                else if (m_SceneState == SceneState::Simulate)
                    OnSceneStop();
            }

            ImGui::SameLine();

            if (ImGui::ImageButton((ImTextureID)(uint64_t)m_IconPause->GetRendererID(), { size, size }, { 0, 1 }, { 1, 0 }, 0,
                m_ScenePaused ? ImVec4{ 0.5f, 0.5f, 0.5f, 1.0f } : ImVec4{0, 0, 0, 0}))
                m_ScenePaused = !m_ScenePaused;

            ImGui::SameLine();

            if (ImGui::ImageButton((ImTextureID)(uint64_t)m_IconStep->GetRendererID(), { size, size }, { 0, 1 }, { 1, 0 }, 0))
            {
                if (!m_ScenePaused)
                    m_ScenePaused = true;
                m_SceneStep = true;
            }

            ImGui::End();
        }
        else ImGui::End();

        ImGui::PopStyleVar(2);
        ImGui::PopStyleColor(3);
    }

    void EditorLayer::OnScenePlay()
    {
        if (m_SceneState == SceneState::Simulate)
            OnSceneStop();

        m_SceneState = SceneState::Play;
        m_ActiveScene = Scene::Copy(m_EditorScene);
        m_ActiveScene->OnRuntimeStart();
        m_HierarchyPanel.SetContext(m_ActiveScene);
    }

    void EditorLayer::OnSceneStop()
    {
        QS_CORE_ASSERT(m_SceneState == SceneState::Simulate || m_SceneState == SceneState::Play,
            "EditorLayer::OnSceneStop was called while the scene state wasn't Play or Simulate");

        if (m_SceneState == SceneState::Play)
            m_ActiveScene->OnRuntimeStop();
        else m_ActiveScene->OnSimulationStop();

        m_SceneState = SceneState::Edit;
        m_ActiveScene = m_EditorScene;
        m_ScenePaused = false;
        m_HierarchyPanel.SetContext(m_ActiveScene);
    }

    void EditorLayer::OnScenePause()
    {

    }

    void EditorLayer::OnSimulationStart()
    {
        if (m_SceneState == SceneState::Play)
            OnSceneStop();

        m_SceneState = SceneState::Simulate;
        m_ActiveScene = Scene::Copy(m_EditorScene);
        m_ActiveScene->OnSimulationStart();
        m_HierarchyPanel.SetContext(m_ActiveScene);
    }

    void EditorLayer::OnDuplicateEntity()
    {
        if (m_SceneState != SceneState::Edit)
            return;

        const Entity selectedEntity = m_InspectorPanel.GetSelectedEntity();
        if (selectedEntity)
            m_InspectorPanel.SetSelectedEntity(m_EditorScene->DuplicateEntity(selectedEntity));
    }

    void EditorLayer::OnOverlayRender()
    {
        if (m_SceneState == SceneState::Play)
        {
            Entity cam = m_ActiveScene->GetPrimaryCamera();
            if (!cam)
                return;
            
            Renderer2D::BeginScene(cam.GetComponent<CameraComponent>().Camera, cam.GetComponent<TransformComponent>().GetTransformMatrix());
        }
        else Renderer2D::BeginScene(m_EditorCamera);

        if (m_ShowPhysicsColliders)
        {
            m_ActiveScene->GetAllEntitiesWith<TransformComponent, BoxCollider2DComponent>()
                .each([&](auto entity, TransformComponent& tc, BoxCollider2DComponent bc)
                {
	                const Vector3 pos = tc.Position + Vector3(bc.Offset.x, bc.Offset.y, 0.001f);
	                const Vector3 scale = tc.Scale * Vector3(bc.Size.x, bc.Size.y, 1.0f) * 2.0f;

	                const Matrix4 transform = glm::translate(Matrix4(1.0f), pos)
                        * glm::rotate(Matrix4(1.0f), tc.Rotation.z, Vector3(0.0f, 0.0f, 1.0f))
                        * glm::scale(Matrix4(1.0f), scale);
                    Renderer2D::DrawRect(transform, { 0.0f, 1.0f, 0.0f, 1.0f });
                });

            m_ActiveScene->GetAllEntitiesWith<TransformComponent, CircleCollider2DComponent>()
                .each([&](auto entity, TransformComponent& tc, CircleCollider2DComponent cc)
                {
	                const Vector3 pos = tc.Position + Vector3(cc.Offset.x, cc.Offset.y, 0.001f);
	                const Vector3 scale = tc.Scale * cc.Radius * 2.0f;

	                const Matrix4 transform = glm::translate(Matrix4(1.0f), pos) * glm::scale(Matrix4(1.0f), scale);
                    Renderer2D::DrawCircle(transform, { 0.0f, 1.0f, 0.0f, 1.0f }, 0.01f);

                });
        }

        Entity selectedEntity = m_InspectorPanel.GetSelectedEntity();
        if (selectedEntity)
        {
            TransformComponent tc = selectedEntity.GetComponent<TransformComponent>();

            const Vector3 pos = tc.Position + Vector3(0.0f, 0.0f, 0.001f);
            const Vector3 scale = tc.Scale;

            const Matrix4 transform = glm::translate(Matrix4(1.0f), pos) * glm::rotate(Matrix4(1.0f), tc.Rotation.z, Vector3(0.0f, 0.0f, 1.0f)) * Math::Scale(scale);
            Renderer2D::DrawRect(transform, { 1.0f, 0.5f, 0.0f, 1.0f }, selectedEntity);
        }

        Renderer2D::EndScene();
    }

    void EditorLayer::OnEvent(Event& e)
    {
        if ((m_SceneState == SceneState::Edit || m_SceneState == SceneState::Simulate) && m_ViewportHovered)
            m_EditorCamera.OnEvent(e);

        EventDispatcher dispatcher(e);

        dispatcher.Dispatch<KeyPressedEvent>([&](KeyPressedEvent e)
        {
            // Scene Commands
            if (e.IsRepeat())
                return false;

            const bool control = Input::GetKey(KeyCode::LeftControl) || Input::GetKey(KeyCode::RightControl);
            const bool shift = Input::GetKey(KeyCode::LeftShift) || Input::GetKey(KeyCode::RightShift);

            switch (e.GetKeyCode())
            {
                case KeyCode::O:
                {
                    if (control)
                        OpenProject();
                    break;
                }
                case KeyCode::S:
                {
                    if (control)
                    {
                        if (shift)
                            SaveSceneAs();
                        else SaveScene();
                    }
                    break;
                }
                case KeyCode::N:
                {
                    if (control)
                        NewScene();
                    break;
                }
            }

            if (!m_ViewportFocused && !m_ViewportHovered)
                return false;

            switch (e.GetKeyCode())
            {
                case KeyCode::D:
                {
                    if (control)
                        OnDuplicateEntity();
                    break;
                }
                case KeyCode::Q:
                {
                    if (!ImGuizmo::IsUsing())
                        m_GizmoType = -1;

                    break;
                }
                case KeyCode::W:
                {
                    if (!ImGuizmo::IsUsing())
                        m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;

                    break;
                }
                case KeyCode::E:
                {
                    if (!ImGuizmo::IsUsing())
                        m_GizmoType = ImGuizmo::OPERATION::ROTATE;

                    break;
                }
                case KeyCode::R:
                {
                    if (control && m_SceneState == SceneState::Edit)
                        ScriptEngine::ReloadAssembly();
                    else if (!ImGuizmo::IsUsing())
                        m_GizmoType = ImGuizmo::OPERATION::SCALE;

                    break;
                }
                case KeyCode::T:
                {
                    if (!ImGuizmo::IsUsing())
                        m_GizmoType = ImGuizmo::OPERATION::BOUNDS;

                    break;
                }
                default:
                    break;
            }

            return false;
        });

        dispatcher.Dispatch<MouseButtonPressedEvent>([&](MouseButtonPressedEvent e)
        {
            if (e.GetMouseButton() == MouseButtonCode::Left)
            {
                if (m_ViewportHovered && !ImGuizmo::IsOver() && !Input::GetKey(KeyCode::LeftControl))
                    m_InspectorPanel.SetSelectedEntity(m_HoveredEntity);
            }

            return false;
        });
    }

    void EditorLayer::NewScene()
    {
        m_EditorScene = CreateRef<Scene>();
        m_ActiveScene = m_EditorScene;
        m_ActiveScene->OnViewportResize(static_cast<uint32_t>(m_ViewportSize.x), static_cast<uint32_t>(m_ViewportSize.y));
        m_HierarchyPanel.SetContext(m_ActiveScene);

        m_EditorScenePath = std::filesystem::path();
    }

    void EditorLayer::OpenScene()
    {
	    const std::string filePath = FileDialogs::OpenFile("Quelos Scene (*.quelos)\0*.quelos\0");
        OpenScene(filePath);
    }

    void EditorLayer::OpenScene(const std::filesystem::path& path)
    {
        if (m_SceneState != SceneState::Edit)
            OnSceneStop();

        if (path.extension() != ".quelos")
            return;

        if (!path.empty())
        {
	        const Ref<Scene> newScene = CreateRef<Scene>();
            SceneSerializer serializer(newScene);
            if (serializer.Deserialize(path.string()))
            {
                m_EditorScene = newScene;

                m_EditorScene->OnViewportResize(static_cast<uint32_t>(m_ViewportSize.x), static_cast<uint32_t>(m_ViewportSize.y));
                m_HierarchyPanel.SetContext(m_EditorScene);

                m_ActiveScene = m_EditorScene;
                m_EditorScenePath = path;
            }
        }
    }
    void EditorLayer::SaveSceneAs()
    {
	    const std::string filePath = FileDialogs::SaveFile("Quelos Scene (*.Quelos)\0*.Quelos\0");
        if (!filePath.empty())
        {
            SerializeScene(m_ActiveScene, filePath);

            m_EditorScenePath = filePath;
        }
    }

    void EditorLayer::SaveScene()
    {
        if (!m_EditorScenePath.empty())
            SerializeScene(m_ActiveScene, m_EditorScenePath);
        else SaveSceneAs();
    }

    void EditorLayer::NewProject()
    {
        Project::New();
    }

    void EditorLayer::OpenProject()
    {
        const std::string projectPath = FileDialogs::OpenFile("Quelos Scene (*.qproj)\0*.qproj\0");
        OpenProject(projectPath);
    }

    void EditorLayer::OpenProject(const std::filesystem::path& path)
    {
        if (path.empty())
            return;

        if (Ref<Project> project = Project::Load(path))
        {
            std::filesystem::path assetsPath = Project::GetAssetDirectory();
            AssetsManager::Init(assetsPath);
            m_ContentBrowserPanel.SetAssetsPath(assetsPath);

            std::filesystem::path scenePath = Project::GetAssetFileSystemPath(Project::GetActiveProject()->GetConfig().StartScene);
            OpenScene(scenePath);
            Application::GetWindow().SetTitle("Quelos Editor - " + path.stem().string() + " - " + scenePath.stem().string());
        }
    }

    void EditorLayer::SaveProject()
    {
        // Project::SaveActive();
    }

    void EditorLayer::SerializeScene(Ref<Scene> scene, const std::filesystem::path& path)
    {
        SceneSerializer serializer(m_ActiveScene);
        serializer.Serialize(m_EditorScenePath);
    }
}

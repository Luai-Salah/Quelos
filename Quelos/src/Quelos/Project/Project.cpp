#include "QSPCH.h"
#include "Project.h"

#include <Quelos/Project/ProjectSerializer.h>

namespace Quelos
{
    Ref<Project> Project::New()
    {
        s_ActiveProject = CreateRef<Project>();
        return s_ActiveProject;
    }

    Ref<Project> Project::Load(const std::filesystem::path& path)
    {
        Ref<Project> project = CreateRef<Project>();

        ProjectSerializer serializer(project);
        if (serializer.Deserialize(path))
        {
            project->m_ActiveProjectDirectory = path.parent_path();
            s_ActiveProject = project;
            return s_ActiveProject;
        }

        return nullptr;
    }

    bool Project::SaveActive(const std::filesystem::path& path)
    {
        ProjectSerializer serializer(s_ActiveProject);
        if (serializer.Serialize(path))
            return false;

        s_ActiveProject->m_ActiveProjectDirectory = path.parent_path();
        return true;
    }

    std::filesystem::path Project::GetAssetDirectory()
    {
       return m_ActiveProjectDirectory / m_Configuration.AssetDirectory;
    }
}

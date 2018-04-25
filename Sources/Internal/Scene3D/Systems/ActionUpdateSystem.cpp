#include "Scene3D/Systems/ActionUpdateSystem.h"

#include "Debug/ProfilerCPU.h"
#include "Debug/ProfilerMarkerNames.h"
#include "Reflection/ReflectionRegistrator.h"
#include "Scene3D/Entity.h"
#include "Scene3D/Scene.h"
#include "Scene3D/Systems/EventSystem.h"
#include "Time/SystemTimer.h"

namespace DAVA
{
DAVA_VIRTUAL_REFLECTION_IMPL(ActionUpdateSystem)
{
    ReflectionRegistrator<ActionUpdateSystem>::Begin()[M::SystemTags("base")]
    .ConstructorByPointer<Scene*>()
    .Method("ProcessFixed", &ActionUpdateSystem::ProcessFixed)[M::SystemProcessInfo(SPI::Group::EngineEnd, SPI::Type::Fixed, 1.01f)]
    .End();
}

ActionUpdateSystem::ActionUpdateSystem(Scene* scene)
    : SceneSystem(scene, ComponentUtils::MakeMask<ActionComponent>())
{
    UnblockAllEvents();
}

void ActionUpdateSystem::SetBlockEvent(ActionComponent::Action::eEvent eventType, bool block)
{
    eventBlocked[eventType] = block;
}

bool ActionUpdateSystem::IsBlockEvent(ActionComponent::Action::eEvent eventType)
{
    return eventBlocked[eventType];
}

void ActionUpdateSystem::UnblockAllEvents()
{
    for (int i = 0; i < ActionComponent::Action::EVENTS_COUNT; i++)
        eventBlocked[i] = false;
}

void ActionUpdateSystem::AddEntity(Entity* entity)
{
    SceneSystem::AddEntity(entity);
    ActionComponent* actionComponent = entity->GetComponent<ActionComponent>();

    DVASSERT(actionComponent != nullptr);

    actionComponent->StartAdd();
}

void ActionUpdateSystem::RemoveEntity(Entity* entity)
{
    ActionComponent* actionComponent = entity->GetComponent<ActionComponent>();

    DVASSERT(actionComponent != nullptr);

    if (actionComponent->IsStarted())
    {
        actionComponent->StopAll();
    }
    SceneSystem::RemoveEntity(entity);
}

void ActionUpdateSystem::PrepareForRemove()
{
    activeActions.clear();
    deleteActions.clear();
}

void ActionUpdateSystem::ProcessFixed(float32 timeElapsed)
{
    DAVA_PROFILER_CPU_SCOPE(ProfilerCPUMarkerName::SCENE_ACTION_UPDATE_SYSTEM);

    DelayedDeleteActions();

    uint32 size = static_cast<uint32>(activeActions.size());
    for (uint32 index = 0; index < size; ++index)
    {
        ActionComponent* component = activeActions[index];
        component->Update(timeElapsed);
    }
}

void ActionUpdateSystem::DelayedDeleteActions()
{
    Vector<ActionComponent*>::iterator end = deleteActions.end();
    for (Vector<ActionComponent*>::iterator it = deleteActions.begin(); it != end; ++it)
    {
        Vector<ActionComponent*>::iterator i = std::find(activeActions.begin(), activeActions.end(), *it);

        if (i != activeActions.end())
        {
            activeActions.erase(i);
        }
    }

    deleteActions.clear();
}

void ActionUpdateSystem::Watch(ActionComponent* component)
{
    activeActions.push_back(component);
}

void ActionUpdateSystem::UnWatch(ActionComponent* component)
{
    deleteActions.push_back(component);
}
}
#pragma once

#include "Base/BaseTypes.h"
#include "Math/MathConstants.h"
#include "Math/Matrix4.h"
#include "Base/Singleton.h"
#include "Entity/SceneSystem.h"
#include "Engine/EngineSettings.h"

namespace DAVA
{
class Entity;
class Transform;
class TransformComponent;

class TransformSystem : public SceneSystem
{
public:
    DAVA_VIRTUAL_REFLECTION(TransformSystem, SceneSystem);

    TransformSystem(Scene* scene);
    ~TransformSystem() override;

    void AddEntity(Entity* entity) override;
    void RemoveEntity(Entity* entity) override;
    void UnregisterEntity(Entity* entity) override;

    void PrepareForRemove() override;
    void Process(float32 timeElapsed) override;

    static const float32 systemOrder;

private:
    Vector<Entity*> updatableEntities;

    void EntityNeedUpdate(Entity* entity);
    void HierarchicAddToUpdate(Entity* entity);

    bool UpdateEntity(Entity* entity, bool forceUpdate = false);

    int32 passedNodes;
    int32 multipliedNodes;
    float32 timeElapsed;
};
}

#include "GameInputSystem.h"

#include "InputUtils.h"

#include "Base/FastName.h"
#include <Debug/ProfilerCPU.h>
#include "Scene3D/Scene.h"
#include "Scene3D/Components/CameraComponent.h"
#include "Scene3D/Components/TransformComponent.h"
#include "Systems/GameModeSystem.h"
#include "Systems/GameModeSystemCars.h"

#include "NetworkCore/Scene3D/Components/NetworkInputComponent.h"
#include "NetworkCore/Scene3D/Components/NetworkTransformComponent.h"
#include "NetworkCore/NetworkCoreUtils.h"

#include "Components/GameStunnableComponent.h"
#include "Components/HealthComponent.h"
#include "Components/SpeedModifierComponent.h"

#include <Physics/Vehicles/VehicleCarComponent.h>
#include <Physics/Core/PhysicsUtils.h>
#include <Physics/Controllers/CharacterControllerComponent.h>

#include <Reflection/ReflectionRegistrator.h>

using namespace DAVA;

DAVA_VIRTUAL_REFLECTION_IMPL(GameInputSystem)
{
    ReflectionRegistrator<GameInputSystem>::Begin()[M::SystemTags("gameinput")]
    .ConstructorByPointer<Scene*>()
    .Method("ProcessFixed", &GameInputSystem::ProcessFixed)[M::SystemProcessInfo(SPI::Group::Gameplay, SPI::Type::Fixed, 5.0f)]
    .End();
}

namespace GameInputSystemDetail
{
static const FastName UP("UP");
static const FastName DOWN("DOWN");
static const FastName LEFT("LEFT");
static const FastName RIGHT("RIGHT");

static const FastName CAM_UP("CAM_UP");
static const FastName CAM_DOWN("CAM_DOWN");
static const FastName CAM_FWD("CAM_FWD");
static const FastName CAM_BKWD("CAM_BKWD");
static const FastName CAM_LEFT("CAM_LEFT");
static const FastName CAM_RIGHT("CAM_RIGHT");

static const FastName LMOVE("LMOVE");
static const FastName RMOVE("RMOVE");

static const FastName TELEPORT("TELEPORT");

static const Vector3 MOV_SPEED(0.f, 10.f, 0.f);
static const float32 ROT_SPEED = 60.f * DEG_TO_RAD;

#ifndef SERVER
static const float32 CAM_MOVE_SPEED_MIN = 100.f;
static const float32 CAM_MOVE_SPEED_MAX = 500.f;
static const float32 CAM_MOVE_SPEED_FACTOR = 1.f;
#endif

template <typename T>
String GetName(const T&)
{
    return "Unknown";
}

template <>
String GetName(const Matrix4&)
{
    return "Matrix4";
}

template <>
String GetName(const Vector3&)
{
    return "Vector3";
}

template <>
String GetName(const Quaternion&)
{
    return "Quaternion";
}

template <typename T>
bool CompareTransform(const T& lhs, const T& rhs, uint32 size, float32 epsilon, uint32 frameId)
{
    for (uint32 i = 0; i < size; ++i)
    {
        if (!FLOAT_EQUAL_EPS(lhs.data[i], rhs.data[i], epsilon))
        {
            Logger::Debug("Transforms aren't equal, diff: %f, index: %d, type: %s, frame: %d", std::abs(lhs.data[i] - rhs.data[i]), i, GetName(lhs).c_str(), frameId);

            return false;
        }
    }
    return true;
}
}

GameInputSystem::GameInputSystem(Scene* scene)
    : BaseSimulationSystem(scene, ComponentUtils::MakeMask<NetworkTransformComponent, NetworkInputComponent>())
{
    using namespace GameInputSystemDetail;

    uint32 keyboardId = InputUtils::GetKeyboardDeviceId();

    entityGroup = scene->AquireEntityGroup<NetworkTransformComponent, NetworkInputComponent>();

    ActionsSingleComponent* actionsSingleComponent = scene->GetSingleComponent<ActionsSingleComponent>();
    actionsSingleComponent->CollectDigitalAction(UP, eInputElements::KB_W, keyboardId);
    actionsSingleComponent->CollectDigitalAction(DOWN, eInputElements::KB_S, keyboardId);
    actionsSingleComponent->CollectDigitalAction(LEFT, eInputElements::KB_A, keyboardId);
    actionsSingleComponent->CollectDigitalAction(RIGHT, eInputElements::KB_D, keyboardId);

    actionsSingleComponent->CollectDigitalAction(CAM_FWD, eInputElements::KB_UP, keyboardId);
    actionsSingleComponent->CollectDigitalAction(CAM_BKWD, eInputElements::KB_DOWN, keyboardId);
    actionsSingleComponent->CollectDigitalAction(CAM_LEFT, eInputElements::KB_LEFT, keyboardId);
    actionsSingleComponent->CollectDigitalAction(CAM_RIGHT, eInputElements::KB_RIGHT, keyboardId);
    actionsSingleComponent->CollectDigitalAction(CAM_UP, eInputElements::KB_EQUALS, keyboardId);
    actionsSingleComponent->CollectDigitalAction(CAM_DOWN, eInputElements::KB_MINUS, keyboardId);

    actionsSingleComponent->AddAvailableAnalogAction(LMOVE, AnalogPrecision::ANALOG_UINT8);
    actionsSingleComponent->AddAvailableAnalogAction(RMOVE, AnalogPrecision::ANALOG_UINT8);
    actionsSingleComponent->AddAvailableAnalogAction(TELEPORT, AnalogPrecision::ANALOG_UINT16);
}

void GameInputSystem::ProcessFixed(float32 timeElapsed)
{
    DAVA_PROFILER_CPU_SCOPE("GameInputSystem::ProcessFixed");

    for (Entity* entity : entityGroup->GetEntities())
    {
        const Vector<ActionsSingleComponent::Actions>& allActions = GetCollectedActionsForClient(GetScene(), entity);

        if (!allActions.empty())
        {
            const auto& actions = allActions.back();
            ApplyDigitalActions(entity, actions.digitalActions, actions.clientFrameId, timeElapsed);
            ApplyAnalogActions(entity, actions.analogActions, actions.clientFrameId, timeElapsed);
#ifdef SERVER
            ApplyCameraDelta(entity, actions.cameraDelta, actions.clientFrameId, timeElapsed);
#endif
        }
    }
}

void GameInputSystem::ApplyDigitalActions(Entity* entity,
                                          const Vector<FastName>& actions,
                                          uint32 clientFrameId,
                                          DAVA::float32 duration)
{
    if (!CanMove(entity))
    {
        return;
    }

    using namespace GameInputSystemDetail;

    TransformComponent* transComp = entity->GetComponent<TransformComponent>();

    if (GetScene()->GetSystem<GameModeSystemCars>() != nullptr)
    {
        VehicleCarComponent* car = entity->GetComponent<VehicleCarComponent>();
        if (car != nullptr)
        {
            float32 acceleration = 0.0f;
            float32 steer = 0.0f;

            for (const FastName& action : actions)
            {
                if (action == UP)
                {
                    acceleration += 1.0f;
                }
                else if (action == DOWN)
                {
                    acceleration -= 1.0f;
                }
                else if (action == LEFT)
                {
                    steer += 1.0f;
                }
                else if (action == RIGHT)
                {
                    steer -= 1.0f;
                }
            }

            car->SetAnalogSteer(Clamp(steer, -1.0f, 1.0f));

            if (acceleration < 0.0f)
            {
                car->SetGear(eVehicleGears::Reverse);
            }
            else if (acceleration != 0.0f)
            {
                car->SetGear(eVehicleGears::First);
            }

            car->SetAnalogAcceleration(std::abs(Clamp(acceleration, -1.0f, 1.0f)));
        }
    }
    else
    {
        SpeedModifierComponent* speedModifier = entity->GetComponent<SpeedModifierComponent>();
        float32 speedFactor = speedModifier ? speedModifier->GetFactor() : 1.f;

        Vector3 vec;
        float32 angle = 0.f;

        for (const FastName& action : actions)
        {
            if (action == UP)
            {
                //#ifndef SERVER
                //            static uint32 index = 0;
                //            if (++index % 200 == 0)
                //            {
                //                duration *= 100;
                //            }
                //#endif
                vec += MOV_SPEED * speedFactor * duration;
            }
            else if (action == DOWN)
            {
                vec -= MOV_SPEED * speedFactor * duration;
            }
            else if (action == LEFT)
            {
                angle -= ROT_SPEED * duration;
            }
            else if (action == RIGHT)
            {
                angle += ROT_SPEED * duration;
            }
        }

        if (!vec.IsZero() || angle != 0.0f)
        {
            const Transform& transform = transComp->GetLocalTransform();
            Quaternion rotation = transform.GetRotation();
            Vector3 position = transform.GetTranslation();
            rotation *= Quaternion::MakeRotation(Vector3::UnitZ, -angle);
            position += rotation.ApplyToVectorFast(vec);
            transComp->SetLocalTransform(Transform(
            position, Vector3(1.0, 1.0, 1.0), rotation));
        }
    }
    
#ifndef SERVER
    CameraComponent* cameraComp = entity->GetComponent<CameraComponent>();
    if (nullptr != cameraComp && cameraComp->GetCamera())
    {
        Camera* camera = cameraComp->GetCamera();
        Vector3 curCamPos = camera->GetPosition();
        float curCamMoveSpeed = Clamp(curCamPos.z * CAM_MOVE_SPEED_FACTOR, CAM_MOVE_SPEED_MIN, CAM_MOVE_SPEED_MAX);
        Vector3 offset;
        for (const FastName& action : actions)
        {
            if (action == CAM_UP)
            {
                offset.z += curCamMoveSpeed * duration;
            }
            if (action == CAM_DOWN)
            {
                offset.z -= curCamMoveSpeed * duration;
            }
            if (action == CAM_FWD)
            {
                offset.y += curCamMoveSpeed * duration;
            }
            if (action == CAM_BKWD)
            {
                offset.y -= curCamMoveSpeed * duration;
            }
            if (action == CAM_RIGHT)
            {
                offset.x += curCamMoveSpeed * duration;
            }
            if (action == CAM_LEFT)
            {
                offset.x -= curCamMoveSpeed * duration;
            }
        }

        camera->SetPosition(camera->GetPosition() + offset);
        camera->SetTarget(camera->GetTarget() + offset);
        // TODO: reconsider
        camera->Rotate(Quaternion());
    }
#endif
}

void GameInputSystem::ApplyAnalogActions(Entity* entity,
                                         const AnalogActionsMap& actions,
                                         uint32 clientFrameId,
                                         float32 duration)
{
    if (!CanMove(entity))
    {
        return;
    }

    using namespace GameInputSystemDetail;
    for (const auto& action : actions)
    {
        if (action.first.actionId == LMOVE)
        {
            Vector2 analogPos = ConvertFixedPrecisionToAnalog(action.first.precision, action.second);
            if (GetScene()->GetSystem<GameModeSystemCars>() != nullptr)
            {
                if (!FLOAT_EQUAL(analogPos.x, 0.0f) && !FLOAT_EQUAL(analogPos.y, 0.0f))
                {
                    VehicleCarComponent* car = entity->GetComponent<VehicleCarComponent>();

                    if (car != nullptr)
                    {
                        car->SetAnalogAcceleration(std::abs(analogPos.y));
                        if (analogPos.y > 0)
                        {
                            car->SetGear(eVehicleGears::Reverse);
                        }
                        else
                        {
                            car->SetGear(eVehicleGears::First);
                        }

                        car->SetAnalogSteer(-analogPos.x);
                    }
                }
            }
            else
            {
                Vector3 vec = -MOV_SPEED * duration * analogPos.y;
                float32 angle = ROT_SPEED * duration * analogPos.x;

                TransformComponent* transComp = entity->GetComponent<TransformComponent>();
                const Transform& transform = transComp->GetLocalTransform();
                Quaternion rotation = transform.GetRotation();
                Vector3 position = transform.GetTranslation();
                rotation *= Quaternion::MakeRotation(Vector3::UnitZ, -angle);
                position += rotation.ApplyToVectorFast(vec);
                transComp->SetLocalTransform(Transform(
                position, Vector3(1.0, 1.0, 1.0), rotation));
            }
        }
        else if (action.first.actionId == TELEPORT)
        {
            if (GetScene()->GetSystem<GameModeSystem>() != nullptr)
            {
                Vector2 analogPos = ConvertFixedPrecisionToAnalog(action.first.precision, action.second);
                DenormalizeAnalog(analogPos, GameInputSystemDetail::TELEPORT_HALF_RANGE);
                Vector3 newPos(analogPos.x, analogPos.y, 0.f);
                TransformComponent* transComp = entity->GetComponent<TransformComponent>();
                const Transform& transform = transComp->GetLocalTransform();
                transComp->SetLocalTransform(Transform(
                newPos, transform.GetScale(), transform.GetRotation()));
            }
        }
    }
}

void GameInputSystem::ApplyCameraDelta(DAVA::Entity* entity, const Quaternion& cameraDelta,
                                       uint32 clientFrameId,
                                       DAVA::float32 duration) const
{
    CameraComponent* cameraComp = entity->GetComponent<CameraComponent>();
    if (nullptr != cameraComp)
    {
        Camera* camera = cameraComp->GetCamera();
        camera->Rotate(cameraDelta);
    }
}

bool GameInputSystem::CanMove(const DAVA::Entity* entity) const
{
    const HealthComponent* healthComponent = entity->GetComponent<HealthComponent>();
    if (healthComponent && healthComponent->GetHealth() == 0)
    {
        return false;
    }

    const GameStunnableComponent* stunComp = entity->GetComponent<GameStunnableComponent>();
    if (stunComp && stunComp->IsStunned())
    {
        return false;
    }

    return true;
}

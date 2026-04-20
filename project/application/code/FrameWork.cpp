#include "FrameWork.h"

/// engine include
#define ENGINE_INCLUDE
#define ENGINE_ECS
#define ENGINE_COMPONENTS
#define ENGINE_SYSTEMS
#include <EngineInclude.h>

// application component
#include "component/AddForceComponent.h"
#include "component/BillboardComponent.h"
#include "component/Camera/CameraController.h"
#include "component/camera/CameraMotionBob.h"
#include "component/Camera/CameraShakeSourceComponent.h"
#include "component/FollowTransformComponent.h"
#include "component/gimmick/BulletSpawner.h"
#include "component/gimmick/BulletSpawnerTrigger.h"
#include "component/gimmick/PathControllerTrigger.h"
#include "component/gimmick/Obstacle.h"
#include "component/gimmick/ObstacleShieldComponent.h"
#include "component/gimmick/ObstacleSpawnGroupComponent.h"
#include "component/gimmick/PathController.h"
#include "component/gimmick/RailPoints.h"
#include "component/gimmick/TimeScaleEffectComponent.h"
#include "component/gimmick/WallRunnableComponent.h"
#include "component/LookAtFromTransforms.h"
#include "component/MouseCondition.h"
#include "component/player/PlayerEffectControlParam.h"
#include "component/player/PlayerInput.h"
#include "component/player/PlayerStatus.h"
#include "component/player/state/PlayerState.h"
#include "component/PlayerStateOverrideCondition.h"
#include "component/SpeedModifiers.h"
#include "component/spline/SplinePoints.h"
#include "component/spline/TireSplinePoints.h"
#include "component/stage/StageData.h"
#include "component/TimerComponent.h"
#include "component/TimerForSpriteComponent.h"
#include "component/ui/Button.h"
#include "component/ui/ButtonGroup.h"
#include "component/ui/SpeedFor3dUIComponent.h"
#include "component/VelocityOverrideComponent.h"

// application
#include "system/collision/AddForceTriggerSystem.h"
#include "system/collision/BulletSpawnerTriggerSystem.h"
#include "system/collision/PathControllerTriggerSystem.h"
#include "system/collision/ObstacleSpawnEventTriggerSystem.h"
#include "system/collision/OnCollisionModifierTargetSystem.h"
#include "system/collision/PlayerAheadCollisionReactionSystem.h"
#include "system/collision/PlayerOnCollision.h"
#include "system/collision/TutorialColliderOnCollision.h"
#include "system/collision/VelocityOverrideTriggerSystem.h"
#include "system/effect/BulletSpawnerWorkSystem.h"
#include "system/effect/CameraShake.h"
#include "system/effect/CreateMeshFromSpline.h"
#include "system/effect/CreateMeshFromTireSpline.h"
#include "system/effect/EffectOnPlayerGearup.h"
#include "system/effect/EffectOnPlayerRun.h"
#include "system/effect/PenaltyTimeSpriteUpdate.h"
#include "system/effect/PlayerExplosionEffectSystem.h"
#include "system/effect/PlayerSpeedFor3dUI.h"
#include "system/effect/TimerForSprite.h"
#include "system/effect/TimeScaleEffectSystem.h"
#include "system/effect/TimeStopEffect.h"
#include "system/initialize/BackFireInitialize.h"
#include "system/Initialize/ClearSceneRankingBuildSystem.h"
#include "system/Initialize/CreatePlaneFromSpeed.h"
#include "system/initialize/CreateSpriteFromTimer.h"
#include "system/initialize/GetClearTime.h"
#include "system/Initialize/InitializeMouseCondition.h"
#include "system/Initialize/RailInitializeSystem.h"
#include "system/Initialize/SelectPreviewSceneInitialize.h"
#include "system/initialize/SettingGameCameraTarget.h"
#include "system/initialize/StartTimerInitialize.h"
#include "system/initialize/TakePlayerToStartPosition.h"
#include "system/initialize/TakeToGoalPosition.h"
#include "system/Initialize/Ui3dObjectInitialize.h"
#include "system/input/ButtonInputSystem.h"
#include "system/input/CameraInputSystem.h"
#include "system/input/PlayerInputSystem.h"
#include "system/movement/ApplyAddForceSystem.h"
#include "system/Movement/ApplySpeedModifiers.h"
#include "system/movement/BillboardTransform.h"
#include "system/Movement/CreateRailMesh.h"
#include "system/movement/FollowCameraUpdateSystem.h"
#include "system/Movement/FollowTransformSystem.h"
#include "system/Movement/LookAtFromTransformsSystem.h"
#include "system/render/PathControllerRenderingSystem.h"
#include "system/movement/PathControllerSystem.h"
#include "system/movement/PauseMainSceneSystem.h"
#include "system/Movement/PlayerFollowSystem.h"
#include "system/movement/PlayerMoveSystem.h"
#include "system/movement/PlayerPathSplineGenerator.h"
#include "system/movement/PlayerUpdateOnTitle.h"
#include "system/movement/RestartSystem.h"
#include "system/movement/SceneTransitionRequestReceiverSystem.h"
#include "system/movement/SceneTransitionRequestSenderSystem.h"
#include "system/movement/StartSequenceSystem.h"
#include "system/Movement/TireTrailGenerateSystem.h"
#include "system/Movement/Ui3dUpdateSystem.h"
#include "system/Movement/VelocityOverrideSystem.h"
#include "system/transition/ApplyMouseConditionSystem.h"
#include "system/transition/AttractModeSystem.h"
#include "system/transition/ButtonGroupSystem.h"
#include "system/transition/ButtonScenePreviewSystem.h"
#include "system/Transition/CameraMotionBobSystem.h"
#include "system/transition/ChangeSceneByButton.h"
#include "system/transition/ExitApplicationByButton.h"
#include "system/transition/FallToFailedSystem.h"
#include "system/transition/FallToRestartSystem.h"
#include "system/transition/GameFailedSceneLaunchSystem.h"
#include "system/transition/PenaltySystem.h"
#include "system/Transition/PlayerStateOverrideSystem.h"
#include "system/transition/RestartOnButtonPressSystem.h"
#include "system/transition/SceneTransition.h"
#include "system/transition/ShowGameUIByInputDevice.h"
#include "system/transition/SubSceneActivateByButton.h"
#include "system/transition/SubSceneDeactivateByButton.h"
#include "system/transition/TimeLimitJudgeSystem.h"
#include "system/Transition/TimerAutoDestroySystem.h"
#include "system/transition/TimerCountDown.h"
#include "system/Transition/TimerCountUp.h"
#include "system/transition/TransitionPlayerState.h"
#include "system/transition/UpdateButtonColorByState.h"

using namespace OriGine;

/// =====================================================
// Application Include
/// =====================================================
// component

FrameWork::FrameWork(){}
FrameWork::~FrameWork(){}

void RegisterUsingComponents(){
	ComponentRegistry* componentRegistry = ComponentRegistry::GetInstance();

	componentRegistry->RegisterComponent<Audio>();

	componentRegistry->RegisterComponent<MouseCondition>();

	componentRegistry->RegisterComponent<EntityReferenceList>();
	componentRegistry->RegisterComponent<SubScene>();

	componentRegistry->RegisterComponent<CameraTransform>();
	componentRegistry->RegisterComponent<Transform>();

	componentRegistry->RegisterComponent<LookAtFromTransforms>();

	componentRegistry->RegisterComponent<StageData>();

	componentRegistry->RegisterComponent<SplinePoints>();
	componentRegistry->RegisterComponent<TireSplinePoints>();

	componentRegistry->RegisterComponent<Material>();
	componentRegistry->RegisterComponent<DirectionalLight>();
	componentRegistry->RegisterComponent<PointLight>();
	componentRegistry->RegisterComponent<SpotLight>();

	componentRegistry->RegisterComponent<CameraAction>();
	componentRegistry->RegisterComponent<ModelNodeAnimation>();
	componentRegistry->RegisterComponent<PrimitiveNodeAnimation>();
	componentRegistry->RegisterComponent<TransformAnimation>();
	componentRegistry->RegisterComponent<TransformRateAnimation>();
	componentRegistry->RegisterComponent<DissolveAnimation>();
	componentRegistry->RegisterComponent<SkinningAnimationComponent>();
	componentRegistry->RegisterComponent<SpriteAnimation>();
	componentRegistry->RegisterComponent<MaterialAnimation>();

	componentRegistry->RegisterComponent<AABBCollider>();
	componentRegistry->RegisterComponent<OBBCollider>();
	componentRegistry->RegisterComponent<SphereCollider>();
	componentRegistry->RegisterComponent<SegmentCollider>();
	componentRegistry->RegisterComponent<CapsuleCollider>();
	componentRegistry->RegisterComponent<RayCollider>();

	componentRegistry->RegisterComponent<CollisionPushBackInfo>();

	componentRegistry->RegisterComponent<SpeedModifiers>();
	componentRegistry->RegisterComponent<Rigidbody>();

	componentRegistry->RegisterComponent<ParticleSystem>();
	componentRegistry->RegisterComponent<GpuParticleEmitter>();
	componentRegistry->RegisterComponent<EntitySpawner>();

	componentRegistry->RegisterComponent<SquashStretchComponent>();
	componentRegistry->RegisterComponent<DissolveEffectParam>();
	componentRegistry->RegisterComponent<DistortionEffectParam>();
	componentRegistry->RegisterComponent<RadialBlurParam>();
	componentRegistry->RegisterComponent<RandomEffectParam>();
	componentRegistry->RegisterComponent<VignetteParam>();
	componentRegistry->RegisterComponent<SpeedlineEffectParam>();
	componentRegistry->RegisterComponent<MaterialEffectPipeLine>();
	componentRegistry->RegisterComponent<GradationComponent>();
	componentRegistry->RegisterComponent<GrayscaleComponent>();

	componentRegistry->RegisterComponent<ModelMeshRenderer>();
	componentRegistry->RegisterComponent<LineRenderer>();
	componentRegistry->RegisterComponent<PlaneRenderer>();
	componentRegistry->RegisterComponent<RingRenderer>();
	componentRegistry->RegisterComponent<BoxRenderer>();
	componentRegistry->RegisterComponent<SphereRenderer>();
	componentRegistry->RegisterComponent<SkyboxRenderer>();
	componentRegistry->RegisterComponent<SpriteRenderer>();
	componentRegistry->RegisterComponent<CylinderRenderer>();

	componentRegistry->RegisterComponent<TimerComponent>();
	componentRegistry->RegisterComponent<TimerForSpriteComponent>();

	componentRegistry->RegisterComponent<SpeedFor3dUIComponent>();

	componentRegistry->RegisterComponent<CameraController>();
	componentRegistry->RegisterComponent<CameraShakeSourceComponent>();
	componentRegistry->RegisterComponent<CameraMotionBob>();

	componentRegistry->RegisterComponent<PlayerInput>();
	componentRegistry->RegisterComponent<PlayerStatus>();
	componentRegistry->RegisterComponent<PlayerState>();
	componentRegistry->RegisterComponent<PlayerEffectControlParam>();

	componentRegistry->RegisterComponent<Button>();
	componentRegistry->RegisterComponent<ButtonGroup>();
	componentRegistry->RegisterComponent<SceneChanger>();

	componentRegistry->RegisterComponent<PlayerStateOverrideCondition>();

	componentRegistry->RegisterComponent<BillboardComponent>();

	componentRegistry->RegisterComponent<FollowTransformComponent>();

	// ギミック系
	componentRegistry->RegisterComponent<RailPoints>();
	componentRegistry->RegisterComponent<PathController>();
	componentRegistry->RegisterComponent<Obstacle>();
	componentRegistry->RegisterComponent<TimeScaleEffectComponent>();
	componentRegistry->RegisterComponent<VelocityOverrideComponent>();
	componentRegistry->RegisterComponent<AddForceComponent>();
	componentRegistry->RegisterComponent<ObstacleShieldComponent>();
	componentRegistry->RegisterComponent<WallRunnableComponent>();
	componentRegistry->RegisterComponent<BulletSpawner>();
	componentRegistry->RegisterComponent<ObstacleSpawnGroupComponent>();
	componentRegistry->RegisterComponent<BulletSpawnerTrigger>();
	componentRegistry->RegisterComponent<PathControllerTrigger>();
}

void RegisterUsingSystems(){
	SystemRegistry* systemRegistry = SystemRegistry::GetInstance();

	/// ====================================================================================================
	// Initialize
	/// ====================================================================================================
	systemRegistry->RegisterSystem<SettingGameCameraTarget>();
	systemRegistry->RegisterSystem<GpuParticleInitialize>();
	systemRegistry->RegisterSystem<ResolveEntityReferences>();
	systemRegistry->RegisterSystem<CreateSpriteFromTimer>();
	systemRegistry->RegisterSystem<Ui3dObjectInitialize>();
	systemRegistry->RegisterSystem<GetClearTime>();
	systemRegistry->RegisterSystem<CameraInitialize>();
	systemRegistry->RegisterSystem<RegisterWindowResizeEvent>();
	systemRegistry->RegisterSystem<InitializeMouseCondition>();

	systemRegistry->RegisterSystem<StartTimerInitialize>();

	systemRegistry->RegisterSystem<BackFireInitialize>();

	systemRegistry->RegisterSystem<SelectPreviewSceneInitialize>();

	systemRegistry->RegisterSystem<TakePlayerToStartPosition>();
	systemRegistry->RegisterSystem<TakeToGoalPosition>();
	systemRegistry->RegisterSystem<CreatePlaneFromSpeed>();

	systemRegistry->RegisterSystem<RailInitializeSystem>();

	systemRegistry->RegisterSystem<ClearSceneRankingBuildSystem>();

	/// ===================================================================================================
	// Input
	/// ===================================================================================================
	systemRegistry->RegisterSystem<ButtonInputSystem>();
	systemRegistry->RegisterSystem<CameraInputSystem>();
	systemRegistry->RegisterSystem<PlayerInputSystem>();

	/// ===================================================================================================
	// StateTransition
	/// ===================================================================================================
	systemRegistry->RegisterSystem<EffectAutoDestroySystem>();
	systemRegistry->RegisterSystem<TimerAutoDestroySystem>();
	systemRegistry->RegisterSystem<ChangeSceneByButton>();
	systemRegistry->RegisterSystem<FallToFailedSystem>();
	systemRegistry->RegisterSystem<FallToRestartSystem>();
	systemRegistry->RegisterSystem<SceneTransition>();
	systemRegistry->RegisterSystem<TransitionPlayerState>();
	systemRegistry->RegisterSystem<UpdateButtonColorByState>();
	systemRegistry->RegisterSystem<ButtonGroupSystem>();
	systemRegistry->RegisterSystem<TimerCountUp>();
	systemRegistry->RegisterSystem<TimerCountDown>();
	systemRegistry->RegisterSystem<TimeLimitJudgeSystem>();
	systemRegistry->RegisterSystem<ShowGameUIByInputDevice>();
	systemRegistry->RegisterSystem<ApplyMouseConditionSystem>();
	systemRegistry->RegisterSystem<SubSceneActivateByButton>();
	systemRegistry->RegisterSystem<SubSceneDeactivateByButton>();
	systemRegistry->RegisterSystem<ExitApplicationByButton>();
	systemRegistry->RegisterSystem<PenaltySystem>();
	systemRegistry->RegisterSystem<CameraMotionBobSystem>();

	systemRegistry->RegisterSystem<GameFailedSceneLaunchSystem>();

	systemRegistry->RegisterSystem<RestartOnButtonPressSystem>();

	systemRegistry->RegisterSystem<ButtonScenePreviewSystem>();

	systemRegistry->RegisterSystem<PlayerStateOverrideSystem>();

	/// =================================================================================================
	// Movement
	/// =================================================================================================
	systemRegistry->RegisterSystem<BillboardTransform>();

	systemRegistry->RegisterSystem<MoveSystemByRigidBody>();
	systemRegistry->RegisterSystem<FollowCameraUpdateSystem>();
	systemRegistry->RegisterSystem<PlayerMoveSystem>();

	systemRegistry->RegisterSystem<PlayerPathSplineGenerator>();
	// systemRegistry->RegisterSystem<TireTrailGenerateSystem>();
	systemRegistry->RegisterSystem<PlayerUpdateOnTitle>();

	systemRegistry->RegisterSystem<StartSequenceSystem>();

	systemRegistry->RegisterSystem<SceneTransitionRequestSenderSystem>();
	systemRegistry->RegisterSystem<SceneTransitionRequestReceiverSystem>();

	systemRegistry->RegisterSystem<ApplySpeedModifiers>();
	systemRegistry->RegisterSystem<ApplyAddForceSystem>();
	systemRegistry->RegisterSystem<VelocityOverrideSystem>();

	systemRegistry->RegisterSystem<PathControllerSystem>();

	systemRegistry->RegisterSystem<SubSceneUpdate>();
	systemRegistry->RegisterSystem<RestartSystem>();
	systemRegistry->RegisterSystem<PauseMainSceneSystem>();

	systemRegistry->RegisterSystem<FollowTransformSystem>();
	systemRegistry->RegisterSystem<PlayerFollowSystem>();
	systemRegistry->RegisterSystem<LookAtFromTransformsSystem>();
	systemRegistry->RegisterSystem<Ui3dUpdateSystem>();

	/// =================================================================================================
	// Collision
	/// =================================================================================================
	systemRegistry->RegisterSystem<CollisionCheckSystem>();
	systemRegistry->RegisterSystem<CollisionPushBackSystem>();

	systemRegistry->RegisterSystem<CollisionTriggeredSceneTransition>();

	systemRegistry->RegisterSystem<ObstacleSpawnEventTriggerSystem>();
	systemRegistry->RegisterSystem<BulletSpawnerTriggerSystem>();
	systemRegistry->RegisterSystem<PathControllerTriggerSystem>();

	systemRegistry->RegisterSystem<PlayerOnCollision>();
	systemRegistry->RegisterSystem<TutorialColliderOnCollision>();
	systemRegistry->RegisterSystem<VelocityOverrideTriggerSystem>();
	systemRegistry->RegisterSystem<AddForceTriggerSystem>();
	systemRegistry->RegisterSystem<OnCollisionModifierTargetSystem>();
	systemRegistry->RegisterSystem<PlayerAheadCollisionReactionSystem>();

	/// =================================================================================================
	// Effect
	/// =================================================================================================
	systemRegistry->RegisterSystem<ParticleSystemWorkSystem>();
	systemRegistry->RegisterSystem<GpuParticleEmitterWorkSystem>();

	systemRegistry->RegisterSystem<EntitySpawnerWorkSystem>();
	systemRegistry->RegisterSystem<BulletSpawnerWorkSystem>();

	systemRegistry->RegisterSystem<PrimitiveNodeAnimationWorkSystem>();
	systemRegistry->RegisterSystem<SkinningAnimationSystem>();
	systemRegistry->RegisterSystem<SpriteAnimationSystem>();
	systemRegistry->RegisterSystem<MaterialAnimationWorkSystem>();
	systemRegistry->RegisterSystem<CameraActionSystem>();
	systemRegistry->RegisterSystem<TransformAnimationWorkSystem>();
	systemRegistry->RegisterSystem<TransformRateAnimationWorkSystem>();
	systemRegistry->RegisterSystem<LightTransformSyncSystem>();
	systemRegistry->RegisterSystem<DissolveAnimationSystem>();

	systemRegistry->RegisterSystem<CameraShake>();
	systemRegistry->RegisterSystem<TimeScaleEffectSystem>();

	systemRegistry->RegisterSystem<ScaleDeformSystem>();

	systemRegistry->RegisterSystem<MaterialEffect>();
	systemRegistry->RegisterSystem<CreateMeshFromSpline>();
	// systemRegistry->RegisterSystem<CreateMeshFromTireSpline>();
	systemRegistry->RegisterSystem<CreateRailMesh>();

	systemRegistry->RegisterSystem<PlayerSpeedFor3dUI>();
	systemRegistry->RegisterSystem<TimerForSprite>();

	systemRegistry->RegisterSystem<EffectOnPlayerGearup>();
	systemRegistry->RegisterSystem<EffectOnPlayerRun>();

	systemRegistry->RegisterSystem<PenaltyTimeSpriteUpdate>();
	systemRegistry->RegisterSystem<TimeStopEffect>();

	systemRegistry->RegisterSystem<AttractModeSystem>();

	systemRegistry->RegisterSystem<PlayerExplosionEffectSystem>();

	/// =================================================================================================
	// Render
	/// =================================================================================================
	systemRegistry->RegisterSystem<SkyboxRender>();
	systemRegistry->RegisterSystem<SpriteRenderSystem>();
	systemRegistry->RegisterSystem<BackGroundSpriteRenderSystem>();
	systemRegistry->RegisterSystem<ParticleRenderSystem>();
	systemRegistry->RegisterSystem<TexturedMeshRenderSystem>();
	systemRegistry->RegisterSystem<OverlayRenderSystem>();
	systemRegistry->RegisterSystem<SkinningMeshRenderSystem>();
	systemRegistry->RegisterSystem<LineRenderSystem>();
	systemRegistry->RegisterSystem<GpuParticleRenderSystem>();

#ifndef _RELEASE
	systemRegistry->RegisterSystem<SkeletonRenderSystem>();
	systemRegistry->RegisterSystem<ColliderRenderingSystem>();
	systemRegistry->RegisterSystem<EmitterShapeRenderingSystem>();
	systemRegistry->RegisterSystem<VelocityRenderingSystem>();
	systemRegistry->RegisterSystem<LightDebugRenderingSystem>();
	systemRegistry->RegisterSystem<PathControllerRenderingSystem>();
#endif // _RELEASE

	/// =================================================================================================
	// PostRender
	/// =================================================================================================
	systemRegistry->RegisterSystem<GrayscaleEffect>();
	systemRegistry->RegisterSystem<SmoothingEffect>();
	systemRegistry->RegisterSystem<VignetteEffect>();
	systemRegistry->RegisterSystem<DistortionEffect>();
	systemRegistry->RegisterSystem<DissolveEffect>();
	systemRegistry->RegisterSystem<RandomEffect>();
	systemRegistry->RegisterSystem<RadialBlurEffect>();
	systemRegistry->RegisterSystem<SubSceneRender>();
	systemRegistry->RegisterSystem<SpeedlineEffect>();
	systemRegistry->RegisterSystem<GradationEffect>();
}

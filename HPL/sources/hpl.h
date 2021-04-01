/*
 * Copyright (C) 2006-2010 - Frictional Games
 *
 * This file is part of HPL1 Engine.
 *
 * HPL1 Engine is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * HPL1 Engine is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with HPL1 Engine.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef HPL_HPL_H
#define HPL_HPL_H

#include "system/System.h"
#include "system/SystemTypes.h"
#include "system/String.h"
#include "system/LogicTimer.h"
#include "system/LowLevelSystem.h"
#include "system/SerializeClass.h"
#include "system/Container.h"

#include "input/Input.h"
#include "input/InputDevice.h"
#include "input/Keyboard.h"
#include "input/Mouse.h"
#include "input/Action.h"
#include "input/ActionKeyboard.h"
#include "input/ActionMouseButton.h"
#include "input/ActionHaptic.h"
#include "input/LowLevelInput.h"

#include "game/Game.h"
#include "game/Updater.h"
#include "game/LowLevelGameSetup.h"
#include "game/Updateable.h"
#include "game/SaveGame.h"

#include "graphics/Graphics.h"
#include "graphics/Renderer3D.h"
#include "graphics/RenderList.h"
#include "graphics/RendererPostEffects.h"
#include "graphics/MaterialHandler.h"
#include "graphics/MeshCreator.h"
#include "graphics/ParticleSystem3D.h"
#include "graphics/ParticleEmitter3D_UserData.h"
#include "graphics/FontData.h"
#include "graphics/GraphicsDrawer.h"
#include "graphics/GfxObject.h"
#include "graphics/FontData.h"
#include "graphics/Texture.h"
#include "graphics/GPUProgram.h"
#include "graphics/VertexBuffer.h"
#include "graphics/Mesh.h"
#include "graphics/SubMesh.h"
#include "graphics/BillBoard.h"
#include "graphics/Beam.h"
#include "graphics/Skeleton.h"
#include "graphics/Bone.h"
#include "graphics/BoneState.h"
#include "graphics/Animation.h"
#include "graphics/AnimationTrack.h"
#include "graphics/OcclusionQuery.h"
#include "graphics/VideoStream.h"

#include "math/MathTypes.h"
#include "math/Math.h"
#include "math/BoundingVolume.h"
#include "math/Frustum.h"
#include "math/Spring.h"
#include "math/PidController.h"
#include "math/CRC.h"

#include "resources/Resources.h"
#include "resources/LowLevelResources.h"
#include "resources/FileSearcher.h"
#include "resources/ImageManager.h"
#include "resources/ResourceImage.h"
#include "resources/GpuProgramManager.h"
#include "resources/ParticleManager.h"
#include "resources/SoundManager.h"
#include "resources/FontManager.h"
#include "resources/ScriptManager.h"
#include "resources/TextureManager.h"
#include "resources/MaterialManager.h"
#include "resources/MeshManager.h"
#include "resources/MeshLoaderHandler.h"
#include "resources/SoundEntityManager.h"
#include "resources/AnimationManager.h"
#include "resources/VideoManager.h"
#include "resources/MeshLoader.h"
#include "resources/ConfigFile.h"
#include "resources/EntityLoader_Object.h"

#include "scene/Scene.h"
#include "scene/World3D.h"
#include "scene/Camera3D.h"
#include "scene/RenderableContainer.h"
#include "scene/MeshEntity.h"
#include "scene/Light3DPoint.h"
#include "scene/Light3DSpot.h"
#include "scene/AnimationState.h"
#include "scene/NodeState.h"
#include "scene/PortalContainer.h"
#include "scene/ColliderEntity.h"
#include "scene/SoundEntity.h"
#include "scene/SectorVisibility.h"

#include "sound/Sound.h"
#include "sound/SoundChannel.h"
#include "sound/SoundData.h"
#include "sound/SoundEnvironment.h"
#include "sound/SoundEntityData.h"
#include "sound/SoundHandler.h"
#include "sound/MusicHandler.h"
#include "sound/LowLevelSound.h"

#include "physics/Physics.h"
#include "physics/PhysicsWorld.h"
#include "physics/PhysicsBody.h"
#include "physics/PhysicsMaterial.h"
#include "physics/CollideShape.h"
#include "physics/LowLevelPhysics.h"
#include "physics/CollideData.h"
#include "physics/CharacterBody.h"
#include "physics/PhysicsController.h"
#include "physics/PhysicsJoint.h"
#include "physics/PhysicsJointBall.h"
#include "physics/PhysicsJointHinge.h"
#include "physics/PhysicsJointScrew.h"
#include "physics/PhysicsJointSlider.h"
#include "physics/SurfaceData.h"

#include "ai/AI.h"
#include "ai/AStar.h"
#include "ai/AINodeContainer.h"
#include "ai/AINodeGenerator.h"
#include "ai/StateMachine.h"

#include "script/ScriptVar.h"
#include "script/Script.h"
#include "script/ScriptFuncs.h" // sigh, this is only for cScriptJointCallback

#include "haptic/Haptic.h"
#include "haptic/LowLevelHaptic.h"
#include "haptic/HapticShape.h"
#include "haptic/HapticForce.h"
#include "haptic/HapticSurface.h"

#include "impl/tinyXML/tinyxml.h"


#endif // HPL_HPL_H

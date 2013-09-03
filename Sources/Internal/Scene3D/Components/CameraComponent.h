/*==================================================================================
    Copyright (c) 2008, binaryzebra
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
    * Neither the name of the binaryzebra nor the
    names of its contributors may be used to endorse or promote products
    derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE binaryzebra AND CONTRIBUTORS "AS IS" AND
    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL binaryzebra BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=====================================================================================*/



#ifndef __DAVAENGINE_CAMERA_COMPONENT_H__
#define __DAVAENGINE_CAMERA_COMPONENT_H__

#include "Base/BaseTypes.h"
#include "Entity/Component.h"

namespace DAVA 
{

class Camera;

class CameraComponent : public Component
{
public:
    CameraComponent(Camera * _camera = 0);
    virtual ~CameraComponent();

	Camera* GetCamera();
	void SetCamera(Camera * _camera);

    IMPLEMENT_COMPONENT_TYPE(CAMERA_COMPONENT);

    virtual Component* Clone(Entity *toEntity);
	virtual void Serialize(KeyedArchive *archive, SceneFileV2 *sceneFile);
	virtual void Deserialize(KeyedArchive *archive, SceneFileV2 *sceneFile);
    
private:
    Camera* camera;
    
public:
    INTROSPECTION_EXTEND(CameraComponent, Component,
        PROPERTY("camera", "Camera", GetCamera, SetCamera, I_SAVE | I_VIEW | I_EDIT)
    );
};


};

#endif //__DAVAENGINE_CAMERA_COMPONENT_H__

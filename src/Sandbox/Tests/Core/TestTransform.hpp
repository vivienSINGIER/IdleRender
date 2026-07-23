#ifndef TEST_TRANSFORM_H_DEFINED
#define TEST_TRANSFORM_H_DEFINED

#include "Test.hpp"
#include "../Render/Generic/Render.h"
#include "Core/Math/Matrix/Matrix.h"
#include "Core/Math/Vector/Vector.h"
#include "Core/Transform/Transform.h"
#include "Core/InputManager.h"

class TestTransform : public Test
{
public:
    void HandleObjectInput(InputManager& _im, Transform& t)
    {
        float dt = 1.0f / 60.0f;
        
        if (_im.IsKey(D))
            t.Move(Vect3f32(1.0f, 0.0f, 0.0f) * dt );
        if (_im.IsKey(Q))
            t.Move(Vect3f32(-1.0f, 0.0f, 0.0f) * dt );
        if (_im.IsKey(Z))
            t.Move(Vect3f32(0.0f, 0.0f, 1.0f) * dt );
        if (_im.IsKey(S))
            t.Move(Vect3f32(0.0f, 0.0f, -1.0f) * dt );
        if (_im.IsKey(SPACE))
            t.Move(Vect3f32(0.0f, 1.0f, 0.0f) * dt );
        if (_im.IsKey(LCTRL))
            t.Move(Vect3f32(0.0f, -1.0f, 0.0f) * dt );
        
        if (_im.IsKey(NUMPAD8))
            t.AddYPR(Vect3f32(0.0f, 1.0f, 0.0f) * dt );
        if (_im.IsKey(NUMPAD5))
            t.AddYPR(Vect3f32(0.0f, -1.0f, 0.0f) * dt );
        if (_im.IsKey(NUMPAD4))
            t.AddYPR(Vect3f32(1.0f, 0.0f, 0.0f) * dt );
        if (_im.IsKey(NUMPAD6))
            t.AddYPR(Vect3f32(-1.0f, 0.0f, 0.0f) * dt );
        if (_im.IsKey(NUMPAD7))
            t.AddYPR(Vect3f32(0.0f, 0.0f, 1.0f) * dt );
        if (_im.IsKey(NUMPAD9))
            t.AddYPR(Vect3f32(0.0f, 0.0f, -1.0f) * dt );
        
        if (_im.IsKey(NUMPAD_ADD))
            t.Scale( 1.01f );
        if (_im.IsKey(NUMPAD_SUBTRACT))
            t.Scale( 0.99f );
        
        _im.HandleInput();
    }
    
    void Run()
    {
        Window window(1080, 720, L"Test", false);
        window.InitD3D12();

        Device* pDevice = window.GetDevice();

        Shader* s = ShaderFactory::CreateLitColored(pDevice);
        Material* green = s->CreateMaterial();
        green->SetFloat4("DiffuseAlbedo", {0.0f, 1.0f, 0.0f, 1.0f});

        Geometry* line = GeometryFactory::BuildLine(pDevice);
        Geometry* cube = GeometryFactory::BuildCube(pDevice);

        Camera cam;
        Transform camT;
        camT.SetPosition(Vect3f32(0.0f, 0.0f, -5.0f));
        camT.LookAt({0.0f, 0.0f, 0.0f});
        
        cam.SetWorld(camT.GetMatrix());
        pDevice->SetMainCamera(&cam);
        
        {
            LightDescriptor point1 = LightHelper::CreateLight(LightType::Point);
            point1.light.Position = Vect3f32(-2.0, -1.0f, -1.0f);
            point1.light.Strength = Vect3f32(1.0f, 1.0f, 1.0f);
            point1.light.Color = Vect4f32(1.0f, 1.0f, 1.0f, 1.0f);
        
            Vector<LightDescriptor> lights = { point1 };
            pDevice->SetLights(lights);
        }
        
        InputManager inputManager;
        inputManager.Initialize(window.GetHWND());
        
        Transform t;
        
        while (window.IsOpen())
        {
            HandleObjectInput(inputManager, t);
            
            window.Update();
            window.Clear();

            pDevice->SetMaterial(green);
            pDevice->Draw(cube, t.GetMatrix());
            
            window.Display();
        }
    }
};

#endif
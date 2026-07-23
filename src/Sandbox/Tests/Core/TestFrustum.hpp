#ifndef TEST_FRUSTUM_H_DEFINED
#define TEST_FRUSTUM_H_DEFINED

#include "Test.hpp"
#include "../Render/Generic/Render.h"
#include "Core/Math/Matrix/Matrix.h"
#include "Core/Math/Vector/Vector.h"
#include "Core/Transform/Transform.h"

#include "Core/InputManager.h"

#include "../Core/Math/Geometry/AABB.h"
#include "Core/Math/Geometry/Frustum.h"
#include "Core/Math/Geometry/OBB.h"
#include "Core/Math/Geometry/Ray.h"
#include "Core/Math/Geometry/Sphere.h"

class TestFrustum : public Test
{
public:
    Geometry* cube;
    Geometry* sphere;
    Geometry* line;
    Camera cam;
    Transform camT;
    
    void HandleObjectInput(InputManager& _im, Transform& t)
    {
        if (_im.IsKey(LCONTROL) == true)
            return;
        
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
        if (_im.IsKey(LSHIFT))
            t.Move(Vect3f32(0.0f, -1.0f, 0.0f) * dt );
        
        if (_im.IsKey(NUMPAD8))
            t.AddYPR(Vect3f32(0.0f, 1.0f, 0.0f) * dt );
        if (_im.IsKey(NUMPAD5))
            t.AddYPR(Vect3f32(0.0f, -1.0f, 0.0f) * dt );
        if (_im.IsKey(NUMPAD4))
            t.AddYPR(Vect3f32(-1.0f, 0.0f, 0.0f) * dt );
        if (_im.IsKey(NUMPAD6))
            t.AddYPR(Vect3f32(1.0f, 0.0f, 0.0f) * dt );
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
    
    void HandleFrustumInput(InputManager& _im, Transform& t)
    {
        if (_im.IsKey(LCONTROL) == false)
            return;
        
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
        if (_im.IsKey(LSHIFT))
            t.Move(Vect3f32(0.0f, -1.0f, 0.0f) * dt );
        
        if (_im.IsKey(NUMPAD8))
            t.AddYPR(Vect3f32(0.0f, 1.0f, 0.0f) * dt );
        if (_im.IsKey(NUMPAD5))
            t.AddYPR(Vect3f32(0.0f, -1.0f, 0.0f) * dt );
        if (_im.IsKey(NUMPAD4))
            t.AddYPR(Vect3f32(-1.0f, 0.0f, 0.0f) * dt );
        if (_im.IsKey(NUMPAD6))
            t.AddYPR(Vect3f32(1.0f, 0.0f, 0.0f) * dt );
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
    
    void HandleCamInput(InputManager& _im)
    {
        if (_im.IsKeyDown(_1))
        {
            Vect3f32 pos = Vect3f32(0.0f, 0.0f, -5.0f);
            camT.SetPosition(pos);
        }
        if (_im.IsKeyDown(_2))
        {
            Vect3f32 pos = Vect3f32(5.0f, 0.0f, 0.0f);
            camT.SetPosition(pos); 
        }
        if (_im.IsKeyDown(_3))
        {
            Vect3f32 pos = Vect3f32(0.1f, 5.0f, 0.0f);
            camT.SetPosition(pos); 
        }
        
        Vect3f32 target = Vect3f32(0.0f, 0.0f, 0.0f);
        camT.LookAt(target);
    }
    
    void DrawLine(Device* _d, Vect3f32 _p1, Vect3f32 _p2)
    {
        Mat4f32 t = Mat4f32::MakeLineToLineTransform(Vect3f32(0.0f), Vect3f32(1.0f, 0.0f, 0.0f), _p1, _p2);
        
        _d->Draw(line, t);
    }
    
    void DrawAABB(Device* _d, AABB _a)
    {
        Mat4f32 scale;
        scale.m00 = _a.Extent().x * 2.0f;
        scale.m11 = _a.Extent().y * 2.0f;
        scale.m22 = _a.Extent().z * 2.0f;
        scale.rows[3] = Vect4f32(_a.Center(), 1.0f);
        
        _d->Draw(cube, scale);
    }
    
    void DrawSphere(Device* _d, Sphere _s)
    {
        Mat4f32 scale;
        scale.m00 = _s.radius * 2.0f;
        scale.m11 = _s.radius * 2.0f;
        scale.m22 = _s.radius * 2.0f;
        scale.rows[3] = Vect4f32(_s.center, 1.0f);
        
        _d->Draw(sphere, scale);
    }
    
    void DrawOBB(Device* _d, OBB _o)
    {
        Mat4f32 t = Mat4f32::MakeTransform(_o.position, _o.extent * 2.0f, _o.orientation.ToMatrix4());
        
        _d->Draw(cube, t);
    }
    
    void DrawFrustum(Device* _d, Frustum _f)
    {
        Vect3f32 points[8];
        Plane::ThreeWayIntersect(_f.nearZ, _f.left, _f.top, &points[0]);
        Plane::ThreeWayIntersect(_f.nearZ, _f.right, _f.top, &points[1]);
        Plane::ThreeWayIntersect(_f.nearZ, _f.right, _f.bottom, &points[2]);
        Plane::ThreeWayIntersect(_f.nearZ, _f.left, _f.bottom, &points[3]);
        
        Plane::ThreeWayIntersect(_f.farZ, _f.left, _f.top, &points[4]);
        Plane::ThreeWayIntersect(_f.farZ, _f.right, _f.top, &points[5]);
        Plane::ThreeWayIntersect(_f.farZ, _f.right, _f.bottom, &points[6]);
        Plane::ThreeWayIntersect(_f.farZ, _f.left, _f.bottom, &points[7]);
        
        for (int i = 0; i < 4; i++)
            DrawLine(_d, points[i], points[(i + 1) % 4]);
        
        for (int i = 0; i < 4; i++)
            DrawLine(_d, points[4 + i], points[4 + (i + 1) % 4]);
        
        for (int i = 0; i < 4; i++)
            DrawLine(_d, points[i], points[4 + i]);
    }
    
    void DrawRay(Device* _d, Ray _r)
    {
        Vect3f32 origin = _r.origin;
        Vect3f32 end = _r.origin + _r.direction * _r.length;
        
        Mat4f32 t = Mat4f32::MakeLineToLineTransform(
            Vect3f32(0.0f, 0.0f, 0.0f), Vect3f32(1.0f, 0.0f, 0.0f),
            origin, end);
        
        _d->Draw(line, t);
    }
        
    void Run()
    {
        Window window(1080, 720, L"Test", false);
        window.InitD3D12();

        Device* pDevice = window.GetDevice();

        Shader* s = ShaderFactory::CreateWireframe(pDevice);
        Material* greenWF = s->CreateMaterial();
        greenWF->SetFloat4("Color", {0.0f, 1.0f, 0.0f, 1.0f});
        Material* redWF = s->CreateMaterial();
        redWF->SetFloat4("Color", {1.0f, 0.0f, 0.0f, 1.0f});
        
        Shader* s2 = ShaderFactory::CreateLitColored(pDevice);
        Material* white = s2->CreateMaterial();
        white->SetFloat4("DiffuseAlbedo", {1.0f, 1.0f, 1.0f, 1.0f});

        line = GeometryFactory::BuildLine(pDevice);
        cube = GeometryFactory::BuildCube(pDevice);
        sphere = GeometryFactory::BuildIcosphere(pDevice, 2);
        
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
        Transform fT;
        AABB aabb = AABB(Vect3f32(-0.1), Vect3f32(0.1));
        float rad = Vect3f32(0.5f).Length();
        Sphere o = Sphere(Vect3f32(), 0.2f);
        OBB obb = OBB(Vect3f32(), Vect3f32(0.5f, 0.2f, 0.1f));
        
        Ray r = Ray(Vect3f32(0.0f), Vect3f32(1.0f, 0.0, 0.0f), 3.0f);
        
        Mat4f32 proj = Mat4f32::MakePerspective(0.5f, 16.f / 9.0f, 0.1f, 3.0f);
        
        while (window.IsOpen())
        {
            HandleObjectInput(inputManager, t);
            HandleFrustumInput(inputManager, fT);
            HandleCamInput(inputManager);
            
            Mat4f32 view = fT.GetInvMatrix();
            Frustum frustum(view, proj);
            
            window.Update();
            window.Clear();

            pDevice->SetMaterial(white);
            DrawFrustum(pDevice, frustum);
            
            if (frustum.Intersects(obb.Transformed(t.GetMatrix())))
            {
                pDevice->SetMaterial(redWF);
                // intersect.SetPosition(intersectPos);
                //
                // XMFLOAT4X4 m = ToD3DMatrix(intersect.GetMatrix());
                //
                // pDevice->Draw(sphere, m);
            }
            else
                pDevice->SetMaterial(greenWF);
            
            DrawOBB(pDevice, obb.Transformed(t.GetMatrix()));
            // DrawAABB(pDevice, aabb);
            // DrawSphere(pDevice, o.Transformed(fT.GetMatrix()));
            // DrawSphere(pDevice, o);
            DrawSphere(pDevice, o.Transformed(fT.GetMatrix()));
            // DrawOBB(pDevice, obb);
            // DrawRay(pDevice, r);
            
            window.Display();
        }
    }
};

#endif
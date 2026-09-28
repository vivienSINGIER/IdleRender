#ifndef TEST_DEBUG_H_DEFINED
#define TEST_DEBUG_H_DEFINED

#include "Test.hpp"
#include "../Render/Generic/Render.h"
#include "Core/Math/Matrix/Matrix.h"
#include "Core/Math/Vector/Vector.h"
#include "Render/Generic/FontRendering/Text.hpp"

class TestDebug : public Test
{
public:
    float Run()
    {
        Window window(1080, 720, L"Test", false);
        window.InitD3D12();

        Device* pDevice = window.GetDevice();
        
        Shader* s = ShaderFactory::CreateUnlitColored(pDevice);
        Material* green = s->CreateMaterial();
        green->SetFloat4("Color", {0.0f, 1.0f, 0.0f, 1.0f});

        Geometry* line = GeometryFactory::BuildLine(pDevice);
        Geometry* cube = GeometryFactory::BuildCube(pDevice);

        Camera cam;
        Transform camT;
        camT.SetPosition(Vect3f32(0.0f, 0.0f, -5.0f));
        camT.LookAt({0.0f, 0.0f, 0.0f});
        
        cam.SetWorld(camT.GetMatrix());
        pDevice->SetMainCamera(&cam);

        Mat4f32 m1 = Mat4f32::MakeLineToLineTransform(
            Vect3f32(0.0f, 0.0f, 0.0f), Vect3f32(1.0f, 0.0f, 0.0f),
            Vect3f32(0.0f, 1.0f, 0.0f), Vect3f32(2.0f, 0.0f, 0.0f) );

        Mat4f32 m2 = Mat4f32::MakeLineToLineTransform(
            Vect3f32(0.0f, 0.0f, 0.0f), Vect3f32(1.0f, 0.0f, 0.0f),
            Vect3f32(0.0f, 0.0f, 0.0f), Vect3f32(0.0f, 1.0f, 0.0f) );

        Mat4f32 m3 = Mat4f32::MakeLineToLineTransform(
            Vect3f32(0.0f, 0.0f, 0.0f), Vect3f32(1.0f, 0.0f, 0.0f),
            Vect3f32(0.0f, 0.0f, 0.0f), Vect3f32(2.0f, 0.0f, 0.0f));

        Vect4f32 a1 = Vect4f32(0.0f, 0.0f, 0.0f, 1.0f);
        Vect4f32 a2 = Vect4f32(1.0f, 0.0f, 0.0f, 1.0f);

        Vect4f32 a1m1 = (a1 * m1);
        Vect4f32 a2m1 = (a2 * m1);
        Vect4f32 a1m2 = (a1 * m2);
        Vect4f32 a2m2 = (a2 * m2);
        Vect4f32 a1m3 = (a1 * m3);
        Vect4f32 a2m3 = (a2 * m3);
        
        std::cout << "a1 * M1 = " << a1m1<< std::endl;
        std::cout << "a2 * M1 = " << a2m1 << std::endl;
        std::cout << "a1 * M2 = " << a1m2 << std::endl;
        std::cout << "a2 * M2 = " << a2m2 << std::endl;
        std::cout << "a1 * M3 = " << a1m3 << std::endl;
        std::cout << "a2 * M3 = " << a2m3 << std::endl;
        
        while (window.IsOpen())
        {
            window.Update();
            window.Clear();

            pDevice->SetMaterial(green);
            pDevice->Draw(line, m1);
            pDevice->Draw(line, m2);
            pDevice->Draw(line, m3);
            
            window.Display();
        }
        return 0.0f;
    }
};

#endif
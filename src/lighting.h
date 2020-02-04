#pragma once


#include "z_buffer_culling.h"


class Lighting : public ZCulling
{
public:
	Lighting(USHORT width, USHORT height, const std::string& obj_file);

	void DrawScene();
    void DrawFace(face objFace, const float4x4& camera, const float4x4& light, float screenScale, float4 screenShift);
	int LoadTexture(const std::string& path);
    void TransformObject(std::vector<face>& faces, float moveX, float moveY, float moveZ, float rotateX, float rotateY, float rotateZ, float scale);
    float4x4 MakeCamera(float3 eyeV, float3 atV, float3 upV);
	float4x4 MakeLight(float3 eyeV, float3 atV, float3 upV);

protected:
    //void BakeShadows();
	std::vector<color> texture;
	int textureHeight, textureWidth;
	std::vector<float> depthMap;
};

bool validate_framebuffer(std::string reference_file, std::vector<color> frame_buffer);
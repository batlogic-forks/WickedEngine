#include "stdafx.h"
#include "Translator.h"
#include "wiRenderer.h"
#include "wiInput.h"
#include "wiMath.h"
#include "ShaderInterop_Renderer.h"

using namespace wiGraphics;
using namespace wiECS;
using namespace wiScene;

PipelineState pso_solidpart;
PipelineState pso_wirepart;
GPUBuffer vertexBuffer_Axis;
GPUBuffer vertexBuffer_Plane;
GPUBuffer vertexBuffer_Origin;
UINT vertexCount_Axis = 0;
UINT vertexCount_Plane = 0;
UINT vertexCount_Origin = 0;

void Translator::LoadShaders()
{
	GraphicsDevice* device = wiRenderer::GetDevice();

	{
		PipelineStateDesc desc;

		desc.vs = wiRenderer::GetVertexShader(VSTYPE_VERTEXCOLOR);
		desc.ps = wiRenderer::GetPixelShader(PSTYPE_VERTEXCOLOR);
		desc.il = wiRenderer::GetInputLayout(ILTYPE_VERTEXCOLOR);
		desc.dss = wiRenderer::GetDepthStencilState(DSSTYPE_XRAY);
		desc.rs = wiRenderer::GetRasterizerState(RSTYPE_DOUBLESIDED);
		desc.bs = wiRenderer::GetBlendState(BSTYPE_ADDITIVE);
		desc.pt = TRIANGLELIST;

		device->CreatePipelineState(&desc, &pso_solidpart);
	}

	{
		PipelineStateDesc desc;

		desc.vs = wiRenderer::GetVertexShader(VSTYPE_VERTEXCOLOR);
		desc.ps = wiRenderer::GetPixelShader(PSTYPE_VERTEXCOLOR);
		desc.il = wiRenderer::GetInputLayout(ILTYPE_VERTEXCOLOR);
		desc.dss = wiRenderer::GetDepthStencilState(DSSTYPE_XRAY);
		desc.rs = wiRenderer::GetRasterizerState(RSTYPE_WIRE_DOUBLESIDED_SMOOTH);
		desc.bs = wiRenderer::GetBlendState(BSTYPE_TRANSPARENT);
		desc.pt = LINELIST;

		device->CreatePipelineState(&desc, &pso_wirepart);
	}
}

Translator::Translator()
{
	GraphicsDevice* device = wiRenderer::GetDevice();

	if (!vertexBuffer_Axis.IsValid())
	{
		{
			XMFLOAT4 verts[] = {
				XMFLOAT4(0,0,0,1), XMFLOAT4(1,1,1,1),
				XMFLOAT4(3,0,0,1), XMFLOAT4(1,1,1,1),
			};
			vertexCount_Axis = arraysize(verts) / 2;

			GPUBufferDesc bd;
			bd.Usage = USAGE_DEFAULT;
			bd.ByteWidth = sizeof(verts);
			bd.BindFlags = BIND_VERTEX_BUFFER;
			bd.CPUAccessFlags = 0;

			SubresourceData InitData;
			InitData.pSysMem = verts;

			device->CreateBuffer(&bd, &InitData, &vertexBuffer_Axis);
		}
	}

	if (!vertexBuffer_Plane.IsValid())
	{
		{
			XMFLOAT4 verts[] = {
				XMFLOAT4(0,0,0,1), XMFLOAT4(1,1,1,1),
				XMFLOAT4(1,0,0,1), XMFLOAT4(1,1,1,1),
				XMFLOAT4(1,1,0,1), XMFLOAT4(1,1,1,1),

				XMFLOAT4(0,0,0,1), XMFLOAT4(1,1,1,1),
				XMFLOAT4(1,1,0,1), XMFLOAT4(1,1,1,1),
				XMFLOAT4(0,1,0,1), XMFLOAT4(1,1,1,1),
			};
			vertexCount_Plane = arraysize(verts) / 2;

			GPUBufferDesc bd;
			bd.Usage = USAGE_DEFAULT;
			bd.ByteWidth = sizeof(verts);
			bd.BindFlags = BIND_VERTEX_BUFFER;
			bd.CPUAccessFlags = 0;

			SubresourceData InitData;
			InitData.pSysMem = verts;
			device->CreateBuffer(&bd, &InitData, &vertexBuffer_Plane);
		}
	}

	if (!vertexBuffer_Origin.IsValid())
	{
		{
			float edge = 0.2f;
			XMFLOAT4 verts[] = {
				XMFLOAT4(-edge,edge,edge,1),   XMFLOAT4(1,1,1,1),
				XMFLOAT4(-edge,-edge,edge,1),  XMFLOAT4(1,1,1,1),
				XMFLOAT4(-edge,-edge,-edge,1), XMFLOAT4(1,1,1,1),
				XMFLOAT4(edge,edge,edge,1),	   XMFLOAT4(1,1,1,1),
				XMFLOAT4(edge,-edge,edge,1),   XMFLOAT4(1,1,1,1),
				XMFLOAT4(-edge,-edge,edge,1),  XMFLOAT4(1,1,1,1),
				XMFLOAT4(edge,edge,-edge,1),   XMFLOAT4(1,1,1,1),
				XMFLOAT4(edge,-edge,-edge,1),  XMFLOAT4(1,1,1,1),
				XMFLOAT4(edge,-edge,edge,1),   XMFLOAT4(1,1,1,1),
				XMFLOAT4(-edge,edge,-edge,1),  XMFLOAT4(1,1,1,1),
				XMFLOAT4(-edge,-edge,-edge,1), XMFLOAT4(1,1,1,1),
				XMFLOAT4(edge,-edge,-edge,1),  XMFLOAT4(1,1,1,1),
				XMFLOAT4(-edge,-edge,edge,1),  XMFLOAT4(1,1,1,1),
				XMFLOAT4(edge,-edge,edge,1),   XMFLOAT4(1,1,1,1),
				XMFLOAT4(edge,-edge,-edge,1),  XMFLOAT4(1,1,1,1),
				XMFLOAT4(edge,edge,edge,1),	   XMFLOAT4(1,1,1,1),
				XMFLOAT4(-edge,edge,edge,1),   XMFLOAT4(1,1,1,1),
				XMFLOAT4(-edge,edge,-edge,1),  XMFLOAT4(1,1,1,1),
				XMFLOAT4(-edge,edge,-edge,1),  XMFLOAT4(1,1,1,1),
				XMFLOAT4(-edge,edge,edge,1),   XMFLOAT4(1,1,1,1),
				XMFLOAT4(-edge,-edge,-edge,1), XMFLOAT4(1,1,1,1),
				XMFLOAT4(-edge,edge,edge,1),   XMFLOAT4(1,1,1,1),
				XMFLOAT4(edge,edge,edge,1),	   XMFLOAT4(1,1,1,1),
				XMFLOAT4(-edge,-edge,edge,1),  XMFLOAT4(1,1,1,1),
				XMFLOAT4(edge,edge,edge,1),	   XMFLOAT4(1,1,1,1),
				XMFLOAT4(edge,edge,-edge,1),   XMFLOAT4(1,1,1,1),
				XMFLOAT4(edge,-edge,edge,1),   XMFLOAT4(1,1,1,1),
				XMFLOAT4(edge,edge,-edge,1),   XMFLOAT4(1,1,1,1),
				XMFLOAT4(-edge,edge,-edge,1),  XMFLOAT4(1,1,1,1),
				XMFLOAT4(edge,-edge,-edge,1),  XMFLOAT4(1,1,1,1),
				XMFLOAT4(-edge,-edge,-edge,1), XMFLOAT4(1,1,1,1),
				XMFLOAT4(-edge,-edge,edge,1),  XMFLOAT4(1,1,1,1),
				XMFLOAT4(edge,-edge,-edge,1),  XMFLOAT4(1,1,1,1),
				XMFLOAT4(edge,edge,-edge,1),   XMFLOAT4(1,1,1,1),
				XMFLOAT4(edge,edge,edge,1),	   XMFLOAT4(1,1,1,1),
				XMFLOAT4(-edge,edge,-edge,1),  XMFLOAT4(1,1,1,1),
			};
			vertexCount_Origin = arraysize(verts) / 2;

			GPUBufferDesc bd;
			bd.Usage = USAGE_DEFAULT;
			bd.ByteWidth = sizeof(verts);
			bd.BindFlags = BIND_VERTEX_BUFFER;
			bd.CPUAccessFlags = 0;

			SubresourceData InitData;
			InitData.pSysMem = verts;
			device->CreateBuffer(&bd, &InitData, &vertexBuffer_Origin);
		}
	}
}

void Translator::Update()
{
	if (selected.empty())
	{
		return;
	}

	dragStarted = false;
	dragEnded = false;

	XMFLOAT4 pointer = wiInput::GetPointer();
	const CameraComponent& cam = wiRenderer::GetCamera();
	XMVECTOR pos = transform.GetPositionV();

	if (enabled)
	{
		PreTranslate();

		if (!dragging)
		{
			XMMATRIX P = cam.GetProjection();
			XMMATRIX V = cam.GetView();
			XMMATRIX W = XMMatrixIdentity();

			dist = wiMath::Distance(transform.GetPosition(), cam.Eye) * 0.05f;

			XMVECTOR o, x, y, z, p, xy, xz, yz;

			o = pos;
			x = o + XMVectorSet(3, 0, 0, 0) * dist;
			y = o + XMVectorSet(0, 3, 0, 0) * dist;
			z = o + XMVectorSet(0, 0, 3, 0) * dist;
			p = XMLoadFloat4(&pointer);
			xy = o + XMVectorSet(0.5f, 0.5f, 0, 0) * dist;
			xz = o + XMVectorSet(0.5f, 0, 0.5f, 0) * dist;
			yz = o + XMVectorSet(0, 0.5f, 0.5f, 0) * dist;


			o = XMVector3Project(o, 0, 0, cam.width, cam.height, 0, 1, P, V, W);
			x = XMVector3Project(x, 0, 0, cam.width, cam.height, 0, 1, P, V, W);
			y = XMVector3Project(y, 0, 0, cam.width, cam.height, 0, 1, P, V, W);
			z = XMVector3Project(z, 0, 0, cam.width, cam.height, 0, 1, P, V, W);
			xy = XMVector3Project(xy, 0, 0, cam.width, cam.height, 0, 1, P, V, W);
			xz = XMVector3Project(xz, 0, 0, cam.width, cam.height, 0, 1, P, V, W);
			yz = XMVector3Project(yz, 0, 0, cam.width, cam.height, 0, 1, P, V, W);

			XMVECTOR oDisV = XMVector3Length(o - p);
			XMVECTOR xyDisV = XMVector3Length(xy - p);
			XMVECTOR xzDisV = XMVector3Length(xz - p);
			XMVECTOR yzDisV = XMVector3Length(yz - p);

			float xDis = wiMath::GetPointSegmentDistance(p, o, x);
			float yDis = wiMath::GetPointSegmentDistance(p, o, y);
			float zDis = wiMath::GetPointSegmentDistance(p, o, z);
			float oDis = XMVectorGetX(oDisV);
			float xyDis = XMVectorGetX(xyDisV);
			float xzDis = XMVectorGetX(xzDisV);
			float yzDis = XMVectorGetX(yzDisV);

			if (oDis < 10)
			{
				state = TRANSLATOR_XYZ;
			}
			else if (xyDis < 20)
			{
				state = TRANSLATOR_XY;
			}
			else if (xzDis < 20)
			{
				state = TRANSLATOR_XZ;
			}
			else if (yzDis < 20)
			{
				state = TRANSLATOR_YZ;
			}
			else if (xDis < 10)
			{
				state = TRANSLATOR_X;
			}
			else if (yDis < 10)
			{
				state = TRANSLATOR_Y;
			}
			else if (zDis < 10)
			{
				state = TRANSLATOR_Z;
			}
			else if (!dragging)
			{
				state = TRANSLATOR_IDLE;
			}
		}

		if (dragging || (state != TRANSLATOR_IDLE && wiInput::Down(wiInput::MOUSE_BUTTON_LEFT)))
		{
			if (!dragging)
			{
				dragStarted = true;
				dragDeltaMatrix = IDENTITYMATRIX;
			}

			XMVECTOR plane, planeNormal;
			if (state == TRANSLATOR_X)
			{
				XMVECTOR axis = XMVectorSet(1, 0, 0, 0);
				XMVECTOR wrong = XMVector3Cross(cam.GetAt(), axis);
				planeNormal = XMVector3Cross(wrong, axis);
			}
			else if (state == TRANSLATOR_Y)
			{
				XMVECTOR axis = XMVectorSet(0, 1, 0, 0);
				XMVECTOR wrong = XMVector3Cross(cam.GetAt(), axis);
				planeNormal = XMVector3Cross(wrong, axis);
			}
			else if (state == TRANSLATOR_Z)
			{
				XMVECTOR axis = XMVectorSet(0, 0, 1, 0);
				XMVECTOR wrong = XMVector3Cross(cam.GetAt(), axis);
				planeNormal = XMVector3Cross(wrong, axis);
			}
			else if (state == TRANSLATOR_XY)
			{
				planeNormal = XMVectorSet(0, 0, 1, 0);
			}
			else if (state == TRANSLATOR_XZ)
			{
				planeNormal = XMVectorSet(0, 1, 0, 0);
			}
			else if (state == TRANSLATOR_YZ)
			{
				planeNormal = XMVectorSet(1, 0, 0, 0);
			}
			else
			{
				// xyz
				planeNormal = cam.GetAt();
			}
			plane = XMPlaneFromPointNormal(pos, XMVector3Normalize(planeNormal));

			RAY ray = wiRenderer::GetPickRay((long)pointer.x, (long)pointer.y);
			XMVECTOR rayOrigin = XMLoadFloat3(&ray.origin);
			XMVECTOR rayDir = XMLoadFloat3(&ray.direction);
			XMVECTOR intersection = XMPlaneIntersectLine(plane, rayOrigin, rayOrigin + rayDir*cam.zFarP);

			ray = wiRenderer::GetPickRay((long)prevPointer.x, (long)prevPointer.y);
			rayOrigin = XMLoadFloat3(&ray.origin);
			rayDir = XMLoadFloat3(&ray.direction);
			XMVECTOR intersectionPrev = XMPlaneIntersectLine(plane, rayOrigin, rayOrigin + rayDir*cam.zFarP);

			XMVECTOR deltaV;
			if (state == TRANSLATOR_X)
			{
				XMVECTOR A = pos, B = pos + XMVectorSet(1, 0, 0, 0);
				XMVECTOR P = wiMath::GetClosestPointToLine(A, B, intersection);
				XMVECTOR PPrev = wiMath::GetClosestPointToLine(A, B, intersectionPrev);
				deltaV = P - PPrev;
			}
			else if (state == TRANSLATOR_Y)
			{
				XMVECTOR A = pos, B = pos + XMVectorSet(0, 1, 0, 0);
				XMVECTOR P = wiMath::GetClosestPointToLine(A, B, intersection);
				XMVECTOR PPrev = wiMath::GetClosestPointToLine(A, B, intersectionPrev);
				deltaV = P - PPrev;
			}
			else if (state == TRANSLATOR_Z)
			{
				XMVECTOR A = pos, B = pos + XMVectorSet(0, 0, 1, 0);
				XMVECTOR P = wiMath::GetClosestPointToLine(A, B, intersection);
				XMVECTOR PPrev = wiMath::GetClosestPointToLine(A, B, intersectionPrev);
				deltaV = P - PPrev;
			}
			else
			{
				deltaV = intersection - intersectionPrev;

				if (isScalator)
				{
					deltaV = XMVectorSplatY(deltaV);
				}
			}

			XMFLOAT3 delta;
			if (isRotator)
			{
				deltaV /= XMVector3Length(intersection - rayOrigin);
				deltaV *= XM_2PI;
			}
			XMStoreFloat3(&delta, deltaV);

			if (isTranslator)
			{
				XMStoreFloat4x4(&dragDeltaMatrix, XMMatrixTranslation(delta.x, delta.y, delta.z) * XMLoadFloat4x4(&dragDeltaMatrix));
				transform.Translate(delta);
			}
			if (isRotator)
			{
				XMMATRIX R = XMMatrixRotationRollPitchYaw(delta.x, delta.y, delta.z);
				XMStoreFloat4x4(&dragDeltaMatrix, R * XMLoadFloat4x4(&dragDeltaMatrix));
				XMVECTOR Q = XMQuaternionRotationMatrix(R);
				XMFLOAT4 quat;
				XMStoreFloat4(&quat, Q);
				transform.Rotate(quat);
			}
			if (isScalator)
			{
				XMFLOAT3 scale = transform.GetScale();
				scale = XMFLOAT3((1.0f / scale.x) * (scale.x + delta.x), (1.0f / scale.y) * (scale.y + delta.y), (1.0f / scale.z) * (scale.z + delta.z));
				XMStoreFloat4x4(&dragDeltaMatrix, XMMatrixScaling(scale.x, scale.y, scale.z) * XMLoadFloat4x4(&dragDeltaMatrix));
				transform.Scale(scale);
			}
			transform.UpdateTransform();

			dragging = true;
		}

		if (!wiInput::Down(wiInput::MOUSE_BUTTON_LEFT))
		{
			if (dragging)
			{
				dragEnded = true;
			}
			dragging = false;
		}

		PostTranslate();

	}
	else
	{
		if (dragging)
		{
			dragEnded = true;
		}
		dragging = false;
	}

	prevPointer = pointer;
}
void Translator::Draw(const CameraComponent& camera, CommandList cmd) const
{
	if (selected.empty())
	{
		return;
	}

	static bool shaders_loaded = false;
	if (!shaders_loaded)
	{
		shaders_loaded = true;
		LoadShaders();
	}

	Scene& scene = wiScene::GetScene();

	GraphicsDevice* device = wiRenderer::GetDevice();

	device->EventBegin("Editor - Translator", cmd);

	XMMATRIX VP = camera.GetViewProjection();

	MiscCB sb;

	XMMATRIX mat = XMMatrixScaling(dist, dist, dist)*XMMatrixTranslationFromVector(transform.GetPositionV()) * VP;
	XMMATRIX matX = mat;
	XMMATRIX matY = XMMatrixRotationZ(XM_PIDIV2)*XMMatrixRotationY(XM_PIDIV2)*mat;
	XMMATRIX matZ = XMMatrixRotationY(-XM_PIDIV2)*XMMatrixRotationZ(-XM_PIDIV2)*mat;

	// Planes:
	{
		device->BindPipelineState(&pso_solidpart, cmd);
		const GPUBuffer* vbs[] = {
			&vertexBuffer_Plane,
		};
		const UINT strides[] = {
			sizeof(XMFLOAT4) + sizeof(XMFLOAT4),
		};
		device->BindVertexBuffers(vbs, 0, arraysize(vbs), strides, nullptr, cmd);
	}

	// xy
	XMStoreFloat4x4(&sb.g_xTransform, matX);
	sb.g_xColor = state == TRANSLATOR_XY ? XMFLOAT4(1, 1, 1, 1) : XMFLOAT4(0.2f, 0.2f, 0, 0.2f);
	device->UpdateBuffer(wiRenderer::GetConstantBuffer(CBTYPE_MISC), &sb, cmd);
	device->BindConstantBuffer(VS, wiRenderer::GetConstantBuffer(CBTYPE_MISC), CB_GETBINDSLOT(MiscCB), cmd);
	device->BindConstantBuffer(PS, wiRenderer::GetConstantBuffer(CBTYPE_MISC), CB_GETBINDSLOT(MiscCB), cmd);
	device->Draw(vertexCount_Plane, 0, cmd);

	// xz
	XMStoreFloat4x4(&sb.g_xTransform, matZ);
	sb.g_xColor = state == TRANSLATOR_XZ ? XMFLOAT4(1, 1, 1, 1) : XMFLOAT4(0.2f, 0.2f, 0, 0.2f);
	device->UpdateBuffer(wiRenderer::GetConstantBuffer(CBTYPE_MISC), &sb, cmd);
	device->BindConstantBuffer(VS, wiRenderer::GetConstantBuffer(CBTYPE_MISC), CB_GETBINDSLOT(MiscCB), cmd);
	device->BindConstantBuffer(PS, wiRenderer::GetConstantBuffer(CBTYPE_MISC), CB_GETBINDSLOT(MiscCB), cmd);
	device->Draw(vertexCount_Plane, 0, cmd);

	// yz
	XMStoreFloat4x4(&sb.g_xTransform, matY);
	sb.g_xColor = state == TRANSLATOR_YZ ? XMFLOAT4(1, 1, 1, 1) : XMFLOAT4(0.2f, 0.2f, 0, 0.2f);
	device->UpdateBuffer(wiRenderer::GetConstantBuffer(CBTYPE_MISC), &sb, cmd);
	device->BindConstantBuffer(VS, wiRenderer::GetConstantBuffer(CBTYPE_MISC), CB_GETBINDSLOT(MiscCB), cmd);
	device->BindConstantBuffer(PS, wiRenderer::GetConstantBuffer(CBTYPE_MISC), CB_GETBINDSLOT(MiscCB), cmd);
	device->Draw(vertexCount_Plane, 0, cmd);

	// Lines:
	{
		device->BindPipelineState(&pso_wirepart, cmd);
		const GPUBuffer* vbs[] = {
			&vertexBuffer_Axis,
		};
		const UINT strides[] = {
			sizeof(XMFLOAT4) + sizeof(XMFLOAT4),
		};
		device->BindVertexBuffers(vbs, 0, arraysize(vbs), strides, nullptr, cmd);
	}

	// x
	XMStoreFloat4x4(&sb.g_xTransform, matX);
	sb.g_xColor = state == TRANSLATOR_X ? XMFLOAT4(1, 1, 1, 1) : XMFLOAT4(1, 0, 0, 1);
	device->UpdateBuffer(wiRenderer::GetConstantBuffer(CBTYPE_MISC), &sb, cmd);
	device->BindConstantBuffer(VS, wiRenderer::GetConstantBuffer(CBTYPE_MISC), CB_GETBINDSLOT(MiscCB), cmd);
	device->BindConstantBuffer(PS, wiRenderer::GetConstantBuffer(CBTYPE_MISC), CB_GETBINDSLOT(MiscCB), cmd);
	device->Draw(vertexCount_Axis, 0, cmd);

	// y
	XMStoreFloat4x4(&sb.g_xTransform, matY);
	sb.g_xColor = state == TRANSLATOR_Y ? XMFLOAT4(1, 1, 1, 1) : XMFLOAT4(0, 1, 0, 1);
	device->UpdateBuffer(wiRenderer::GetConstantBuffer(CBTYPE_MISC), &sb, cmd);
	device->BindConstantBuffer(VS, wiRenderer::GetConstantBuffer(CBTYPE_MISC), CB_GETBINDSLOT(MiscCB), cmd);
	device->BindConstantBuffer(PS, wiRenderer::GetConstantBuffer(CBTYPE_MISC), CB_GETBINDSLOT(MiscCB), cmd);
	device->Draw(vertexCount_Axis, 0, cmd);

	// z
	XMStoreFloat4x4(&sb.g_xTransform, matZ);
	sb.g_xColor = state == TRANSLATOR_Z ? XMFLOAT4(1, 1, 1, 1) : XMFLOAT4(0, 0, 1, 1);
	device->UpdateBuffer(wiRenderer::GetConstantBuffer(CBTYPE_MISC), &sb, cmd);
	device->BindConstantBuffer(VS, wiRenderer::GetConstantBuffer(CBTYPE_MISC), CB_GETBINDSLOT(MiscCB), cmd);
	device->BindConstantBuffer(PS, wiRenderer::GetConstantBuffer(CBTYPE_MISC), CB_GETBINDSLOT(MiscCB), cmd);
	device->Draw(vertexCount_Axis, 0, cmd);

	// Origin:
	{
		device->BindPipelineState(&pso_solidpart, cmd);
		const GPUBuffer* vbs[] = {
			&vertexBuffer_Origin,
		};
		const UINT strides[] = {
			sizeof(XMFLOAT4) + sizeof(XMFLOAT4),
		};
		device->BindVertexBuffers(vbs, 0, arraysize(vbs), strides, nullptr, cmd);
		XMStoreFloat4x4(&sb.g_xTransform, mat);
		sb.g_xColor = state == TRANSLATOR_XYZ ? XMFLOAT4(1, 1, 1, 1) : XMFLOAT4(0.25f, 0.25f, 0.25f, 1);
		device->UpdateBuffer(wiRenderer::GetConstantBuffer(CBTYPE_MISC), &sb, cmd);
		device->BindConstantBuffer(VS, wiRenderer::GetConstantBuffer(CBTYPE_MISC), CB_GETBINDSLOT(MiscCB), cmd);
		device->BindConstantBuffer(PS, wiRenderer::GetConstantBuffer(CBTYPE_MISC), CB_GETBINDSLOT(MiscCB), cmd);
		device->Draw(vertexCount_Origin, 0, cmd);
	}

	device->EventEnd(cmd);
}

void Translator::PreTranslate()
{
	Scene& scene = wiScene::GetScene();

	// Find the center of all the entities that are selected:
	XMVECTOR centerV = XMVectorSet(0, 0, 0, 0);
	float count = 0;
	for (auto& x : selected)
	{
		TransformComponent* transform = scene.transforms.GetComponent(x.entity);
		if (transform != nullptr)
		{
			centerV = XMVectorAdd(centerV, transform->GetPositionV());
			count += 1.0f;
		}
	}

	// Offset translator to center position and perform attachments:
	if (count > 0)
	{
		centerV /= count;
		XMFLOAT3 center;
		XMStoreFloat3(&center, centerV);
		transform.ClearTransform();
		transform.Translate(center);
		transform.UpdateTransform();
	}

	// translator "bind matrix"
	XMMATRIX B = XMMatrixInverse(nullptr, XMLoadFloat4x4(&transform.world));

	for (auto& x : selected)
	{
		TransformComponent* transform_selected = scene.transforms.GetComponent(x.entity);
		if (transform_selected != nullptr)
		{
			// selected to world space:
			transform_selected->ApplyTransform();

			// selected to translator local space:
			transform_selected->MatrixTransform(B);
		}
	}
}
void Translator::PostTranslate()
{
	Scene& scene = wiScene::GetScene();

	for (auto& x : selected)
	{
		TransformComponent* transform_selected = scene.transforms.GetComponent(x.entity);
		if (transform_selected != nullptr)
		{
			transform_selected->UpdateTransform_Parented(transform);

			// selected to world space:
			transform_selected->ApplyTransform();

			// selected to parent local space (if has parent):
			const HierarchyComponent* hier = scene.hierarchy.GetComponent(x.entity);
			if (hier != nullptr)
			{
				const TransformComponent* transform_parent = scene.transforms.GetComponent(hier->parentID);
				if (transform_parent != nullptr)
				{
					transform_selected->MatrixTransform(XMMatrixInverse(nullptr, XMLoadFloat4x4(&transform_parent->world)));
				}
			}
		}
	}
}


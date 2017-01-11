#include "Vector.h"
#include "Matrix3.h"

namespace DAVA
{
const Vector2 Vector2::Zero(0.0f, 0.0f);
const Vector2 Vector2::UnitX(1.0f, 0.0f);
const Vector2 Vector2::UnitY(0.0f, 1.0f);

const Vector3 Vector3::Zero(0.0f, 0.0f, 0.0f);
const Vector3 Vector3::UnitX(1.0f, 0.0f, 0.0f);
const Vector3 Vector3::UnitY(0.0f, 1.0f, 0.0f);
const Vector3 Vector3::UnitZ(0.0f, 0.0f, 1.0f);

const Vector4 Vector4::Zero(0.0f, 0.0f, 0.0f, 0.0f);

Vector2 Vector2::Rotate(const Vector2& in, float32 angleRad)
{
    DAVA::Matrix3 rotateMatrix;
    rotateMatrix.BuildRotation(angleRad);
    return in * rotateMatrix;
}
}

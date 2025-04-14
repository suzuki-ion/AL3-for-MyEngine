#pragma once
#include <stdexcept>
#include <cmath>

namespace MyEngine {

struct Matrix4x4;

struct Vector3 {
    // 大量に呼び出されるであろうデフォルトコンストラクタは軽量化のため何もしないようにしておく
    Vector3() noexcept = default;

    constexpr Vector3(float x, float y, float z) noexcept : x(x), y(y), z(z) {}
    explicit constexpr Vector3(float v) noexcept : x(v), y(v), z(v) {}
    Vector3(const Vector3 &vector) : x(vector.x), y(vector.y), z(vector.z) {}

    float operator[](const int index) const noexcept;
    float &operator[](const int index) noexcept;
    Vector3 &operator=(const Vector3 &vector) noexcept;
    Vector3 &operator+=(const Vector3 &vector) noexcept;
    Vector3 &operator-=(const Vector3 &vector) noexcept;
    Vector3 &operator*=(const float scalar) noexcept;
    Vector3 &operator*=(const Vector3 &vector) noexcept;
    Vector3 &operator/=(const float scalar);
    Vector3 &operator/=(const Vector3 &vector);
    bool operator==(const Vector3 &vector) const noexcept;
    bool operator!=(const Vector3 &vector) const noexcept;

    /// @brief ベクトルの内積を計算する
    /// @param vector 内積を計算するベクトル
    /// @return 内積
    [[nodiscard]] constexpr float Dot(const Vector3 &vector) const noexcept;

    /// @brief ベクトルの外積を計算する
    /// @param vector 外積を計算するベクトル
    /// @return 外積
    [[nodiscard]] Vector3 Cross(const Vector3 &vector) const noexcept;

    /// @brief ベクトルの長さを計算する
    /// @return ベクトルの長さ
    [[nodiscard]] float Length() const noexcept;

    /// @brief ベクトルの長さの二乗を計算する
    /// @return ベクトルの長さの二乗
    [[nodiscard]] constexpr float LengthSquared() const noexcept;

    /// @brief ベクトルを正規化する
    /// @return 正規化されたベクトル
    [[nodiscard]] inline const Vector3 Normalize() const;

    /// @brief ベクトルを射影する
    /// @param vector 射影するベクトル
    /// @return 射影されたベクトル
    [[nodiscard]] inline constexpr const Vector3 Projection(const Vector3 &vector) const noexcept;

    /// @brief 垂線を求める
    /// @param vector 垂線を求めるベクトル
    /// @return 垂線
    [[nodiscard]] inline constexpr const Vector3 Rejection(const Vector3 &vector) const noexcept;

    /// @brief ベクトルを反射する
    /// @param normal 法線ベクトル
    /// @return 反射されたベクトル
    [[nodiscard]] inline constexpr const Vector3 Refrection(const Vector3 &normal) const noexcept;

    /// @brief ベクトルを反射する
    /// @param normal 法線ベクトル
    /// @param eta 屈折率
    /// @return 反射されたベクトル
    [[nodiscard]] inline constexpr const Vector3 Refrection(const Vector3 &normal, const float eta) const noexcept;

    /// @brief ベクトル間の距離を計算する
    /// @param vector 距離を計算するベクトル
    /// @return ベクトル間の距離
    [[nodiscard]] inline float Distance(const Vector3 &vector) const;

    /// @brief ベクトルを行列で変換する
    /// @param mat 変換行列
    /// @return 変換されたベクトル
    [[nodiscard]] Vector3 Transform(const Matrix4x4 &mat) const noexcept;

    float x;
    float y;
    float z;
};

inline constexpr const Vector3 operator-(const Vector3 &vector) noexcept;
inline constexpr const Vector3 operator+(const Vector3 &vector1, const Vector3 &vector2) noexcept;
inline constexpr const Vector3 operator-(const Vector3 &vector1, const Vector3 &vector2) noexcept;
inline constexpr const Vector3 operator*(const Vector3 &vector, const float scalar) noexcept;
inline constexpr const Vector3 operator*(const float scalar, const Vector3 &vector) noexcept;
inline constexpr const Vector3 operator*(const Vector3 &vector1, const Vector3 &vector2) noexcept;
inline constexpr const Vector3 operator/(const Vector3 &vector, const float scalar);
inline constexpr const Vector3 operator/(const Vector3 &vector1, const Vector3 &vector2);
inline constexpr const Vector3 operator*(const Matrix4x4 &mat, const Vector3 &vector) noexcept;
inline constexpr const Vector3 operator*(const Vector3 &vector, const Matrix4x4 &mat) noexcept;

} // namespace MyEngine
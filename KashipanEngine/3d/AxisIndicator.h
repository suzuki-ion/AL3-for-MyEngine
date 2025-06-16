#pragma once

namespace KashipanEngine {

// 前方宣言
class Camera;

class AxisIndicator {
public:
    static void Initialize();
    static void Finalize();
    static void Draw(Camera *camera);
};

} // namespace KashipanEngine
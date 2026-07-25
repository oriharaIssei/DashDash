#include "PlayerMoveUtils.h"

/// ECS
#include "component/player/state/PlayerState.h"

/// math
#include "SpringDamper.h"

using namespace OriGine;

namespace PlayerMoveUtils {

bool IsHitGround(const Vec3f& _collNormal) {
    // 正規化済み法線のY成分は、その面が真上を向いていれば1、垂直な壁なら0になる。
    // つまりY成分はそのまま「面の上向き具合」を表すので、閾値と比較するだけで地面と壁を切り分けられる。
    // kGroundCheckThreshold=0.7fは約45度に相当し、これより急な斜面は登れず壁として扱われる
    return _collNormal[Y] > kGroundCheckThreshold;
}

WallContactResult EvaluateWallContact(float _parallelFactor, const PlayerStatus* _status) {
    // 壁に沿って移動している場合は壁衝突とみなす
    if (_parallelFactor > _status->GetWallCheckThreshold()) {
        if (_status->CanWallRun()) {
            return WallContactResult::WallRun;
        }
    }
    return WallContactResult::WallHit;
}

/// <summary>
/// 速度を壁面に沿った成分だけに変換する(壁走りの進行方向)。
/// 速度をそのまま使うと壁にめり込む向きの成分が残り、壁に押し付けられて減速してしまうため、
/// 壁に垂直な成分を取り除いて「壁を滑る」動きにする。
/// </summary>
OriGine::Vec3f ComputeWallRunDirection(const OriGine::Vec3f& _velocity, const OriGine::Vec3f& _wallNormal) {
    // ベクトルの平面への射影: v - n*(v・n)
    // (v・n) は速度のうち壁法線方向の長さ。それを法線方向に引き算することで
    // 壁に垂直な成分が0になり、残りは必ず壁面に平行なベクトルになる
    return _velocity - _wallNormal * OriGine::Vec3f::Dot(_velocity, _wallNormal);
}

/// <summary>
/// ウィリー（壁登り）時の登坂方向を計算する
/// </summary>
/// <param name="_wallNormal">壁の法線</param>
/// <param name="_up">上方向</param>
/// <returns>登坂方向</returns>
OriGine::Vec3f ComputeWheelieDirection(const OriGine::Vec3f& _wallNormal, const OriGine::Vec3f& _up) {
    // ワールドの上方向を壁面へ射影して「壁に貼り付いたまま最も上を向く方向」を求める。
    // 上方向をそのまま使うと壁から浮いてしまい、壁を登っているように見えないため
    float dot            = Vec3f::Dot(_wallNormal, _up);
    Vec3f climbDirection = axisY - (_wallNormal * dot);
    climbDirection       = climbDirection.normalize();
    return climbDirection;
}

bool IsWallRight(const OriGine::Vec3f& _direction, const OriGine::Vec3f& _wallNormal, const OriGine::Vec3f& _up) {
    // Cross(up, wallNormal) は壁に沿って水平に走る軸(壁面の「横方向」)を作る。
    // これと進行方向の内積の符号は、進行方向から見て壁が左右どちら側にあるかで反転するため、
    // 正なら壁は右側と判定できる。角度を求める必要がなく符号だけで済むので計算が軽い
    return Vec3f::Dot(Vec3f::Cross(_up, _wallNormal), _direction) > 0.0f;
}

OriGine::Vec3f UpdatePlanarVelocity(PlayerStatus* _playerStatus, const Vec3f& _velocity, float _sideInput, const Vec3f& _forwardDirection, float _deltaTime) {
    // 正面方向と上方向の外積で右方向を作り、正面/右の2軸からなる水平面上の座標系を組む。
    // ワールドのXZ軸で直接扱わず正面基準にするのは、プレイヤーがどちらを向いていても
    // 「横入力＝画面上の左右」として一貫して扱えるようにするため
    Vec3f rightDirection = axisY.cross(_forwardDirection).normalize();

    // 現在速度を正面/横方向成分に分解
    Vec2f dirVel = {Vec3f::Dot(_velocity, rightDirection), Vec3f::Dot(_velocity, _forwardDirection)};

    // 横入力に応じた目標横速度を算出
    Vec2f targetDirSpeed = _playerStatus->GetCurrentMaxDirectionalSpeed();
    targetDirSpeed[X] *= _sideInput;

    // 現在の最大ギア速度に対する速度割合から、速度に応じた減衰特性を補間する
    // 高速域ほどsmoothTimeを大きく(＝反応を鈍く)することで、低速では小回りが利き、
    // 高速では急ハンドルが効かない挙動になり、スピード感と操作の重みを両立させている
    float speedT     = Vec2f(_velocity[X], _velocity[Z]).length() / _playerStatus->CalculateSpeedByGearLevel(kMaxPlayerGearLevel);
    Vec2f smoothTime = Lerp(_playerStatus->GetMinSmoothTime(), _playerStatus->GetMaxSmoothTime(), speedT);
    Vec2f limitSpeed = Lerp(_playerStatus->GetMinLimitDirectionalAccel(), _playerStatus->GetMaxLimitDirectionalAccel(), speedT);

    // 横/正面成分それぞれをスムーズダンプで目標速度へ近づける
    Vec2f currentDirSpeed = _playerStatus->GetCurrentDirectionalSpeed();
    dirVel[X]             = OriGine::SmoothDamp(dirVel[X], targetDirSpeed[X], currentDirSpeed[X], smoothTime[X], _deltaTime, limitSpeed[X]);
    dirVel[Y]             = OriGine::SmoothDamp(dirVel[Y], targetDirSpeed[Y], currentDirSpeed[Y], smoothTime[Y], _deltaTime, limitSpeed[Y]);
    _playerStatus->SetCurrentDirectionalSpeed(currentDirSpeed);

    Vec3f result = _forwardDirection * dirVel[Y] + rightDirection * dirVel[X];

    // 現在の最大速度を超えないようクランプ
    // 正面/横の2成分を独立に制限すると斜め方向の合成速度が最大値のsqrt(2)倍まで伸びてしまうため、
    // 合成後のベクトル長で判定する。平方根を避けるため両辺を2乗して比較している
    float currentMaxSpeed = _playerStatus->GetCurrentMaxSpeed();
    if (result.lengthSq() >= currentMaxSpeed * currentMaxSpeed) {
        result = result.normalize() * currentMaxSpeed;
    }

    // 落下/ジャンプはRigidbodyの重力側で管理しているため、ここではY成分に手を触れず元の値を戻す。
    // 上のクランプは水平成分のみに掛けたいので、Yの加算はクランプ後に行う必要がある
    return result + Vec3f(0.f, _velocity[Y], 0.f);
}

}

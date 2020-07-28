#include<DxLib.h>
#include<cmath>
#include"Geometry.h"


using namespace std;

///カプセル形状の描画
void DrawCapsule(const Capsule& cap,unsigned int color) {
	DrawCircle(cap.posA.x, cap.posA.y, cap.radius, color);
	DrawCircle(cap.posB.x, cap.posB.y, cap.radius, color);

	auto v = cap.posB - cap.posA;
	auto angle = atan2(v.y, v.x)-DX_PI_F/2.0f;
	//なんか誤差があるため1を足している
	auto w = cos(angle)*(cap.radius+1.0f) ;
	auto h = sin(angle)*(cap.radius+1.0f) ;

	DrawQuadrangle(
		cap.posA.x - w, cap.posA.y - h,
		cap.posB.x - w, cap.posB.y - h,
		cap.posB.x + w, cap.posB.y + h,
		cap.posA.x + w, cap.posA.y + h, color, true);
}

///とある点を中心に回転して、その回転後の座標を返します。
///@param center 回転中心点
///@param angle 回転角度
///@param pos 元の座標
///@return 変換後の座標
Matrix RotatePosition(const Position2& center, float angle) {
	//①中心を原点に平行移動して
	//②原点中心に回転して
	//③中心を元の座標へ戻す

	Matrix mat = MultipleMat(TranslateMat(center.x, center.y),
		MultipleMat(RotateMat(angle),
			TranslateMat(-center.x, -center.y)));
	return mat;
}

///centerを中心点として、カプセル形状を回転してください。
///なお、カプセルには端点Aと端点Bがあるが、どちらの点もcenterを中心に回転してください。
///前回のRotateMatrixの実装は済んでいるため、すぐにできると思います。
Capsule RotateCapsule(Position2 center, float angle, const Capsule &cap) {
	cap.posA = MultipleMat();
	cap.posB = MultipleMat();
	Capsule ret = cap;
	return ret;
}

/// <summary>
/// 値をクランプ(範囲外に行ったらその範囲の端の値を返す)
/// </summary>
/// <param name="value">値</param>
/// <param name="minVal">最小値</param>
/// <param name="maxVal">最大値</param>
/// <returns>クランプされた値</returns>
float Clamp(float value, float minVal = 0.0f, float maxVal = 1.0f) {
	//今は値をそのまま返していますが、クランプ(最小値最大値の範囲を超えたら最小値もしくは最大値にする)した
	//値を返してください。
	return max(minVal,min(value,maxVal));
}

//カプセルと円が当たったか？
//これに関しては、内積を使用します。現在内積と外積の実装を「敢えて」外していますので
//Geometry.cppの中にあるDotを実装してください。
bool IsHit(const Capsule& cap, const Circle& cc) {
	//手順
	//①まず、カプセル形状の端点cap.posAからccの中心点までのベクトルvpを作ります。
	auto vp = cc.pos - cap.posA;
	//②次にカプセル形状そのもののベクトルposA→posBへのベクトルvを作ります。
	auto v = cap.posB - cap.posA;
	//③①と②の内積を求めます。
	auto num = Dot(vp, v);
	//④③の結果を②の大きさの2乗で割ります
	num = num / v.SQMagnitude();
	//⑤④の結果をクランプします
	//⑥⑤の結果を②の結果にかけます
	v = v * Clamp(num);
	//⑦①のベクトルから②のベクトルを引きます
	//⑧⑦のベクトルの大きさを測ります
	auto anser = (vp - v).Magnitude();
	//⑨⑧の値と、cap.radius+cc.radiusの値を比較します。
	return (anser < (cap.radius + cc.radius));
	
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	ChangeWindowMode(true);

	DxLib_Init();
	SetDrawScreen(DX_SCREEN_BACK);

	Circle c(50, Position2(50, 50));
	Capsule cap(20,Position2(100,200),Position2(300,200));

	char keystate[256];
	
	float angle = 0.0f;
	while (ProcessMessage() == 0) {
		ClearDrawScreen();

		int mx, my;
		GetMousePoint(&mx, &my);
		int minput = GetMouseInput();

		//カプセル回転
		if (minput&MOUSE_INPUT_LEFT) {
			angle = 0.1f;
		}
		else if (minput&MOUSE_INPUT_RIGHT) {
			angle = -0.1f;
		}
		else {
			angle = 0.0f;
		}

		GetHitKeyStateAll(keystate);

		//円うごく
		if (keystate[KEY_INPUT_RIGHT]) {
			c.pos.x = min(640, c.pos.x + 5);
		}
		else if (keystate[KEY_INPUT_LEFT]) {
			c.pos.x = max(0, c.pos.x -5);
		}
		if (keystate[KEY_INPUT_UP]) {
			c.pos.y = max(0, c.pos.y - 5);
		}
		else if (keystate[KEY_INPUT_DOWN]) {
			c.pos.y = min(480, c.pos.y + 5);
		}


		//カプセル形状をRotatePositionの結果の行列で回転させてください
		cap=RotateCapsule(Position2(mx, my), angle, cap);

		auto color = 0xffffff;
		if (IsHit(cap, c)) {
			color = 0xffaaaa;
		}

		DrawCircle(c.pos.x, c.pos.y, c.radius, color);
		DrawCapsule(cap, color);

		ScreenFlip();
	}

	DxLib_End();


}
#include<DxLib.h>
#include<cmath>
#include"Geometry.h"


using namespace std;

void DrawWood(const Capsule& cap, int handle) {


	auto v = cap.posB - cap.posA;
	auto angle = atan2(v.y, v.x) + DX_PI_F / 2.0f;
	auto w = cos(angle)*cap.radius;
	auto h = sin(angle)*cap.radius;

	DrawModiGraph(
		cap.posA.x - w, cap.posA.y - h,
		cap.posB.x - w, cap.posB.y - h,
		cap.posB.x + w, cap.posB.y + h,
		cap.posA.x + w, cap.posA.y + h, handle,true);
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

	Position2 pos = center;

	Matrix mat = MultipleMat(
	TranslateMat(center.x, center.y), MultipleMat(RotateMat(angle),TranslateMat(-center.x, -center.y)));
	return mat;
	//これを書き換えて、特定の点を中心に回転を行うようにしてください。
}

float Clamp(float value, float minVal = 0.0f, float maxVal = 1.0f) {
	
	return max(minVal, min(value, maxVal));
}

//カプセルと円が当たったか？
bool IsHit(const Capsule& cap, const Circle& cc) {
	//①まず、カプセル形状の端点cap.posAからccの中心点までのベクトルvpを作ります。
	auto vp = cc.pos - cap.posA;
	//②次にカプセル形状そのもののベクトルposA→posBへのベクトルvを作ります。
	auto v = cap.posB - cap.posA;
	//③①と②の内積を求めます。
	auto num = Dot(vp, v);
	//④③の結果を②の大きさの2乗で割ります
	num = num / (v.x*v.x + v.y*v.y);
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
	SetMainWindowText("1916024_筒井竜平");
	SetGraphMode(512, 800, 32);
	DxLib_Init();
	SetDrawScreen(DX_SCREEN_BACK);

	int sw, sh;//スクリーン幅、高さ
	GetDrawScreenSize(&sw, &sh);

	auto woodH = LoadGraph("img/wood.png");
	int wdW, wdH;
	GetGraphSize(woodH, &wdW, &wdH);
	wdW = 200;

	auto cascadeH = LoadGraph("img/cascade_chip.png");
	auto chipH = LoadGraph("img/atlas0.png");
	auto RockH = LoadGraph("img/rock.png");
	auto ClearH = LoadGraph("img/clear.png");

	Circle rock = { 15,{256,0} };

	Capsule cap(20,Position2((sw-wdW)/2,sh-100),Position2((sw - wdW) / 2+wdW,sh-100));

	Position2 motoA = cap.posA;
	Position2 motoB = cap.posB;

	char keystate[256];
	
	float angle = 0.0f;

	int frame = 0;
	bool isLeft = false;
	bool bakuhaFlag = false;
	bool clearFrag = false;
	bool doropFrag = false;

	while (ProcessMessage() == 0) {
		ClearDrawScreen();
		GetHitKeyStateAll(keystate);

		DrawBox(0, 0, sw, sh, 0xaaffff, true);

		int mx = 0, my = 0;
		if (clearFrag == false)
		{
			if (keystate[KEY_INPUT_LEFT]) {
				isLeft = true;
			}
			else if (keystate[KEY_INPUT_RIGHT]) {
				isLeft = false;
			}

			if (isLeft) {
				mx = cap.posA.x;
				my = cap.posA.y;
			}
			else {
				mx = cap.posB.x;
				my = cap.posB.y;
			}

			if ((!doropFrag)&&(max(cap.posA.x, cap.posB.x) - min(cap.posA.x, cap.posB.x) >= 100))
			{
				if (keystate[KEY_INPUT_Z]) {
					angle = -0.05f;
				}
				else if (keystate[KEY_INPUT_X]) {
					angle = 0.05f;
				}
				else {
					angle = 0.0f;
				}
			}
			else
			{
				if (cap.posA.y >= cap.posB.y)
				{
					mx = cap.posA.x;
					my = cap.posA.y;
				}
				else
				{
					mx = cap.posB.x;
					my = cap.posB.y;
				}
				doropFrag = true;
			}
			//当たり判定を完成させて当たったときの反応を書いてください
			if (IsHit(cap, rock)) {
				bakuhaFlag = true;
			}
			else
			{
				bakuhaFlag = false;
			}
		}
		//カプセル回転
		Matrix rotMat=RotatePosition(Position2(mx, my), angle);
		cap.posA = MultipleVec(rotMat, cap.posA);
		cap.posB = MultipleVec(rotMat, cap.posB);

		//背景の描画
		//滝
		int chipIdx = (frame/4) % 3;
		constexpr int dest_chip_size = 32;
		int destY = 48;
		while (destY < sh) {
			int destX = 0;
			while (destX < sw) {
				DrawRectExtendGraph(
					destX, destY, destX + dest_chip_size, destY+dest_chip_size,
					chipIdx * 16, 0,
					16, 16,
					cascadeH, false);
				destX += dest_chip_size;
			}
			destY += dest_chip_size;
		}

		DrawRectGraph(0, 0, 96, 0, 32, 32, chipH, true);
		DrawRectGraph(sw - 32, 0, 32, 0, 32, 32, chipH, true);
		DrawRectGraph(0, 32, 96, 32, 32, 32, chipH, true);
		DrawRectGraph(sw-32, 32, 32, 32, 32, 32, chipH, true);
		destY = 64;
		while (destY < sh) {
			DrawRectGraph(0, destY, 96, 64, 32, 32, chipH, true);
			DrawRectGraph(sw - 32, destY, 32, 64, 32, 32, chipH, true);
			destY += dest_chip_size;
		}


		DrawWood(cap, woodH);
		if (!clearFrag && !doropFrag)
		{
			DrawCircle(mx, my, 30, 0xff0000, false, 3);
		}
		if ((rock.pos.y > sh)||bakuhaFlag == true)
		{
			rock.pos.y = -100;
			rock.pos.x = rand() % (48 * 9) + 30;
		}
		if (!clearFrag)
		{
			rock.pos.y++;
		}
		DrawGraph(rock.pos.x, rock.pos.y, RockH, true);
		if (cap.posA.y < 30 && cap.posB.y < 30)
		{
			DrawGraph(0, 0, ClearH, true);
			angle = 0.0f;
			clearFrag = true;
		}
		if (!clearFrag)
		{
		++frame;

		}
		if ((bakuhaFlag) || ((cap.posB.y > sh + 50) && (cap.posA.y > sh + 50)) ||((cap.posA.x < -10&& cap.posB.x < -10)||(cap.posA.x > sw&& cap.posB.x > sw)))
		{
			cap.posA = motoA;
			cap.posB = motoB;
			doropFrag = false;
		}
		ScreenFlip();
	}

	DxLib_End();


}
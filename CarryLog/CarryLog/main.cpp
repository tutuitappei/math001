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


///�Ƃ���_�𒆐S�ɉ�]���āA���̉�]��̍��W��Ԃ��܂��B
///@param center ��]���S�_
///@param angle ��]�p�x
///@param pos ���̍��W
///@return �ϊ���̍��W
Matrix RotatePosition(const Position2& center, float angle) {
	//�@���S�����_�ɕ��s�ړ�����
	//�A���_���S�ɉ�]����
	//�B���S�����̍��W�֖߂�

	Position2 pos = center;

	Matrix mat = MultipleMat(
	TranslateMat(center.x, center.y), MultipleMat(RotateMat(angle),TranslateMat(-center.x, -center.y)));
	return mat;
	//��������������āA����̓_�𒆐S�ɉ�]���s���悤�ɂ��Ă��������B
}

float Clamp(float value, float minVal = 0.0f, float maxVal = 1.0f) {
	
	return max(minVal, min(value, maxVal));
}

//�J�v�Z���Ɖ~�������������H
bool IsHit(const Capsule& cap, const Circle& cc) {
	//�@�܂��A�J�v�Z���`��̒[�_cap.posA����cc�̒��S�_�܂ł̃x�N�g��vp�����܂��B
	auto vp = cc.pos - cap.posA;
	//�A���ɃJ�v�Z���`�󂻂̂��̂̃x�N�g��posA��posB�ւ̃x�N�g��v�����܂��B
	auto v = cap.posB - cap.posA;
	//�B�@�ƇA�̓��ς����߂܂��B
	auto num = Dot(vp, v);
	//�C�B�̌��ʂ��A�̑傫����2��Ŋ���܂�
	num = num / (v.x*v.x + v.y*v.y);
	//�D�C�̌��ʂ��N�����v���܂�
	//�E�D�̌��ʂ��A�̌��ʂɂ����܂�
	v = v * Clamp(num);
	//�F�@�̃x�N�g������A�̃x�N�g���������܂�
	//�G�F�̃x�N�g���̑傫���𑪂�܂�
	auto anser = (vp - v).Magnitude();
	//�H�G�̒l�ƁAcap.radius+cc.radius�̒l���r���܂��B
	return (anser < (cap.radius + cc.radius));
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	ChangeWindowMode(true);
	SetMainWindowText("1916024_���䗳��");
	SetGraphMode(512, 800, 32);
	DxLib_Init();
	SetDrawScreen(DX_SCREEN_BACK);

	int sw, sh;//�X�N���[�����A����
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
			//�����蔻������������ē��������Ƃ��̔����������Ă�������
			if (IsHit(cap, rock)) {
				bakuhaFlag = true;
			}
			else
			{
				bakuhaFlag = false;
			}
		}
		//�J�v�Z����]
		Matrix rotMat=RotatePosition(Position2(mx, my), angle);
		cap.posA = MultipleVec(rotMat, cap.posA);
		cap.posB = MultipleVec(rotMat, cap.posB);

		//�w�i�̕`��
		//��
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
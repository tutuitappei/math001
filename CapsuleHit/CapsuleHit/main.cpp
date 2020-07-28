#include<DxLib.h>
#include<cmath>
#include"Geometry.h"


using namespace std;

///�J�v�Z���`��̕`��
void DrawCapsule(const Capsule& cap,unsigned int color) {
	DrawCircle(cap.posA.x, cap.posA.y, cap.radius, color);
	DrawCircle(cap.posB.x, cap.posB.y, cap.radius, color);

	auto v = cap.posB - cap.posA;
	auto angle = atan2(v.y, v.x)-DX_PI_F/2.0f;
	//�Ȃ񂩌덷�����邽��1�𑫂��Ă���
	auto w = cos(angle)*(cap.radius+1.0f) ;
	auto h = sin(angle)*(cap.radius+1.0f) ;

	DrawQuadrangle(
		cap.posA.x - w, cap.posA.y - h,
		cap.posB.x - w, cap.posB.y - h,
		cap.posB.x + w, cap.posB.y + h,
		cap.posA.x + w, cap.posA.y + h, color, true);
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

	Matrix mat = MultipleMat(TranslateMat(center.x, center.y),
		MultipleMat(RotateMat(angle),
			TranslateMat(-center.x, -center.y)));
	return mat;
}

///center�𒆐S�_�Ƃ��āA�J�v�Z���`�����]���Ă��������B
///�Ȃ��A�J�v�Z���ɂ͒[�_A�ƒ[�_B�����邪�A�ǂ���̓_��center�𒆐S�ɉ�]���Ă��������B
///�O���RotateMatrix�̎����͍ς�ł��邽�߁A�����ɂł���Ǝv���܂��B
Capsule RotateCapsule(Position2 center, float angle, const Capsule &cap) {
	cap.posA = MultipleMat();
	cap.posB = MultipleMat();
	Capsule ret = cap;
	return ret;
}

/// <summary>
/// �l���N�����v(�͈͊O�ɍs�����炻�͈̔͂̒[�̒l��Ԃ�)
/// </summary>
/// <param name="value">�l</param>
/// <param name="minVal">�ŏ��l</param>
/// <param name="maxVal">�ő�l</param>
/// <returns>�N�����v���ꂽ�l</returns>
float Clamp(float value, float minVal = 0.0f, float maxVal = 1.0f) {
	//���͒l�����̂܂ܕԂ��Ă��܂����A�N�����v(�ŏ��l�ő�l�͈̔͂𒴂�����ŏ��l�������͍ő�l�ɂ���)����
	//�l��Ԃ��Ă��������B
	return max(minVal,min(value,maxVal));
}

//�J�v�Z���Ɖ~�������������H
//����Ɋւ��ẮA���ς��g�p���܂��B���ݓ��ςƊO�ς̎������u�����āv�O���Ă��܂��̂�
//Geometry.cpp�̒��ɂ���Dot���������Ă��������B
bool IsHit(const Capsule& cap, const Circle& cc) {
	//�菇
	//�@�܂��A�J�v�Z���`��̒[�_cap.posA����cc�̒��S�_�܂ł̃x�N�g��vp�����܂��B
	auto vp = cc.pos - cap.posA;
	//�A���ɃJ�v�Z���`�󂻂̂��̂̃x�N�g��posA��posB�ւ̃x�N�g��v�����܂��B
	auto v = cap.posB - cap.posA;
	//�B�@�ƇA�̓��ς����߂܂��B
	auto num = Dot(vp, v);
	//�C�B�̌��ʂ��A�̑傫����2��Ŋ���܂�
	num = num / v.SQMagnitude();
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

		//�J�v�Z����]
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

		//�~������
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


		//�J�v�Z���`���RotatePosition�̌��ʂ̍s��ŉ�]�����Ă�������
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
//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include <vector>
#include <math.h>
#include "Unit1.h"
#include <Windows.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
using namespace std;


struct vec3d
{
	float x, y, z;
};

struct triangle
{
	vec3d p[3];
};

struct mesh
{
	vector<triangle> tris;
};
 //matrix
struct mat4x4
{
	float m[4][4] = { 0 };
};




mesh meshCube;

mat4x4 matProj;

TForm1 *Form1;

bool bIsFullscreen = false;
TRect trPos;

//result image
Graphics::TBitmap *im = new Graphics::TBitmap();

float fDst = 4.0f;
float fAngle=0.0f;

void Draw3D();
//input, output, matrix
void MultiplyMatrixVector(vec3d &i, vec3d &o, mat4x4 &m);

//---------------------------------------------------------------------------


void MultiplyMatrixVector(vec3d &i, vec3d &o, mat4x4 &m)
	{
		o.x = i.x * m.m[0][0] + i.y * m.m[1][0] + i.z * m.m[2][0] + m.m[3][0];
		o.y = i.x * m.m[0][1] + i.y * m.m[1][1] + i.z * m.m[2][1] + m.m[3][1];
		o.z = i.x * m.m[0][2] + i.y * m.m[1][2] + i.z * m.m[2][2] + m.m[3][2];
		float w = i.x * m.m[0][3] + i.y * m.m[1][3] + i.z * m.m[2][3] + m.m[3][3];

		if (w != 0.0f)
		{
			o.x /= w; o.y /= w; o.z /= w;
		}
	}

void Draw3D()
{
mat4x4 matRotZ,matRotX;
fAngle+=0.05f;

// Rotation Z
		matRotZ.m[0][0] = cosf(fAngle);
		matRotZ.m[0][1] = sinf(fAngle);
		matRotZ.m[1][0] = -sinf(fAngle);
		matRotZ.m[1][1] = cosf(fAngle);
		matRotZ.m[2][2] = 1;
		matRotZ.m[3][3] = 1;

// Rotation X
		matRotX.m[0][0] = 1;
		matRotX.m[1][1] = cosf(fAngle * 0.5f);
		matRotX.m[1][2] = sinf(fAngle * 0.5f);
		matRotX.m[2][1] = -sinf(fAngle * 0.5f);
		matRotX.m[2][2] = cosf(fAngle * 0.5f);
		matRotX.m[3][3] = 1;

im->Canvas->Brush->Style = bsSolid;
im->Canvas->Brush->Color =clBlack;
im->Canvas->Pen->Color =clBlack;
if(im->Height!=Form1->ClientHeight || im->Width != Form1->ClientWidth )
{
im->Height=Form1->ClientHeight;
im->Width = Form1->ClientWidth;
}
im->Canvas->Rectangle(0,0,im->Width,im->Height);

for(auto tri: meshCube.tris)
{
  triangle triProj, triTranslated,triRotatedZ, triRotatedZX;

  MultiplyMatrixVector(tri.p[0],triRotatedZ.p[0],matRotZ);
  MultiplyMatrixVector(tri.p[1],triRotatedZ.p[1],matRotZ);
  MultiplyMatrixVector(tri.p[2],triRotatedZ.p[2],matRotZ);

  MultiplyMatrixVector(triRotatedZ.p[0],triRotatedZX.p[0],matRotX);
  MultiplyMatrixVector(triRotatedZ.p[1],triRotatedZX.p[1],matRotX);
  MultiplyMatrixVector(triRotatedZ.p[2],triRotatedZX.p[2],matRotX);

  triTranslated = triRotatedZX;

  triTranslated.p[0].z = triRotatedZX.p[0].z+fDst;
  triTranslated.p[1].z = triRotatedZX.p[1].z+fDst;
  triTranslated.p[2].z = triRotatedZX.p[2].z+fDst;


  MultiplyMatrixVector(triTranslated.p[0],triProj.p[0],matProj);
  MultiplyMatrixVector(triTranslated.p[1],triProj.p[1],matProj);
  MultiplyMatrixVector(triTranslated.p[2],triProj.p[2],matProj);

im->Canvas->Brush->Color= clYellow;
im->Canvas->Pen->Color= clRed;
//im->Canvas->Brush->Style = bsClear;

triProj.p[0].x+=1.0f;
triProj.p[0].y+=1.0f;

triProj.p[1].x+=1.0f;
triProj.p[1].y+=1.0f;

triProj.p[2].x+=1.0f;
triProj.p[2].y+=1.0f;

triProj.p[0].x*=0.5f * (float)Form1->ClientWidth;
triProj.p[0].y*=0.5f* (float)Form1->ClientHeight;

triProj.p[1].x*=0.5f * (float)Form1->ClientWidth;
triProj.p[1].y*=0.5f* (float)Form1->ClientHeight;

triProj.p[2].x*=0.5f * (float)Form1->ClientWidth;
triProj.p[2].y*=0.5f* (float)Form1->ClientHeight;

TPoint p[3];
p[0]=Point(triProj.p[0].x,triProj.p[0].y);
p[1]= Point(triProj.p[1].x,triProj.p[1].y);
p[2]=Point(triProj.p[2].x,triProj.p[2].y);


im->Canvas->MoveTo(p[0].x,p[0].y);
im->Canvas->LineTo(p[1].x,p[1].y);
im->Canvas->LineTo(p[2].x,p[2].y);
im->Canvas->LineTo(p[0].x,p[0].y);

//im->Canvas->Brush->Style = bsSolid;
//im->Canvas->Polygon(p,2);



}
im->Canvas->Brush->Style = bsClear;
im->Canvas->Font->Color = clWhite;
im->Canvas->TextOut(10,10,"press ALT + Enter for fulscreen mode.");
im->Canvas->TextOut(10,40,"press ESC for Exit.");


Form1->Canvas->Draw(0,0,im);
}


__fastcall TForm1::TForm1(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TForm1::FormCreate(TObject *Sender)
{
RegisterHotKey(Handle,0,MOD_ALT,VK_RETURN);

meshCube.tris = {
		// SOUTH
		{ 0.0f, 0.0f, 0.0f,    0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 0.0f },
		{ 0.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 0.0f, 0.0f },

		// EAST
		{ 1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f },
		{ 1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 0.0f, 1.0f },

		// NORTH
		{ 1.0f, 0.0f, 1.0f,    1.0f, 1.0f, 1.0f,    0.0f, 1.0f, 1.0f },
		{ 1.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 0.0f, 1.0f },

		// WEST
		{ 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 1.0f, 0.0f },
		{ 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 0.0f,    0.0f, 0.0f, 0.0f },

		// TOP
		{ 0.0f, 1.0f, 0.0f,    0.0f, 1.0f, 1.0f,    1.0f, 1.0f, 1.0f },
		{ 0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 1.0f, 0.0f },

		// BOTTOM
		{ 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f },
		{ 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f,    1.0f, 0.0f, 0.0f },

		};

		// Projection Matrix
		float fNear = 0.1f;
		float fFar = 1000.0f;
		float fFov = 90.0f;
		float fAspectRatio = (float)Form1->ClientHeight / (float)Form1->ClientWidth;
		float fFovRad = 1.0f / tanf(fFov * 0.5f / 180.0f * 3.14159f);

		matProj.m[0][0] = fAspectRatio * fFovRad;
		matProj.m[1][1] = fFovRad;
		matProj.m[2][2] = fFar / (fFar - fNear);
		matProj.m[3][2] = (-fFar * fNear) / (fFar - fNear);
		matProj.m[2][3] = 1.0f;
		matProj.m[3][3] = 0.0f;

}
//---------------------------------------------------------------------------
void __fastcall TForm1::Timer1Timer(TObject *Sender)
{
Draw3D();
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

void __fastcall TForm1::FormClose(TObject *Sender, TCloseAction &Action)
{
UnregisterHotKey( Handle, 1 );
}
//---------------------------------------------------------------------------


void __fastcall TForm1::ApplicationEvents1Message(tagMSG &Msg, bool &Handled)
{
if((Msg.message == WM_HOTKEY)&&(Msg.wParam==0))
{
 if (!bIsFullscreen)
 {
 trPos =TRect(Left,Top,Width,Height);
 BorderStyle = bsNone;
 SetBounds(0,0,Screen->Width,Screen->Height);

 }
 else
 {
 BorderStyle = bsSizeable;
 SetBounds(trPos.Left,trPos.Top,trPos.Right,trPos.Bottom);
 }
 bIsFullscreen = !bIsFullscreen;
}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)

{
if (Key==VK_ESCAPE) {
     Form1->Close();
}
}
//---------------------------------------------------------------------------


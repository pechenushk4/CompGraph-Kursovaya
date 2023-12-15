#include "Render.h"

#include <sstream>
#include <iostream>

#include <windows.h>
#include <GL\GL.h>
#include <GL\GLU.h>

#include "MyOGL.h"

#include "Camera.h"
#include "Light.h"
#include "Primitives.h"

#include "GUItextRectangle.h"

bool textureMode = true;
bool lightMode = true;

//����� ��� ��������� ������
class CustomCamera : public Camera
{
public:
	//��������� ������
	double camDist;
	//���� �������� ������
	double fi1, fi2;

	
	//������� ������ �� ���������
	CustomCamera()
	{
		camDist = 15;
		fi1 = 1;
		fi2 = 1;
	}

	
	//������� ������� ������, ������ �� ����� ��������, ���������� �������
	void SetUpCamera()
	{
		//�������� �� ������� ������ ������
		lookPoint.setCoords(0, 0, 0);

		pos.setCoords(camDist*cos(fi2)*cos(fi1),
			camDist*cos(fi2)*sin(fi1),
			camDist*sin(fi2));

		if (cos(fi2) <= 0)
			normal.setCoords(0, 0, -1);
		else
			normal.setCoords(0, 0, 1);

		LookAt();
	}

	void CustomCamera::LookAt()
	{
		//������� ��������� ������
		gluLookAt(pos.X(), pos.Y(), pos.Z(), lookPoint.X(), lookPoint.Y(), lookPoint.Z(), normal.X(), normal.Y(), normal.Z());
	}



}  camera;   //������� ������ ������


//����� ��� ��������� �����
class CustomLight : public Light
{
public:
	CustomLight()
	{
		//��������� ������� �����
		pos = Vector3(1, 1, 3);
	}

	
	//������ ����� � ����� ��� ���������� �����, ���������� �������
	void  DrawLightGhismo()
	{
		glDisable(GL_LIGHTING);

		
		glColor3d(0.9, 0.8, 0);
		Sphere s;
		s.pos = pos;
		s.scale = s.scale*0.08;
		s.Show();
		
		if (OpenGL::isKeyPressed('G'))
		{
			glColor3d(0, 0, 0);
			//����� �� ��������� ����� �� ����������
			glBegin(GL_LINES);
			glVertex3d(pos.X(), pos.Y(), pos.Z());
			glVertex3d(pos.X(), pos.Y(), 0);
			glEnd();

			//������ ���������
			Circle c;
			c.pos.setCoords(pos.X(), pos.Y(), 0);
			c.scale = c.scale*1.5;
			c.Show();
		}

	}

	void SetUpLight()
	{
		GLfloat amb[] = { 0.2, 0.2, 0.2, 0 };
		GLfloat dif[] = { 1.0, 1.0, 1.0, 0 };
		GLfloat spec[] = { .7, .7, .7, 0 };
		GLfloat position[] = { pos.X(), pos.Y(), pos.Z(), 1. };

		// ��������� ��������� �����
		glLightfv(GL_LIGHT0, GL_POSITION, position);
		// �������������� ����������� �����
		// ������� ��������� (���������� ����)
		glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
		// ��������� ������������ �����
		glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
		// ��������� ���������� ������������ �����
		glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

		glEnable(GL_LIGHT0);
	}


} light;  //������� �������� �����




//������ ���������� ����
int mouseX = 0, mouseY = 0;

void mouseEvent(OpenGL *ogl, int mX, int mY)
{
	int dx = mouseX - mX;
	int dy = mouseY - mY;
	mouseX = mX;
	mouseY = mY;

	//������ ���� ������ ��� ������� ����� ������ ����
	if (OpenGL::isKeyPressed(VK_RBUTTON))
	{
		camera.fi1 += 0.01*dx;
		camera.fi2 += -0.01*dy;
	}

	
	//������� ���� �� ���������, � ����� ��� ����
	if (OpenGL::isKeyPressed('G') && !OpenGL::isKeyPressed(VK_LBUTTON))
	{
		LPPOINT POINT = new tagPOINT();
		GetCursorPos(POINT);
		ScreenToClient(ogl->getHwnd(), POINT);
		POINT->y = ogl->getHeight() - POINT->y;

		Ray r = camera.getLookRay(POINT->x, POINT->y);

		double z = light.pos.Z();

		double k = 0, x = 0, y = 0;
		if (r.direction.Z() == 0)
			k = 0;
		else
			k = (z - r.origin.Z()) / r.direction.Z();

		x = k*r.direction.X() + r.origin.X();
		y = k*r.direction.Y() + r.origin.Y();

		light.pos = Vector3(x, y, z);
	}

	if (OpenGL::isKeyPressed('G') && OpenGL::isKeyPressed(VK_LBUTTON))
	{
		light.pos = light.pos + Vector3(0, 0, 0.02*dy);
	}

	
}

void mouseWheelEvent(OpenGL *ogl, int delta)
{

	if (delta < 0 && camera.camDist <= 1)
		return;
	if (delta > 0 && camera.camDist >= 100)
		return;

	camera.camDist += 0.01*delta;

}

void keyDownEvent(OpenGL *ogl, int key)
{
	if (key == 'L')
	{
		lightMode = !lightMode;
	}

	if (key == 'T')
	{
		textureMode = !textureMode;
	}

	if (key == 'R')
	{
		camera.fi1 = 1;
		camera.fi2 = 1;
		camera.camDist = 15;

		light.pos = Vector3(1, 1, 3);
	}

	if (key == 'F')
	{
		light.pos = camera.pos;
	}
}

void keyUpEvent(OpenGL *ogl, int key)
{
	
}



GLuint texId;

//����������� ����� ������ ��������
void initRender(OpenGL *ogl)
{
	//��������� �������

	//4 ����� �� �������� �������
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	//��������� ������ ��������� �������
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//�������� ��������
	glEnable(GL_TEXTURE_2D);
	

	//������ ����������� ���������  (R G B)
	RGBTRIPLE *texarray;

	//������ ��������, (������*������*4      4, ���������   ����, �� ������� ������������ �� 4 ����� �� ������� �������� - R G B A)
	char *texCharArray;
	int texW, texH;
	OpenGL::LoadBMP("texture.bmp", &texW, &texH, &texarray);
	OpenGL::RGBtoChar(texarray, texW, texH, &texCharArray);

	
	
	//���������� �� ��� ��������
	glGenTextures(1, &texId);
	//������ ��������, ��� ��� ����� ����������� � ���������, ����� ����������� �� ����� ��
	glBindTexture(GL_TEXTURE_2D, texId);

	//��������� �������� � �����������, � ���������� ��� ������  ��� �� �����
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray);

	//�������� ������
	free(texCharArray);
	free(texarray);

	//������� ����
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


	//������ � ���� ����������� � "������"
	ogl->mainCamera = &camera;
	ogl->mainLight = &light;

	// ������������ �������� : �� ����� ����� ����� 1
	glEnable(GL_NORMALIZE);

	// ���������� ������������� ��� �����
	glEnable(GL_LINE_SMOOTH); 


	//   ������ ��������� ���������
	//  �������� GL_LIGHT_MODEL_TWO_SIDE - 
	//                0 -  ������� � ���������� �������� ���������(�� ���������), 
	//                1 - ������� � ���������� �������������� ������� ��������       
	//                �������������� ������� � ���������� ��������� ����������.    
	//  �������� GL_LIGHT_MODEL_AMBIENT - ������ ������� ���������, 
	//                �� ��������� �� ���������
	// �� ��������� (0.2, 0.2, 0.2, 1.0)

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);

	camera.fi1 = -1.3;
	camera.fi2 = 0.8;
}

class t_vector {
public:
	double x, y, z;
	t_vector(double a, double b, double c) {
		x = a;
		y = b;
		z = c;
	};
};

double vec_length(t_vector* vec)
{
	double  result;

	result = sqrt((vec->x * vec->x) + (vec->y * vec->y) + (vec->z * vec->z));
	return (result);
}

void  vec_normalize(t_vector* vec)
{
	double  length;

	length = vec_length(vec);
	vec->x /= length;
	vec->y /= length;
	vec->z /= length;
}

auto vec_dot_product(t_vector* vec1, t_vector* vec2)
{
	auto x = vec1->y * vec2->z - vec1->z * vec2->y;
	auto y = -(vec1->x) * vec2->z + vec1->z * vec2->x;
	auto z = vec1->x * vec2->y - vec1->y * vec2->x;
	return new t_vector(x, y, z);
}

auto getVec(double a[], double b[]) {
	double ret[] = { a[0] - b[0], a[1] - b[1], a[2] - b[2] };
	return new t_vector(ret[0], ret[1], ret[2]);
}

void Render(OpenGL *ogl)
{



	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	glEnable(GL_DEPTH_TEST);
	if (textureMode)
		glEnable(GL_TEXTURE_2D);

	if (lightMode)
		glEnable(GL_LIGHTING);


	//��������������
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	//��������� ���������
	GLfloat amb[] = { 0.2, 0.2, 0.1, 1. };
	GLfloat dif[] = { 0.4, 0.65, 0.5, 1. };
	GLfloat spec[] = { 0.9, 0.8, 0.3, 1. };
	GLfloat sh = 0.1f * 256;


	//�������
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
	//��������
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
	//����������
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec); 
		//������ �����
		glMaterialf(GL_FRONT, GL_SHININESS, sh);

	//���� ���� �������, ��� ����������� (����������� ���������)
	glShadeModel(GL_SMOOTH);
	//===================================
	//������� ���  
	double A[] = { 0, 0, 0 };
	double B[] = { 3, -7, 0 };
	double C[] = { 7, -2, 0 };
	double D[] = { 8, 1, 0 };
	double E[] = { 6, 2, 0 };
	double F[] = { 12, 9, 0 };
	double G[] = { 6, 7, 0 };
	double H[] = { 13, -3, 0 };

	double A1[] = { 0, 0, 5 };
	double B1[] = { 3, -7, 5 };
	double C1[] = { 7, -2, 5 };
	double D1[] = { 8, 1, 5 };
	double E1[] = { 6, 2, 5 };
	double F1[] = { 12, 9, 5 };
	double G1[] = { 6, 7, 5 };
	double H1[] = { 13, -3, 5 };
	
	t_vector* normVec = NULL;

	t_vector* tempVector = NULL;
	t_vector* tempVector2 = NULL;

	glBegin(GL_POLYGON);

	tempVector = getVec(B, A);
	tempVector2 = getVec(B, C);
	normVec = vec_dot_product(tempVector, tempVector2);

	vec_normalize(normVec);
	glNormal3d(normVec->x, normVec->y, normVec->z);

	glVertex3dv(A);
	glVertex3dv(B);
	glVertex3dv(C);
	glVertex3dv(D);
	glVertex3dv(E);
	glEnd();

	glBegin(GL_POLYGON);

	tempVector = getVec(F, D);
	tempVector2 = getVec(F, G);
	normVec = vec_dot_product(tempVector, tempVector2);

	vec_normalize(normVec);
	glNormal3d(normVec->x, normVec->y, normVec->z);

	glVertex3dv(D);
	glVertex3dv(F);
	glVertex3dv(G);
	glVertex3dv(E);
	glEnd();

	glBegin(GL_TRIANGLES);

	tempVector = getVec(H, C);
	tempVector2 = getVec(H, D);
	normVec = vec_dot_product(tempVector, tempVector2);

	vec_normalize(normVec);
	glNormal3d(normVec->x, normVec->y, normVec->z);

	glVertex3dv(C);
	glVertex3dv(H);
	glVertex3dv(D);
	glEnd();

	glBegin(GL_QUADS);

	tempVector = getVec(A1, A);
	tempVector2 = getVec(A1, B1);
	normVec = vec_dot_product(tempVector, tempVector2);

	vec_normalize(normVec);
	glNormal3d(normVec->x, normVec->y, normVec->z);

	glColor3d(0.5, 0.02, 0.9);
	glVertex3dv(A);
	glVertex3dv(A1);
	glVertex3dv(B1);
	glVertex3dv(B);
	glEnd();

	glBegin(GL_QUADS);

	tempVector = getVec(B1, B);
	tempVector2 = getVec(B1, C1);
	normVec = vec_dot_product(tempVector, tempVector2);

	vec_normalize(normVec);
	glNormal3d(normVec->x, normVec->y, normVec->z);

	glColor3dv(A);
	glVertex3dv(B);
	glVertex3dv(B1);
	glVertex3dv(C1);
	glVertex3dv(C);
	glEnd();

	glBegin(GL_QUADS);

	tempVector = getVec(C1, C);
	tempVector2 = getVec(C1, H1);
	normVec = vec_dot_product(tempVector, tempVector2);

	vec_normalize(normVec);
	glNormal3d(normVec->x, normVec->y, normVec->z);

	glVertex3dv(C);
	glVertex3dv(C1);
	glVertex3dv(H1);
	glVertex3dv(H);
	glEnd();

	glBegin(GL_QUADS);

	tempVector = getVec(H1, H);
	tempVector2 = getVec(H1, D1);
	normVec = vec_dot_product(tempVector, tempVector2);

	vec_normalize(normVec);
	glNormal3d(normVec->x, normVec->y, normVec->z);

	glVertex3dv(H);
	glVertex3dv(H1);
	glVertex3dv(D1);
	glVertex3dv(D);
	glEnd();

	glBegin(GL_QUADS);

	tempVector = getVec(D1, D);
	tempVector2 = getVec(D1, F1);
	normVec = vec_dot_product(tempVector, tempVector2);

	vec_normalize(normVec);
	glNormal3d(normVec->x, normVec->y, normVec->z);

	glVertex3dv(D);
	glVertex3dv(D1);
	glVertex3dv(F1);
	glVertex3dv(F);
	glEnd();

	glBegin(GL_QUADS);

	tempVector = getVec(F1, F);
	tempVector2 = getVec(F1, G1);
	normVec = vec_dot_product(tempVector, tempVector2);

	vec_normalize(normVec);
	glNormal3d(normVec->x, normVec->y, normVec->z);

	glColor3d(0, 1, 0);
	glVertex3dv(F);
	glVertex3dv(F1);
	glVertex3dv(G1);
	glVertex3dv(G);
	glEnd();

	glBegin(GL_QUADS);

	tempVector = getVec(G1, G);
	tempVector2 = getVec(G1, E1);
	normVec = vec_dot_product(tempVector, tempVector2);

	vec_normalize(normVec);
	glNormal3d(normVec->x, normVec->y, normVec->z);

	glColor3d(0, 0, 0);
	glVertex3dv(G);
	glVertex3dv(G1);
	glVertex3dv(E1);
	glVertex3dv(E);
	glEnd();

	glBegin(GL_QUADS);

	tempVector = getVec(E1, E);
	tempVector2 = getVec(E1, A1);
	normVec = vec_dot_product(tempVector, tempVector2);

	vec_normalize(normVec);
	glNormal3d(normVec->x, normVec->y, normVec->z);

	glVertex3dv(E);
	glVertex3dv(E1);
	glVertex3dv(A1);
	glVertex3dv(A);
	glEnd();

	glBegin(GL_POLYGON);

	tempVector = getVec(B1, C1);
	tempVector2 = getVec(B1, A1);
	normVec = vec_dot_product(tempVector, tempVector2);

	vec_normalize(normVec);
	glNormal3d(normVec->x, normVec->y, normVec->z);

	glVertex3dv(A1);
	glVertex3dv(B1);
	glVertex3dv(C1);
	glVertex3dv(D1);
	glVertex3dv(E1);
	glEnd();

	glBegin(GL_POLYGON);

	tempVector = getVec(F1, G1);
	tempVector2 = getVec(F1, D1);
	normVec = vec_dot_product(tempVector, tempVector2);

	vec_normalize(normVec);
	glNormal3d(normVec->x, normVec->y, normVec->z);

	glVertex3dv(D1);
	glVertex3dv(F1);
	glVertex3dv(G1);
	glVertex3dv(E1);
	glEnd();

	glBegin(GL_TRIANGLES);

	tempVector = getVec(H1, D1);
	tempVector2 = getVec(H1, C1);
	normVec = vec_dot_product(tempVector, tempVector2);

	vec_normalize(normVec);
	glNormal3d(normVec->x, normVec->y, normVec->z);

	glVertex3dv(C1);
	glVertex3dv(H1);
	glVertex3dv(D1);
	glEnd();

	//������ ��������� ���������� ��������
	double AA[2] = { -4, -4 };
	double BB[2] = { 4, -4 };
	double CC[2] = { 4, 4 };
	double DD[2] = { -4, 4 };

	glBindTexture(GL_TEXTURE_2D, texId);

	glColor3d(0.6, 0.6, 0.6);
	glBegin(GL_QUADS);

	glNormal3d(0, 0, 1);
	glTexCoord2d(0, 0);
	glVertex2dv(AA);
	glTexCoord2d(1, 0);
	glVertex2dv(BB);
	glTexCoord2d(1, 1);
	glVertex2dv(CC);
	glTexCoord2d(0, 1);
	glVertex2dv(DD);

	glEnd();
	//����� ��������� ���������� ��������


   //��������� ������ ������

	
	glMatrixMode(GL_PROJECTION);	//������ �������� ������� ��������. 
	                                //(���� ��������� ��������, ����� �� ������������.)
	glPushMatrix();   //��������� ������� ������� ������������� (������� ��������� ������������� ��������) � ���� 				    
	glLoadIdentity();	  //��������� ��������� �������
	glOrtho(0, ogl->getWidth(), 0, ogl->getHeight(), 0, 1);	 //������� ����� ������������� ��������

	glMatrixMode(GL_MODELVIEW);		//������������� �� �����-��� �������
	glPushMatrix();			  //��������� ������� ������� � ���� (��������� ������, ����������)
	glLoadIdentity();		  //���������� �� � ������

	glDisable(GL_LIGHTING);



	GuiTextRectangle rec;		   //������� ����� ��������� ��� ������� ������ � �������� ������.
	rec.setSize(300, 150);
	rec.setPosition(10, ogl->getHeight() - 150 - 10);


	std::stringstream ss;
	ss << "T - ���/���� �������" << std::endl;
	ss << "L - ���/���� ���������" << std::endl;
	ss << "F - ���� �� ������" << std::endl;
	ss << "G - ������� ���� �� �����������" << std::endl;
	ss << "G+��� ������� ���� �� ���������" << std::endl;
	ss << "�����. �����: (" << light.pos.X() << ", " << light.pos.Y() << ", " << light.pos.Z() << ")" << std::endl;
	ss << "�����. ������: (" << camera.pos.X() << ", " << camera.pos.Y() << ", " << camera.pos.Z() << ")" << std::endl;
	ss << "��������� ������: R="  << camera.camDist << ", fi1=" << camera.fi1 << ", fi2=" << camera.fi2 << std::endl;
	
	rec.setText(ss.str().c_str());
	rec.Draw();

	glMatrixMode(GL_PROJECTION);	  //��������������� ������� �������� � �����-��� �������� �� �����.
	glPopMatrix();


	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

}
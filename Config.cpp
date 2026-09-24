#include "Config.h"
#include <GL\gl.h>
#include <cmath>
HGLRC hRC = NULL;    //Контекст рендеринга
HDC hDC = NULL;      //Приватный контекст устройства GDI
HWND hWnd = NULL;    //Дискриптор окна
HINSTANCE hInstance; //Ну и наконец дискриптор предложения

bool keys[256];         //Массив булевых значений клавиш клавиатуры
bool active = true;     //Активно ли наше окно
bool fullscreen = true; //Переменная фуллскрина
bool blend;   //Смешивание ВКЛ/ВЫКЛ
bool light;   //Свет ВКЛ/ВЫКЛ
bool rmbDown = false;

bool lp;      //L нажата?
bool fp;      //F нажата?
bool bp;      //B нажата?
bool tp;      //T нажата?

int winWidth = 1024, winHeight = 768;
int mouseDX = 0, mouseDY = 0;
GLfloat aspect = (GLfloat)winWidth / (GLfloat)winHeight;

GLfloat x, y, z = -5.0f;
GLfloat spin;
GLfloat camX = 0.0f, camY = 0.0f, camZ = 5.0f;
GLfloat camYaw = 0.0f, camPitch = 0.0f;

GLuint filter;     //Фильтр
GLuint texture[4]; //Кол-во текстур
GLuint loop;       //Используется для циклов

GLfloat LightAmbient[4] = {0.5f, 0.5f, 0.5f, 1.0f};  //Значение фонового света
GLfloat LightDiffuse[4] = {1.0f, 1.0f, 1.0f, 1.0f};  //Значения диффузного света
GLfloat LightPosition[4] = {0.0f, 0.0f, 2.0f, 1.0f}; //Позиция света
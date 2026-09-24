#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <windows.h>
#include <gl\gl.h>

extern HGLRC hRC;    //Контекст рендеринга
extern HDC hDC;      //Приватный контекст устройства GDI
extern HWND hWnd;    //Дискриптор окна
extern HINSTANCE hInstance; //Ну и наконец дискриптор предложения

extern bool keys[256];  //Массив булевых значений клавиш клавиатуры
extern bool active;     //Активно ли наше окно
extern bool fullscreen; //Переменная фуллскрина
extern bool blend;   //Смешивание ВКЛ/ВЫКЛ
extern bool light;   //Свет ВКЛ/ВЫКЛ
extern bool lp;      //L нажата?
extern bool fp;      //F нажата?
extern bool bp;      //B нажата?
extern bool tp;      //T нажата?

extern int mouseDX, mouseDY;

extern GLfloat x, y, z;
extern GLfloat xrot, yrot, zrot;

extern GLfloat camX, camY, camZ; //Позиция камеры
extern GLfloat camYaw, camPitch; //Поворот камеры (рыскание, наклон)
extern int winWidth, winHeight;
extern bool rmbDown;

extern GLuint filter;     //Фильтр
extern GLuint texture[3]; //Кол-во текстур
extern GLuint loop;       //Используется для циклов

extern GLfloat LightAmbient[4];  //Значение фонового света
extern GLfloat LightDiffuse[4];  //Значения диффузного света
extern GLfloat LightPosition[4]; //Позиция света

#endif
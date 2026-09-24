#ifndef INITGAME_H
#define INITGAME_H

#include <GL\gl.h> //OpenGL
bool LoadTextures(); //Загрузка картинки и конвертирование в текстуру
GLvoid ResizeWindow(GLsizei width, GLsizei height); //Функция для изменения размеров окна
bool InitGL(GLsizei Width, GLsizei Height); //Инициализируем окно
bool CreateGameWindow(LPCTSTR title, int width, int height, int bits, bool fullscreenflag); //Функция создания окна
GLvoid KillWindow(GLvoid); //Функция уничтожения окна
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

#endif
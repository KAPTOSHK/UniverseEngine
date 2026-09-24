#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <windows.h>  //Библиотека винды
#include <stdio.h>    //Ввод/вывод
#include <cmath>      //Математика
#include <gl\gl.h>    //Основная библиотека OpenGL
#include <gl\glu.h>   //Глю тоже нужен
#include <tchar.h>    //Для надписей
#include "InitGame.h"
#include "Config.h"

//Функция отрисовки сценыы
GLvoid DrawScene() {

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

	// ─── Камера ────────────────────────────────
    glRotatef(-camPitch, 1.0f, 0.0f, 0.0f);   //Наклон
    glRotatef(-camYaw,   0.0f, 1.0f, 0.0f);   //Поворот
    glTranslatef(-camX, -camY, -camZ);        //Сдвиг мира относительно камеры

	glPushMatrix();
		glTranslatef(x, y, z);
		glRotatef(xrot,1.0f,0.0f,0.0f);		// Вращение по оси X
		glRotatef(yrot,0.0f,1.0f,0.0f);		// Вращение по оси Y
		glRotatef(zrot,0.0f,0.0f,1.0f);		// Вращение по оси Z
		glBindTexture(GL_TEXTURE_2D, texture[0]);
	
		glBegin(GL_QUADS);
			//Передняя грань
			glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f); //Низ лево
			glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f); //Низ право
			glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f); //Верх право
			glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f); //Верх лево
	
			//Задняя грань
			glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f); //Низ право
			glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f); //Верх право
			glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f); //Верх лево
			glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f); //Низ лево
	
			//Верхняя грань
			glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f); //Верх лево
			glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f,  1.0f,  1.0f); //Низ лево
			glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f,  1.0f,  1.0f); //Низ право
			glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f); //Верх право
	
			//Нижняя грань
			glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, -1.0f, -1.0f); //Верх право
			glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f, -1.0f, -1.0f); //Верх лево
			glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f); //Низ лево
			glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f); //Низ право
	
			//Правая грань
			glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f); //Низ право
			glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f); //Верх право
			glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f); //Верх лево
			glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f); //Низ лево
	
			//Левая грань
			glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f); //Низ лево
			glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f); //Низ право
			glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f); //Верх право
			glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f); //Верх лево
		glEnd();
	glPopMatrix();

	xrot += 0.3f;
	yrot += 0.2f;
	zrot += 0.4f;
}



int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	MSG msg;
	bool done = false;
	
	if(MessageBox(NULL, "Do You Want To Launch Application In Fullscreen Mode?", "Launch In Fullscreen Mode?", MB_YESNO | MB_ICONQUESTION) == IDNO) fullscreen = false;
	if(!CreateGameWindow("Universe Engine", 1024, 768, 32, fullscreen)) return 0;
	
	while(!done) {
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			if(msg.message == WM_QUIT) done = true;
			else {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else {
			if(active) {

				camYaw   += mouseDX * 0.1f;
				camPitch += mouseDY * 0.1f;    // минус, потому что Y в WinAPI вниз

				if (camPitch >  89) camPitch =  89;
				if (camPitch < -89) camPitch = -89;

				mouseDX = 0;
				mouseDY = 0;
				if(keys[VK_ESCAPE]) done = true;
				else {
					// Ограничиваем наклон, чтобы не перевернуться
					if (camPitch >  89.0f) camPitch =  89.0f;
					if (camPitch < -89.0f) camPitch = -89.0f;

					// (Центрирование делает WndProc через WM_MOUSEMOVE — можно и тут,
					// но тогда будет двойное центрирование; выберите что-то одно.)

					DrawScene();
					SwapBuffers(hDC);

					if (keys['L'] && !lp) {
						lp = true;
						light = !light;
						if(!light) glDisable(GL_LIGHTING);
						else glEnable(GL_LIGHTING);
					}
					if(!keys['L']) lp = false;

					if(keys['F'] && !fp) {
						fp = true;
						filter += 1;
						if (filter>2) filter = 0;
					}
					if(!keys['F']) fp = false;

					float yawRad   = camYaw * 3.14159265f / 180.0f;
					float forwardX =  sinf(yawRad);
					float forwardZ =  cosf(yawRad);
					float rightX   = -cosf(yawRad);
					float rightZ   =  sinf(yawRad);

					if(keys['W']) {
						camX -= forwardX * 0.1f;
						camZ -= forwardZ * 0.1f;
					}

					if(keys['S']) {
						camX += forwardX * 0.1f;
						camZ += forwardZ * 0.1f;
					}

					if(keys['A']) {
						camX += rightX   * 0.1f;
						camZ += rightZ   * 0.1f;
					}

					if(keys['D']) {
						camX -= rightX   * 0.1f;
						camZ -= rightZ   * 0.1f;
					}

					if(keys[VK_UP])    camPitch += 1.0f;
					if(keys[VK_DOWN])  camPitch -= 1.0f;
					if(keys[VK_LEFT])  camYaw   += 1.0f;
					if(keys[VK_RIGHT]) camYaw   -= 1.0f;

					if(keys['B'] && !bp) {
						bp = true;
						blend = !blend;
						if (blend) {
							glEnable(GL_BLEND);
							glDisable(GL_DEPTH_TEST);
						}
						else {
							glDisable(GL_BLEND);
							glEnable(GL_DEPTH_TEST);
						}
					}
					if(!keys['B']) bp = false;
				}
			}
			if(keys[VK_F1]) {
				keys[VK_F1] = false;
				KillWindow();
				fullscreen = !fullscreen;
				if(!CreateGameWindow("Universe Engine", 1024, 768, 32, fullscreen)) return 0;
			}
		}
	}
	KillWindow();
	return(msg.wParam);
}
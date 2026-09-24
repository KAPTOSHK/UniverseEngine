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

GLvoid DrawSphere(GLfloat radius, int rings, int sectors) {
    const float PI = 3.14159265f;

    for (int r = 0; r < rings; ++r) {
        glBegin(GL_QUAD_STRIP);
        for (int s = 0; s <= sectors; ++s) {
            for (int k = 0; k < 2; ++k) {
                float theta = PI * (float)(r + k) / (float)rings;
                float phi   = 2.0f * PI * (float)s / (float)sectors;

                float x = sinf(theta) * cosf(phi);
                float y = cosf(theta);
                float z = sinf(theta) * sinf(phi);

                // Текстурные координаты
                glTexCoord2f((float)s / sectors, 1.0f - (float)(r + k) / rings);

                // Нормаль для освещения
                glNormal3f(x, y, z);

                // Вершина
                glVertex3f(x * radius, y * radius, z * radius);
            }
        }
        glEnd();
    }
}

//Функция отрисовки сценыы
GLvoid DrawScene() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

	//--------------------------Камера------------------------------------------------------------------------------------------------
    glRotatef(-camPitch, 1.0f, 0.0f, 0.0f);   //Наклон
    glRotatef(-camYaw,   0.0f, 1.0f, 0.0f);   //Поворот
    glTranslatef(-camX, -camY, -camZ);        //Сдвиг мира относительно камеры
	//--------------------------------------------------------------------------------------------------------------------------------

	//--------------------------------------------Скайбокс----------------------------------------------------------------------------
	glPushMatrix();
		glTranslatef(0.0f, 0.0f, 0.0f); //Сдвиг
    	glBindTexture(GL_TEXTURE_2D, texture[2]); //Скайбокс
    	DrawSphere(100000000.0f, 40, 40); //Радиус, широта, долгота
	glPopMatrix();
	//--------------------------------------------------------------------------------------------------------------------------------

	//-----------------------------------------Солнце---------------------------------------------------------------------------------
	glPushMatrix();
    	glTranslatef(0.0f, 0.0f, 0.0f); //Сдвиг
    	glRotatef(spin, 0.0f, 1.0f, 0.0f); //Вращение Солнца
    	glBindTexture(GL_TEXTURE_2D, texture[1]); //Текстура Солнца
    	DrawSphere(13920.0f, 40, 40); //Радиус, широта, долгота

		//-------------------Меркурий------------------------------------------------------------------------------
		glPushMatrix();
    		glTranslatef(57900.0f, 0.0f, 0.0f); //Сдвиг (расстояние центра Меркурия от центра Солнца)
    		glRotatef(spin, 0.0f, 1.0f, 0.0f); //Вращение Меркурия
    		glBindTexture(GL_TEXTURE_2D, texture[3]); //Текстура Меркурия
    		DrawSphere(2000.4397f, 40, 40); //Радиус, широта, долгота
		glPopMatrix();
		//--------------------------------------------------------------------------------------------------------------------------------

	glPopMatrix();
	spin += 0.2f;
	//--------------------------------------------------------------------------------------------------------------------------------
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

					float yawRad   = camYaw   * 3.14159265f / 180.0f;
					float pitchRad = camPitch * 3.14159265f / 180.0f;

					float cosPitch = cosf(pitchRad);
					float sinPitch = sinf(pitchRad);

					// «Вперёд» по взгляду (3D)
					float forwardX = -sinf(yawRad) * cosPitch;
					float forwardY =  sinPitch;
					float forwardZ = -cosf(yawRad) * cosPitch;

					// «Вправо» — всегда горизонтально, pitch не влияет
					float rightX =  cosf(yawRad);
					float rightZ = -sinf(yawRad);

					// Движение
					if(keys['W']) { camX += forwardX * 100.0f; camY += forwardY * 100.0f; camZ += forwardZ * 100.0f; }
					if(keys['S']) { camX -= forwardX * 100.0f; camY -= forwardY * 100.0f; camZ -= forwardZ * 100.0f; }
					if(keys['A']) { camX -= rightX   * 100.0f; camZ -= rightZ   * 100.0f; }
					if(keys['D']) { camX += rightX   * 100.0f; camZ += rightZ   * 100.0f; }

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
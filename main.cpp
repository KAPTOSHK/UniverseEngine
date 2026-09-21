#include <windows.h>  //Библиотека винды
#include <gl\gl.h>    //Основная библиотека OpenGL
#include <gl\glu.h>   //Глю тоже нужен
#include <tchar.h>    //Для надписей
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

HGLRC hRC = NULL;    //Контекст рендеринга
HDC hDC = NULL;      //Приватный контекст устройства GDI
HWND hWnd = NULL;    //Дискриптор окна
HINSTANCE hInstance; //Ну и наконец дескриптор ппредложения

bool keys[256];         //Массив булевых значений клавиш клавиат
bool active = true;     //Активно ли наше окно
bool fullscreen = true; //Переменная фуллскрина

GLfloat xrot;
GLfloat yrot;
GLfloat zrot;

GLuint texture[1];

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM); //Прототип функции WndProc

// Загрузка картинки и конвертирование в текстуру
GLvoid LoadGLTextures() {
    int width, height, nrChannels;
    
    // Переворачиваем текстуру по оси Y, так как у OpenGL и PNG разные начала координат
    stbi_set_flip_vertically_on_load(true); 

    // Загружаем картинку с помощью stb_image (принудительно запрашиваем 3 канала - RGB)
    unsigned char* data = stbi_load("Texture.png", &width, &height, &nrChannels, STBI_rgb);

	glEnable(GL_TEXTURE_2D);
    // Создание текстуры
    glGenTextures(1, &texture[0]);
    glBindTexture(GL_TEXTURE_2D, texture[0]);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    // Передаем данные в OpenGL. Вместо устаревшей "3" используем константу GL_RGB
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    
    // Обязательно освобождаем память, которую выделила stb_image
    stbi_image_free(data);
}

//Функция для изменения размеров окна
GLvoid ResizeGLScene(GLsizei width, GLsizei height) {
	if(height == 0) height = 1; //Предотвращаем деление на 0
	glViewport(0, 0, width, height); //Сброс текущей области вывода
	glMatrixMode(GL_PROJECTION); //Выбор матрицы проекций
	glLoadIdentity(); //Сброс матрицы проекций
	gluPerspective(45.0f, (GLfloat)width/ (GLfloat)height, 0.1f, 100.0f); //Узнаем соотношения размеров окна
	glMatrixMode(GL_MODELVIEW); //Выбор матрицы вида моделей
	glLoadIdentity(); //Сброс матрицы вида моделей
}

bool InitGL(GLsizei Width, GLsizei Height) {
	LoadGLTextures();           //Загрузка текстур
	glEnable(GL_TEXTURE_2D);    //Разрешение наложение текстуры
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0);
	glDepthFunc(GL_LESS);
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(45.0f, (GLfloat)Width / (GLfloat)Height, 0.1f, 100.0f);

	glMatrixMode(GL_MODELVIEW);
	return true;
}

//Функция отрисовки сцены
bool DrawGLScene(GLvoid) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Вычищаем буферы глубины и цвета
		
	glLoadIdentity();
	glTranslatef(1.5f, 0.0f, -6.0);
	glPushMatrix();

	glRotatef(xrot, 1.0f, 0.0f, 0.0f);
	glRotatef(yrot, 0.0f, 1.0f, 0.0f);
	glRotatef(zrot, 0.0f, 0.0f, 1.0f);

	glBindTexture(GL_TEXTURE_2D, texture[0]);

	glBegin(GL_QUADS);
		//Передняя грань
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);	//Низ лево
		glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);	//Низ право
		glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);	//Верх право
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);	//Верх лево

		//Задняя грань
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);	//Низ право
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);	//Верх право
		glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);	//Верх лево
		glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);	//Низ лево

		//Верхняя грань
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);	//Верх лево
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f,  1.0f,  1.0f);	//Низ лево
		glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f,  1.0f,  1.0f);	//Низ право
		glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);	//Верх право

		//Нижняя грань
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, -1.0f, -1.0f);	//Верх право
		glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f, -1.0f, -1.0f);	//Верх лево
		glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);	//Низ лево
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);	//Низ право

		//Правая грань
		glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);	//Низ право
		glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);	//Верх право
		glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);	//Верх лево
		glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);	//Низ лево

		//Левая грань
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);	//Низ лево
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);	//Низ право
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);	//Верх право
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);	//Верх лево
	glEnd();
	glPopMatrix();

	xrot += 0.3f; //Ось вращения X
	yrot += 0.2f; //Ось вращения Y
	zrot += 0.4f; //Ось вращения Z

	return true; //Успех
}

//Функция уничтожения окна
GLvoid KillGLWindow(GLvoid) {
	//Если в фуллскрине
	if(fullscreen) {
		ChangeDisplaySettings(NULL, 0); //Переходим в оконный
		ShowCursor(true); //Показываем курсор
	}
	//Есть ли контекст рендеринга?
	if(hRC) {
		//Возможно ли освободить RC и DC?
		if(!wglMakeCurrent(NULL, NULL)) MessageBox(NULL, "Release Of DC And RC Failed", "SHUTDOWN ERROR.", MB_OK | MB_ICONINFORMATION);
		//Возможно ли удалить RC?
		if(!wglDeleteContext(hRC)) MessageBox(NULL, "Release Rendering Context Failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		hRC = NULL;
	}
	//Возможно ли уничтожить DC?
	if(hDC && !ReleaseDC(hWnd, hDC)) {
        MessageBox( NULL, "Release Device Context Failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION );
        hDC=NULL;
	}
	//Возможно ли ничтожить окно?
	if(hWnd && !DestroyWindow(hWnd)) {
		MessageBox(NULL, "Could Not Release hWnd.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		hWnd = NULL;
	}
	//Возможно ли разрегистрировать класс?
	if(!UnregisterClass("OpenGL", hInstance)) {
		MessageBox(NULL, "Could Not Unregidter Class.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		hInstance = NULL;
	}
}

//Функция создания окна
bool CreateGLWindow(LPCTSTR title, int width, int height, int bits, bool fullscreenflag) {
	GLuint PixelFormat; //Формат пикселей
	WNDCLASS wc; //Структура класса окна
	DWORD dwExStyle; //Расширеный стиль окна
	DWORD dwStyle; //Обычный стиль окна
	
	RECT WindowRect; //Grabs Rectangle Upper Left / Lower Right Values
	WindowRect.left = (long)0; //Лево в 0
	WindowRect.right = (long)width; //Право в 0
	WindowRect.top = (long)0; //Верх в 0
	WindowRect.bottom = (long)height; //Низ в 0
	
	fullscreen = fullscreenflag; //Устанавливаем значение фуллскрина
	
	hInstance = GetModuleHandle(NULL); //Наш десриптор
	wc.style = CS_HREDRAW | CS_VREDRAW |CS_OWNDC; //Перерисуем при перемещении и создаем скрытый DC
	wc.lpfnWndProc = (WNDPROC) WndProc; //Процедура обработки сообщений
	wc.cbClsExtra = 0; //Нет дополнительной информаци для окна
	wc.cbWndExtra = 0; //Нет дополнительной информаци для окна
	wc.hInstance = hInstance; //Устанавливаем наш дескриптор
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO); //Иконка
	wc.hCursor = LoadCursor(NULL, IDC_ARROW); //Указатель мыши
	wc.hbrBackground = NULL; //Без фона
	wc.lpszMenuName = NULL; //Без меню
	wc.lpszClassName = "OpenGL"; //Имя класса
	
	//Если класс не зарегистрировался
	if(!RegisterClass(&wc)) {
		MessageBox(NULL, "Failed To Register The Window Class.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return false;
	}
	
	//Если мы в фуллскрине
	if(fullscreen) {
		DEVMODE dmScreenSettings; //Режим устройства
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings)); //Очистка для хранения
		dmScreenSettings.dmSize = sizeof(dmScreenSettings); //Размер структуры Devmode
		dmScreenSettings.dmPelsHeight = height; //Ширина экрана
		dmScreenSettings.dmBitsPerPel = bits; //Высота экрана
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT; //Режим пикселя
		//Пытаемся установить выбранный режим и получить результат. Примечание: CDS_FULLSCREEN убирает панель управления
		if(ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL) {
			//Если фуллскрин невозможен
			if( MessageBox(NULL, "The Requested Fullscreen Mode Is Not Supported By\nYour Video Card. Use Windowed Mode Instead?", "NeHe GL", MB_YESNO | MB_ICONEXCLAMATION) == IDYES ) fullscreen = false;
			else {
				MessageBox(NULL, "Program Will Now Close.", "ERROR", MB_OK | MB_ICONSTOP); //Закрываем окно
				return false; //Выходим из программы
			}
		}
	}
	//Если мы все еще в фулскрине
	if(fullscreen) {
		dwExStyle = WS_EX_APPWINDOW; //Расширеный стиль окна
		dwStyle = WS_POPUP; //Обычный стиль окна
		ShowCursor(false); //Скрыть курсор
	}
	//Если оконный режим
	else {
		dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE; //Расширеный стиль окна
		dwStyle = WS_OVERLAPPEDWINDOW; //Обычный стиль окна
	}
	
	AdjustWindowRectEx(&WindowRect, dwStyle, false, dwExStyle); //Подбираем окну размеры
	
	if (!(hWnd = CreateWindowEx(dwExStyle, _T("OpenGL"), title, WS_CLIPSIBLINGS | WS_CLIPCHILDREN | dwStyle, 0, 0, WindowRect.right-WindowRect.left, WindowRect.bottom-WindowRect.top, NULL, NULL, hInstance, NULL))) {
		KillGLWindow(); //Убиваем окно
		MessageBox(NULL, "Window Creation Error.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return false; //Выходим
	}
	
	//pfd сообщает Windows каким будет вывод на экран каждого пикселя
	static PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR), //Размер дескриптора
		1, //Версия
		PFD_DRAW_TO_WINDOW | //Формат окна
		PFD_SUPPORT_OPENGL | //Формат OpenGL
		PFD_DOUBLEBUFFER, //Формат для двойного буфера
		PFD_TYPE_RGBA, //Требуется RGBA
		(BYTE)bits, //Бит глубины цвета
		0, 0, 0, 0, 0, 0, //Игнор цветовых битов
		0, //Без буффера прозрачности
		0, //Игнор сдвигового бита
		0, //Нет буффера накопления
		0, 0, 0, 0, //Игнор битов накопления
		32, //32 битный Z-буфер (буфер глубины)
		0, //Нет буфера трафарета
		0, //Нет вспомогательных буферов
		PFD_MAIN_PLANE,
		0,
		0, 0, 0
	};
	
	if(!(hDC = GetDC(hWnd))) {
		KillGLWindow();
		MessageBox(NULL, "Can`t Create A GL Device Context.", "ERROR,", MB_OK | MB_ICONEXCLAMATION);
		return false;
	}
	
	if(!(PixelFormat = ChoosePixelFormat(hDC, &pfd))) {
    	KillGLWindow();
   		MessageBox(NULL, "Can't Find A Suitable PixelFormat.", "ERROR", MB_OK | MB_ICONEXCLAMATION );
    	return false;
	}
	
  	if(!SetPixelFormat(hDC, PixelFormat, &pfd)) {
		KillGLWindow();
		MessageBox(NULL, "Can't Set The PixelFormat.", "ERROR", MB_OK | MB_ICONEXCLAMATION );
    	return false;
	}
	
  	if(!(hRC = wglCreateContext(hDC))) {
    	KillGLWindow();
    	MessageBox(NULL, "Can't Create A GL Rendering Context.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return false;
	}
  
  	if(!wglMakeCurrent(hDC, hRC)) {
		KillGLWindow();
    	MessageBox(NULL, "Can't Activate The GL Rendering Context.", "ERROR", MB_OK | MB_ICONEXCLAMATION );
    	return false;
  	}
  	
  	ShowWindow(hWnd, SW_SHOW);
  	SetForegroundWindow(hWnd);
  	SetFocus(hWnd);
  	ResizeGLScene(width, height);
  	
  	if(!InitGL(width, height)) {
  		KillGLWindow();
  		MessageBox(NULL, _T("Initialization Failed."), _T("ERROR"), MB_OK | MB_ICONEXCLAMATION);
  		return false;
	}
	return true;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
		case WM_ACTIVATE:
			if(!HIWORD(wParam)) active = true;
			else active = false;
			return 0;
		case WM_SYSCOMMAND:
			switch(wParam) {
				case SC_SCREENSAVE:
				case SC_MONITORPOWER:
				return 0;
			}
			break;
		case WM_CLOSE:
			PostQuitMessage(0);
			return 0;
		case WM_KEYDOWN:
			keys[wParam] = true;
			return 0;
		case WM_KEYUP:
			keys[wParam] = false;
			return 0;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	MSG msg;
	bool done = false;
	
	if(MessageBox(NULL, "Do You Want To Launch Application In Fullscreen Mode?", "Launch In Fullscreen Mode?", MB_YESNO | MB_ICONQUESTION) == IDNO) fullscreen = false;
	if(!CreateGLWindow("Universe Engine", 1024, 768, 32, fullscreen)) return 0;
	
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
				if(keys[VK_ESCAPE]) done = true;
				else {
					DrawGLScene();
					SwapBuffers(hDC);
				}
			}
			
			if(keys[VK_F1]) {
				keys[VK_F1] = false;
				KillGLWindow();
				fullscreen = !fullscreen;
				if(!CreateGLWindow("Universe Engine", 1024, 768, 32, fullscreen)) return 0;
			}
		}
	}
	KillGLWindow();
	return(msg.wParam);
}
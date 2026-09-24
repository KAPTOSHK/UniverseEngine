#include <GL\glu.h>
#include <cmath>
#include <windows.h>
#include "InitGame.h"
#include "stb_image.h"
#include "Config.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM); //Прототип функции WndProc

void PerspectiveInfinite(float fovDeg, float aspect, float nearZ) {
	if (aspect <= 0.0f) aspect = 1.0f;   // ← защита
    if (nearZ <= 0.0f) nearZ = 0.1f;     // ← защита
	
    float fovRad = fovDeg * 3.14159265f / 180.0f;
    float f = 1.0f / tanf(fovRad / 2.0f);

    float m[16] = {
        f / aspect, 0,          0,   0,
        0,          f,          0,   0,
        0,          0,         -1,  -1,        // ← m[2][2] = -1, m[2][3] = -1
        0,          0, -2.0f * nearZ, 0        // ← m[3][2] = -2*near, m[3][3] = 0
    };

    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(m);
    glMatrixMode(GL_MODELVIEW);
}

GLuint LoadTexture(const char* filename) {
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);

    unsigned char* data = stbi_load(filename, &width, &height, &nrChannels, STBI_rgb);
    if (!data) {
        MessageBoxA(NULL, filename, "Texture load failed", MB_OK);
        return 0;
    }

    GLuint id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

    stbi_image_free(data);
    return id;
}

bool LoadTextures() {
    glEnable(GL_TEXTURE_2D);

    texture[0] = LoadTexture("Texture.png");
    texture[1] = LoadTexture("Textures/Sun/Sun_2K.png");
	texture[2] = LoadTexture("Textures/StarsMilkyWay/Stars_Milky_Way_2K.png");
	texture[3] = LoadTexture("Textures/Mercury/Mercury_2K.png");

    // если хоть одна не загрузилась — ошибка
    for (int i = 0; i < 4; ++i) if (texture[i] == 0) return false;
    return true;
}

//Функция для изменения размеров окна
GLvoid ResizeWindow(GLsizei width, GLsizei height) {
	if(height == 0) height = 1; //Предотвращаем деление на 0
	glViewport(0, 0, width, height); //Сброс текущей области вывода
	glMatrixMode(GL_PROJECTION); //Выбор матрицы проекций
	glLoadIdentity(); //Сброс матрицы проекций
	PerspectiveInfinite(45.0f, (GLfloat)width / (GLfloat)height, 0.1f); //Узнаем соотношения размеров окна
	glMatrixMode(GL_MODELVIEW); //Выбор матрицы вида моделей
	glLoadIdentity(); //Сброс матрицы вида моделей
}

bool InitGL(GLsizei Width, GLsizei Height) {
	if(!LoadTextures()) return false;
	//glEnable(GL_LIGHTING);
	//glEnable(GL_TEXTURE_2D);    //Разрешение наложение текстуры
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);
	glClearDepth(1.0);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);

	glColor4f(1.0f, 1.0f, 1.0f, 0.5f);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	PerspectiveInfinite(45.0f, (GLfloat)Width / (GLfloat)Height, 0.1f); //Узнаем соотношения размеров окна

	glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);  //Установка фонового света
	glLightfv(GL_LIGHT1, GL_DIFFUSE, LightAmbient);  //Установка диффузного света

	glMatrixMode(GL_MODELVIEW);
	return true;
}

//Функция создания окна
bool CreateGameWindow(LPCTSTR title, int width, int height, int bits, bool fullscreenflag) {
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
		MessageBoxA(NULL, "Failed To Register The Window Class.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
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
			if(MessageBoxA(NULL, "The Requested Fullscreen Mode Is Not Supported By\nYour Video Card. Use Windowed Mode Instead?", "NeHe GL", MB_YESNO | MB_ICONEXCLAMATION) == IDYES) fullscreen = false;
			else {
				MessageBoxA(NULL, "Program Will Now Close.", "ERROR", MB_OK | MB_ICONSTOP); //Закрываем окно
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
	
	if (!(hWnd = CreateWindowEx(dwExStyle, "OpenGL", title, WS_CLIPSIBLINGS | WS_CLIPCHILDREN | dwStyle, 0, 0, WindowRect.right-WindowRect.left, WindowRect.bottom-WindowRect.top, NULL, NULL, hInstance, NULL))) {
		KillWindow(); //Убиваем окно
		MessageBoxA(NULL, "Window Creation Error.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
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
		KillWindow();
		MessageBoxA(NULL, "Can`t Create A GL Device Context.", "ERROR,", MB_OK | MB_ICONEXCLAMATION);
		return false;
	}
	
	if(!(PixelFormat = ChoosePixelFormat(hDC, &pfd))) {
    	KillWindow();
   		MessageBoxA(NULL, "Can't Find A Suitable PixelFormat.", "ERROR", MB_OK | MB_ICONEXCLAMATION );
    	return false;
	}
	
  	if(!SetPixelFormat(hDC, PixelFormat, &pfd)) {
		KillWindow();
		MessageBoxA(NULL, "Can't Set The PixelFormat.", "ERROR", MB_OK | MB_ICONEXCLAMATION );
    	return false;
	}
	
  	if(!(hRC = wglCreateContext(hDC))) {
    	KillWindow();
    	MessageBoxA(NULL, "Can't Create A GL Rendering Context.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return false;
	}
  
  	if(!wglMakeCurrent(hDC, hRC)) {
		KillWindow();
    	MessageBoxA(NULL, "Can't Activate The GL Rendering Context.", "ERROR", MB_OK | MB_ICONEXCLAMATION );
    	return false;
  	}
  	
  	ShowWindow(hWnd, SW_SHOW);
  	SetForegroundWindow(hWnd);
  	SetFocus(hWnd);
  	ResizeWindow(width, height);
  	
  	if(!InitGL(width, height)) {
  		KillWindow();
  		MessageBoxA(NULL, "Initialization Failed.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
  		return false;
	}

	RAWINPUTDEVICE rid;
	rid.usUsagePage = 0x01;   // Generic Desktop Controls
	rid.usUsage     = 0x02;   // Mouse
	rid.dwFlags     = 0;
	rid.hwndTarget  = hWnd;

	if (!RegisterRawInputDevices(&rid, 1, sizeof(rid))) MessageBoxA(NULL, "Raw Input registration failed", "Error", MB_OK);

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
	
		case WM_INPUT: {
			if(!rmbDown) return 0;
		    UINT size = 0;
		    GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &size, sizeof(RAWINPUTHEADER));
		
		    BYTE* buffer = new BYTE[size];
		    if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, buffer, &size, sizeof(RAWINPUTHEADER)) == size) {
		        RAWINPUT* raw = (RAWINPUT*)buffer;
		        if (raw->header.dwType == RIM_TYPEMOUSE) {
		            mouseDX += raw->data.mouse.lLastX;
		            mouseDY += raw->data.mouse.lLastY;
		        }
		    }
		    delete[] buffer;
		    return 0;
		}

		case WM_SIZE:
    		if (hRC) {   // только если OpenGL уже инициализирован
    		    ResizeWindow(LOWORD(lParam), HIWORD(lParam));
    		}
    		return 0;

		case WM_RBUTTONDOWN:
		    rmbDown = true;
		    ShowCursor(false);        // спрятать курсор, пока держим RMB
		    return 0;

		case WM_RBUTTONUP:
		    rmbDown = false;
		    ShowCursor(true);         // вернуть курсор
		    return 0;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

//Функция уничтожения окна
GLvoid KillWindow(GLvoid) {
	//Если в фуллскрине
	if(fullscreen) {
		ChangeDisplaySettings(NULL, 0); //Переходим в оконный
		ShowCursor(true); //Показываем курсор
	}
	//Есть ли контекст рендеринга?
	if(hRC) {
		//Возможно ли освободить RC и DC?
		if(!wglMakeCurrent(NULL, NULL)) MessageBoxA(NULL, "Release Of DC And RC Failed", "SHUTDOWN ERROR.", MB_OK | MB_ICONINFORMATION);
		//Возможно ли удалить RC?
		if(!wglDeleteContext(hRC)) MessageBoxA(NULL, "Release Rendering Context Failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		hRC = NULL;
	}
	//Возможно ли уничтожить DC?
	if(hDC && !ReleaseDC(hWnd, hDC)) {
        MessageBoxA( NULL, "Release Device Context Failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION );
        hDC=NULL;
	}
	//Возможно ли ничтожить окно?
	if(hWnd && !DestroyWindow(hWnd)) {
		MessageBoxA(NULL, "Could Not Release hWnd.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		hWnd = NULL;
	}
	//Возможно ли разрегистрировать класс?
	if(!UnregisterClassA("OpenGL", hInstance)) {
		MessageBoxA(NULL, "Could Not Unregidter Class.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		hInstance = NULL;
	}
}
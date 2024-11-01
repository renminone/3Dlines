#include <Windows.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "tinyxml2.h"
#include <iostream>
#include <cmath>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

float intersectX, intersectY, intersectZ;

float cameraDistance = 100.0f; // Начальная дистанция камеры
float cameraAngleX = 45.0f;     // Угол по оси X
float cameraAngleY = 45.0f;     // Угол по оси Y
float offsetX = 0.0f; // Смещение по оси X
float offsetY = 0.0f; // Смещение по оси Y
int lastX, lastY;               // Хранит положение курсора
bool leftMouseDown = false;      // Флаг для ЛКМ
bool rightMouseDown = false;     // Флаг для ПКМ

void init()
{
	glClearColor(0, 0, 0, 1.0); // Черный фон
	glEnable(GL_DEPTH_TEST);     // Включение буфера глубины
}

void sectionplane(const std::string& filename)
{
	float x1, y1, z1, x2, y2, z2, x3, y3, z3, r, g, b;
	const float scaleFactor = 50.0f; // Коэффициент масштабирования
	tinyxml2::XMLDocument doc;

	// Загружаем XML-файл
	if (doc.LoadFile(filename.c_str()) != tinyxml2::XML_SUCCESS)
	{
		std::cerr << "Ошибка загрузки XML-файла!" << std::endl;
		return;
	}

	// Извлекаем данные из XML
	tinyxml2::XMLElement* lineElement = doc.FirstChildElement("line");
	if (!lineElement)
	{
		std::cerr << "Не найден элемент <line> в XML-файле!" << std::endl;
		return;
	}

	tinyxml2::XMLElement* firstElement = lineElement->FirstChildElement("first");
	if (firstElement)
	{
		firstElement->QueryFloatAttribute("x1", &x1);
		firstElement->QueryFloatAttribute("y1", &y1);
		firstElement->QueryFloatAttribute("z1", &z1);
	}

	tinyxml2::XMLElement* secondElement = lineElement->FirstChildElement("second");
	if (secondElement)
	{
		secondElement->QueryFloatAttribute("x2", &x2);
		secondElement->QueryFloatAttribute("y2", &y2);
		secondElement->QueryFloatAttribute("z2", &z2);
	}

	tinyxml2::XMLElement* thirdElement = lineElement->FirstChildElement("third");
	if (thirdElement)
	{
		thirdElement->QueryFloatAttribute("x3", &x3);
		thirdElement->QueryFloatAttribute("y3", &y3);
		thirdElement->QueryFloatAttribute("z3", &z3);
	}

	tinyxml2::XMLElement* colorElement = lineElement->FirstChildElement("color");
	if (colorElement)
	{
		colorElement->QueryFloatAttribute("r", &r);
		colorElement->QueryFloatAttribute("g", &g);
		colorElement->QueryFloatAttribute("b", &b);
	}

	// Вычисляем центр плоскости (среднее значение координат трех точек)
	float centerX = (x1 + x2 + x3) / 3.0f;
	float centerY = (y1 + y2 + y3) / 3.0f;
	float centerZ = (z1 + z2 + z3) / 3.0f;

	// Масштабируем точки относительно центра плоскости
	x1 = centerX + (x1 - centerX) * scaleFactor;
	y1 = centerY + (y1 - centerY) * scaleFactor;
	z1 = centerZ + (z1 - centerZ) * scaleFactor;

	x2 = centerX + (x2 - centerX) * scaleFactor;
	y2 = centerY + (y2 - centerY) * scaleFactor;
	z2 = centerZ + (z2 - centerZ) * scaleFactor;

	x3 = centerX + (x3 - centerX) * scaleFactor;
	y3 = centerY + (y3 - centerY) * scaleFactor;
	z3 = centerZ + (z3 - centerZ) * scaleFactor;

	// Начинаем отрисовку плоскости
	glBegin(GL_TRIANGLES);  // Используем треугольник для задания плоскости
	glColor3f(r, g, b); // Устанавливаем цвет плоскости
	glVertex3f(x1, y1, z1); // Первая точка
	glVertex3f(x2, y2, z2); // Вторая точка
	glVertex3f(x3, y3, z3); // Третья точка
	glEnd();
}

void drawLine(float x0, float y0, float z0, float length, const char* direction, float r, float g, float b)
{
	float x1 = x0, y1 = y0, z1 = z0;

	glColor3f(r, g, b);
	// Вычисляем конечные координаты в зависимости от направления
	if (strcmp(direction, "X") == 0) { x1 = x0 + length; }
	else if (strcmp(direction, "Y") == 0) { y1 = y0 + length; }
	else if (strcmp(direction, "Z") == 0) { z1 = z0 + length; }
	else if (strcmp(direction, "XY") == 0) { x1 = x0 + length / sqrt(2); y1 = y0 + length / sqrt(2); }
	else if (strcmp(direction, "YZ") == 0) { y1 = y0 + length / sqrt(2); z1 = z0 + length / sqrt(2); }
	else if (strcmp(direction, "XZ") == 0) { x1 = x0 + length / sqrt(2); z1 = z0 + length / sqrt(2); }

	glBegin(GL_LINES);
	glVertex3f(x0, y0, z0); // Начало линии
	glVertex3f(x1, y1, z1); // Конец линии
	glEnd();
}

void lineinit(const std::string & filename)
{
	float x0, y0, z0, length, r, g, b;
	const char* direction = nullptr; // Инициализируем direction как nullptr

	tinyxml2::XMLDocument doc;

	// Загружаем XML-файл
	if (doc.LoadFile(filename.c_str()) != tinyxml2::XML_SUCCESS)
	{
		std::cerr << "Ошибка загрузки XML-файла!" << std::endl;
		return;
	}

	// Получаем корневой элемент
	tinyxml2::XMLElement* lineElement = doc.FirstChildElement("line");
	if (!lineElement)
	{
		std::cerr << "Не найден элемент <line> в XML-файле!" << std::endl;
		return;
	}

	// Получаем начальные координаты
	tinyxml2::XMLElement* startElement = lineElement->FirstChildElement("start");
	if (startElement)
	{
		startElement->QueryFloatAttribute("x", &x0);
		startElement->QueryFloatAttribute("y", &y0);
		startElement->QueryFloatAttribute("z", &z0);
	}

	// Получаем длину линии
	tinyxml2::XMLElement* lengthElement = lineElement->FirstChildElement("length");
	if (lengthElement)
	{
		lengthElement->QueryFloatAttribute("value", &length);
	}

	// Получаем направление линии
	tinyxml2::XMLElement* directionElement = lineElement->FirstChildElement("direction");
	if (directionElement)
	{
		direction = directionElement->Attribute("value");
		if (direction == nullptr)
		{
			std::cerr << "Не удалось получить направление!" << std::endl;
			return;
		}
	}
	else
	{
		std::cerr << "Не найден элемент <direction> в XML-файле!" << std::endl;
		return;
	}

	// Получаем цвет линии
	tinyxml2::XMLElement* colorElement = lineElement->FirstChildElement("color");
	if (colorElement)
	{
		colorElement->QueryFloatAttribute("r", &r);
		colorElement->QueryFloatAttribute("g", &g);
		colorElement->QueryFloatAttribute("b", &b);
	}

	// Рисуем линию с полученными параметрами
	drawLine(x0, y0, z0, length, direction, r, g, b);
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Настройка перспективной проекции
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, 1.0, 1.0, 1000.0);

	// Установка камеры с учетом смещений
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(cameraDistance * cos(cameraAngleX * M_PI / 180.0) * sin(cameraAngleY * M_PI / 180.0) + offsetX,
		cameraDistance * sin(cameraAngleX * M_PI / 180.0) + offsetY,
		cameraDistance * cos(cameraAngleX * M_PI / 180.0) * cos(cameraAngleY * M_PI / 180.0),
		0.0 + offsetX, 0.0 + offsetY, 0.0, 0.0, 1.0, 0.0);

	glLineWidth(2);

	// Рисуем оси координат
	glBegin(GL_LINES);

	// Ось X - красная
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(50000.0f, 0.0f, 0.0f);

	// Ось Y - зеленая
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 50000.0f, 0.0f);

	// Ось Z - синяя
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 50000.0f);
	glEnd();

	sectionplane("Section.xml");
	lineinit("Line1.xml");
	lineinit("Line2.xml");
	lineinit("Line3.xml");

	glFlush();
	glutSwapBuffers();
}

void mouseWheel(int button, int dir, int x, int y)
{
	// Изменяем расстояние камеры в зависимости от прокрутки колесика мыши
	if (dir > 0)
	{ cameraDistance -= 5.0f; } // Приближаем камеру
	else
	{ cameraDistance += 5.0f; } // Удаляем камеру
	glutPostRedisplay(); // Перерисовываем сцену
}

void mouseMotion(int x, int y)
{
	if (leftMouseDown)
	{
		// Изменяем смещение при зажатой ПКМ
		offsetX += (x - lastX) * 0.1f; // Перемещение по X
		offsetY += (y - lastY) * 0.1f; // Перемещение по Y
	}
	else if (rightMouseDown)
	{
		// Изменяем угол обзора при зажатой ЛКМ
		cameraAngleY += (x - lastX) * 0.5f; // Вращение по Y
		cameraAngleX -= (y - lastY) * 0.5f; // Вращение по X

		// Ограничиваем угол по оси X (чтобы не перевернуть камеру)
		if (cameraAngleX < -89.0f) cameraAngleX = -89.0f;
		if (cameraAngleX > 89.0f) cameraAngleX = 89.0f;
	}

	lastX = x; // Обновляем последнее положение мыши по оси X
	lastY = y; // Обновляем последнее положение мыши по оси Y
	glutPostRedisplay(); // Перерисовываем сцену
}

void mouseButton(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON)
	{
		leftMouseDown = (state == GLUT_DOWN);
	}
	else if (button == GLUT_RIGHT_BUTTON)
	{
		rightMouseDown = (state == GLUT_DOWN);
	}

	lastX = x; // Сохраняем текущее положение мыши
	lastY = y;
}

void keyboard(unsigned char key, int x, int y)
{
	if (key == 'r' || key == 'R')
	{
		glutPostRedisplay();
	}
}
int main(int argc, char** argv)
{
	setlocale(LC_ALL, "Rus");
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(1920, 1080);
	glutCreateWindow("TestTask1");

	init();

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard); // Добавляем обработчик клавиатуры
	glutMouseFunc(mouseButton);         // Обработчик для нажатия кнопок мыши
	glutMotionFunc(mouseMotion);         // Обработчик для движения мыши
	glutMouseWheelFunc(mouseWheel);     // Обработчик для прокрутки колесика мыши

	//Временное решение.
	std::cout << "X - красная." << std::endl;
	std::cout << "Y - зелёная." << std::endl;
	std::cout << "Z - синяя." << std::endl;
	std::cout << "Колёсико мыши - приближение/отдаление." << std::endl;
	std::cout << "Зажатие ЛКМ - перемещение." << std::endl;
	std::cout << "Зажатие ПКМ - вращение." << std::endl;
	std::cout << "R - обновление экрана (если необходимо обновить параметры XML, то сделайте это в самом файле перед нажатием" << std::cout;

	glutMainLoop();
	return 0;
}

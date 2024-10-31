#include <Windows.h>
#include <GL\glew.h>
#include <GL\freeglut.h>
#include "tinyxml2.h"
#include <iostream>
#define elif else if

void init() {
	glClearColor(0, 0, 0, 1.0); // Черный фон
	glEnable(GL_DEPTH_TEST);     // Включение буфера глубины
}

void drawLine(float x0, float y0, float z0, float length, const char* direction, float r, float g, float b) {
	float x1 = x0, y1 = y0, z1 = z0;

	glColor3f(r, g, b);
	// Вычисляем конечные координаты в зависимости от направления
	if (strcmp(direction, "X") == 0) { x1 = x0 + length; }
	elif (strcmp(direction, "Y") == 0) { y1 = y0 + length; }
	elif (strcmp(direction, "Z") == 0) { z1 = z0 + length; }
	elif (strcmp(direction, "XY") == 0) { x1 = x0 + length / sqrt(2); y1 = y0 + length / sqrt(2); }
	elif (strcmp(direction, "YZ") == 0) { y1 = y0 + length / sqrt(2); z1 = z0 + length / sqrt(2); }
	elif (strcmp(direction, "XZ") == 0) { x1 = x0 + length / sqrt(2); z1 = z0 + length / sqrt(2); }
//	std::cout << x1 << " " << y1 << " " << z1 << std::endl; //отладочная строчка

	glBegin(GL_LINES);
	glVertex3f(x0, y0, z0); // Начало линии
	glVertex3f(x1, y1, z1); // Конец линии
	glEnd();
}

void lineinit(const std::string& filename) {
	float x0, y0, z0, length, r, g, b;
	const char* direction = nullptr; // Инициализируем direction как nullptr

	tinyxml2::XMLDocument doc;

	// Загружаем XML-файл
	if (doc.LoadFile(filename.c_str()) != tinyxml2::XML_SUCCESS) {
		std::cerr << "Ошибка загрузки XML-файла!" << std::endl;
		return;
	}

	// Получаем корневой элемент
	tinyxml2::XMLElement* lineElement = doc.FirstChildElement("line");
	if (!lineElement) {
		std::cerr << "Не найден элемент <line> в XML-файле!" << std::endl;
		return;
	}

	// Получаем начальные координаты
	tinyxml2::XMLElement* startElement = lineElement->FirstChildElement("start");
	if (startElement) {
		startElement->QueryFloatAttribute("x", &x0);
		startElement->QueryFloatAttribute("y", &y0);
		startElement->QueryFloatAttribute("z", &z0);
	}

	// Получаем длину линии
	tinyxml2::XMLElement* lengthElement = lineElement->FirstChildElement("length");
	if (lengthElement) {
		lengthElement->QueryFloatAttribute("value", &length);
	}

	// Получаем направление линии
	tinyxml2::XMLElement* directionElement = lineElement->FirstChildElement("direction");
	if (directionElement) {
		direction = directionElement->Attribute("value");
		if (direction == nullptr) {
			std::cerr << "Не удалось получить направление!" << std::endl;
			return;
		}
	}
	else {
		std::cerr << "Не найден элемент <direction> в XML-файле!" << std::endl;
		return;
	}

	// Получаем цвет линии
	tinyxml2::XMLElement* colorElement = lineElement->FirstChildElement("color");
	if (colorElement) {
		colorElement->QueryFloatAttribute("r", &r);
		colorElement->QueryFloatAttribute("g", &g);
		colorElement->QueryFloatAttribute("b", &b);
	}

	// Рисуем линию с полученными параметрами
	drawLine(x0, y0, z0, length, direction, r, g, b);
}

void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Настройка перспективной проекции
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, 1.0, 1.0, 1000.0);

	// Установка камеры
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(50.0, 50.0, 50.0,   // Камера отдалена для охвата всей сцены
		0.0, 0.0, 0.0,   // Точка, на которую смотрит камера
		0.0, 1.0, 0.0);  // Вектор "вверх"

	glLineWidth(2);

	// Рисуем оси координат
	glBegin(GL_LINES);

	// Ось X - красная
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(500.0f, 0.0f, 0.0f);

	// Ось Y - зеленая
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 500.0f, 0.0f);

	// Ось Z - синяя
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 500.0f);

	glEnd();
	lineinit("Line1.xml");
	lineinit("Line2.xml");
	lineinit("Line3.xml");

	glFlush();
	glutSwapBuffers();
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(1920, 1080);
	glutCreateWindow("TestTask1");

	init();

	glutDisplayFunc(display);
	glutMainLoop();
	return 0;
}

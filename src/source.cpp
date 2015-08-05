#include <QtGui>
#include <QApplication>
#include "window.h"
#include <memory>

int main(int argc, char *argv[])
{
QApplication a(argc, argv);
std::unique_ptr<Window> w(Window::create());
if (w.get() == nullptr)
	return 1;

w->show();
return a.exec();
}
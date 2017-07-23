#include "MyForm.h"
//#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
using namespace DIY_SIMRACING_12;

int main()
{
	
	MyForm^ interfaz = gcnew MyForm();

	// Create the main window and run it
	Application::Run(interfaz);
	
	return 0;
}
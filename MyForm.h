#pragma once
#include "SerialPort.h"
#using <System.dll>

namespace DIY_SIMRACING_12 {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace std;
	using namespace System::Runtime::InteropServices;

	/// <summary>
	/// Resumen de MyForm
	/// </summary>
	public ref class MyForm : public System::Windows::Forms::Form
	{
	public:
		MyForm(void)
		{
			InitializeComponent();
			//
			//TODO: agregar código de constructor aquí
			//
			//port = gcnew String();

			this->getPorts();
			//this->cambiarComboBox(0);

			timer1->Start();
		}
	private: 
		System::Windows::Forms::Timer^  timer1;
		String ^port;
		array<String ^> ^ portList;
	private: System::Windows::Forms::MenuStrip^  menuStrip1;
	private: System::Windows::Forms::ToolStripMenuItem^  archivoToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  ayudaToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  salirToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  acercaDeToolStripMenuItem;
			 SerialPort* arduino;

		void getPorts()
		{

			array<String ^> ^serialPorts = nullptr;
			try
			{
				// Get a list of serial port names.
				serialPorts = System::IO::Ports::SerialPort::GetPortNames();
			}
			catch (Win32Exception^ ex)
			{
				Console::WriteLine(ex->Message);
			}

			Console::WriteLine("The following serial ports were found:");
			// Display each port name to the console.
			for each(String^ port in serialPorts)
			{
			Console::WriteLine("\\\\.\\" + port);
			}

			//Si son distintos, refresca la lista de puertos
			if (!comparaArrayPuertos(portList, serialPorts)) {

				portList = serialPorts;
				this->comboBox1->Items->Clear();
				this->comboBox1->Items->AddRange(serialPorts);
			}
		}
		
		void conectar(String^ port) 
		{
			char* port_name = (char*)(void*) Marshal::StringToHGlobalAnsi(port);

			Console::WriteLine("conectar():: " + port);

			delete arduino;
			arduino = new SerialPort(port_name);

		}
		void sendDataArduino() {

			unsigned char trama[] = {0x11, 0xAC};
			arduino->writeSerialPort(trama, 2);

			for (size_t i = 0; i < 5; i++)
			{
				Console::WriteLine("sendDataArduino" + i);
			}
		}
		bool comparaArrayPuertos(array<String ^> ^ uno, array<String ^> ^ dos) {

			if (uno == nullptr || dos == nullptr  ||uno->GetLength(0) != dos->GetLength(0)) {
				return false;
			}
			else {
				for (int i = 0; i < uno->Length - 1; i++) {

					if (!(uno[i]->Equals(dos[i]))) return false;
				}
			}
			return true;
		}

	public:
		void cambiarComboBox(int index) {

			comboBox1->SelectedIndex = index;
		}
		void setLabelConnect(bool estado) {

			if (estado) {

				this->label5->Text = "Conectado [ C" + this->port->Split('C')[1] + " ]";
				this->label5->ForeColor = System::Drawing::Color::Green;
			}
			else {
				this->label5->Text = "Desconectado";
				this->label5->ForeColor = System::Drawing::Color::Red;
			}
		}
		void addListaCombo(array<System::Object^>^ items) {

			this->comboBox1->Items->Clear();
			this->comboBox1->Items->AddRange(items);
		}

	protected:
		/// <summary>
		/// Limpiar los recursos que se estén usando.
		/// </summary>
		~MyForm()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::ComponentModel::IContainer^  components;

	private:
		/// <summary>
		/// Variable del diseñador necesaria.
		/// </summary>

	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::GroupBox^  groupBox1;


	private: System::Windows::Forms::Label^  label5;
	private: System::Windows::Forms::GroupBox^  groupBox2;
	private: System::Windows::Forms::ComboBox^  comboBox1;





#pragma region Windows Form Designer generated code
		/// <summary>
		/// Método necesario para admitir el Diseñador. No se puede modificar
		/// el contenido de este método con el editor de código.
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
			this->comboBox1 = (gcnew System::Windows::Forms::ComboBox());
			this->groupBox2 = (gcnew System::Windows::Forms::GroupBox());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->timer1 = (gcnew System::Windows::Forms::Timer(this->components));
			this->menuStrip1 = (gcnew System::Windows::Forms::MenuStrip());
			this->archivoToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->ayudaToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->salirToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->acercaDeToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->groupBox1->SuspendLayout();
			this->groupBox2->SuspendLayout();
			this->menuStrip1->SuspendLayout();
			this->SuspendLayout();
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(16, 96);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(111, 13);
			this->label1->TabIndex = 2;
			this->label1->Text = L"Selecciona un puerto:";
			// 
			// groupBox1
			// 
			this->groupBox1->Controls->Add(this->comboBox1);
			this->groupBox1->Controls->Add(this->groupBox2);
			this->groupBox1->Controls->Add(this->label1);
			this->groupBox1->Location = System::Drawing::Point(12, 36);
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(260, 213);
			this->groupBox1->TabIndex = 4;
			this->groupBox1->TabStop = false;
			// 
			// comboBox1
			// 
			this->comboBox1->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->comboBox1->FormattingEnabled = true;
			this->comboBox1->Location = System::Drawing::Point(133, 93);
			this->comboBox1->Name = L"comboBox1";
			this->comboBox1->Size = System::Drawing::Size(93, 21);
			this->comboBox1->TabIndex = 8;
			this->comboBox1->SelectedIndexChanged += gcnew System::EventHandler(this, &MyForm::comboBox1_SelectedIndexChanged);
			// 
			// groupBox2
			// 
			this->groupBox2->Controls->Add(this->label5);
			this->groupBox2->Location = System::Drawing::Point(6, 19);
			this->groupBox2->Name = L"groupBox2";
			this->groupBox2->Size = System::Drawing::Size(248, 60);
			this->groupBox2->TabIndex = 7;
			this->groupBox2->TabStop = false;
			this->groupBox2->Text = L"Estado";
			// 
			// label5
			// 
			this->label5->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label5->Location = System::Drawing::Point(6, 16);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(236, 33);
			this->label5->TabIndex = 6;
			this->label5->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// timer1
			// 
			this->timer1->Interval = 500;
			this->timer1->Tick += gcnew System::EventHandler(this, &MyForm::timer1_Tick);
			// 
			// menuStrip1
			// 
			this->menuStrip1->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(2) {
				this->archivoToolStripMenuItem,
					this->ayudaToolStripMenuItem
			});
			this->menuStrip1->Location = System::Drawing::Point(0, 0);
			this->menuStrip1->Name = L"menuStrip1";
			this->menuStrip1->Size = System::Drawing::Size(284, 24);
			this->menuStrip1->TabIndex = 5;
			this->menuStrip1->Text = L"menuStrip";
			// 
			// archivoToolStripMenuItem
			// 
			this->archivoToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) { this->salirToolStripMenuItem });
			this->archivoToolStripMenuItem->Name = L"archivoToolStripMenuItem";
			this->archivoToolStripMenuItem->Size = System::Drawing::Size(60, 20);
			this->archivoToolStripMenuItem->Text = L"Archivo";
			// 
			// ayudaToolStripMenuItem
			// 
			this->ayudaToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) { this->acercaDeToolStripMenuItem });
			this->ayudaToolStripMenuItem->Name = L"ayudaToolStripMenuItem";
			this->ayudaToolStripMenuItem->Size = System::Drawing::Size(53, 20);
			this->ayudaToolStripMenuItem->Text = L"Ayuda";
			// 
			// salirToolStripMenuItem
			// 
			this->salirToolStripMenuItem->Name = L"salirToolStripMenuItem";
			this->salirToolStripMenuItem->Size = System::Drawing::Size(152, 22);
			this->salirToolStripMenuItem->Text = L"Salir";
			this->salirToolStripMenuItem->Click += gcnew System::EventHandler(this, &MyForm::salirToolStripMenuItem_Click);
			// 
			// acercaDeToolStripMenuItem
			// 
			this->acercaDeToolStripMenuItem->Name = L"acercaDeToolStripMenuItem";
			this->acercaDeToolStripMenuItem->Size = System::Drawing::Size(152, 22);
			this->acercaDeToolStripMenuItem->Text = L"Acerca de";
			// 
			// MyForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(284, 261);
			this->Controls->Add(this->groupBox1);
			this->Controls->Add(this->menuStrip1);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedSingle;
			this->MainMenuStrip = this->menuStrip1;
			this->MaximizeBox = false;
			this->Name = L"MyForm";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = L"Arduino Display 1.2";
			this->groupBox1->ResumeLayout(false);
			this->groupBox1->PerformLayout();
			this->groupBox2->ResumeLayout(false);
			this->menuStrip1->ResumeLayout(false);
			this->menuStrip1->PerformLayout();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion

		private: System::Void comboBox1_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {

			Object^ selectedItem = comboBox1->SelectedItem;
			String ^portNew = "\\\\.\\" + selectedItem->ToString();
			if (this->port == nullptr || !(this->port->Equals( portNew )) ) {

				this->port = portNew;
				Console::WriteLine("ComboBox::conectar():"+this->port);
				this->conectar(this->port);
			}
			
		}

		private: System::Void timer1_Tick(System::Object^  sender, System::EventArgs^  e) {

			//this->getPorts();

			//Codigo que se ejecuta cada cierto intervalo dentro de la interfaz
			if (arduino != nullptr && arduino->isConnected()) {
				
				this->setLabelConnect(true);
				//sendDataArduino();				
				Console::WriteLine("Connected: " + this->port);
			}
			else {
				this->setLabelConnect(false);
				Console::WriteLine("Disconnected");
			}
		}
		private: System::Void salirToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {

			Application::Exit();
		}
};

}

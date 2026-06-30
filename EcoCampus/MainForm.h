#pragma once
#define _BYTE_DEFINED

#include "Grafo.h"
#include <msclr/marshal_cppstd.h>
#include <windows.h>
#include <chrono>
#include <vector>
#include <string>

#undef byte

namespace EcoCampus {

    using namespace System;
    using namespace System::Windows::Forms;
    using namespace System::Drawing;
    using namespace System::Drawing::Drawing2D;

    public ref class MainForm : public Form {
    private:
        MapaCampus* mapa;
        TabControl^ tabControl;

        ComboBox^ cbInicioDFS;
        Button^ btnDFS;
        ListBox^ lbResultadosDFS;

        ComboBox^ cbInicioBFS;
        Button^ btnBFS;
        ListBox^ lbResultadosBFS;

        ComboBox^ cbOrigenCamino;
        ComboBox^ cbDestinoCamino;
        Button^ btnCamino;
        ListBox^ lbResultadosCamino;

        ComboBox^ cbEscenarioMovilidad;
        ComboBox^ cbTipoEstudiante;
        Button^ btnSimular;
        ListBox^ lbResultadosSimulacion;

        Button^ btnConexo;
        ListBox^ lbResultadosConexo;

        ListBox^ lbResultadosAnalisis;
        ComboBox^ cbAnalisisOrigen;
        ComboBox^ cbAnalisisDestino;
        Button^ btnComparar;

        ComboBox^ cbNodoBloqueo;
        Button^ btnBloqNodo;
        Button^ btnDesbloqNodo;
        ComboBox^ cbAristaOrigenBloq;
        ComboBox^ cbAristaDestinoBloq;
        Button^ btnBloqArista;
        Button^ btnDesbloqArista;
        ListBox^ lbResultadosBloqueos;
        ComboBox^ cbOrigenRecalculo;
        ComboBox^ cbDestinoRecalculo;

        PictureBox^ pbMapa;
        PictureBox^ pbDFS;
        PictureBox^ pbBFS;
        PictureBox^ pbCamino;
        PictureBox^ pbSimulacion;
        PictureBox^ pbAnalisis;
        PictureBox^ pbBloqueos;
        PictureBox^ pbConexo;

        Color colorFondo = Color::FromArgb(240, 248, 245);
        Color colorPanel = Color::White;
        Color colorPrimario = Color::FromArgb(46, 125, 90);
        Color colorAzul = Color::FromArgb(33, 150, 243);
        Color colorRojo = Color::FromArgb(198, 40, 40);
        Color colorTexto = Color::FromArgb(45, 45, 45);

        System::Drawing::Font^ fuenteTitulo = gcnew System::Drawing::Font(L"Segoe UI", 10, FontStyle::Bold);
        System::Drawing::Font^ fuenteNormal = gcnew System::Drawing::Font(L"Segoe UI", 9, FontStyle::Regular);
        System::Drawing::Font^ fuenteBoton = gcnew System::Drawing::Font(L"Segoe UI", 9, FontStyle::Bold);

        std::vector<int>* recorridoVisual;
        std::vector<std::pair<int, int>>* aristasVisuales;

        std::vector<int>* recorridoAnimacion;
        int pasoAnimacion = 0;
        Timer^ timerAnimacion;
        ListBox^ listaAnimacionActual;
        String^ tituloAnimacionFinal = L"";

        std::vector<int>* recorridoBFSAnimado;
        std::vector<int>* recorridoDFSAnimado;
        std::vector<int>* recorridoBFSVisual;
        std::vector<int>* recorridoDFSVisual;
        int pasoComparacion = 0;
        Timer^ timerComparacion;
        String^ tituloVisual = L"Mapa general";
        int nodoInicioVisual = -1;
        int nodoFinVisual = -1;
        bool mostrarSoloRuta = false;

    public:
        MainForm() {
            mapa = new MapaCampus();
            recorridoVisual = new std::vector<int>();
            aristasVisuales = new std::vector<std::pair<int, int>>();
            recorridoAnimacion = new std::vector<int>();
            recorridoBFSAnimado = new std::vector<int>();
            recorridoDFSAnimado = new std::vector<int>();
            recorridoBFSVisual = new std::vector<int>();
            recorridoDFSVisual = new std::vector<int>();

            timerAnimacion = gcnew Timer();
            timerAnimacion->Interval = 650;
            timerAnimacion->Tick += gcnew EventHandler(this, &MainForm::timerAnimacion_Tick);

            timerComparacion = gcnew Timer();
            timerComparacion->Interval = 850;
            timerComparacion->Tick += gcnew EventHandler(this, &MainForm::timerComparacion_Tick);

            listaAnimacionActual = nullptr;

            if (!mapa->cargarDesdeArchivo("campus.txt")) {
                MessageBox::Show("No se pudo cargar campus.txt", "Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
                Application::Exit();
                return;
            }

            InitializeComponent();
            CargarCombos();
        }

        ~MainForm() {
            delete mapa;
            delete recorridoVisual;
            delete aristasVisuales;
            delete recorridoAnimacion;
            delete recorridoBFSAnimado;
            delete recorridoDFSAnimado;
            delete recorridoBFSVisual;
            delete recorridoDFSVisual;
        }

    private:
        void EstilizarFormulario() {
            this->BackColor = colorFondo;
            this->ForeColor = colorTexto;
            this->Font = fuenteNormal;
        }

        void EstilizarTabControl() {
            tabControl->Appearance = TabAppearance::Normal;
            tabControl->SizeMode = TabSizeMode::Fixed;
            tabControl->ItemSize = Drawing::Size(170, 34);
            tabControl->Padding = Point(10, 6);
            tabControl->Font = gcnew System::Drawing::Font(L"Segoe UI", 9, FontStyle::Bold);
        }

        void EstilizarTabPage(TabPage^ tp) {
            tp->BackColor = colorFondo;
            tp->ForeColor = colorTexto;
        }

        void EstilizarLabel(Label^ lbl) {
            lbl->AutoSize = true;
            lbl->ForeColor = colorTexto;
            lbl->Font = fuenteTitulo;
            lbl->BackColor = Color::Transparent;
        }

        void EstilizarCombo(ComboBox^ cb, int ancho) {
            cb->Width = ancho;
            cb->DropDownStyle = ComboBoxStyle::DropDownList;
            cb->Font = fuenteNormal;
            cb->BackColor = Color::White;
            cb->ForeColor = colorTexto;
        }

        void EstilizarBoton(Button^ btn, Color fondo) {
            btn->BackColor = fondo;
            btn->ForeColor = Color::White;
            btn->FlatStyle = FlatStyle::Flat;
            btn->FlatAppearance->BorderSize = 0;
            btn->Font = fuenteBoton;
            btn->Height = 38;
            btn->Cursor = Cursors::Hand;
        }

        void EstilizarListBox(ListBox^ lb) {
            lb->BackColor = Color::White;
            lb->ForeColor = colorTexto;
            lb->BorderStyle = BorderStyle::FixedSingle;
            lb->Font = gcnew System::Drawing::Font(L"Consolas", 10, FontStyle::Regular);
            lb->ItemHeight = 20;
            lb->IntegralHeight = false;
        }

        Panel^ CrearPanelSuperior(int alto) {
            Panel^ panel = gcnew Panel();
            panel->Dock = DockStyle::Top;
            panel->Height = alto;
            panel->BackColor = colorPanel;
            panel->Padding = System::Windows::Forms::Padding(12);
            return panel;
        }

        int ObtenerIdSeleccionado(ComboBox^ cb) {
            if (cb == nullptr || cb->SelectedItem == nullptr) return -1;
            std::string nombre = msclr::interop::marshal_as<std::string>(cb->SelectedItem->ToString());
            return mapa->buscarNodoPorNombre(nombre);
        }

        int ObtenerIdPorNombre(const std::string& nombre) {
            return mapa->buscarNodoPorNombre(nombre);
        }

        void SeleccionarPorNombre(ComboBox^ cb, String^ nombre) {
            int idx = cb->FindStringExact(nombre);
            if (idx >= 0) cb->SelectedIndex = idx;
        }

        Color ObtenerColorNodo(const Punto& n) {
            std::string nombre = n.nombre;

            if (nombre.find("Entrada") != std::string::npos)
                return Color::FromArgb(46, 204, 113);

            if (nombre.find("Parqueo") != std::string::npos)
                return Color::FromArgb(149, 165, 166);

            if (nombre.find("Comedor") != std::string::npos)
                return Color::FromArgb(243, 156, 18);

            if (nombre.find("Centro de Referencia") != std::string::npos)
                return Color::FromArgb(155, 89, 182);

            if (nombre.find("Puente") != std::string::npos)
                return Color::FromArgb(241, 196, 15);

            if (nombre.find("Administrativas") != std::string::npos)
                return Color::FromArgb(52, 152, 219);

            if (nombre.find("Lactancia") != std::string::npos)
                return Color::FromArgb(231, 76, 60);

            if (nombre.find("Auditorio") != std::string::npos)
                return Color::FromArgb(230, 126, 34);

            if (nombre.find("Piso") != std::string::npos)
                return Color::FromArgb(52, 152, 219);

            if (!n.esInterior)
                return Color::FromArgb(129, 199, 132);

            return Color::FromArgb(207, 216, 220);
        }

        Point ObtenerPunto(double x, double y, float escala, float offsetX, float offsetY) {
            return Point(
                (int)(x * escala + offsetX),
                (int)(y * escala + offsetY)
            );
        }

        void LimpiarVisualizacion() {
            if (timerAnimacion != nullptr) timerAnimacion->Stop();
            if (timerComparacion != nullptr) timerComparacion->Stop();
            recorridoVisual->clear();
            aristasVisuales->clear();
            recorridoAnimacion->clear();
            recorridoBFSAnimado->clear();
            recorridoDFSAnimado->clear();
            recorridoBFSVisual->clear();
            recorridoDFSVisual->clear();
            tituloVisual = L"Mapa general";
            nodoInicioVisual = -1;
            nodoFinVisual = -1;
            mostrarSoloRuta = false;
        }

        void ConstruirAristasDesdeRecorrido(const std::vector<int>& recorrido) {
            aristasVisuales->clear();

            for (size_t i = 1; i < recorrido.size(); i++) {
                aristasVisuales->push_back(std::make_pair(recorrido[i - 1], recorrido[i]));
            }
        }

        bool EsAristaResaltada(int a, int b) {
            for (size_t i = 0; i < aristasVisuales->size(); i++) {
                int x = (*aristasVisuales)[i].first;
                int y = (*aristasVisuales)[i].second;

                if ((x == a && y == b) || (x == b && y == a)) {
                    return true;
                }
            }
            return false;
        }

        bool EsNodoResaltado(int idNodo) {
            for (size_t i = 0; i < recorridoVisual->size(); i++) {
                if ((*recorridoVisual)[i] == idNodo) return true;
            }
            return false;
        }

        PictureBox^ CrearMapaVisual() {
            PictureBox^ pb = gcnew PictureBox();
            pb->Dock = DockStyle::Fill;
            pb->BackColor = Color::FromArgb(245, 250, 247);
            pb->BorderStyle = BorderStyle::FixedSingle;
            pb->Paint += gcnew PaintEventHandler(this, &MainForm::pbMapa_Paint);
            return pb;
        }

        TableLayoutPanel^ CrearVistaConMapa(ListBox^ lista, PictureBox^ mapaVisual) {
            TableLayoutPanel^ layout = gcnew TableLayoutPanel();

            layout->Dock = DockStyle::Fill;
            layout->ColumnCount = 2;
            layout->RowCount = 1;
            layout->BackColor = colorFondo;

            // Parte escrita a la izquierda
            layout->ColumnStyles->Add(gcnew ColumnStyle(SizeType::Percent, 40));

            // Mapa a la derecha
            layout->ColumnStyles->Add(gcnew ColumnStyle(SizeType::Percent, 60));

            lista->Dock = DockStyle::Fill;
            lista->Margin = System::Windows::Forms::Padding(4);

            mapaVisual->Dock = DockStyle::Fill;
            mapaVisual->BackColor = Color::FromArgb(245, 250, 247);
            mapaVisual->BorderStyle = BorderStyle::FixedSingle;
            mapaVisual->Margin = System::Windows::Forms::Padding(4);

            layout->Controls->Add(lista, 0, 0);
            layout->Controls->Add(mapaVisual, 1, 0);

            return layout;
        }

        void InvalidarMapas() {
            if (pbMapa != nullptr) pbMapa->Invalidate();
            if (pbDFS != nullptr) pbDFS->Invalidate();
            if (pbBFS != nullptr) pbBFS->Invalidate();
            if (pbCamino != nullptr) pbCamino->Invalidate();
            if (pbSimulacion != nullptr) pbSimulacion->Invalidate();
            if (pbAnalisis != nullptr) pbAnalisis->Invalidate();
            if (pbBloqueos != nullptr) pbBloqueos->Invalidate();
            if (pbConexo != nullptr) pbConexo->Invalidate();
        }

        bool ExisteEnVector(std::vector<int>* vec, int idNodo) {
            if (vec == nullptr) return false;
            for (size_t i = 0; i < vec->size(); i++) {
                if ((*vec)[i] == idNodo) return true;
            }
            return false;
        }

        bool EsAristaDeVector(std::vector<int>* vec, int a, int b) {
            if (vec == nullptr || vec->size() < 2) return false;
            for (size_t i = 1; i < vec->size(); i++) {
                int x = (*vec)[i - 1];
                int y = (*vec)[i];
                if ((x == a && y == b) || (x == b && y == a)) return true;
            }
            return false;
        }

        void IniciarAnimacion(std::vector<int> recorridoCompleto, ListBox^ lista, String^ tituloDurante, String^ tituloFinal) {
            timerAnimacion->Stop();
            timerComparacion->Stop();

            recorridoVisual->clear();
            aristasVisuales->clear();
            recorridoAnimacion->clear();
            recorridoBFSVisual->clear();
            recorridoDFSVisual->clear();

            for (int id : recorridoCompleto) {
                recorridoAnimacion->push_back(id);
            }

            pasoAnimacion = 0;
            listaAnimacionActual = lista;
            tituloVisual = tituloDurante;
            tituloAnimacionFinal = tituloFinal;

            if (listaAnimacionActual != nullptr) {
                listaAnimacionActual->Items->Add(L"");
                listaAnimacionActual->Items->Add(L"--- ANIMACION PASO A PASO ---");
            }

            InvalidarMapas();
            timerAnimacion->Start();
        }

        void timerAnimacion_Tick(Object^ sender, EventArgs^ e) {
            if (pasoAnimacion >= (int)recorridoAnimacion->size()) {
                timerAnimacion->Stop();
                tituloVisual = tituloAnimacionFinal;
                InvalidarMapas();
                return;
            }

            int nodoActual = (*recorridoAnimacion)[pasoAnimacion];

            recorridoVisual->push_back(nodoActual);

            if (recorridoVisual->size() >= 2) {
                int anterior = (*recorridoVisual)[recorridoVisual->size() - 2];
                int actual = (*recorridoVisual)[recorridoVisual->size() - 1];
                aristasVisuales->push_back(std::make_pair(anterior, actual));
            }

            if (listaAnimacionActual != nullptr) {
                String^ nombre = gcnew String(mapa->nombreNodo(nodoActual).c_str());
                listaAnimacionActual->Items->Add(String::Format(L"Paso {0}: {1}", pasoAnimacion + 1, nombre));
                listaAnimacionActual->TopIndex = listaAnimacionActual->Items->Count - 1;
            }

            pasoAnimacion++;
            InvalidarMapas();
        }

        void IniciarComparacionAnimada(std::vector<int> bfs, std::vector<int> dfs) {
            timerAnimacion->Stop();
            timerComparacion->Stop();

            recorridoVisual->clear();
            aristasVisuales->clear();

            recorridoBFSVisual->clear();
            recorridoDFSVisual->clear();

            recorridoBFSAnimado->clear();
            recorridoDFSAnimado->clear();

            for (int id : bfs) recorridoBFSAnimado->push_back(id);
            for (int id : dfs) recorridoDFSAnimado->push_back(id);

            pasoComparacion = 0;
            tituloVisual = L"Comparacion animada BFS vs DFS";

            lbResultadosAnalisis->Items->Add(L"");
            lbResultadosAnalisis->Items->Add(L"--- COMPARACION ANIMADA ---");
            lbResultadosAnalisis->Items->Add(L"Azul = BFS");
            lbResultadosAnalisis->Items->Add(L"Morado = DFS");

            InvalidarMapas();
            timerComparacion->Start();
        }

        void timerComparacion_Tick(Object^ sender, EventArgs^ e) {
            bool quedaBFS = pasoComparacion < (int)recorridoBFSAnimado->size();
            bool quedaDFS = pasoComparacion < (int)recorridoDFSAnimado->size();

            if (!quedaBFS && !quedaDFS) {
                timerComparacion->Stop();
                tituloVisual = L"Comparacion BFS vs DFS finalizada";
                InvalidarMapas();
                return;
            }

            if (quedaBFS) {
                int nodoBFS = (*recorridoBFSAnimado)[pasoComparacion];
                recorridoBFSVisual->push_back(nodoBFS);

                String^ nombreBFS = gcnew String(mapa->nombreNodo(nodoBFS).c_str());
                lbResultadosAnalisis->Items->Add(String::Format(L"BFS paso {0}: {1}", pasoComparacion + 1, nombreBFS));
            }

            if (quedaDFS) {
                int nodoDFS = (*recorridoDFSAnimado)[pasoComparacion];
                recorridoDFSVisual->push_back(nodoDFS);

                String^ nombreDFS = gcnew String(mapa->nombreNodo(nodoDFS).c_str());
                lbResultadosAnalisis->Items->Add(String::Format(L"DFS paso {0}: {1}", pasoComparacion + 1, nombreDFS));
            }

            lbResultadosAnalisis->TopIndex = lbResultadosAnalisis->Items->Count - 1;
            pasoComparacion++;
            InvalidarMapas();
        }


        void ReiniciarMapaVisual() {
            if (timerAnimacion != nullptr) timerAnimacion->Stop();
            if (timerComparacion != nullptr) timerComparacion->Stop();

            recorridoVisual->clear();
            aristasVisuales->clear();
            recorridoAnimacion->clear();
            recorridoBFSAnimado->clear();
            recorridoDFSAnimado->clear();
            recorridoBFSVisual->clear();
            recorridoDFSVisual->clear();

            tituloVisual = L"Mapa general";
            nodoInicioVisual = -1;
            nodoFinVisual = -1;
            mostrarSoloRuta = false;
            listaAnimacionActual = nullptr;

            InvalidarMapas();
        }

        void tabControl_SelectedIndexChanged(Object^ sender, EventArgs^ e) {
            ReiniciarMapaVisual();
        }

        void InitializeComponent() {
            this->Text = L"Sistema de Navegacion Eco-Campus UTN";
            this->Size = Drawing::Size(1200, 760);
            this->StartPosition = FormStartPosition::CenterScreen;
            this->MinimumSize = Drawing::Size(950, 650);

            EstilizarFormulario();

            tabControl = gcnew TabControl();
            tabControl->Dock = DockStyle::Fill;
            EstilizarTabControl();
            tabControl->SelectedIndexChanged += gcnew EventHandler(this, &MainForm::tabControl_SelectedIndexChanged);

            TabPage^ tpMapa = gcnew TabPage(L"Ver mapa");
            EstilizarTabPage(tpMapa);

            pbMapa = gcnew PictureBox();
            pbMapa->Dock = DockStyle::Fill;
            pbMapa->BackColor = Color::FromArgb(245, 250, 247);
            pbMapa->BorderStyle = BorderStyle::FixedSingle;
            pbMapa->Paint += gcnew PaintEventHandler(this, &MainForm::pbMapa_Paint);
            tpMapa->Controls->Add(pbMapa);

            TabPage^ tpDFS = gcnew TabPage(L"Recorrido DFS");
            EstilizarTabPage(tpDFS);

            Panel^ panelDFS = CrearPanelSuperior(95);

            Label^ lblDFS = gcnew Label();
            lblDFS->Text = L"Nodo de inicio:";
            lblDFS->Location = Point(20, 15);
            EstilizarLabel(lblDFS);

            cbInicioDFS = gcnew ComboBox();
            cbInicioDFS->Location = Point(20, 45);
            EstilizarCombo(cbInicioDFS, 260);

            btnDFS = gcnew Button();
            btnDFS->Text = L"Ejecutar DFS";
            btnDFS->Location = Point(300, 42);
            btnDFS->Width = 140;
            EstilizarBoton(btnDFS, colorPrimario);
            btnDFS->Click += gcnew EventHandler(this, &MainForm::btnDFS_Click);

            panelDFS->Controls->Add(lblDFS);
            panelDFS->Controls->Add(cbInicioDFS);
            panelDFS->Controls->Add(btnDFS);

            lbResultadosDFS = gcnew ListBox();
            lbResultadosDFS->Dock = DockStyle::Fill;
            EstilizarListBox(lbResultadosDFS);

            pbDFS = CrearMapaVisual();
            tpDFS->Controls->Add(CrearVistaConMapa(lbResultadosDFS, pbDFS));
            tpDFS->Controls->Add(panelDFS);

            TabPage^ tpBFS = gcnew TabPage(L"Recorrido BFS");
            EstilizarTabPage(tpBFS);

            Panel^ panelBFS = CrearPanelSuperior(95);

            Label^ lblBFS = gcnew Label();
            lblBFS->Text = L"Nodo de inicio:";
            lblBFS->Location = Point(20, 15);
            EstilizarLabel(lblBFS);

            cbInicioBFS = gcnew ComboBox();
            cbInicioBFS->Location = Point(20, 45);
            EstilizarCombo(cbInicioBFS, 260);

            btnBFS = gcnew Button();
            btnBFS->Text = L"Ejecutar BFS";
            btnBFS->Location = Point(300, 42);
            btnBFS->Width = 140;
            EstilizarBoton(btnBFS, colorAzul);
            btnBFS->Click += gcnew EventHandler(this, &MainForm::btnBFS_Click);

            panelBFS->Controls->Add(lblBFS);
            panelBFS->Controls->Add(cbInicioBFS);
            panelBFS->Controls->Add(btnBFS);

            lbResultadosBFS = gcnew ListBox();
            lbResultadosBFS->Dock = DockStyle::Fill;
            EstilizarListBox(lbResultadosBFS);

            pbBFS = CrearMapaVisual();
            tpBFS->Controls->Add(CrearVistaConMapa(lbResultadosBFS, pbBFS));
            tpBFS->Controls->Add(panelBFS);

            TabPage^ tpConexo = gcnew TabPage(L"Verificar conexidad");
            EstilizarTabPage(tpConexo);

            Panel^ panelConexo = CrearPanelSuperior(95);

            btnConexo = gcnew Button();
            btnConexo->Text = L"Verificar conectividad";
            btnConexo->Location = Point(20, 30);
            btnConexo->Width = 220;
            EstilizarBoton(btnConexo, colorPrimario);
            btnConexo->Click += gcnew EventHandler(this, &MainForm::btnConexo_Click);

            panelConexo->Controls->Add(btnConexo);

            lbResultadosConexo = gcnew ListBox();
            lbResultadosConexo->Dock = DockStyle::Fill;
            EstilizarListBox(lbResultadosConexo);

            pbConexo = CrearMapaVisual();
            tpConexo->Controls->Add(CrearVistaConMapa(lbResultadosConexo, pbConexo));
            tpConexo->Controls->Add(panelConexo);

            TabPage^ tpCamino = gcnew TabPage(L"Buscar camino");
            EstilizarTabPage(tpCamino);

            Panel^ panelCamino = CrearPanelSuperior(110);

            Label^ lblOrigen = gcnew Label();
            lblOrigen->Text = L"Origen:";
            lblOrigen->Location = Point(20, 15);
            EstilizarLabel(lblOrigen);

            cbOrigenCamino = gcnew ComboBox();
            cbOrigenCamino->Location = Point(20, 45);
            EstilizarCombo(cbOrigenCamino, 250);

            Label^ lblDestino = gcnew Label();
            lblDestino->Text = L"Destino:";
            lblDestino->Location = Point(300, 15);
            EstilizarLabel(lblDestino);

            cbDestinoCamino = gcnew ComboBox();
            cbDestinoCamino->Location = Point(300, 45);
            EstilizarCombo(cbDestinoCamino, 250);

            btnCamino = gcnew Button();
            btnCamino->Text = L"Buscar camino DFS";
            btnCamino->Location = Point(580, 42);
            btnCamino->Width = 170;
            EstilizarBoton(btnCamino, colorPrimario);
            btnCamino->Click += gcnew EventHandler(this, &MainForm::btnCamino_Click);

            panelCamino->Controls->Add(lblOrigen);
            panelCamino->Controls->Add(cbOrigenCamino);
            panelCamino->Controls->Add(lblDestino);
            panelCamino->Controls->Add(cbDestinoCamino);
            panelCamino->Controls->Add(btnCamino);

            lbResultadosCamino = gcnew ListBox();
            lbResultadosCamino->Dock = DockStyle::Fill;
            EstilizarListBox(lbResultadosCamino);

            pbCamino = CrearMapaVisual();
            tpCamino->Controls->Add(CrearVistaConMapa(lbResultadosCamino, pbCamino));
            tpCamino->Controls->Add(panelCamino);

            TabPage^ tpSimulacion = gcnew TabPage(L"Simulacion de escenarios");
            EstilizarTabPage(tpSimulacion);

            Panel^ panelSim = CrearPanelSuperior(95);

            Label^ lblMovilidad = gcnew Label();
            lblMovilidad->Text = L"Tipo de movilidad:";
            lblMovilidad->Location = Point(20, 15);
            EstilizarLabel(lblMovilidad);

            cbEscenarioMovilidad = gcnew ComboBox();
            cbEscenarioMovilidad->Location = Point(20, 45);
            EstilizarCombo(cbEscenarioMovilidad, 280);
            cbEscenarioMovilidad->Items->Add(L"Normal");
            cbEscenarioMovilidad->Items->Add(L"Silla de ruedas");
            cbEscenarioMovilidad->Items->Add(L"Muletas");
            cbEscenarioMovilidad->Items->Add(L"Dificultad con pendientes");
            cbEscenarioMovilidad->SelectedIndex = 0;
            cbEscenarioMovilidad->SelectedIndexChanged += gcnew EventHandler(this, &MainForm::cbMovilidad_Changed);

            Label^ lblEstudiante = gcnew Label();
            lblEstudiante->Text = L"Tipo de estudiante:";
            lblEstudiante->Location = Point(340, 15);
            EstilizarLabel(lblEstudiante);

            cbTipoEstudiante = gcnew ComboBox();
            cbTipoEstudiante->Location = Point(340, 45);
            EstilizarCombo(cbTipoEstudiante, 220);
            cbTipoEstudiante->Items->Add(L"Regular");
            cbTipoEstudiante->Items->Add(L"Nuevo");
            cbTipoEstudiante->SelectedIndex = 0;

            btnSimular = gcnew Button();
            btnSimular->Text = L"Simular ruta";
            btnSimular->Location = Point(600, 42);
            btnSimular->Width = 170;
            EstilizarBoton(btnSimular, colorPrimario);
            btnSimular->Click += gcnew EventHandler(this, &MainForm::btnSimular_Click);

            panelSim->Controls->Add(lblMovilidad);
            panelSim->Controls->Add(cbEscenarioMovilidad);
            panelSim->Controls->Add(lblEstudiante);
            panelSim->Controls->Add(cbTipoEstudiante);
            panelSim->Controls->Add(btnSimular);

            lbResultadosSimulacion = gcnew ListBox();
            lbResultadosSimulacion->Dock = DockStyle::Fill;
            EstilizarListBox(lbResultadosSimulacion);

            pbSimulacion = CrearMapaVisual();
            tpSimulacion->Controls->Add(CrearVistaConMapa(lbResultadosSimulacion, pbSimulacion));
            tpSimulacion->Controls->Add(panelSim);

            TabPage^ tpAnalisis = gcnew TabPage(L"Analisis de complejidad");
            EstilizarTabPage(tpAnalisis);

            Panel^ panelAnalisis = CrearPanelSuperior(140);

            Label^ lblAnalisisOrigen = gcnew Label();
            lblAnalisisOrigen->Text = L"Nodo de inicio:";
            lblAnalisisOrigen->Location = Point(20, 15);
            EstilizarLabel(lblAnalisisOrigen);

            cbAnalisisOrigen = gcnew ComboBox();
            cbAnalisisOrigen->Location = Point(20, 45);
            EstilizarCombo(cbAnalisisOrigen, 250);

            Label^ lblAnalisisDestino = gcnew Label();
            lblAnalisisDestino->Text = L"Nodo destino:";
            lblAnalisisDestino->Location = Point(300, 15);
            EstilizarLabel(lblAnalisisDestino);

            cbAnalisisDestino = gcnew ComboBox();
            cbAnalisisDestino->Location = Point(300, 45);
            EstilizarCombo(cbAnalisisDestino, 250);

            btnComparar = gcnew Button();
            btnComparar->Text = L"Comparar BFS vs DFS";
            btnComparar->Location = Point(580, 42);
            btnComparar->Width = 190;
            EstilizarBoton(btnComparar, colorAzul);
            btnComparar->Click += gcnew EventHandler(this, &MainForm::btnComparar_Click);

            panelAnalisis->Controls->Add(lblAnalisisOrigen);
            panelAnalisis->Controls->Add(cbAnalisisOrigen);
            panelAnalisis->Controls->Add(lblAnalisisDestino);
            panelAnalisis->Controls->Add(cbAnalisisDestino);
            panelAnalisis->Controls->Add(btnComparar);

            lbResultadosAnalisis = gcnew ListBox();
            lbResultadosAnalisis->Dock = DockStyle::Fill;
            EstilizarListBox(lbResultadosAnalisis);

            pbAnalisis = CrearMapaVisual();
            tpAnalisis->Controls->Add(CrearVistaConMapa(lbResultadosAnalisis, pbAnalisis));
            tpAnalisis->Controls->Add(panelAnalisis);

            TabPage^ tpBloqueos = gcnew TabPage(L"Puntos de fallos y bloqueos");
            EstilizarTabPage(tpBloqueos);

            Panel^ panelBloq = CrearPanelSuperior(210);

            Label^ lblNodo = gcnew Label();
            lblNodo->Text = L"Bloquear nodo:";
            lblNodo->Location = Point(20, 15);
            EstilizarLabel(lblNodo);

            cbNodoBloqueo = gcnew ComboBox();
            cbNodoBloqueo->Location = Point(20, 45);
            EstilizarCombo(cbNodoBloqueo, 250);

            btnBloqNodo = gcnew Button();
            btnBloqNodo->Text = L"Bloquear";
            btnBloqNodo->Location = Point(290, 42);
            btnBloqNodo->Width = 120;
            EstilizarBoton(btnBloqNodo, colorRojo);
            btnBloqNodo->Click += gcnew EventHandler(this, &MainForm::btnBloquearNodo_Click);

            btnDesbloqNodo = gcnew Button();
            btnDesbloqNodo->Text = L"Desbloquear";
            btnDesbloqNodo->Location = Point(420, 42);
            btnDesbloqNodo->Width = 120;
            EstilizarBoton(btnDesbloqNodo, colorPrimario);
            btnDesbloqNodo->Click += gcnew EventHandler(this, &MainForm::btnDesbloquearNodo_Click);

            Label^ lblArista = gcnew Label();
            lblArista->Text = L"Bloquear arista:";
            lblArista->Location = Point(20, 90);
            EstilizarLabel(lblArista);

            cbAristaOrigenBloq = gcnew ComboBox();
            cbAristaOrigenBloq->Location = Point(20, 120);
            EstilizarCombo(cbAristaOrigenBloq, 220);

            cbAristaDestinoBloq = gcnew ComboBox();
            cbAristaDestinoBloq->Location = Point(250, 120);
            EstilizarCombo(cbAristaDestinoBloq, 220);

            btnBloqArista = gcnew Button();
            btnBloqArista->Text = L"Bloquear";
            btnBloqArista->Location = Point(490, 117);
            btnBloqArista->Width = 120;
            EstilizarBoton(btnBloqArista, colorRojo);
            btnBloqArista->Click += gcnew EventHandler(this, &MainForm::btnBloquearArista_Click);

            btnDesbloqArista = gcnew Button();
            btnDesbloqArista->Text = L"Desbloquear";
            btnDesbloqArista->Location = Point(620, 117);
            btnDesbloqArista->Width = 120;
            EstilizarBoton(btnDesbloqArista, colorPrimario);
            btnDesbloqArista->Click += gcnew EventHandler(this, &MainForm::btnDesbloquearArista_Click);

            Label^ lblOrigenRec = gcnew Label();
            lblOrigenRec->Text = L"Origen para recalculo:";
            lblOrigenRec->Location = Point(770, 15);
            EstilizarLabel(lblOrigenRec);

            cbOrigenRecalculo = gcnew ComboBox();
            cbOrigenRecalculo->Location = Point(770, 45);
            EstilizarCombo(cbOrigenRecalculo, 240);

            Label^ lblDestinoRec = gcnew Label();
            lblDestinoRec->Text = L"Destino para recalculo:";
            lblDestinoRec->Location = Point(770, 90);
            EstilizarLabel(lblDestinoRec);

            cbDestinoRecalculo = gcnew ComboBox();
            cbDestinoRecalculo->Location = Point(770, 120);
            EstilizarCombo(cbDestinoRecalculo, 240);

            panelBloq->Controls->Add(lblNodo);
            panelBloq->Controls->Add(cbNodoBloqueo);
            panelBloq->Controls->Add(btnBloqNodo);
            panelBloq->Controls->Add(btnDesbloqNodo);
            panelBloq->Controls->Add(lblArista);
            panelBloq->Controls->Add(cbAristaOrigenBloq);
            panelBloq->Controls->Add(cbAristaDestinoBloq);
            panelBloq->Controls->Add(btnBloqArista);
            panelBloq->Controls->Add(btnDesbloqArista);
            panelBloq->Controls->Add(lblOrigenRec);
            panelBloq->Controls->Add(cbOrigenRecalculo);
            panelBloq->Controls->Add(lblDestinoRec);
            panelBloq->Controls->Add(cbDestinoRecalculo);

            lbResultadosBloqueos = gcnew ListBox();
            lbResultadosBloqueos->Dock = DockStyle::Fill;
            EstilizarListBox(lbResultadosBloqueos);

            pbBloqueos = CrearMapaVisual();
            tpBloqueos->Controls->Add(CrearVistaConMapa(lbResultadosBloqueos, pbBloqueos));
            tpBloqueos->Controls->Add(panelBloq);

            tabControl->Controls->Add(tpMapa);
            tabControl->Controls->Add(tpDFS);
            tabControl->Controls->Add(tpBFS);
            tabControl->Controls->Add(tpConexo);
            tabControl->Controls->Add(tpCamino);
            tabControl->Controls->Add(tpSimulacion);
            tabControl->Controls->Add(tpAnalisis);
            tabControl->Controls->Add(tpBloqueos);

            this->Controls->Add(tabControl);
        }

        void CargarCombos() {
            auto nodos = mapa->getNodos();

            for (size_t i = 0; i < nodos.size(); i++) {
                String^ nombre = gcnew String(nodos[i].nombre.c_str());

                cbInicioDFS->Items->Add(nombre);
                cbInicioBFS->Items->Add(nombre);
                cbOrigenCamino->Items->Add(nombre);
                cbDestinoCamino->Items->Add(nombre);
                cbNodoBloqueo->Items->Add(nombre);
                cbAristaOrigenBloq->Items->Add(nombre);
                cbAristaDestinoBloq->Items->Add(nombre);
                cbAnalisisOrigen->Items->Add(nombre);
                cbAnalisisDestino->Items->Add(nombre);
                cbOrigenRecalculo->Items->Add(nombre);
                cbDestinoRecalculo->Items->Add(nombre);
            }

            if (cbInicioDFS->Items->Count > 0) cbInicioDFS->SelectedIndex = 0;
            if (cbInicioBFS->Items->Count > 0) cbInicioBFS->SelectedIndex = 0;
            if (cbOrigenCamino->Items->Count > 0) cbOrigenCamino->SelectedIndex = 0;
            if (cbDestinoCamino->Items->Count > 1) cbDestinoCamino->SelectedIndex = 1;
            if (cbNodoBloqueo->Items->Count > 0) cbNodoBloqueo->SelectedIndex = 0;
            if (cbAristaOrigenBloq->Items->Count > 0) cbAristaOrigenBloq->SelectedIndex = 0;
            if (cbAristaDestinoBloq->Items->Count > 1) cbAristaDestinoBloq->SelectedIndex = 1;
            if (cbAnalisisOrigen->Items->Count > 0) cbAnalisisOrigen->SelectedIndex = 0;
            if (cbAnalisisDestino->Items->Count > 1) cbAnalisisDestino->SelectedIndex = 1;
            if (cbOrigenRecalculo->Items->Count > 0) cbOrigenRecalculo->SelectedIndex = 0;
            if (cbDestinoRecalculo->Items->Count > 1) cbDestinoRecalculo->SelectedIndex = 1;

            SeleccionarPorNombre(cbInicioDFS, L"Entrada Principal / Piso 4");
            SeleccionarPorNombre(cbInicioBFS, L"Entrada Principal / Piso 4");
            SeleccionarPorNombre(cbOrigenCamino, L"Entrada Principal / Piso 4");
            SeleccionarPorNombre(cbDestinoCamino, L"Piso 1");
            SeleccionarPorNombre(cbAnalisisOrigen, L"Entrada Principal / Piso 4");
            SeleccionarPorNombre(cbAnalisisDestino, L"Comedor/Soda");
            SeleccionarPorNombre(cbOrigenRecalculo, L"Entrada Principal / Piso 4");
            SeleccionarPorNombre(cbDestinoRecalculo, L"Comedor/Soda");
            SeleccionarPorNombre(cbNodoBloqueo, L"Puente Comedor");
            SeleccionarPorNombre(cbAristaOrigenBloq, L"Entrada Principal / Piso 4");
            SeleccionarPorNombre(cbAristaDestinoBloq, L"Puente Comedor");
        }

        void pbMapa_Paint(Object^ sender, PaintEventArgs^ e) {
            Graphics^ g = e->Graphics;
            g->SmoothingMode = SmoothingMode::AntiAlias;

            PictureBox^ mapaActual = safe_cast<PictureBox^>(sender);

            LinearGradientBrush^ fondo = gcnew LinearGradientBrush(
                System::Drawing::Rectangle(0, 0, mapaActual->Width, mapaActual->Height),
                Color::FromArgb(237, 247, 241),
                Color::FromArgb(220, 238, 229),
                LinearGradientMode::Vertical
            );

            g->FillRectangle(fondo, 0, 0, mapaActual->Width, mapaActual->Height);

            auto nodos = mapa->getNodos();
            auto aristas = mapa->getAristas();

            float escalaX = mapaActual->Width / 900.0f;
            float escalaY = mapaActual->Height / 700.0f;

            float escala = Math::Min(escalaX, escalaY);

            float anchoMapa = 900.0f * escala;
            float altoMapa = 700.0f * escala;

            float offsetX = (mapaActual->Width - anchoMapa) / 2.0f;
            float offsetY = (mapaActual->Height - altoMapa) / 2.0f;

            Pen^ penNormal = gcnew Pen(Color::FromArgb(145, 158, 171), 2);
            Pen^ penBloqueada = gcnew Pen(Color::FromArgb(211, 47, 47), 4);
            Pen^ penResaltada = gcnew Pen(Color::FromArgb(46, 125, 90), 5);
            Pen^ penBFS = gcnew Pen(Color::FromArgb(33, 150, 243), 5);
            Pen^ penDFS = gcnew Pen(Color::FromArgb(126, 87, 194), 5);

            System::Drawing::Font^ font = gcnew System::Drawing::Font(L"Segoe UI", 8.5f, FontStyle::Bold);
            System::Drawing::Font^ fontTitulo = gcnew System::Drawing::Font(L"Segoe UI", 11, FontStyle::Bold);
            System::Drawing::Font^ fontPaso = gcnew System::Drawing::Font(L"Segoe UI", 8, FontStyle::Bold);

            g->FillRectangle(gcnew SolidBrush(Color::FromArgb(235, 255, 255, 255)), 12, 12, 420, 38);
            g->DrawRectangle(gcnew Pen(Color::FromArgb(170, 190, 205, 190)), 12, 12, 420, 38);
            g->DrawString(tituloVisual, fontTitulo, Brushes::DarkSlateGray, 22, 21);

            if (recorridoBFSVisual->size() > 0 || recorridoDFSVisual->size() > 0) {
                g->FillRectangle(gcnew SolidBrush(Color::FromArgb(235, 255, 255, 255)), 12, 55, 180, 50);
                g->FillEllipse(gcnew SolidBrush(Color::FromArgb(33, 150, 243)), 24, 67, 14, 14);
                g->DrawString(L"BFS", font, Brushes::Black, 45, 64);
                g->FillEllipse(gcnew SolidBrush(Color::FromArgb(126, 87, 194)), 24, 87, 14, 14);
                g->DrawString(L"DFS", font, Brushes::Black, 45, 84);
            }

            for (size_t i = 0; i < aristas.size(); i++) {
                Conexion c = aristas[i];

                Point p1 = ObtenerPunto(nodos[c.origen].x, nodos[c.origen].y, escala, offsetX, offsetY);
                Point p2 = ObtenerPunto(nodos[c.destino].x, nodos[c.destino].y, escala, offsetX, offsetY);

                Pen^ penActual = penNormal;

                if (c.bloqueada) {
                    penActual = penBloqueada;
                }
                else if (EsAristaDeVector(recorridoBFSVisual, c.origen, c.destino)) {
                    penActual = penBFS;
                }
                else if (EsAristaDeVector(recorridoDFSVisual, c.origen, c.destino)) {
                    penActual = penDFS;
                }
                else if (EsAristaResaltada(c.origen, c.destino)) {
                    penActual = penResaltada;
                }

                g->DrawLine(penActual, p1, p2);

                Point medio((p1.X + p2.X) / 2, (p1.Y + p2.Y) / 2);

                String^ textoDistancia = c.distanciaBase.ToString(L"0.0") + L" m";

                System::Drawing::Font^ fontDistancia = gcnew System::Drawing::Font(
                    L"Segoe UI",
                    7.0f,
                    FontStyle::Bold
                );

                Drawing::SizeF sizeDist = g->MeasureString(textoDistancia, fontDistancia);

                // Separar el texto de la línea
                float dx = (float)(p2.X - p1.X);
                float dy = (float)(p2.Y - p1.Y);
                float largo = (float)Math::Sqrt(dx * dx + dy * dy);

                float textoX = medio.X;
                float textoY = medio.Y;

                if (largo > 0) {
                    float normalX = -dy / largo;
                    float normalY = dx / largo;

                    textoX = medio.X + normalX * 12 - sizeDist.Width / 2;
                    textoY = medio.Y + normalY * 12 - sizeDist.Height / 2;
                }

                // Fondo pequeño más visible
                g->FillRectangle(
                    gcnew SolidBrush(Color::FromArgb(210, 255, 255, 255)),
                    (int)textoX - 3,
                    (int)textoY - 2,
                    (int)sizeDist.Width + 6,
                    (int)sizeDist.Height + 4
                );

                // Borde suave
                g->DrawRectangle(
                    gcnew Pen(Color::FromArgb(180, 180, 180, 180)),
                    (int)textoX - 3,
                    (int)textoY - 2,
                    (int)sizeDist.Width + 6,
                    (int)sizeDist.Height + 4
                );

                // Texto principal
                g->DrawString(
                    textoDistancia,
                    fontDistancia,
                    gcnew SolidBrush(Color::FromArgb(55, 40, 130)),
                    textoX,
                    textoY
                );
            }

            for (size_t i = 0; i < nodos.size(); i++) {
                Punto n = nodos[i];
                Point p = ObtenerPunto(n.x, n.y, escala, offsetX, offsetY);

                Color rellenoColor = ObtenerColorNodo(n);
                Brush^ brush = gcnew SolidBrush(rellenoColor);

                int radio = 24;

                if (ExisteEnVector(recorridoBFSVisual, (int)i)) {
                    radio = 31;
                    brush = gcnew SolidBrush(Color::FromArgb(33, 150, 243));
                }
                else if (ExisteEnVector(recorridoDFSVisual, (int)i)) {
                    radio = 31;
                    brush = gcnew SolidBrush(Color::FromArgb(126, 87, 194));
                }
                else if (EsNodoResaltado((int)i)) {
                    radio = 30;
                    brush = gcnew SolidBrush(Color::FromArgb(255, 235, 59));
                }

                if ((int)i == nodoInicioVisual) {
                    radio = 34;
                    brush = gcnew SolidBrush(Color::FromArgb(76, 175, 80));
                }
                else if ((int)i == nodoFinVisual) {
                    radio = 34;
                    brush = gcnew SolidBrush(Color::FromArgb(244, 67, 54));
                }

                g->FillEllipse(gcnew SolidBrush(Color::FromArgb(70, 0, 0, 0)), p.X - radio / 2 + 3, p.Y - radio / 2 + 3, radio, radio);
                g->FillEllipse(brush, p.X - radio / 2, p.Y - radio / 2, radio, radio);
                g->DrawEllipse(gcnew Pen(Color::White, 2), p.X - radio / 2, p.Y - radio / 2, radio, radio);
                g->DrawEllipse(gcnew Pen(Color::FromArgb(70, 70, 70), 1), p.X - radio / 2, p.Y - radio / 2, radio, radio);

                for (size_t paso = 0; paso < recorridoVisual->size(); paso++) {
                    if ((*recorridoVisual)[paso] == (int)i) {
                        String^ numeroPaso = String::Format(L"{0}", (int)paso + 1);
                        g->DrawString(numeroPaso, fontPaso, Brushes::Black, p.X - 6, p.Y - 8);
                        break;
                    }
                }

                for (size_t paso = 0; paso < recorridoBFSVisual->size(); paso++) {
                    if ((*recorridoBFSVisual)[paso] == (int)i) {
                        String^ numeroPaso = String::Format(L"B{0}", (int)paso + 1);
                        g->DrawString(numeroPaso, fontPaso, Brushes::White, p.X - 9, p.Y - 8);
                        break;
                    }
                }

                for (size_t paso = 0; paso < recorridoDFSVisual->size(); paso++) {
                    if ((*recorridoDFSVisual)[paso] == (int)i) {
                        String^ numeroPaso = String::Format(L"D{0}", (int)paso + 1);
                        g->DrawString(numeroPaso, fontPaso, Brushes::White, p.X - 9, p.Y - 8);
                        break;
                    }
                }

                String^ nombre = gcnew String(n.nombre.c_str());
                Drawing::SizeF size = g->MeasureString(nombre, font);

                float offsetX = p.X - (size.Width / 2);
                float offsetY = p.Y - 28;

                if (nombre->Contains("Ascensor")) offsetY = p.Y - 50;
                else if (nombre->Contains("Escalera Izquierda")) { offsetX = p.X - size.Width - 10; offsetY = p.Y - 10; }
                else if (nombre->Contains("Escalera Derecha")) { offsetX = p.X + 10; offsetY = p.Y - 10; }
                else if (nombre->Contains("Piso")) offsetY = p.Y + 15;
                else if (nombre->Contains("Auditorio")) { offsetX = p.X + 15; offsetY = p.Y - 25; }
                else if (nombre->Contains("Centro")) offsetX = p.X - size.Width - 15;
                else if (nombre->Contains("Comedor")) { offsetX = p.X - size.Width - 10; offsetY = p.Y + 5; }
                else if (nombre->Contains("Parqueo")) offsetX = p.X + 10;
                else if (nombre->Contains("Lactancia")) offsetY = p.Y + 15;

                g->DrawString(nombre, font, Brushes::Black, offsetX, offsetY);
            }
        }

        void btnDFS_Click(Object^ sender, EventArgs^ e) {
            int id = ObtenerIdSeleccionado(cbInicioDFS);
            auto recorrido = mapa->dfsConDistancias(id);

            lbResultadosDFS->Items->Clear();
            lbResultadosDFS->Items->Add(L"Recorrido DFS desde " + cbInicioDFS->Text + L":");

            recorridoVisual->clear();
            for (size_t i = 0; i < recorrido.size(); i++) {
                int nodoId = recorrido[i].first;
                double dist = recorrido[i].second;
                recorridoVisual->push_back(nodoId);

                String^ nombre = gcnew String(mapa->nombreNodo(nodoId).c_str());
                lbResultadosDFS->Items->Add(String::Format(L"{0} (distancia acumulada: {1:F2} m)", nombre, dist));
            }

            std::vector<int> recorridoParaAnimar = *recorridoVisual;

            recorridoVisual->clear();
            aristasVisuales->clear();
            recorridoBFSVisual->clear();
            recorridoDFSVisual->clear();

            nodoInicioVisual = id;
            nodoFinVisual = -1;

            IniciarAnimacion(recorridoParaAnimar, lbResultadosDFS, L"Animando recorrido DFS...", L"Visualizacion del recorrido DFS");
        }

        void btnBFS_Click(Object^ sender, EventArgs^ e) {
            int id = ObtenerIdSeleccionado(cbInicioBFS);
            auto recorrido = mapa->bfsConDistancias(id);

            lbResultadosBFS->Items->Clear();
            lbResultadosBFS->Items->Add(L"Recorrido BFS desde " + cbInicioBFS->Text + L":");

            recorridoVisual->clear();
            for (size_t i = 0; i < recorrido.size(); i++) {
                int nodoId = recorrido[i].first;
                double dist = recorrido[i].second;
                recorridoVisual->push_back(nodoId);

                String^ nombre = gcnew String(mapa->nombreNodo(nodoId).c_str());
                lbResultadosBFS->Items->Add(String::Format(L"{0} (distancia acumulada: {1:F2} m)", nombre, dist));
            }

            std::vector<int> recorridoParaAnimar = *recorridoVisual;

            recorridoVisual->clear();
            aristasVisuales->clear();
            recorridoBFSVisual->clear();
            recorridoDFSVisual->clear();

            nodoInicioVisual = id;
            nodoFinVisual = -1;

            IniciarAnimacion(recorridoParaAnimar, lbResultadosBFS, L"Animando recorrido BFS...", L"Visualizacion del recorrido BFS");
        }

        void btnConexo_Click(Object^ sender, EventArgs^ e) {
            bool conexo = mapa->esConexo();

            lbResultadosConexo->Items->Clear();
            if (conexo)
                lbResultadosConexo->Items->Add(L"El campus ES completamente conexo (todos los puntos estan conectados).");
            else
                lbResultadosConexo->Items->Add(L"El campus NO es conexo (hay puntos inalcanzables).");

            LimpiarVisualizacion();
            tituloVisual = conexo ? L"Grafo conexo" : L"Grafo no conexo";
            InvalidarMapas();
        }

        void btnCamino_Click(Object^ sender, EventArgs^ e) {
            int origen = ObtenerIdSeleccionado(cbOrigenCamino);
            int destino = ObtenerIdSeleccionado(cbDestinoCamino);

            auto camino = mapa->encontrarCaminoDFS(origen, destino);

            lbResultadosCamino->Items->Clear();

            recorridoVisual->clear();
            aristasVisuales->clear();

            if (camino.empty()) {
                lbResultadosCamino->Items->Add(L"No existe camino (DFS).");
                tituloVisual = L"No existe camino DFS";
            }
            else {
                lbResultadosCamino->Items->Add(L"Camino encontrado con DFS:");

                for (int i : camino) {
                    recorridoVisual->push_back(i);
                    lbResultadosCamino->Items->Add(gcnew String(mapa->nombreNodo(i).c_str()));
                }

                ConstruirAristasDesdeRecorrido(*recorridoVisual);
                tituloVisual = L"Visualizacion del camino DFS";
            }

            nodoInicioVisual = origen;
            nodoFinVisual = destino;
            InvalidarMapas();
        }

        void cbMovilidad_Changed(Object^ sender, EventArgs^ e) {
            int tipo = cbEscenarioMovilidad->SelectedIndex;
            mapa->setTipoMovilidad(tipo);

            lbResultadosSimulacion->Items->Clear();
            lbResultadosSimulacion->Items->Add(L"Modo de movilidad cambiado a: " + cbEscenarioMovilidad->Text);

            if (tipo == 0) {
                lbResultadosSimulacion->Items->Add(L"Se permiten todas las rutas disponibles.");
            }
            else if (tipo == 1) {
                lbResultadosSimulacion->Items->Add(L"Solo se permiten rutas accesibles sin escaleras.");
            }
            else if (tipo == 2) {
                lbResultadosSimulacion->Items->Add(L"Se penalizan las escaleras.");
            }
            else if (tipo == 3) {
                lbResultadosSimulacion->Items->Add(L"Las rutas con pendiente tienen mayor costo.");
            }

            tituloVisual = L"Visualizacion del escenario de movilidad";
            InvalidarMapas();
        }

        void btnSimular_Click(Object^ sender, EventArgs^ e) {
            int inicio = mapa->buscarNodoPorNombre("Entrada Principal / Piso 4");
            int destino = mapa->buscarNodoPorNombre("Piso 1");

            int crd = mapa->buscarNodoPorNombre("Centro de Referencia Digital");
            int soda = mapa->buscarNodoPorNombre("Comedor/Soda");

            bool nuevo = (cbTipoEstudiante->SelectedIndex == 1);

            lbResultadosSimulacion->Items->Clear();
            recorridoVisual->clear();
            aristasVisuales->clear();

            if (nuevo) {
                lbResultadosSimulacion->Items->Add(L"Ruta estudiante NUEVO:");
                lbResultadosSimulacion->Items->Add(L"Pasa por puntos de referencia (CRD y Soda)");

                auto r1 = mapa->dijkstra(inicio, crd);
                auto r2 = mapa->dijkstra(crd, soda);
                auto r3 = mapa->dijkstra(soda, destino);

                for (int i : r1.first) {
                    if (recorridoVisual->empty() || recorridoVisual->back() != i) recorridoVisual->push_back(i);
                    lbResultadosSimulacion->Items->Add(gcnew String(mapa->nombreNodo(i).c_str()));
                }

                for (int i : r2.first) {
                    if (recorridoVisual->empty() || recorridoVisual->back() != i) recorridoVisual->push_back(i);
                    lbResultadosSimulacion->Items->Add(gcnew String(mapa->nombreNodo(i).c_str()));
                }

                for (int i : r3.first) {
                    if (recorridoVisual->empty() || recorridoVisual->back() != i) recorridoVisual->push_back(i);
                    lbResultadosSimulacion->Items->Add(gcnew String(mapa->nombreNodo(i).c_str()));
                }

                tituloVisual = L"Visualizacion de simulacion: estudiante nuevo";
            }
            else {
                lbResultadosSimulacion->Items->Add(L"Ruta estudiante REGULAR:");
                lbResultadosSimulacion->Items->Add(L"Busca la ruta mas corta");

                auto res = mapa->dijkstra(inicio, destino);

                for (int i : res.first) {
                    recorridoVisual->push_back(i);
                    lbResultadosSimulacion->Items->Add(gcnew String(mapa->nombreNodo(i).c_str()));
                }

                tituloVisual = L"Visualizacion de simulacion: estudiante regular";
            }

            ConstruirAristasDesdeRecorrido(*recorridoVisual);
            nodoInicioVisual = inicio;
            nodoFinVisual = destino;
            InvalidarMapas();
        }

        void btnComparar_Click(Object^ sender, EventArgs^ e) {
            int inicio = ObtenerIdSeleccionado(cbAnalisisOrigen);
            int destino = ObtenerIdSeleccionado(cbAnalisisDestino);

            if (inicio < 0 || destino < 0) {
                lbResultadosAnalisis->Items->Clear();
                lbResultadosAnalisis->Items->Add(L"Seleccione nodo de inicio y destino validos.");
                return;
            }

            auto startBFS = std::chrono::steady_clock::now();
            auto recBFS = mapa->bfsConDistancias(inicio);
            auto endBFS = std::chrono::steady_clock::now();
            double tiempoBFS = std::chrono::duration<double, std::milli>(endBFS - startBFS).count();

            int nodosAntesBFS = 0;
            for (size_t i = 0; i < recBFS.size(); i++) {
                if (recBFS[i].first == destino) break;
                nodosAntesBFS++;
            }

            auto startDFS = std::chrono::steady_clock::now();
            auto recDFS = mapa->dfsConDistancias(inicio);
            auto endDFS = std::chrono::steady_clock::now();
            double tiempoDFS = std::chrono::duration<double, std::milli>(endDFS - startDFS).count();

            int nodosAntesDFS = 0;
            for (size_t i = 0; i < recDFS.size(); i++) {
                if (recDFS[i].first == destino) break;
                nodosAntesDFS++;
            }

            lbResultadosAnalisis->Items->Clear();
            lbResultadosAnalisis->Items->Add(L"=== ANALISIS DE COMPLEJIDAD ===");
            lbResultadosAnalisis->Items->Add(String::Format(L"Origen: {0}, Destino: {1}", cbAnalisisOrigen->Text, cbAnalisisDestino->Text));
            lbResultadosAnalisis->Items->Add(String::Format(L"BFS: tiempo {0:F4} ms, nodos antes de destino: {1}", tiempoBFS, nodosAntesBFS));
            lbResultadosAnalisis->Items->Add(String::Format(L"DFS: tiempo {0:F4} ms, nodos antes de destino: {1}", tiempoDFS, nodosAntesDFS));

            if (nodosAntesBFS < nodosAntesDFS)
                lbResultadosAnalisis->Items->Add(L"BFS visito menos nodos antes de encontrar el destino.");
            else if (nodosAntesDFS < nodosAntesBFS)
                lbResultadosAnalisis->Items->Add(L"DFS visito menos nodos antes de encontrar el destino.");
            else
                lbResultadosAnalisis->Items->Add(L"Ambos visitaron la misma cantidad de nodos.");

            std::vector<int> bfsAnimar;
            std::vector<int> dfsAnimar;

            for (size_t i = 0; i < recBFS.size(); i++) {
                bfsAnimar.push_back(recBFS[i].first);
                if (recBFS[i].first == destino) break;
            }

            for (size_t i = 0; i < recDFS.size(); i++) {
                dfsAnimar.push_back(recDFS[i].first);
                if (recDFS[i].first == destino) break;
            }

            nodoInicioVisual = inicio;
            nodoFinVisual = destino;

            IniciarComparacionAnimada(bfsAnimar, dfsAnimar);
        }

        void MostrarEstadoRutaAlternativa() {
            int origen = ObtenerIdSeleccionado(cbOrigenRecalculo);
            int destino = ObtenerIdSeleccionado(cbDestinoRecalculo);

            if (origen < 0 || destino < 0) {
                lbResultadosBloqueos->Items->Add(L"Seleccione origen y destino validos para el recalculo.");
                return;
            }

            lbResultadosBloqueos->Items->Add(L"--- RECALCULO EN TIEMPO REAL ---");
            lbResultadosBloqueos->Items->Add(String::Format(
                L"Origen: {0}, Destino: {1}",
                cbOrigenRecalculo->Text,
                cbDestinoRecalculo->Text
            ));

            //  IMPORTANTE: resetear movilidad antes del cálculo
            mapa->setTipoMovilidad(0);

            // Calcular ruta alternativa
            auto res = mapa->caminoMasCorto(origen, destino);

            recorridoVisual->clear();
            aristasVisuales->clear();
            recorridoBFSVisual->clear();
            recorridoDFSVisual->clear();

            if (res.first.empty()) {
                lbResultadosBloqueos->Items->Add(L"No hay ruta disponible con los bloqueos actuales.");
                tituloVisual = L"Sin ruta disponible por bloqueos";
            }
            else {
                lbResultadosBloqueos->Items->Add(L"Ruta alternativa encontrada:");

                for (size_t i = 0; i < res.first.size(); i++) {
                    int idNodo = res.first[i];
                    recorridoVisual->push_back(idNodo);

                    String^ nombre = gcnew String(mapa->nombreNodo(idNodo).c_str());
                    lbResultadosBloqueos->Items->Add(
                        String::Format(L"{0}. {1}", (int)i + 1, nombre)
                    );
                }

                lbResultadosBloqueos->Items->Add(
                    L"Distancia total: " + res.second.ToString(L"F2") + L" m"
                );

                ConstruirAristasDesdeRecorrido(*recorridoVisual);
                tituloVisual = L"Ruta recalculada por bloqueos";
            }

            nodoInicioVisual = origen;
            nodoFinVisual = destino;

            InvalidarMapas();
            lbResultadosBloqueos->Items->Add(L"");
        }

        void btnBloquearNodo_Click(Object^ sender, EventArgs^ e) {
            int id = ObtenerIdSeleccionado(cbNodoBloqueo);
            mapa->bloquearNodo(id, true);

            lbResultadosBloqueos->Items->Clear();
            lbResultadosBloqueos->Items->Add(L"Nodo " + cbNodoBloqueo->Text + L" bloqueado.");

            MostrarEstadoRutaAlternativa();
        }

        void btnDesbloquearNodo_Click(Object^ sender, EventArgs^ e) {
            int id = ObtenerIdSeleccionado(cbNodoBloqueo);
            mapa->bloquearNodo(id, false);

            lbResultadosBloqueos->Items->Clear();
            lbResultadosBloqueos->Items->Add(L"Nodo " + cbNodoBloqueo->Text + L" desbloqueado.");
            tituloVisual = L"Nodo desbloqueado visualmente";
            InvalidarMapas();
            MostrarEstadoRutaAlternativa();
        }

        void btnBloquearArista_Click(Object^ sender, EventArgs^ e) {
            int orig = ObtenerIdSeleccionado(cbAristaOrigenBloq);
            int dest = ObtenerIdSeleccionado(cbAristaDestinoBloq);

            mapa->bloquearArista(orig, dest, true);

            lbResultadosBloqueos->Items->Clear();
            lbResultadosBloqueos->Items->Add(
                L"Arista entre " + cbAristaOrigenBloq->Text + L" y " + cbAristaDestinoBloq->Text + L" bloqueada."
            );

            MostrarEstadoRutaAlternativa();
        }

        void btnDesbloquearArista_Click(Object^ sender, EventArgs^ e) {
            int orig = ObtenerIdSeleccionado(cbAristaOrigenBloq);
            int dest = ObtenerIdSeleccionado(cbAristaDestinoBloq);

            mapa->bloquearArista(orig, dest, false);

            lbResultadosBloqueos->Items->Clear();
            lbResultadosBloqueos->Items->Add(L"Arista entre " + cbAristaOrigenBloq->Text + L" y " + cbAristaDestinoBloq->Text + L" desbloqueada.");
            tituloVisual = L"Arista desbloqueada visualmente";
            InvalidarMapas();
            MostrarEstadoRutaAlternativa();
        }
    };
}
#pragma once
#include <Windows.h>
#include <MMSystem.h>
#include <fstream>
#include <sstream>
#include "CImagen.hpp"
#include "CEntidad.hpp"
#include "CRey.hpp"
#include "CArregloAliados.hpp"
#include "CArregloCorruptos.hpp"
#include "CArregloAsesinos.hpp"
#include "Laberinto.hpp"
#include "Vida.hpp"
#include "CBenji.hpp"
#include "CPortal.hpp"
#include "CAnimacion.hpp"

template <class B>
ref class CJuego
{
	//Hay un problema con los templates
	Laberinto^ laberinto;
	CPortal^ portales;
	CAnimacion^ blue;
	CImagen^ asesino_anim;
	CRey^ rey;
	CArregloAliados<B>^ aliados;
	CArregloCorruptos<B>^ corruptos;
	CArregloAsesinos<B>^ asesinos;
	CBenji^ benji;
	Vida^ vidas;
	//Para mover el aliado es lo de abajo pero se le puede poner en el arreglo CArregloAliados
	short new_dx;
	short new_dy;
	short n;
	short timer;
	short timer_ganar;
	short timer_limite;
	short timer_negociacion;
	bool negociado;
public:
	CJuego(System::Drawing::Rectangle area_dibujo, short n, short velocidadcorrupt, short nvidas, short timer_limite) :
		timer(0), timer_limite(timer_limite*10), timer_ganar(0), timer_negociacion(0), n(n), negociado(false)
	{
		this->laberinto = gcnew Laberinto();
		this->portales = gcnew CPortal("img\\portal.png");
		this->rey = gcnew CRey("img\\rey.png", System::Drawing::Rectangle(CAS_ANCHO, CAS_ANCHO, CAS_ANCHO-5, CAS_ANCHO), 4, 4, nvidas);
		this->aliados = gcnew CArregloAliados("img\\aliado.png", n, area_dibujo.Width, area_dibujo.Height, laberinto);
		this->corruptos = gcnew CArregloCorruptos("img\\corrupt.png", n * 0.4, area_dibujo.Width, area_dibujo.Height, laberinto, this->rey->getX(), this->rey->getY(), velocidadcorrupt);
		this->benji = gcnew CBenji("img\\benji.png", System::Drawing::Rectangle(CAS_ANCHO * (ANCHO-1), CAS_ANCHO, CAS_ANCHO, CAS_ANCHO), 4, 1);
		this->vidas = gcnew Vida("img\\vida.png", nvidas);
		this->asesinos = gcnew CArregloAsesinos("img\\assassin.png", n * 0.6, area_dibujo.Width, area_dibujo.Height, laberinto, this->rey->getX(), this->rey->getY(), velocidadcorrupt*2);
	}

	~CJuego() {
		delete rey, aliados, corruptos, asesinos, vidas, benji;
		if (blue != nullptr) delete blue;
		if (asesino_anim != nullptr) delete asesino_anim;
	}

	bool jugar(Graphics^ graficador, short w, short h, bool* ganaste) {
		renderizar(graficador, w, h);
		if (!benji->get_salvado() && rey->hay_colision(benji))
			benji->set_salvado(true);
		if (benji->get_salvado())
			timer_ganar++;
		if (rey->muerto()) {
			*ganaste = false;
			return true;
		}
		if (timer_ganar >= 600) {
			*ganaste = true;
			return true;
		}
		return false;
	}

	void mover_rey(short new_dx, short new_dy, bool cam) { 
		this->rey->set_direccion(new_dx, new_dy);
		this->rey->set_caminando(cam);
	}
	void guardado() {
		fstream gu("richiw.txt", ios::out);   //guardar archivo
		gu << "\n\t\t                                    _            ";
		gu << "\n\t\t                                   | |           ";
		gu << "\n\t\t  _ __ ___   __ _ _ __ ___ __ _  __| | ___  _ __ ";
		gu << "\n\t\t| '_ ` _ \\ / _` | '__/ __/ _` |/ _` |/ _ \\| '__|";
		gu << "\n\t\t | | | | | | (_| | | | (__ (_| | (_| | (_) | |   ";
		gu << "\n\t\t |_| |_| |_|\\__,_|_|  \\___\\__,_|\\__,_|\\___/|_|";
		gu << "\n\nNumero total de aliados: "<<n;
		gu << "\nAsesinos: " << n * 0.6;//Asesinos en el mapa
		gu << "\n"; //Laberinto
		gu << "\nTiempo: "<<timer; //Tiempo
		gu << "\nRey: X = " << rey->get_dx() << " | Y = " << rey->get_dy(); //Ubicacion rey  xddd
		gu << "\nCantidad de vidas restantes: " rey.get_vida(); //Vidas
		gu << "\n"; //Aliados en el mapa(cantidad y ubicacion)
		gu << "\n"; //Aliados con el prota
		gu << "\nCorruptos: "<< n * 0.4; //Corruptos en el mapa
		gu << "\nTenemos a Benji: " << benji->get_salvado(); //Tenemos a Benji??
		gu << "\n"; //
		gu << "\n"; //
		gu.close();
	}
	void recuperado() {
		fstream le("richiw.txt", ios::in);
		string sas, space;
		while (getline(le,sas)) {
			stringstream am(sas);
			getline(am, space, ' ');
			//
			getline(am, space, ' ');
			//
			getline(am, space, ' ');
			//
			getline(am, space, ' ');
			//
			getline(am, space, ' ');
			//
		}
		le.close();
	}

private:
	void renderizar(Graphics^ graficador, short w, short h) {
		++timer;
		if (timer == timer_limite) {
			negociado = true;
			this->blue = gcnew CAnimacion("img\\blue.png", System::Drawing::Rectangle(CAS_ANCHO * ANCHO / 2, 0, CAS_ANCHO * ANCHO / 2, CAS_ANCHO * ALTO), 4, 3, 2);
			this->asesino_anim = gcnew CImagen("img\\asesinogrande.png", System::Drawing::Rectangle(200, 200, CAS_ANCHO * ANCHO / 3, CAS_ANCHO * ALTO));
		}
		if (timer < timer_limite || timer > timer_limite + 15) {
			this->laberinto->renderizar(graficador, w, h);
			this->portales->renderizar(graficador, laberinto, asesinos, corruptos, rey, w, h, aliados);
			this->aliados->renderizar(graficador, w, h, rey, laberinto);
			this->corruptos->renderizar(graficador, w, h, laberinto, this->rey, this->vidas, aliados);
			this->rey->renderizar(graficador, w, h, laberinto);
			if (negociado) this->asesinos->renderizar(graficador, w, h, laberinto, this->rey, this->vidas);
			this->benji->renderizar_benji(graficador, this->rey);
			this->vidas->renderizar(graficador);
		}
		else {
			SolidBrush^ pincel = gcnew SolidBrush(Color::Black);
			System::Drawing::Rectangle area = System::Drawing::Rectangle(0, 0, w, h);
			graficador->FillRectangle(pincel, area);
			blue->dibujar(graficador);
			asesino_anim->dibujar(graficador);
		}
	}
};


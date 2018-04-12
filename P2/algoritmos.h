#ifndef _ALGORITMOS_H
#define _ALGORITMOS_H

#include <iostream>
#include <vector>
#include <cstdio>
#include <sstream>
#include <fstream>
#include <math.h>
#include <cmath>
#include <stdlib.h>
#include <algorithm>
#include <random>
#include <cstdlib>
#include <functional>
#include <string>
#include <numeric>
#include <cmath>
#include <chrono>
#include <omp.h>
#include <map>
#include <set>
#include <deque>


using namespace std;


class Algoritmo
{
	public:
		void csvReader(string fichero);
		void mostrarDatosYEtiquetas();
		int tamanioDatos();
		vector<float> KNNClasif(vector<int> & train, vector<int> & test, vector<float> & w);
		void ES(vector<int> & train,vector<float> & w);
		void ILS(vector<int> & train,vector<float> & w);
		vector<float> RELIEF(vector<int> & train, vector<int> & test);	
		pair<vector<float>,float> localSearch(vector<int> & indicesMatrizDatos, int cantidadIndicada, vector<float> & wini);
		vector<float> geneticoGeneracional(vector<int> indicesMatrizDatos);
		vector<float> AGGBLX(vector<int> indicesMatrizDatos);
		float calculaDistancia(int & fila1, int & fila2, vector<float> & w);
		void imprime(vector<int> v);
		void imprime(vector<float> v);
		int tamanioDatosCols();
		void evolucionDiferencialRand(vector<int> & train,vector<vector<float>> & poblacion, vector<float> & mejorIndividuo);
		void evolucionDiferencialCurrentToBest(vector<int> & train,vector<vector<float>> & poblacion, vector<float> & mejorIndividuo);
		vector<float> AGGCA(vector<int> indicesMatrizDatos);
		vector<float> AGEBLX(vector<int> indicesMatrizDatos);
		vector<float> AGECA(vector<int> indicesMatrizDatos);
		vector<float> AMCA1(vector<int> indicesMatrizDatos);
		vector<float> AMCA2(vector<int> indicesMatrizDatos);
		vector<float> AMCA3(vector<int> indicesMatrizDatos);
		

	private:
		vector<vector<float>> matrizDatos;
		vector<string> etiquetas;
		
		float tasaClasificacion(vector<string> & nuevasEtiquetas, vector<int> & indicesMatrizDatos);
		float tasaReduccion(vector<float> & w);
		vector<float> mutar(vector<float> & w, int t);
		void actualizarPesosRELIEF(vector<float> & w, vector<float> & filaActual, int amigo, int enemigo);
		vector<float> generarVecino(vector<float>  & w, int & posModificar);
		bool Encontrar(vector<int> & v, int x);
		float fRand(float fMin, float fMax);
		vector<int> torneoBinario(vector<float> & tasasPoblacion, int cantidadAleatorio, int tamanioPoblacion);
		void operadorCruceCA(vector<float> & P1, vector<float> & P2, vector<float> & hijo);
		pair<vector<float>,vector<float>> operadorCruceBLX(vector<float> & P1, vector<float> & P2);
};

#endif

#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <vector>
#include "algoritmos.h"
#include <time.h> 
#include <stdio.h> 
#include <chrono>
#include <omp.h>
#include <stdlib.h>

using namespace std::chrono;

vector<pair< vector<int>, vector<int> >> particiones;
pair< vector<int>, vector<int> > particion1;
pair< vector<int>, vector<int> > particion2;
pair< vector<int>, vector<int> > particion3;
pair< vector<int>, vector<int> > particion4;
pair< vector<int>, vector<int> > particion5;
vector<int> indices;
Algoritmo *lectorCSV;

void crearParticiones(vector<pair< vector<int>, vector<int> >> & particiones)
{

	int tamanioTest = indices.size()/5;
	int limInfTest = -tamanioTest;
	int limSupTest = limInfTest+tamanioTest;
	//recorremos las 5 particiones distintas
	for(int i = 0; i<particiones.size(); i++)
	{
		//declaramos los limites superior e inferior de los sucesivos conjuntos test
		limInfTest += tamanioTest;
		//en caso de que sobren datos por el hecho de que la cantidad total
		//no sea divisible entre cinco hacemos que el ultimo conjunto de test
		//sea hasta el final de los datos (para tenerlos todos en cuenta)
		if(i == 4 && indices.size()%5 != 0)
			limSupTest = indices.size();
		else
			limSupTest = limInfTest + tamanioTest; 


		//metemos los indices de los datos obtenidos
		for(int e = 0; e<indices.size(); e++)
		{
			//para crear la partición test correspondiente
			if(e >= limInfTest && e < limSupTest)
			{
				particiones[i].second.push_back(indices[e]);
			}
			//para crear la partición train correspondiente
			else 
			{
				particiones[i].first.push_back(indices[e]);
			}
		}
	}

	// el atributo first de cada una de las particiones seran los conjuntos de datos 
	// del train, en cambio el atributo second se usara para el test.
}

void inicializarVectorPesos(vector<float> & w)
{
	float num = 0;
	//inicializamos los pesos con valores aleatorios entre 0 y 1
	for(int i = 0; i < lectorCSV->tamanioDatosCols(); i++)
	{
		num=1+rand()%(1001-1);
		num = num/1000.0;
		w.push_back(num);
	}
}

void cargarDatos(string fichero)
{
	lectorCSV = new Algoritmo;
	//leemos el csv
	lectorCSV->csvReader(fichero);

	//creamos la matriz con 5 particiones
	particiones.push_back(particion1);
	particiones.push_back(particion2);
	particiones.push_back(particion3);
	particiones.push_back(particion4);
	particiones.push_back(particion5);

	//creamos el vector de indices para la generación de las particiones
	for(int i = 0; i < lectorCSV->tamanioDatos(); i++)
	{
		indices.push_back(i);
	}

	//desordenamos los indices
	random_shuffle(indices.begin(), indices.end());

	//creamos las 5 particiones tal y como se indica en el guión (cada partición tendrá una parte
	//para los datos del Train y otra para los del Test, todas las particiones son distintas entre
	//ellas).
	crearParticiones(particiones);

}

vector<float> ES(int indiceParticion)
{
	clock_t begin, end; 
	double time;
	vector<float> resultadoES;
	vector<float> w;
	
	//comenzamos probando el clasificador 1-NN con el conjunto de datos train de la partición 0
	//inicializamos el vector de pesos original a valores aleatorios
	inicializarVectorPesos(w);
	
	//Llamamos al algoritmo pasandole el conjunto train y medimos el tiempo que tarda en ejecutarse
	begin = clock(); 
	lectorCSV->ES(particiones[indiceParticion].first,w);
	end = clock();
	time = double(end - begin) / CLOCKS_PER_SEC;

	//sacamos los resultados que queremos con el clasifia¡cador KNN
	resultadoES = lectorCSV->KNNClasif(particiones[indiceParticion].first, particiones[indiceParticion].second,w);

	//SACAMOS LOS RESULTADOS POR PANTALLA
	// cout << "Particion: " << indiceParticion << endl;
	// cout << "Funcion objetivo:" << resultadoES[0] << "%" << endl;
	// cout << "Tasa de clasificacion: " << resultadoES[1] << "%" << endl;
	// cout << "Tasa de reduccion: " << resultadoES[2] << "%" << endl;
	// cout << "Tiempo de ejecucion del algoritmo: " << time << " segundos" << endl << endl;
	//para hacer más rapida la extración de datos y el paso a tablas hacemos que
	//los resultados se vean de esta forma por pantalla:
	cout << resultadoES[1] << "            " << resultadoES[2] << "             " << resultadoES[0] << "       " << time << endl;
	// cout << "----------------------------------------------------------------------------" << endl;

	//Devolvemos lo resultados para sacar las medias fuera de la funcion
	vector<float> resultado;
	resultado.push_back(resultadoES[0]);
	resultado.push_back(resultadoES[1]);
	resultado.push_back(resultadoES[2]);
	resultado.push_back(time);

	return resultado;
}


vector<float> KNN(int indiceParticion)
{
	clock_t begin, end; 
	double time;
	vector<float> resultado;
	vector<float> w;

	//comenzamos probando el clasificador 1-NN con el conjunto de datos train de la partición 0
	//inicializamos el vector de pesos original a 1
	for(int i= 0; i<lectorCSV->tamanioDatosCols(); i++)
	{
		w.push_back(1.0);
	}
	
	begin = clock(); 
	resultado = lectorCSV->KNNClasif(particiones[indiceParticion].first, particiones[indiceParticion].second,w);
	end = clock();

	time = double(end - begin) / CLOCKS_PER_SEC;

	//SACAMOS LOS RESULTADOS POR PANTALLA
	// cout << "Particion: " << indiceParticion << endl;
	// cout << "Funcion objetivo:" << resultado[0] << "%" << endl;
	// cout << "Tasa de clasificacion: " << resultado[1] << "%" << endl;
	// cout << "Tasa de reduccion: " << resultado[2] << "%" << endl;
	// cout << "Tiempo de ejecucion del algoritmo: " << time << " segundos" << endl << endl;

	//para hacer más rapida la extración de datos y el paso a tablas hacemos que
	//los resultados se vean de esta forma por pantalla:
	cout << resultado[1] << "            " << resultado[2] << "             " << resultado[0] << "       " << time << endl;


	// cout << "----------------------------------------------------------------------------" << endl;

	//Devolvemos lo resultados para sacar las medias fuera de la funcion
	vector<float> resultados;
	resultados.push_back(resultado[0]);
	resultados.push_back(resultado[1]);
	resultados.push_back(resultado[2]);
	resultados.push_back(time);

	return resultados;
}

vector<float> RELIEF(int indiceParticion)
{

	clock_t begin, end; 
	double time;
	vector<float> resultado, resul;
	vector<float> w;
	
	//comenzamos probando el clasificador 1-NN con el conjunto de datos train de la partición 0
	//inicializamos el vector de pesos original a valores aleatorios
	inicializarVectorPesos(w);
	
	//Llamamos al algoritmo pasandole el conjunto train y medimos el tiempo que tarda en ejecutarse
	begin = clock(); 
	w = lectorCSV->RELIEF(particiones[indiceParticion].first,particiones[indiceParticion].first);
	end = clock();
	time = double(end - begin) / CLOCKS_PER_SEC;

	//sacamos los resultados que queremos con el clasifia¡cador KNN
	resul = lectorCSV->KNNClasif(particiones[indiceParticion].first, particiones[indiceParticion].second,w);

	//SACAMOS LOS RESULTADOS POR PANTALLA
	// cout << "Particion: " << indiceParticion << endl;
	// cout << "Funcion objetivo:" << resul[0] << "%" << endl;
	// cout << "Tasa de clasificacion: " << resul[1] << "%" << endl;
	// cout << "Tasa de reduccion: " << resul[2] << "%" << endl;
	// cout << "Tiempo de ejecucion del algoritmo: " << time << " segundos" << endl << endl;
	//para hacer más rapida la extración de datos y el paso a tablas hacemos que
	//los resultados se vean de esta forma por pantalla:
	cout << resul[1] << "            " << resul[2] << "             " << resul[0] << "       " << time << endl;
	// cout << "----------------------------------------------------------------------------" << endl;

	//Devolvemos lo resultados para sacar las medias fuera de la funcion
	resultado.push_back(resul[0]);
	resultado.push_back(resul[1]);
	resultado.push_back(resul[2]);
	resultado.push_back(time);

	return resultado;


}

vector<float> ILS(int indiceParticion)
{

	clock_t begin, end; 
	double time;
	vector<float> resultado, resul;
	vector<float> w;
	
	//comenzamos probando el clasificador 1-NN con el conjunto de datos train de la partición 0
	//inicializamos el vector de pesos original a valores aleatorios
	inicializarVectorPesos(w);
	
	//Llamamos al algoritmo pasandole el conjunto train y medimos el tiempo que tarda en ejecutarse
	begin = clock(); 
	lectorCSV->ILS(particiones[indiceParticion].first,w);
	end = clock();
	time = double(end - begin) / CLOCKS_PER_SEC;

	//sacamos los resultados que queremos con el clasifia¡cador KNN
	resul = lectorCSV->KNNClasif(particiones[indiceParticion].first, particiones[indiceParticion].second,w);

	//SACAMOS LOS RESULTADOS POR PANTALLA
	// cout << "Particion: " << indiceParticion << endl;
	// cout << "Funcion objetivo:" << resul[0] << "%" << endl;
	// cout << "Tasa de clasificacion: " << resul[1] << "%" << endl;
	// cout << "Tasa de reduccion: " << resul[2] << "%" << endl;
	// cout << "Tiempo de ejecucion del algoritmo: " << time << " segundos" << endl << endl;
	//para hacer más rapida la extración de datos y el paso a tablas hacemos que
	//los resultados se vean de esta forma por pantalla:
	cout << resul[1] << "            " << resul[2] << "             " << resul[0] << "       " << time << endl;
	// cout << "----------------------------------------------------------------------------" << endl;

	//Devolvemos lo resultados para sacar las medias fuera de la funcion
	resultado.push_back(resul[0]);
	resultado.push_back(resul[1]);
	resultado.push_back(resul[2]);
	resultado.push_back(time);

	return resultado;


}

vector<float> DER(int indiceParticion)
{

	clock_t begin, end; 
	double time;
	vector<float> resultado, resul;
	vector<float> w;
	int tamaPoblacion = 50;
	vector<vector<float>> poblacion;
	vector<float> individuo;
	float gen = 0;
	
	//Creamos una poblacion de forma aleatoria
	for(int i=0; i < tamaPoblacion; i++)
	{
		for(int e = 0; e < lectorCSV->tamanioDatosCols(); e++)
		{
			gen = 1+rand()%(1001-1);
			gen = gen/1000.0;	
			individuo.push_back(gen);
		}
		poblacion.push_back(individuo);
		individuo.clear();
	}
	
	//Llamamos al algoritmo pasandole el conjunto train y medimos el tiempo que tarda en ejecutarse
	begin = clock(); 
	lectorCSV->evolucionDiferencialRand(particiones[indiceParticion].first, poblacion ,w);
	end = clock();
	time = double(end - begin) / CLOCKS_PER_SEC;

	//sacamos los resultados que queremos con el clasifia¡cador KNN
	resul = lectorCSV->KNNClasif(particiones[indiceParticion].first, particiones[indiceParticion].second,w);

	//SACAMOS LOS RESULTADOS POR PANTALLA
	// cout << "Particion: " << indiceParticion << endl;
	// cout << "Funcion objetivo:" << resul[0] << "%" << endl;
	// cout << "Tasa de clasificacion: " << resul[1] << "%" << endl;
	// cout << "Tasa de reduccion: " << resul[2] << "%" << endl;
	// cout << "Tiempo de ejecucion del algoritmo: " << time << " segundos" << endl << endl;
	//para hacer más rapida la extración de datos y el paso a tablas hacemos que
	//los resultados se vean de esta forma por pantalla:
	cout << resul[1] << "            " << resul[2] << "             " << resul[0] << "       " << time << endl;
	// cout << "----------------------------------------------------------------------------" << endl;

	//Devolvemos lo resultados para sacar las medias fuera de la funcion
	resultado.push_back(resul[0]);
	resultado.push_back(resul[1]);
	resultado.push_back(resul[2]);
	resultado.push_back(time);

	return resultado;


}


vector<float> localSearch(int indiceParticion)
{
	clock_t begin, end;
	double tiempo;
	vector<float> pesos,wini;
	float num = 0;
	pair<vector<float>,float> solucion;
	vector<float> resultado, resul;
	vector<float> w;


	//inicializamos el vector de pesos con valores aleatorios en el intervalo [0,1]
	for(int i = 0; i < lectorCSV->tamanioDatosCols(); i++)
	{
		num=1+rand()%(1001-1);
		num = num/1000.0;
		wini.push_back(num);
	}
		
	//comenzamos probando el clasificador 1-NN con el conjunto de datos train de la partición 0
	//inicializamos el vector de pesos original a valores aleatorios
	inicializarVectorPesos(w);

	//medimos el tiempo de la local search
	begin = clock(); 
	solucion = lectorCSV->localSearch(particiones[indiceParticion].first,1000,wini);
	end = clock();
	tiempo = double(end - begin) / CLOCKS_PER_SEC;
	//calculamos su tasa de clasificación
	resul = lectorCSV->KNNClasif(particiones[indiceParticion].first, particiones[indiceParticion].second,solucion.first);

	cout << resul[1] << "            " << resul[2] << "             " << resul[0] << "       " << tiempo << endl;

	//Devolvemos lo resultados para sacar las medias fuera de la funcion
	resultado.push_back(resul[0]);
	resultado.push_back(resul[1]);
	resultado.push_back(resul[2]);
	resultado.push_back(tiempo);


	return resul;
}

vector<float> AGG_BLX(int indiceParticion)
{
	clock_t begin, end; 
	double time;
	vector<float> resultado, resul;
	vector<float> w;
	
	//comenzamos probando el clasificador 1-NN con el conjunto de datos train de la partición 0
	//inicializamos el vector de pesos original a valores aleatorios
	inicializarVectorPesos(w);
	
	//Llamamos al algoritmo pasandole el conjunto train y medimos el tiempo que tarda en ejecutarse
	begin = clock(); 
	w = lectorCSV->AGGBLX(particiones[indiceParticion].first);
	end = clock();
	time = double(end - begin) / CLOCKS_PER_SEC;

	//sacamos los resultados que queremos con el clasifia¡cador KNN
	resul = lectorCSV->KNNClasif(particiones[indiceParticion].first, particiones[indiceParticion].second,w);

	//SACAMOS LOS RESULTADOS POR PANTALLA
	// cout << "Particion: " << indiceParticion << endl;
	// cout << "Funcion objetivo:" << resul[0] << "%" << endl;
	// cout << "Tasa de clasificacion: " << resul[1] << "%" << endl;
	// cout << "Tasa de reduccion: " << resul[2] << "%" << endl;
	// cout << "Tiempo de ejecucion del algoritmo: " << time << " segundos" << endl << endl;
	//para hacer más rapida la extración de datos y el paso a tablas hacemos que
	//los resultados se vean de esta forma por pantalla:
	cout << resul[1] << "            " << resul[2] << "             " << resul[0] << "       " << time << endl;
	// cout << "----------------------------------------------------------------------------" << endl;

	//Devolvemos lo resultados para sacar las medias fuera de la funcion
	resultado.push_back(resul[0]);
	resultado.push_back(resul[1]);
	resultado.push_back(resul[2]);
	resultado.push_back(time);

	return resultado;
}

vector<float> AMCA3(int indiceParticion)
{
	clock_t begin, end; 
	double time;
	vector<float> resultado, resul;
	vector<float> w;
	
	//comenzamos probando el clasificador 1-NN con el conjunto de datos train de la partición 0
	//inicializamos el vector de pesos original a valores aleatorios
	inicializarVectorPesos(w);
	
	//Llamamos al algoritmo pasandole el conjunto train y medimos el tiempo que tarda en ejecutarse
	begin = clock(); 
	w = lectorCSV->AMCA3(particiones[indiceParticion].first);
	end = clock();
	time = double(end - begin) / CLOCKS_PER_SEC;

	//sacamos los resultados que queremos con el clasifia¡cador KNN
	resul = lectorCSV->KNNClasif(particiones[indiceParticion].first, particiones[indiceParticion].second,w);

	//SACAMOS LOS RESULTADOS POR PANTALLA
	// cout << "Particion: " << indiceParticion << endl;
	// cout << "Funcion objetivo:" << resul[0] << "%" << endl;
	// cout << "Tasa de clasificacion: " << resul[1] << "%" << endl;
	// cout << "Tasa de reduccion: " << resul[2] << "%" << endl;
	// cout << "Tiempo de ejecucion del algoritmo: " << time << " segundos" << endl << endl;
	//para hacer más rapida la extración de datos y el paso a tablas hacemos que
	//los resultados se vean de esta forma por pantalla:
	cout << resul[1] << "            " << resul[2] << "             " << resul[0] << "       " << time << endl;
	// cout << "----------------------------------------------------------------------------" << endl;

	//Devolvemos lo resultados para sacar las medias fuera de la funcion
	resultado.push_back(resul[0]);
	resultado.push_back(resul[1]);
	resultado.push_back(resul[2]);
	resultado.push_back(time);

	return resultado;

}



vector<float> DECTB(int indiceParticion)
{

	clock_t begin, end; 
	double time;
	vector<float> resultado, resul;
	vector<float> w;
	int tamaPoblacion = 50;
	vector<vector<float>> poblacion;
	vector<float> individuo;
	float gen = 0;
	
	//Creamos una poblacion de forma aleatoria
	for(int i=0; i < tamaPoblacion; i++)
	{
		for(int e = 0; e < lectorCSV->tamanioDatosCols(); e++)
		{
			gen = 1+rand()%(1001-1);
			gen = gen/1000.0;	
			individuo.push_back(gen);
		}
		poblacion.push_back(individuo);
		individuo.clear();
	}
	
	//Llamamos al algoritmo pasandole el conjunto train y medimos el tiempo que tarda en ejecutarse
	begin = clock(); 
	lectorCSV->evolucionDiferencialCurrentToBest(particiones[indiceParticion].first, poblacion, w);
	end = clock();
	time = double(end - begin) / CLOCKS_PER_SEC;

	//sacamos los resultados que queremos con el clasifia¡cador KNN
	resul = lectorCSV->KNNClasif(particiones[indiceParticion].first, particiones[indiceParticion].second,w);

	//SACAMOS LOS RESULTADOS POR PANTALLA
	// cout << "Particion: " << indiceParticion << endl;
	// cout << "Funcion objetivo:" << resul[0] << "%" << endl;
	// cout << "Tasa de clasificacion: " << resul[1] << "%" << endl;
	// cout << "Tasa de reduccion: " << resul[2] << "%" << endl;
	// cout << "Tiempo de ejecucion del algoritmo: " << time << " segundos" << endl << endl;
	//para hacer más rapida la extración de datos y el paso a tablas hacemos que
	//los resultados se vean de esta forma por pantalla:
	cout << resul[1] << "            " << resul[2] << "             " << resul[0] << "       " << time << endl;
	// cout << "----------------------------------------------------------------------------" << endl;

	//Devolvemos lo resultados para sacar las medias fuera de la funcion
	resultado.push_back(resul[0]);
	resultado.push_back(resul[1]);
	resultado.push_back(resul[2]);
	resultado.push_back(time);

	return resultado;


}

int main()
{

	float semilla;

	cout << endl << "Elige el valor de la semilla aleatoria (0 por defecto)" << endl;
	cin >> semilla;

	//ponemos la semilla para la generación estática de números aleatorios
	if(semilla == 0)
	{
		cout << endl << "Escogida semilla por defecto" << endl;
		srand(7);
	}
	else
	{
		srand(semilla);
	}


	//leemos los datos y creamos las particiones correspondientes
	cargarDatos("sonar.csv");

	//MENU
	vector<float> resultados;
	int algoritmo = -1;
	float tiempo = 0;
	float tasaClas = 0;
	float tasaRed = 0;
	float agregado = 0;
	cout << endl << "ELIGE UN ALGORITMO PARA LANZAR: " << endl;
	cout << "1.Algoritmo KNN" << endl <<"2.Algoritmo Enfriamiento Simulado" << endl << "3.Algoritmo ILS" << endl;
	cout << "4.Algoritmo Evolucion Diferencial Rand" << endl;
	cout << "5.Algoritmo Evolucion Diferencial Current Best" << endl;
	cout << "6.Algoritmo RELIEF" << endl;
	cout << "7.Algoritmo Local Search" << endl;
	cout << "8.Algoritmo AGG_BLX" << endl;
	cout << "9.Algoritmo AMCA3" << endl;
	cin >> algoritmo;

	cout << lectorCSV->tamanioDatos() << " " << lectorCSV->tamanioDatosCols() << endl;
 
	switch(algoritmo)
	{

		case 1:
			///////////////////////////////////////////////////////////////////////////////////
			// Pruebas con KNN
			cout << "_____________________________________________________________"<< endl;
			cout << "_____________________________________________________________"<< endl;
			cout << "Algoritmo: KNN" << endl;

			cout << "Tasa Class.      Tasa Red.      Agregado      Tiempo" << endl;

			resultados = KNN(0);
			agregado += resultados[0];
			tasaClas += resultados[1];
			tasaRed += resultados[2];
			tiempo += resultados[3];			

			resultados = KNN(1);
			agregado += resultados[0];
			tasaClas += resultados[1];
			tasaRed += resultados[2];
			tiempo += resultados[3];	
			
			resultados = KNN(2);
			agregado += resultados[0];
			tasaClas += resultados[1];
			tasaRed += resultados[2];
			tiempo += resultados[3];	
			
			resultados = KNN(3);
			agregado += resultados[0];
			tasaClas += resultados[1];
			tasaRed += resultados[2];
			tiempo += resultados[3];	
			
			resultados = KNN(4);
			agregado += resultados[0];
			tasaClas += resultados[1];
			tasaRed += resultados[2];
			tiempo += resultados[3];	
			
			// cout << "TIEMPO MEDIO -------> " << (tiempo)/5 << endl;
			// cout << "AGREGADO -------> " << (agregado)/5 << endl;
			// cout << "TASA DE CLASIFICACION MEDIA -------> " << (tasaClas)/5 << endl;
			// cout << "TASA DE REDUCCIÓN MEDIA -------> " << (tasaRed)/5 << endl;

			//para hacer más rapida la extración de datos y el paso a tablas hacemos que
			//los resultados se vean de esta forma por pantalla:
			cout << (tasaClas)/5 << "            " << (tasaRed)/5 << "             " << (agregado)/5 << "       " << (tiempo)/5 << endl;
			
			tiempo = 0;
			tasaClas = 0;
			tasaRed = 0;
			agregado = 0;
		break;
		case 2:
			///////////////////////////////////////////////////////////////////////////////////
			// Pruebas con ES
			cout << "_____________________________________________________________"<< endl;
			cout << "_____________________________________________________________"<< endl;
			cout << "Algoritmo: Enfriamiento Simulado" << endl;

			cout << "Tasa Class.      Tasa Red.      Agregado      Tiempo" << endl;
			resultados = ES(0);
			agregado += resultados[0];
			tasaClas += resultados[1];
			tasaRed += resultados[2];
			tiempo += resultados[3];			

			resultados = ES(1);
			agregado += resultados[0];
			tasaClas += resultados[1];
			tasaRed += resultados[2];
			tiempo += resultados[3];	
			
			resultados = ES(2);
			agregado += resultados[0];
			tasaClas += resultados[1];
			tasaRed += resultados[2];
			tiempo += resultados[3];	
			
			resultados = ES(3);
			agregado += resultados[0];
			tasaClas += resultados[1];
			tasaRed += resultados[2];
			tiempo += resultados[3];	
			
			resultados = ES(4);
			agregado += resultados[0];
			tasaClas += resultados[1];
			tasaRed += resultados[2];
			tiempo += resultados[3];	
			
			// cout << "TIEMPO MEDIO -------> " << (tiempo)/5 << endl;
			// cout << "AGREGADO -------> " << (agregado)/5 << endl;
			// cout << "TASA DE CLASIFICACION MEDIA -------> " << (tasaClas)/5 << endl;
			// cout << "TASA DE REDUCCIÓN MEDIA ES -------> " << (tasaRed)/5 << endl;

			//para hacer más rapida la extración de datos y el paso a tablas hacemos que
			//los resultados se vean de esta forma por pantalla:
			cout << (tasaClas)/5 << "            " << (tasaRed)/5 << "             " << (agregado)/5 << "       " << (tiempo)/5 << endl;
			
			tiempo = 0;
			tasaClas = 0;
			tasaRed = 0;
			agregado = 0;
		break;
		case 3:
			///////////////////////////////////////////////////////////////////////////////////
			// Pruebas con ES
			cout << "_____________________________________________________________"<< endl;
			cout << "_____________________________________________________________"<< endl;
			cout << "Algoritmo: Iterative Local Search" << endl;

			cout << "Tasa Class.      Tasa Red.      Agregado      Tiempo" << endl;
			resultados = ILS(0);
			agregado += resultados[0];
			tasaClas += resultados[1];
			tasaRed += resultados[2];
			tiempo += resultados[3];			

			resultados = ILS(1);
			agregado += resultados[0];
			tasaClas += resultados[1];
			tasaRed += resultados[2];
			tiempo += resultados[3];	
			
			resultados = ILS(2);
			agregado += resultados[0];
			tasaClas += resultados[1];
			tasaRed += resultados[2];
			tiempo += resultados[3];	
			
			resultados = ILS(3);
			agregado += resultados[0];
			tasaClas += resultados[1];
			tasaRed += resultados[2];
			tiempo += resultados[3];	
			
			resultados = ILS(4);
			agregado += resultados[0];
			tasaClas += resultados[1];
			tasaRed += resultados[2];
			tiempo += resultados[3];	
			
			// cout << "TIEMPO MEDIO -------> " << (tiempo)/5 << endl;
			// cout << "AGREGADO -------> " << (agregado)/5 << endl;
			// cout << "TASA DE CLASIFICACION MEDIA -------> " << (tasaClas)/5 << endl;
			// cout << "TASA DE REDUCCIÓN MEDIA ES -------> " << (tasaRed)/5 << endl;

			//para hacer más rapida la extración de datos y el paso a tablas hacemos que
			//los resultados se vean de esta forma por pantalla:
			cout << (tasaClas)/5 << "            " << (tasaRed)/5 << "             " << (agregado)/5 << "       " << (tiempo)/5 << endl;
			
			tiempo = 0;
			tasaClas = 0;
			tasaRed = 0;
			agregado = 0;
		break;
		case 4:
			///////////////////////////////////////////////////////////////////////////////////
			// Pruebas con ES
			cout << "_____________________________________________________________"<< endl;
			cout << "_____________________________________________________________"<< endl;
			cout << "Algoritmo: Evolucion Diferencial: Rand" << endl;

			cout << "Tasa Class.      Tasa Red.      Agregado      Tiempo" << endl;
			resultados = DER(0);
			agregado += resultados[0];
			tasaClas += resultados[1];
			tasaRed += resultados[2];
			tiempo += resultados[3];			

			resultados = DER(1);
			agregado += resultados[0];
			tasaClas += resultados[1];
			tasaRed += resultados[2];
			tiempo += resultados[3];	
			
			resultados = DER(2);
			agregado += resultados[0];
			tasaClas += resultados[1];
			tasaRed += resultados[2];
			tiempo += resultados[3];	
			
			resultados = DER(3);
			agregado += resultados[0];
			tasaClas += resultados[1];
			tasaRed += resultados[2];
			tiempo += resultados[3];	
			
			resultados = DER(4);
			agregado += resultados[0];
			tasaClas += resultados[1];
			tasaRed += resultados[2];
			tiempo += resultados[3];	
			
			// cout << "TIEMPO MEDIO -------> " << (tiempo)/5 << endl;
			// cout << "AGREGADO -------> " << (agregado)/5 << endl;
			// cout << "TASA DE CLASIFICACION MEDIA -------> " << (tasaClas)/5 << endl;
			// cout << "TASA DE REDUCCIÓN MEDIA ES -------> " << (tasaRed)/5 << endl;

			//para hacer más rapida la extración de datos y el paso a tablas hacemos que
			//los resultados se vean de esta forma por pantalla:
			cout << (tasaClas)/5 << "            " << (tasaRed)/5 << "             " << (agregado)/5 << "       " << (tiempo)/5 << endl;
			
			tiempo = 0;
			tasaClas = 0;
			tasaRed = 0;
			agregado = 0;
		break;
		case 5:
			///////////////////////////////////////////////////////////////////////////////////
			// Pruebas con ES
			cout << "_____________________________________________________________"<< endl;
			cout << "_____________________________________________________________"<< endl;
			cout << "Algoritmo: Evolucion Diferencial: Current To Best" << endl;

			cout << "Tasa Class.      Tasa Red.      Agregado      Tiempo" << endl;
			resultados = DECTB(0);
			agregado += resultados[0];
			tasaClas += resultados[1];
			tasaRed += resultados[2];
			tiempo += resultados[3];			

			resultados = DECTB(1);
			agregado += resultados[0];
			tasaClas += resultados[1];
			tasaRed += resultados[2];
			tiempo += resultados[3];	
			
			resultados = DECTB(2);
			agregado += resultados[0];
			tasaClas += resultados[1];
			tasaRed += resultados[2];
			tiempo += resultados[3];	
			
			resultados = DECTB(3);
			agregado += resultados[0];
			tasaClas += resultados[1];
			tasaRed += resultados[2];
			tiempo += resultados[3];	
			
			resultados = DECTB(4);
			agregado += resultados[0];
			tasaClas += resultados[1];
			tasaRed += resultados[2];
			tiempo += resultados[3];	
			
			// cout << "TIEMPO MEDIO -------> " << (tiempo)/5 << endl;
			// cout << "AGREGADO -------> " << (agregado)/5 << endl;
			// cout << "TASA DE CLASIFICACION MEDIA -------> " << (tasaClas)/5 << endl;
			// cout << "TASA DE REDUCCIÓN MEDIA ES -------> " << (tasaRed)/5 << endl;

			//para hacer más rapida la extración de datos y el paso a tablas hacemos que
			//los resultados se vean de esta forma por pantalla:
			cout << (tasaClas)/5 << "            " << (tasaRed)/5 << "             " << (agregado)/5 << "       " << (tiempo)/5 << endl;
			
			tiempo = 0;
			tasaClas = 0;
			tasaRed = 0;
			agregado = 0;
		break;
		case 6: 
			///////////////////////////////////////////////////////////////////////////////////
			// Pruebas con RELIEF
			cout << "_____________________________________________________________"<< endl;
			cout << "_____________________________________________________________"<< endl;
			cout << "Algoritmo: RELIEF" << endl;

			cout << "Tasa Class.      Tasa Red.      Agregado      Tiempo" << endl;
			resultados = RELIEF(0);
			agregado += resultados[0];
			tasaClas += resultados[1];
			tasaRed += resultados[2];
			tiempo += resultados[3];			

			resultados = RELIEF(1);
			agregado += resultados[0];
			tasaClas += resultados[1];
			tasaRed += resultados[2];
			tiempo += resultados[3];	
			
			resultados = RELIEF(2);
			agregado += resultados[0];
			tasaClas += resultados[1];
			tasaRed += resultados[2];
			tiempo += resultados[3];	
			
			resultados = RELIEF(3);
			agregado += resultados[0];
			tasaClas += resultados[1];
			tasaRed += resultados[2];
			tiempo += resultados[3];	
			
			resultados = RELIEF(4);
			agregado += resultados[0];
			tasaClas += resultados[1];
			tasaRed += resultados[2];
			tiempo += resultados[3];	
			
			// cout << "TIEMPO MEDIO -------> " << (tiempo)/5 << endl;
			// cout << "AGREGADO -------> " << (agregado)/5 << endl;
			// cout << "TASA DE CLASIFICACION MEDIA -------> " << (tasaClas)/5 << endl;
			// cout << "TASA DE REDUCCIÓN MEDIA -------> " << (tasaRed)/5 << endl;

			//para hacer más rapida la extración de datos y el paso a tablas hacemos que
			//los resultados se vean de esta forma por pantalla:
			cout << (tasaClas)/5 << "            " << (tasaRed)/5 << "             " << (agregado)/5 << "       " << (tiempo)/5 << endl;
			
			tiempo = 0;
			tasaClas = 0;
			tasaRed = 0;
			agregado = 0;
		break;
		case 7: 
			///////////////////////////////////////////////////////////////////////////////////
			// Pruebas con RELIEF
			cout << "_____________________________________________________________"<< endl;
			cout << "_____________________________________________________________"<< endl;
			cout << "Algoritmo: LocalSearch" << endl;

			cout << "Tasa Class.      Tasa Red.      Agregado      Tiempo" << endl;
			resultados = localSearch(0);
			agregado += resultados[0];
			tasaClas += resultados[1];
			tasaRed += resultados[2];
			tiempo += resultados[3];			

			resultados = localSearch(1);
			agregado += resultados[0];
			tasaClas += resultados[1];
			tasaRed += resultados[2];
			tiempo += resultados[3];	
			
			resultados = localSearch(2);
			agregado += resultados[0];
			tasaClas += resultados[1];
			tasaRed += resultados[2];
			tiempo += resultados[3];	
			
			resultados = localSearch(3);
			agregado += resultados[0];
			tasaClas += resultados[1];
			tasaRed += resultados[2];
			tiempo += resultados[3];	
			
			resultados = localSearch(4);
			agregado += resultados[0];
			tasaClas += resultados[1];
			tasaRed += resultados[2];
			tiempo += resultados[3];	
			
			// cout << "TIEMPO MEDIO -------> " << (tiempo)/5 << endl;
			// cout << "AGREGADO -------> " << (agregado)/5 << endl;
			// cout << "TASA DE CLASIFICACION MEDIA -------> " << (tasaClas)/5 << endl;
			// cout << "TASA DE REDUCCIÓN MEDIA -------> " << (tasaRed)/5 << endl;

			//para hacer más rapida la extración de datos y el paso a tablas hacemos que
			//los resultados se vean de esta forma por pantalla:
			cout << (tasaClas)/5 << "            " << (tasaRed)/5 << "             " << (agregado)/5 << "       " << (tiempo)/5 << endl;
			
			tiempo = 0;
			tasaClas = 0;
			tasaRed = 0;
			agregado = 0;
		break;
		case 8: 
			///////////////////////////////////////////////////////////////////////////////////
			cout << "_____________________________________________________________"<< endl;
			cout << "_____________________________________________________________"<< endl;
			cout << "Algoritmo: AGG_BLX" << endl;

			cout << "Tasa Class.      Tasa Red.      Agregado      Tiempo" << endl;
			resultados = AGG_BLX(0);
			agregado += resultados[0];
			tasaClas += resultados[1];
			tasaRed += resultados[2];
			tiempo += resultados[3];			

			resultados = AGG_BLX(1);
			agregado += resultados[0];
			tasaClas += resultados[1];
			tasaRed += resultados[2];
			tiempo += resultados[3];	
			
			resultados = AGG_BLX(2);
			agregado += resultados[0];
			tasaClas += resultados[1];
			tasaRed += resultados[2];
			tiempo += resultados[3];	
			
			resultados = AGG_BLX(3);
			agregado += resultados[0];
			tasaClas += resultados[1];
			tasaRed += resultados[2];
			tiempo += resultados[3];	
			
			resultados = AGG_BLX(4);
			agregado += resultados[0];
			tasaClas += resultados[1];
			tasaRed += resultados[2];
			tiempo += resultados[3];	
			
			// cout << "TIEMPO MEDIO -------> " << (tiempo)/5 << endl;
			// cout << "AGREGADO -------> " << (agregado)/5 << endl;
			// cout << "TASA DE CLASIFICACION MEDIA -------> " << (tasaClas)/5 << endl;
			// cout << "TASA DE REDUCCIÓN MEDIA -------> " << (tasaRed)/5 << endl;

			//para hacer más rapida la extración de datos y el paso a tablas hacemos que
			//los resultados se vean de esta forma por pantalla:
			cout << (tasaClas)/5 << "            " << (tasaRed)/5 << "             " << (agregado)/5 << "       " << (tiempo)/5 << endl;
			
			tiempo = 0;
			tasaClas = 0;
			tasaRed = 0;
			agregado = 0;
		break;
		case 9: 
			///////////////////////////////////////////////////////////////////////////////////
			// Pruebas con RELIEF
			cout << "_____________________________________________________________"<< endl;
			cout << "_____________________________________________________________"<< endl;
			cout << "Algoritmo: AMCA10" << endl;

			cout << "Tasa Class.      Tasa Red.      Agregado      Tiempo" << endl;
			resultados = AMCA3(0);
			agregado += resultados[0];
			tasaClas += resultados[1];
			tasaRed += resultados[2];
			tiempo += resultados[3];			

			resultados = AMCA3(1);
			agregado += resultados[0];
			tasaClas += resultados[1];
			tasaRed += resultados[2];
			tiempo += resultados[3];	
			
			resultados = AMCA3(2);
			agregado += resultados[0];
			tasaClas += resultados[1];
			tasaRed += resultados[2];
			tiempo += resultados[3];	
			
			resultados = AMCA3(3);
			agregado += resultados[0];
			tasaClas += resultados[1];
			tasaRed += resultados[2];
			tiempo += resultados[3];	
			
			resultados = AMCA3(4);
			agregado += resultados[0];
			tasaClas += resultados[1];
			tasaRed += resultados[2];
			tiempo += resultados[3];	
			
			// cout << "TIEMPO MEDIO -------> " << (tiempo)/5 << endl;
			// cout << "AGREGADO -------> " << (agregado)/5 << endl;
			// cout << "TASA DE CLASIFICACION MEDIA -------> " << (tasaClas)/5 << endl;
			// cout << "TASA DE REDUCCIÓN MEDIA -------> " << (tasaRed)/5 << endl;

			//para hacer más rapida la extración de datos y el paso a tablas hacemos que
			//los resultados se vean de esta forma por pantalla:
			cout << (tasaClas)/5 << "            " << (tasaRed)/5 << "             " << (agregado)/5 << "       " << (tiempo)/5 << endl;
			
			tiempo = 0;
			tasaClas = 0;
			tasaRed = 0;
			agregado = 0;
		break;
		
	}


	return 0;
}
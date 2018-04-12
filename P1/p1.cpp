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

vector<vector<int>> particiones;
vector<int> particion1;
vector<int> particion2;
vector<int> particion3;
vector<int> particion4;
vector<int> particion5;
Algoritmo *lectorCSV;

float crearTestTrain(vector<int> particion, vector<int> & train, vector<int> & test)
{
	//creamos los conjuntos de datos train y test que iremos sobreescribiendo en las
	//sucesivas particiones
	for(int i = 0; i < particion.size(); i++)
	{
		if(i<(particion.size()/2))
			train.push_back(particion[i]);
		else
			test.push_back(particion[i]);
	}

}

pair<float,float> KNN(int indiceParticion)
{
	clock_t begin, end; 
	double time;
	vector<int> train, test;
	float tasaClasificacion;
	vector<float> w;
	

	//creamos los vector train y test
	crearTestTrain(particiones[indiceParticion],train,test);

	//comenzamos probando el clasificador 1-NN con el conjunto de datos train de la partición 0
	//inicializamos el vector de pesos original a 1
	for(int i= 0; i<lectorCSV->tamanioDatosCols(); i++)
	{
		w.push_back(1.0);
	}
	
	begin = clock(); 
	tasaClasificacion = lectorCSV->KNNClasif(train, test,w);
	end = clock();

	
	time = double(end - begin) / CLOCKS_PER_SEC;
	cout << "Tasa de clasificacion 1-NN particion " << indiceParticion <<  "-train: " << tasaClasificacion << "%" << endl;
	cout << "Tiempo de ejecucion del algoritmo: " << time << endl << endl;

	pair<float,float> resultado;
	resultado.first = time;
	resultado.second = tasaClasificacion;

	begin = clock(); 
	tasaClasificacion = lectorCSV->KNNClasif(test, train,w);
	end = clock();
	time = double(end - begin) / CLOCKS_PER_SEC;
	cout << "Tasa de clasificacion 1-NN particion " << indiceParticion <<  "-test: " << tasaClasificacion << "%" << endl;
	cout << "Tiempo de ejecucion del algoritmo: " << time << endl << endl;

	cout << "----------------------------------------------------------------------------" << endl;

	//reseteamos el tamaño de los vectores
	train.clear();
	test.clear();

	resultado.first += time;
	resultado.second += tasaClasificacion;

	resultado.first /= 2;
	resultado.second /= 2;

	return resultado;
}

pair<float,float> cargarDatos(string fichero)
{
	lectorCSV = new Algoritmo;
	//leemos el csv
	lectorCSV->csvReader(fichero);

	// //para mostrar los datos al completo
	// lectorCSV->mostrarDatosYEtiquetas();

	// para mostrar la última linea de los datos
	// for(int i = 0; i<lectorCSV->matrizDatos[1].size();i++)
	// 	cout << lectorCSV->matrizDatos[lectorCSV->matrizDatos.size()-1][i] << endl;

	//creamos la matriz con 5 particiones
	particiones.push_back(particion1);
	particiones.push_back(particion2);
	particiones.push_back(particion3);
	particiones.push_back(particion4);
	particiones.push_back(particion5);

	//rellenamos las particiones con indices
	for(int i = 0; i < 5; i++)
		for(int j = 0; j < lectorCSV->tamanioDatos(); j++)
			particiones[i].push_back(j);

	//cambiamos el orden de cada una de las particiones.
	random_shuffle(particiones[0].begin(),particiones[0].end());
	random_shuffle(particiones[1].begin(),particiones[1].end());
	random_shuffle(particiones[2].begin(),particiones[2].end());
	random_shuffle(particiones[3].begin(),particiones[3].end());
	random_shuffle(particiones[4].begin(),particiones[4].end());
}

pair<float,float> RELIEF(int indiceParticion)
{
	clock_t begin, end;
	double time;
	vector<int> train, test;
	float tasaClasificacion;
	vector<float> pesos;

	//creamos los vector train y test
	crearTestTrain(particiones[indiceParticion],train,test);
	//lanzamos el algoritmo RELIEF para el train
	pesos = lectorCSV->RELIEF(train,test);
	
	//sacamos los pesos por pantalla
	// for(int i = 0; i< pesos.size(); i++)
		// cout << pesos[i] << " ";

	begin = clock(); 
	tasaClasificacion = lectorCSV->KNNClasif(train, test,pesos);
	end = clock();
	time = double(end - begin) / CLOCKS_PER_SEC;
	cout << "Tasa de clasificacion para datos test 1-NN particion " << indiceParticion <<  "-con los pesos calculados para train: " << tasaClasificacion << "%" << endl;
	cout << "Tiempo de ejecucion del algoritmo: " << time << endl << endl;
	pesos.clear();

	pair<float,float> resultado;
	resultado.first = time;
	resultado.second = tasaClasificacion;


	//hacemos lo mismo pero al reves, calculamos primero unos pesos con los datos test y luego los
	//validamos con los datos train
	//lanzamos el algoritmo RELIEF para el train
	//primero calculamos el RELIEF para el test
	//lanzamos el algoritmo RELIEF para el train
	pesos.clear();
	pesos = lectorCSV->RELIEF(test,train);
	begin = clock(); 
	tasaClasificacion = lectorCSV->KNNClasif(test, train,pesos);
	end = clock();
	time = double(end - begin) / CLOCKS_PER_SEC;
	cout << "Tasa de clasificacion para datos train 1-NN particion " << indiceParticion <<  "-con los pesos calculados para test: " << tasaClasificacion << "%" << endl;
	cout << "Tiempo de ejecucion del algoritmo: " << time << endl << endl;

	cout << "----------------------------------------------------------------------------" << endl;

	resultado.first += time;
	resultado.second += tasaClasificacion;

	resultado.first /= 2;
	resultado.second /= 2;

	return resultado;

}

pair<float,float> localSearch(int indiceParticion)
{
	clock_t begin, end;
	double tiempo;
	vector<int> train, test;
	float tasaClasificacion;
	vector<float> pesos,wini;
	float num = 0;
	pair<vector<float>,float> solucion;


	//inicializamos el vector de pesos con valores aleatorios en el intervalo [0,1]
	for(int i = 0; i < lectorCSV->tamanioDatosCols(); i++)
	{
		num=1+rand()%(1001-1);
		num = num/1000.0;
		wini.push_back(num);
	}
		
	//creamos los vectores train y test
	crearTestTrain(particiones[indiceParticion],train,test);

	//medimos el tiempo de la local search
	begin = clock(); 
	solucion = lectorCSV->localSearch(train,20,wini);
	end = clock();
	tiempo = double(end - begin) / CLOCKS_PER_SEC;
	//calculamos su tasa de clasificación
	cout << "Tasa de clasificacion para datos train con respecto al test LOCAL SEARCH particion " << indiceParticion <<  ":" << solucion.second << "%" << endl;
	cout << "Tiempo de ejecucion LOCAL SEARCH: " << tiempo << endl << endl;
	pesos.clear();

	pair<float,float> resultado;
	resultado.first = tiempo;
	resultado.second = solucion.second;


	//medimos el tiempo de la local search
	begin = clock(); 
	solucion = lectorCSV->localSearch(test,20,wini);
	end = clock();
	tiempo = double(end - begin) / CLOCKS_PER_SEC;
	//calculamos su tasa de clasificación
	cout << "Tasa de clasificacion para datos test con respecto al train LOCAL SEARCH particion " << indiceParticion <<  ":" << solucion.second << "%" << endl;
	cout << "Tiempo de ejecucion LOCAL SEARCH: " << tiempo << endl << endl;
	pesos.clear();

	cout << "----------------------------------------------------------------------------" << endl;

	resultado.first += tiempo;
	resultado.second += solucion.second;

	resultado.first /= 2;
	resultado.second /= 2;


	return resultado;
}

pair<float,float> AGG_BLX(int indiceParticion)
{
	clock_t begin, end;
	double tiempo;
	vector<int> train, test;
	float tasaClasificacion;
	vector<float> pesos;
		
	//creamos los vectores train y test
	crearTestTrain(particiones[indiceParticion],train,test);

	// for(int i = 0; i<train.size();i++)
	// {
	// 	cout << i << " -> " << train[i] << endl;
	// }

	//medimos el tiempo del blx para el train
	begin = clock(); 
	pesos = lectorCSV->AGGBLX(train);
	end = clock();

	// for(int i = 0; i<pesos.size(); i++)
	// 	cout << pesos[i] << " " ;
	tiempo = double(end - begin) / CLOCKS_PER_SEC;
	//calculamos su tasa de clasificación
	tasaClasificacion = lectorCSV->KNNClasif(train,test,pesos);
	cout << "Tasa de clasificacion para datos train con respecto al test AGG_BLX particion " << indiceParticion <<  ":" << tasaClasificacion << "%" << endl;
	cout << "Tiempo de ejecucion AGG_BLX: " << tiempo << endl << endl;
	pesos.clear();


	pair<float,float> resultado;
	resultado.first = tiempo;
	resultado.second = tasaClasificacion;

	//medimos el tiempo del blx para el test
	begin = clock(); 
	pesos = lectorCSV->AGGBLX(test);
	end = clock();

	// for(int i = 0; i<pesos.size(); i++)
	// 	cout << pesos[i] << " " ;
	tiempo = double(end - begin) / CLOCKS_PER_SEC;
	//calculamos su tasa de clasificación
	tasaClasificacion = lectorCSV->KNNClasif(test,train,pesos);
	cout << "Tasa de clasificacion para datos test con respecto al train AGG_BLX particion " << indiceParticion <<  ":" << tasaClasificacion << "%" << endl;
	cout << "Tiempo de ejecucion AGG_BLX: " << tiempo << endl << endl;
	pesos.clear();

	cout << "----------------------------------------------------------------------------" << endl;

	resultado.first += tiempo;
	resultado.second += tasaClasificacion;

	resultado.first /= 2;
	resultado.second /= 2;


	return resultado;
}

pair<float,float> AGG_CA(int indiceParticion)
{
	clock_t begin, end;
	double tiempo;
	vector<int> train, test;
	float tasaClasificacion;
	vector<float> pesos;
		
	//creamos los vectores train y test
	crearTestTrain(particiones[indiceParticion],train,test);

	//medimos el tiempo de AGGCA
	begin = clock(); 
	pesos = lectorCSV->AGGCA(train);
	end = clock();

	// for(int i = 0; i<pesos.size(); i++)
	// 	cout << pesos[i] << " " ;
	tiempo = double(end - begin) / CLOCKS_PER_SEC;
	//calculamos su tasa de clasificación
	tasaClasificacion = lectorCSV->KNNClasif(train, test,pesos);
	cout << "Tasa de clasificacion para datos train con respecto al test AGG_CA particion " << indiceParticion <<  ":" << tasaClasificacion << "%" << endl;
	cout << "Tiempo de ejecucion AGG_CA: " << tiempo << endl << endl;
	pesos.clear();


	pair<float,float> resultado;
	resultado.first = tiempo;
	resultado.second = tasaClasificacion;

	//medimos el tiempo de AGGCA
	begin = clock(); 
	pesos = lectorCSV->AGGCA(test);
	end = clock();

	// for(int i = 0; i<pesos.size(); i++)
	// 	cout << pesos[i] << " " ;
	tiempo = double(end - begin) / CLOCKS_PER_SEC;
	//calculamos su tasa de clasificación
	tasaClasificacion = lectorCSV->KNNClasif(test, train,pesos);
	cout << "Tasa de clasificacion para datos test con respecto al train AGG_CA particion " << indiceParticion <<  ":" << tasaClasificacion << "%" << endl;
	cout << "Tiempo de ejecucion AGG_CA: " << tiempo << endl << endl;
	pesos.clear();

	cout << "----------------------------------------------------------------------------" << endl;

	resultado.first += tiempo;
	resultado.second += tasaClasificacion;

	resultado.first /= 2;
	resultado.second /= 2;


	return resultado;
}

pair<float,float> AGE_BLX(int indiceParticion)
{
	clock_t begin, end;
	double tiempo;
	vector<int> train, test;
	float tasaClasificacion;
	vector<float> pesos;
		
	//creamos los vectores train y test
	crearTestTrain(particiones[indiceParticion],train,test);

	//medimos el tiempo de la local search
	begin = clock(); 
	pesos = lectorCSV->AGEBLX(train);
	end = clock();

	// for(int i = 0; i<pesos.size(); i++)
	// 	cout << pesos[i] << " " ;
	tiempo = double(end - begin) / CLOCKS_PER_SEC;
	//calculamos su tasa de clasificación
	tasaClasificacion = lectorCSV->KNNClasif(train, test,pesos);
	cout << "Tasa de clasificacion para datos train con respecto al test AGG_CA particion " << indiceParticion <<  ":" << tasaClasificacion << "%" << endl;
	cout << "Tiempo de ejecucion AGE_BLX: " << tiempo << endl << endl;
	pesos.clear();


	pair<float,float> resultado;
	resultado.first = tiempo;
	resultado.second = tasaClasificacion;

	//medimos el tiempo de la local search
	begin = clock(); 
	pesos = lectorCSV->AGEBLX(test);
	end = clock();

	// for(int i = 0; i<pesos.size(); i++)
	// 	cout << pesos[i] << " " ;
	tiempo = double(end - begin) / CLOCKS_PER_SEC;
	//calculamos su tasa de clasificación
	tasaClasificacion = lectorCSV->KNNClasif(test, train,pesos);
	cout << "Tasa de clasificacion para datos test con respecto al train AGG_CA particion " << indiceParticion <<  ":" << tasaClasificacion << "%" << endl;
	cout << "Tiempo de ejecucion AGE_BLX: " << tiempo << endl << endl;
	pesos.clear();

	cout << "----------------------------------------------------------------------------" << endl;

	resultado.first += tiempo;
	resultado.second += tasaClasificacion;

	resultado.first /= 2;
	resultado.second /= 2;


	return resultado;
}

pair<float,float> AGE_CA(int indiceParticion)
{
	clock_t begin, end;
	double tiempo;
	vector<int> train, test;
	float tasaClasificacion;
	vector<float> pesos;
		
	//creamos los vectores train y test
	crearTestTrain(particiones[indiceParticion],train,test);

	//medimos el tiempo de la local search
	begin = clock(); 
	pesos = lectorCSV->AGECA(train);
	end = clock();

	// for(int i = 0; i<pesos.size(); i++)
	// 	cout << pesos[i] << " " ;
	tiempo = double(end - begin) / CLOCKS_PER_SEC;
	//calculamos su tasa de clasificación
	tasaClasificacion = lectorCSV->KNNClasif(train, test,pesos);
	cout << "Tasa de clasificacion para datos train con respecto al test AGG_CA particion " << indiceParticion <<  ":" << tasaClasificacion << "%" << endl;
	cout << "Tiempo de ejecucion AGE_CA: " << tiempo << endl << endl;
	pesos.clear();

	pair<float,float> resultado;
	resultado.first = tiempo;
	resultado.second = tasaClasificacion;

	//medimos el tiempo de la local search
	begin = clock(); 
	pesos = lectorCSV->AGECA(test);
	end = clock();

	// for(int i = 0; i<pesos.size(); i++)
	// 	cout << pesos[i] << " " ;
	tiempo = double(end - begin) / CLOCKS_PER_SEC;
	//calculamos su tasa de clasificación
	tasaClasificacion = lectorCSV->KNNClasif(test, train,pesos);
	cout << "Tasa de clasificacion para datos test con respecto al train AGG_CA particion " << indiceParticion <<  ":" << tasaClasificacion << "%" << endl;
	cout << "Tiempo de ejecucion AGE_CA: " << tiempo << endl << endl;
	pesos.clear();

	cout << "----------------------------------------------------------------------------" << endl;

	resultado.first += tiempo;
	resultado.second += tasaClasificacion;

	resultado.first /= 2;
	resultado.second /= 2;


	return resultado;
}

pair<float,float> AMCA1(int indiceParticion)
{
	clock_t begin, end;
	double tiempo;
	vector<int> train, test;
	float tasaClasificacion;
	vector<float> pesos;
		
	//creamos los vectores train y test
	crearTestTrain(particiones[indiceParticion],train,test);

	//medimos el tiempo de la local search
	begin = clock(); 
	pesos = lectorCSV->AMCA1(train);
	end = clock();

	// for(int i = 0; i<pesos.size(); i++)
	// 	cout << pesos[i] << " " ;
	tiempo = double(end - begin) / CLOCKS_PER_SEC;
	//calculamos su tasa de clasificación
	tasaClasificacion = lectorCSV->KNNClasif(train, test,pesos);
	cout << "Tasa de clasificacion para datos train con respecto al test AMCA1 particion " << indiceParticion <<  ":" << tasaClasificacion << "%" << endl;
	cout << "Tiempo de ejecucion AMCA1: " << tiempo << endl << endl;
	pesos.clear();

	pair<float,float> resultado;
	resultado.first = tiempo;
	resultado.second = tasaClasificacion;

	//medimos el tiempo de la local search
	begin = clock(); 
	pesos = lectorCSV->AMCA1(test);
	end = clock();

	// for(int i = 0; i<pesos.size(); i++)
	// 	cout << pesos[i] << " " ;
	tiempo = double(end - begin) / CLOCKS_PER_SEC;
	//calculamos su tasa de clasificación
	tasaClasificacion = lectorCSV->KNNClasif(test, train,pesos);
	cout << "Tasa de clasificacion para datos test con respecto al train AMCA1 particion " << indiceParticion <<  ":" << tasaClasificacion << "%" << endl;
	cout << "Tiempo de ejecucion AMCA1: " << tiempo << endl << endl;
	pesos.clear();

	cout << "----------------------------------------------------------------------------" << endl;

	resultado.first += tiempo;
	resultado.second += tasaClasificacion;

	resultado.first /= 2;
	resultado.second /= 2;


	return resultado;
}

pair<float,float> AMCA2(int indiceParticion)
{
	clock_t begin, end;
	double tiempo;
	vector<int> train, test;
	float tasaClasificacion;
	vector<float> pesos;
		
	//creamos los vectores train y test
	crearTestTrain(particiones[indiceParticion],train,test);

	//medimos el tiempo de la local search
	begin = clock(); 
	pesos = lectorCSV->AMCA2(train);
	end = clock();

	// for(int i = 0; i<pesos.size(); i++)
	// 	cout << pesos[i] << " " ;
	tiempo = double(end - begin) / CLOCKS_PER_SEC;
	//calculamos su tasa de clasificación
	tasaClasificacion = lectorCSV->KNNClasif(train, test,pesos);
	cout << "Tasa de clasificacion para datos train con respecto al test AMCA2 particion " << indiceParticion <<  ":" << tasaClasificacion << "%" << endl;
	cout << "Tiempo de ejecucion AMCA2: " << tiempo << endl << endl;
	pesos.clear();

	pair<float,float> resultado;
	resultado.first = tiempo;
	resultado.second = tasaClasificacion;


	//medimos el tiempo de la local search
	begin = clock(); 
	pesos = lectorCSV->AMCA2(test);
	end = clock();

	// for(int i = 0; i<pesos.size(); i++)
	// 	cout << pesos[i] << " " ;
	tiempo = double(end - begin) / CLOCKS_PER_SEC;
	//calculamos su tasa de clasificación
	tasaClasificacion = lectorCSV->KNNClasif(test, train,pesos);
	cout << "Tasa de clasificacion para datos test con respecto al train AMCA2 particion " << indiceParticion <<  ":" << tasaClasificacion << "%" << endl;
	cout << "Tiempo de ejecucion AMCA2: " << tiempo << endl << endl;
	pesos.clear();

	cout << "----------------------------------------------------------------------------" << endl;

	resultado.first += tiempo;
	resultado.second += tasaClasificacion;

	resultado.first /= 2;
	resultado.second /= 2;


	return resultado;
}

pair<float,float> AMCA3(int indiceParticion)
{
	clock_t begin, end;
	double tiempo;
	vector<int> train, test;
	float tasaClasificacion;
	vector<float> pesos;
		
	//creamos los vectores train y test
	crearTestTrain(particiones[indiceParticion],train,test);

	//medimos el tiempo de la local search
	begin = clock(); 
	pesos = lectorCSV->AMCA3(train);
	end = clock();

	// for(int i = 0; i<pesos.size(); i++)
	// 	cout << pesos[i] << " " ;
	tiempo = double(end - begin) / CLOCKS_PER_SEC;
	//calculamos su tasa de clasificación
	tasaClasificacion = lectorCSV->KNNClasif(train, test,pesos);
	cout << "Tasa de clasificacion para datos train con respecto al test AMCA3 particion " << indiceParticion <<  ":" << tasaClasificacion << "%" << endl;
	cout << "Tiempo de ejecucion AMCA3: " << tiempo << endl << endl;
	pesos.clear();

	pair<float,float> resultado;
	resultado.first = tiempo;
	resultado.second = tasaClasificacion;

	//medimos el tiempo de la local search
	begin = clock(); 
	pesos = lectorCSV->AMCA3(test);
	end = clock();

	// for(int i = 0; i<pesos.size(); i++)
	// 	cout << pesos[i] << " " ;
	tiempo = double(end - begin) / CLOCKS_PER_SEC;
	//calculamos su tasa de clasificación
	tasaClasificacion = lectorCSV->KNNClasif(test, train,pesos);
	cout << "Tasa de clasificacion para datos test con respecto al train AMCA3 particion " << indiceParticion <<  ":" << tasaClasificacion << "%" << endl;
	cout << "Tiempo de ejecucion AMCA3: " << tiempo << endl << endl;
	pesos.clear();

	cout << "----------------------------------------------------------------------------" << endl;

	resultado.first += tiempo;
	resultado.second += tasaClasificacion;

	resultado.first /= 2;
	resultado.second /= 2;


	return resultado;

}



int main()
{


	//obtenemos el numero de procesadores del pc
	omp_set_num_threads(omp_get_num_procs());

	//ponemos la semilla para la generación estática de números aleatorios
	srand(7);

	//leemos los datos
	cargarDatos("cancer.csv");

	//MENU
	pair<float,float> tiempo_tasa;
	int algoritmo = -1;
	float tiempo = 0;
	float tasa = 0;
	cout << endl << "ELIGE UN ALGORITMO PARA LANZAR: " << endl;
	cout << "1.Algoritmo 1-NN" << endl << "2.Algoritmo RELIEF" << endl;
	cout << "3.Algoritmo LOCAL SEARCH" << endl;
	cout << "4.Algoritmo GENETICO GENERACIONAL - BLX" << endl;
	cout << "5.Algoritmo GENETICO GENERACIONAL - CA" << endl;
	cout << "6.Algoritmo GENETICO ESTACIONARIO - BLX" << endl;
	cout << "7.Algoritmo GENETICO ESTACIONARIO - CA" << endl;
	cout << "8.Algoritmo MEMETICO GENERACIONAL cambio generacion completa - CA" << endl;
	cout << "9.Algoritmo MEMETICO GENERACIONAL cambio solo un subconjunto - CA" << endl;
	cout << "10.Algoritmo MEMETICO GENERACIONAL cambio los mejores - CA" << endl;
	cin >> algoritmo;


	cout << lectorCSV->tamanioDatos() << " " << lectorCSV->tamanioDatosCols() << endl;
 
	switch(algoritmo)
	{

		case 1:
			///////////////////////////////////////////////////////////////////////////////////
			// PRUEBAS CON 1-NN 
			cout << "_____________________________________________________________"<< endl;
			cout << "_____________________________________________________________"<< endl;

			tiempo_tasa = KNN(0);
			tiempo += tiempo_tasa.first;
			tasa += tiempo_tasa.second;

			tiempo_tasa = KNN(1);
			tiempo += tiempo_tasa.first;
			tasa += tiempo_tasa.second;
			
			tiempo_tasa = KNN(2);
			tiempo += tiempo_tasa.first;
			tasa += tiempo_tasa.second;
			
			tiempo_tasa = KNN(3);
			tiempo += tiempo_tasa.first;
			tasa += tiempo_tasa.second;
			
			tiempo_tasa = KNN(4);
			tiempo += tiempo_tasa.first;
			tasa += tiempo_tasa.second;
			

			cout << "TIEMPO MEDIO KNN -------> " << (tiempo)/5 << endl;
			cout << "TASA MEDIA KNN -------> " << (tasa)/5 << endl;

			tiempo = 0;
			tasa = 0;
		break;
		case 2:
			///////////////////////////////////////////////////////////////////////////////////
			// PRUEBAS CON RELIEF 
			cout << "_____________________________________________________________"<< endl;
			cout << "_____________________________________________________________"<< endl;	
			tiempo_tasa = RELIEF(0);
			tiempo += tiempo_tasa.first;
			tasa += tiempo_tasa.second;

			tiempo_tasa = RELIEF(1);
			tiempo += tiempo_tasa.first;
			tasa += tiempo_tasa.second;

			tiempo_tasa = RELIEF(2);
			tiempo += tiempo_tasa.first;
			tasa += tiempo_tasa.second;

			tiempo_tasa = RELIEF(3);
			tiempo += tiempo_tasa.first;
			tasa += tiempo_tasa.second;

			tiempo_tasa = RELIEF(4);
			tiempo += tiempo_tasa.first;
			tasa += tiempo_tasa.second;

			cout << "TIEMPO MEDIO RELIEF -------> " << (tiempo)/5 << endl;
			cout << "TASA MEDIA RELIEF -------> " << (tasa)/5 << endl;

			tiempo = 0;
			tasa = 0;
		break;
		case 3:
			///////////////////////////////////////////////////////////////////////////////////
			// PRUEBAS CON LOCAL SEARCH 
			cout << "_____________________________________________________________"<< endl;
			cout << "_____________________________________________________________"<< endl;

			tiempo_tasa = localSearch(0);
			tiempo += tiempo_tasa.first;
			tasa += tiempo_tasa.second;

			tiempo_tasa = localSearch(1);
			tiempo += tiempo_tasa.first;
			tasa += tiempo_tasa.second;

			tiempo_tasa = localSearch(2);
			tiempo += tiempo_tasa.first;
			tasa += tiempo_tasa.second;

			tiempo_tasa = localSearch(3);
			tiempo += tiempo_tasa.first;
			tasa += tiempo_tasa.second;

			tiempo_tasa = localSearch(4);
			tiempo += tiempo_tasa.first;
			tasa += tiempo_tasa.second;

			cout << "TIEMPO MEDIO LOCAL SEARCH -------> " << (tiempo)/5 << endl;
			cout << "TASA MEDIA LOCAL SEARCH -------> " << (tasa)/5 << endl;

			tiempo = 0;
			tasa = 0;

		break;
		case 4:
			///////////////////////////////////////////////////////////////////////////////////
			// PRUEBAS CON ALGORITMO GENÉTICO GENERACIONAL
			cout << "_____________________________________________________________"<< endl;
			cout << "_____________________________________________________________"<< endl;

			tiempo_tasa = AGG_BLX(0);
			tiempo += tiempo_tasa.first;
			tasa += tiempo_tasa.second;

			tiempo_tasa = AGG_BLX(1);
			tiempo += tiempo_tasa.first;
			tasa += tiempo_tasa.second;

			tiempo_tasa = AGG_BLX(2);
			tiempo += tiempo_tasa.first;
			tasa += tiempo_tasa.second;

			tiempo_tasa = AGG_BLX(3);
			tiempo += tiempo_tasa.first;
			tasa += tiempo_tasa.second;

			tiempo_tasa = AGG_BLX(4);
			tiempo += tiempo_tasa.first;
			tasa += tiempo_tasa.second;

			cout << "TIEMPO MEDIO AGGBLX -------> " << (tiempo)/5 << endl;
			cout << "TASA MEDIA AGGBLX -------> " << (tasa)/5 << endl;

			tiempo = 0;
			tasa = 0;


		break;
		case 5:
			cout << "_____________________________________________________________"<< endl;
			cout << "_____________________________________________________________"<< endl;

			tiempo_tasa = AGG_CA(0);
			tiempo += tiempo_tasa.first;
			tasa += tiempo_tasa.second;

			tiempo_tasa = AGG_CA(1);
			tiempo += tiempo_tasa.first;
			tasa += tiempo_tasa.second;

			tiempo_tasa = AGG_CA(2);
			tiempo += tiempo_tasa.first;
			tasa += tiempo_tasa.second;

			tiempo_tasa = AGG_CA(3);
			tiempo += tiempo_tasa.first;
			tasa += tiempo_tasa.second;

			tiempo_tasa = AGG_CA(4);
			tiempo += tiempo_tasa.first;
			tasa += tiempo_tasa.second;

			cout << "TIEMPO MEDIO AGGCA -------> " << (tiempo)/5 << endl;
			cout << "TASA MEDIA AGGCA -------> " << (tasa)/5 << endl;

			tiempo = 0;
			tasa = 0;

		break;
		case 6:
			cout << "_____________________________________________________________"<< endl;
			cout << "_____________________________________________________________"<< endl;

			tiempo_tasa = AGE_BLX(0);
			tiempo += tiempo_tasa.first;
			tasa += tiempo_tasa.second;

			tiempo_tasa = AGE_BLX(1);
			tiempo += tiempo_tasa.first;
			tasa += tiempo_tasa.second;

			tiempo_tasa = AGE_BLX(2);
			tiempo += tiempo_tasa.first;
			tasa += tiempo_tasa.second;

			tiempo_tasa = AGE_BLX(3);
			tiempo += tiempo_tasa.first;
			tasa += tiempo_tasa.second;

			tiempo_tasa = AGE_BLX(4);
			tiempo += tiempo_tasa.first;
			tasa += tiempo_tasa.second;

			cout << "TIEMPO MEDIO AGEBLX -------> " << (tiempo)/5 << endl;
			cout << "TASA MEDIA AGEBLX -------> " << (tasa)/5 << endl;

			tiempo = 0;
			tasa = 0;

		break;
		case 7:
			cout << "_____________________________________________________________"<< endl;
			cout << "_____________________________________________________________"<< endl;

			tiempo_tasa = AGE_CA(0);
			tiempo += tiempo_tasa.first;
			tasa += tiempo_tasa.second;

			tiempo_tasa = AGE_CA(1);
			tiempo += tiempo_tasa.first;
			tasa += tiempo_tasa.second;

			tiempo_tasa = AGE_CA(2);
			tiempo += tiempo_tasa.first;
			tasa += tiempo_tasa.second;

			tiempo_tasa = AGE_CA(3);
			tiempo += tiempo_tasa.first;
			tasa += tiempo_tasa.second;

			tiempo_tasa = AGE_CA(4);
			tiempo += tiempo_tasa.first;
			tasa += tiempo_tasa.second;

			cout << "TIEMPO MEDIO AGECA -------> " << (tiempo)/5 << endl;
			cout << "TASA MEDIA AGECA -------> " << (tasa)/5 << endl;

			tiempo = 0;
			tasa = 0;

		break;
		case 8:
			cout << "_____________________________________________________________"<< endl;
			cout << "_____________________________________________________________"<< endl;



			tiempo_tasa = AMCA1(0);
			tiempo += tiempo_tasa.first;
			tasa += tiempo_tasa.second;

			tiempo_tasa = AMCA1(1);
			tiempo += tiempo_tasa.first;
			tasa += tiempo_tasa.second;

			tiempo_tasa = AMCA1(2);
			tiempo += tiempo_tasa.first;
			tasa += tiempo_tasa.second;

			tiempo_tasa = AMCA1(3);
			tiempo += tiempo_tasa.first;
			tasa += tiempo_tasa.second;

			tiempo_tasa = AMCA1(4);
			tiempo += tiempo_tasa.first;
			tasa += tiempo_tasa.second;

			cout << "TIEMPO MEDIO AMCA1 -------> " << (tiempo)/5 << endl;
			cout << "TASA MEDIA AMCA1 -------> " << (tasa)/5 << endl;

			tiempo = 0;
			tasa = 0;

		break;
		case 9:
			cout << "_____________________________________________________________"<< endl;
			cout << "_____________________________________________________________"<< endl;

			tiempo_tasa = AMCA2(0);
			tiempo += tiempo_tasa.first;
			tasa += tiempo_tasa.second;

			tiempo_tasa = AMCA2(1);
			tiempo += tiempo_tasa.first;
			tasa += tiempo_tasa.second;

			tiempo_tasa = AMCA2(2);
			tiempo += tiempo_tasa.first;
			tasa += tiempo_tasa.second;

			tiempo_tasa = AMCA2(3);
			tiempo += tiempo_tasa.first;
			tasa += tiempo_tasa.second;

			tiempo_tasa = AMCA2(4);
			tiempo += tiempo_tasa.first;
			tasa += tiempo_tasa.second;

			cout << "TIEMPO MEDIO AMCA2 -------> " << (tiempo)/5 << endl;
			cout << "TASA MEDIA AMCA2 -------> " << (tasa)/5 << endl;

			tiempo = 0;
			tasa = 0;

		break;
		case 0:
			cout << "_____________________________________________________________"<< endl;
			cout << "_____________________________________________________________"<< endl;

			tiempo_tasa = AMCA3(0);
			tiempo += tiempo_tasa.first;
			tasa += tiempo_tasa.second;

			tiempo_tasa = AMCA3(1);
			tiempo += tiempo_tasa.first;
			tasa += tiempo_tasa.second;

			tiempo_tasa = AMCA3(2);
			tiempo += tiempo_tasa.first;
			tasa += tiempo_tasa.second;

			tiempo_tasa = AMCA3(3);
			tiempo += tiempo_tasa.first;
			tasa += tiempo_tasa.second;

			tiempo_tasa = AMCA3(4);
			tiempo += tiempo_tasa.first;
			tasa += tiempo_tasa.second;

			cout << "TIEMPO MEDIO AMCA3 -------> " << (tiempo)/5 << endl;
			cout << "TASA MEDIA AMCA3 -------> " << (tasa)/5 << endl;

			tiempo = 0;
			tasa = 0;

		break;
	}


	return 0;
}
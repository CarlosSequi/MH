#include "algoritmos.h"

default_random_engine generator;
normal_distribution<float> distribution(0.0,0.3);


int Algoritmo::tamanioDatos()
{
	return matrizDatos.size();
}

void Algoritmo::csvReader(string fichero)
{
	//declaramos el flujo del fichero
	ifstream file ( fichero );
	//declaramos la cabecera
	string cabecera;
	//la "leemos", descartándola, ya que no la necesitamos
	getline ( file, cabecera, '\n' );
	//contador para el posterior bucle for
	int i = 0;
	//para leer cadenas de cada linea
	string cadena;

	vector<float> v;
	matrizDatos.push_back(v);

	while ( file.good() )
	{
		//leemos una primera linea
	    getline ( file, cadena, '\n' ); 
	    
	    //si la linea es distinta del caracter fin de fichero
	    if(cadena != "\0")
	    {
		    //Quitamos el número de linea: (el primer caracter del csv es un número)
		   	istringstream linea (cadena);
		   	getline ( linea, cadena, ',' );

		   	//hasta que no se halla leido la linea completa:
		   	while(!linea.eof())
		   	{
		   		//leemos cada una de las cadenas de la linea
		   		getline ( linea, cadena, ',' );
		   		//en caso de encontrar la cadena '"' significa que estamos ante una etiqueta
		   		if(cadena.find('"') != string::npos)
		   		{	//eliminamos la doble comilla final
		   			cadena.pop_back();
		   			//eliminamos la doble comilla inicial
		   			cadena = cadena.substr(1);
		   			//insertamos la etiqueta en el vector de etiquetas
		   			etiquetas.push_back(cadena);
		   		}
		   		else//en caso de ser un valor numérico
		   		{
		   			//convertimos lo leido en un float
		   			float dato = atof(cadena.c_str());
		   			//lo introducimos en la matriz de datos
		   			matrizDatos[i].push_back(dato);
		   		}
		   	}

		   //aumentamos el contador de iteraciones
		   i++;
		   vector<float> v;
		   matrizDatos.push_back(v);
		}
	}
	matrizDatos.pop_back();
}

int Algoritmo::tamanioDatosCols()
{
	return matrizDatos[0].size();
}


void Algoritmo::mostrarDatosYEtiquetas()
{
	for(int i =0; i<matrizDatos.size(); i++)
	{
		cout << etiquetas[i] << " ";
		for(int j = 0; j<matrizDatos[i].size(); j++)
		{
			cout << matrizDatos[i][j] << " ";
			if(j == matrizDatos[i].size()-1)
				cout << endl << endl;
		}
	}
}

float Algoritmo::tasaClasificacion(vector<string> & nuevasEtiquetas, vector<int> & indicesMatrizDatos)
{
	float cantidadBienClasificados = 0;
	float tasa = 0.0;
	//calcuamos el número de etiquetas bien clasificadas
	for(int i = 0; i < nuevasEtiquetas.size(); i++)
	{
		if(nuevasEtiquetas[i] == etiquetas[indicesMatrizDatos[i]])
			cantidadBienClasificados++;
	}

	//cout << cantidadBienClasificados << "  " << nuevasEtiquetas.size() << " " << indicesMatrizDatos.size(); 

	//calculamos la tasa de clasificación
	tasa = 100 * cantidadBienClasificados / nuevasEtiquetas.size();
	tasa *= 1.0;
	
	return tasa;
}

float Algoritmo::calculaDistancia(int & a, int & b, vector<float> & w)
{
	float dist=0;
	
	vector<float>::iterator ita,itb;
	vector<float>::iterator itw = w.begin();
	ita = matrizDatos[a].begin();
	itb = matrizDatos[b].begin();

	//en caso de ser un conjunto de datos impar hacemos la primera iteracion 
	//para poder hacer el desenrollado de bucles de dos en dos
	if(w.size()%2 != 0)
	{
		dist += *itw * ((*ita-*itb) * (*ita-*itb));
		itw++;
		ita++;
		itb++; 
	}
	//calculamos el resto con el desenrollado de bucles
	for(itw; itw != w.end(); itw++)
	{
		dist += *itw * ((*ita-*itb) * (*ita-*itb));
		ita++;
		itb++;
		itw++;

		dist += *itw * ((*ita-*itb) * (*ita-*itb));
		ita++;
		itb++;

	}


	return sqrt(dist);
}

void Algoritmo::actualizarPesosRELIEF(vector<float> & w, vector<float> & filaActual, int amigo, int enemigo)
{
	for(int i = 0; i<w.size(); i++)
	{
		w[i] = w[i] + abs(filaActual[i]-matrizDatos[enemigo][i]) - abs(filaActual[i]-matrizDatos[amigo][i]);
	}
}

vector<float> Algoritmo::RELIEF(vector<int>  & train, vector<int>  & test)
{
	//creamos el vector de pesos inicializado a cero
	vector<float> w, wAux;
	int enemigo = -1;
	int amigo = -1;
	float distanciaVecino = 0;
	float distMinAmigo = 1000;
	float distMinEnemigo = 1000;

	//para poder usar el mismo metodo de calcular distancias que usa KNN
	//creo un vector de pesos auxiliar iniciado a 1, el cual no interfiere
	//en los resultados
	
	//inicializamos el vector de pesos original a 0
	for(int i= 0; i<matrizDatos[0].size(); i++)
	{
		w.push_back(0);
		wAux.push_back(1);
	}

	for(int i = 0; i < train.size(); i++)
	{
		for(int e = 0; e < test.size(); e++)
		{
			if(test == train)//en el caso de la búsqueda local
			{
				if(train[i] != test[e])//leave-one-out
				{
					distanciaVecino = calculaDistancia(train[i], test[e], wAux);
					//para solo amigos
					if(etiquetas[train[i]] == etiquetas[test[e]])
					{
						if(distMinAmigo > distanciaVecino)
						{
							amigo = test[e];
							distMinAmigo = distanciaVecino;
						}
					}
					//para solo enemigos
					else
					{
						if(distMinEnemigo > distanciaVecino)
						{
							enemigo = test[e];
							distMinEnemigo = distanciaVecino;
						}
					}
				}
			}
			else
			{
				distanciaVecino = calculaDistancia(train[i], test[e], wAux);
					//para solo amigos
					if(etiquetas[train[i]] == etiquetas[test[e]])
					{
						if(distMinAmigo > distanciaVecino)
						{
							amigo = test[e];
							distMinAmigo = distanciaVecino;
						}
					}
					//para solo enemigos
					else
					{
						if(distMinEnemigo > distanciaVecino)
						{
							enemigo = test[e];
							distMinEnemigo = distanciaVecino;
						}
					}
			}
		}
		distMinAmigo = 1000;
		distMinEnemigo = 1000;
		//actualizamos pesos
		actualizarPesosRELIEF(w,matrizDatos[train[i]], amigo, enemigo);
	}

	//extraemos el máximo valor del vector de pesos
	float *w_max = max_element(&w[0],&(w[0])+w.size());

	//normalizamos los pesos
	for(int i = 0; i<w.size(); i++)
	{
		if(w[i] < 0)
		{
			w[i] = 0;
		}
		else
		{
			w[i] = w[i]/(*w_max);
		}
	}


	return w;
}

float Algoritmo::KNNClasif(vector<int> & train, vector<int> & test, vector<float> & w)
{
	float minimaDistancia = 100;
	int indiceVecinoMasCercano = -1;
	float cantidadBienClasificados = 0;

	for(vector<int>::iterator i = train.begin(); i != train.end(); i++)
	{
		for(vector<int>::iterator e = test.begin(); e != test.end(); e++)
		{
				if(i != e)//leave-one-out
				{
					//calculamos la distancia del vecino e del actual i
					float distanciaVecino = calculaDistancia(*i, *e, w); 
					//en caso de ser menor que la distancia minima almacenada hasta ahora, actualizamos 
					//al nuevo vecino más cercano
					if(distanciaVecino < minimaDistancia)
					{
						indiceVecinoMasCercano = *e;
						minimaDistancia = distanciaVecino;
					}
				}
		}
		//para calcular la tasa de clasificacion:
		if(etiquetas[indiceVecinoMasCercano] == etiquetas[*i])
			cantidadBienClasificados++;
		//actualizamos el valor de minimaDistancia
		minimaDistancia = 100;
	}
	//calculamos la tasa de clasificación del algoritmo
	float tasa = 100 * cantidadBienClasificados / train.size();

	return tasa*1.0;
}

pair<vector<float>, float> Algoritmo::localSearch(vector<int> & indicesMatrizDatos, int cantidadIndicada, vector<float> & wini)
{
	float tasaClasificacionVecino, tasaClasificacionSolucionActual;
	int posModificar = 0;
	int indiceModificacion = 0;
	bool hayMejora = true;
	pair<vector<float>, float> solucion;
	
	int iteracionesSobreVecino = -1;

	//generamos el vector de posiciones a modificar de forma aleatoria
	vector<int> posModificadas(matrizDatos[0].size(),0);
	iota(posModificadas.begin(), posModificadas.end(), 0);
	random_shuffle(posModificadas.begin(), posModificadas.end());
	// for(int i = 0; i<posModificadas.size(); i++)
	// 	cout << posModificadas[i] << " " ;


	//inicializamos solucionActual y vecino
	vector<float> solucionActual = wini;
	vector<float> vecino = wini;

	//calculamos la tasa de clasificación del vector de pesos inicial wini
	tasaClasificacionSolucionActual = KNNClasif(indicesMatrizDatos,indicesMatrizDatos,solucionActual);

	for(int i = 0; i < (solucionActual.size()*cantidadIndicada) && i < 15000  /*&& hayMejora*/; i++)
	{
		//calculamos de forma aleatoria la posicion a mutar de la solucion actual
		posModificar = posModificadas[indiceModificacion];
		indiceModificacion++;

		//generamos un vector de pesos vecino
		vecino = generarVecino(solucionActual, posModificar);

		//calculamos la tasa de clasifiacion de ese vecino con 1-NN
		tasaClasificacionVecino = KNNClasif(indicesMatrizDatos,indicesMatrizDatos,vecino);
	
		//en caso de que el vecino sea mejor que la solución actual...
		if(tasaClasificacionSolucionActual < tasaClasificacionVecino)
		{			
			//volvemos a iniciar el vector de modificados
			random_shuffle(posModificadas.begin(), posModificadas.end());
			indiceModificacion = 0;
			tasaClasificacionSolucionActual = tasaClasificacionVecino;
			//sustituimos la solución actual por la mejor
			solucionActual = vecino; 
			
		}
		else if(iteracionesSobreVecino == solucionActual.size()-1)
		{
			random_shuffle(posModificadas.begin(), posModificadas.end());
			indiceModificacion = 0;
			//cout << "NO HAY MEJORA A PARTIR DE LA ITERACION " << i << endl;
		}

	}

	solucion.first=solucionActual;
	solucion.second=tasaClasificacionSolucionActual;

	return solucion;
}

//método que me encuentra un valor entero dentro de un vector
bool Algoritmo::Encontrar(vector<int> & v, int x)
{
	for(int i = 0; i < v.size(); i++)
	{
		if(v[i] == x)
		{
			return true;
		}
	}

	return false;
}

vector<float> Algoritmo::generarVecino(vector<float> & w, int & posModificar)
{
	
  	//generamos un número aleatorio de media 0 y varianza 0.03
  	float number = distribution(generator);
  	//hacemos la copia del nuevo vecino
  	vector<float> nuevoVecino =  w;
  	//lo modificamos en la posición correspondiente
  	nuevoVecino[posModificar] += number;

  	//cout << "MODIFICACION VECINO " << number;
  	//truncamos el valor generado en caso de pasarse de los limites
  	if(nuevoVecino[posModificar] > 1)
  	{
  		nuevoVecino[posModificar] = 1;
  	}
  	else if(nuevoVecino[posModificar] < 0)
  	{
  		nuevoVecino[posModificar] = 0;
  	}
  	//devolvemos el nuevo vecino
  	return nuevoVecino;
}

vector<float> Algoritmo::AGGBLX(vector<int> indicesMatrizDatos)
{
	float probabilidadMutacion = 0.001;
	float probabilidadCruce = 0.7;
	int cantidadEvaluaciones = 0;
	//inicializamos el vector de pesos con valores aleatorios en el intervalo [0,1]
	float num = 0;
	vector<vector<float>> poblacion;
	vector<float> individuo;

	for(int i = 0; i < 30; i++)
	{
		for(int e = 0; e < matrizDatos[0].size(); e++)
		{
			num=1+rand()%(1001-1);
			num = num/1000.0;
			individuo.push_back(num);
		}
		poblacion.push_back(individuo);
		individuo.clear();
	}

	
	//evaluamos cada uno de los individuos de la poblacion
	//y almacenamos cada una de las tasas en el vector
	vector<float> tasasIndividuos;
	tasasIndividuos.reserve(30);
	for(int i = 0; i<poblacion.size(); i++)
	{
		tasasIndividuos.push_back(KNNClasif(indicesMatrizDatos,indicesMatrizDatos,poblacion[i]));
		cantidadEvaluaciones++;
	}

	//vector con los indices de los mejores padres calculados con el torneo binario
	vector<int> mejoresPadres;
	pair<vector<float>,vector<float>> parHijos;
	int cantidadCruces = tasasIndividuos.size()/2*probabilidadCruce;
	cantidadCruces += 1;
	vector<vector<float>> nuevaPoblacion;
	vector<float> tasasIndividuosNuevaPoblacion;
	bool mejorPadrePerdido = true;
	int s = 0;
	while(cantidadEvaluaciones < 15000)
	{
		//calculamos el mejor de los padres
		int mejorPadre = 0;
		for(int i = 0; i<tasasIndividuos.size();i++)
		{
			mejorPadre = (tasasIndividuos[mejorPadre] < tasasIndividuos[i])? i : mejorPadre;
		}
		//recogemos los mejores padres
		mejoresPadres = torneoBinario(tasasIndividuos,30,30);
		//generamos los hijos de los mejores padres teniendo en cuenta la cantidad de cruces calculada
		for(int i = 0; i < cantidadCruces*2; i+=2)
		{
			parHijos = operadorCruceBLX(poblacion[mejoresPadres[i]], poblacion[mejoresPadres[i+1]]);
			nuevaPoblacion.push_back(parHijos.first);
			nuevaPoblacion.push_back(parHijos.second);
		}

		
		//rellenamos la poblacion con el resto de mejores padres que no se hayan cruzado
		for(int i = cantidadCruces*2; i < poblacion.size(); i++)
		{
			nuevaPoblacion.push_back(poblacion[mejoresPadres[i]]);
			if(i == mejorPadre)
				mejorPadrePerdido = false;
			
		}

		//MUTAMOS
		int cantidadMutaciones = probabilidadMutacion * poblacion.size() * poblacion[0].size();
		cantidadMutaciones++;
		int individuoAMutar = rand()%poblacion.size();
		int genAMutar = rand()%poblacion[0].size();
		nuevaPoblacion[individuoAMutar] = generarVecino(nuevaPoblacion[individuoAMutar], genAMutar);
		int peorIndividuo = 0;
		//volvemos a realizar las evaluaciones de la nueva poblacion
		for(int i = 0; i<poblacion.size(); i++)
		{
			tasasIndividuosNuevaPoblacion.push_back(KNNClasif(indicesMatrizDatos,indicesMatrizDatos,nuevaPoblacion[i]));
			cantidadEvaluaciones++;
			//calculamos el peor individuo
			peorIndividuo = (tasasIndividuosNuevaPoblacion[peorIndividuo] < tasasIndividuosNuevaPoblacion[i])? i : peorIndividuo;
		}

		//cogemos el mejor de la poblacion anterior y lo sustituimos por el peor de la nueva poblacion
		if(mejorPadrePerdido)
		{
			nuevaPoblacion[peorIndividuo] = poblacion[mejorPadre];
		}
		
		//actualizamos la nueva poblacion
		poblacion = nuevaPoblacion;
		tasasIndividuos = tasasIndividuosNuevaPoblacion;

		nuevaPoblacion.clear();
		tasasIndividuosNuevaPoblacion.clear();
		mejorPadrePerdido = true;
		mejoresPadres.clear();

		// cout << cantidadEvaluaciones << endl;
	}

	
	int mejorPadre = 0;
	for(int i = 0; i<tasasIndividuos.size();i++)
	{
		mejorPadre = (tasasIndividuos[mejorPadre] < tasasIndividuos[i])? i : mejorPadre;
	}

	vector<float> mejorIndividuo = poblacion[mejorPadre];

	return mejorIndividuo;

}

float Algoritmo::fRand(float fMin, float fMax)
{
    float f = (float)rand() / RAND_MAX;
    return fMin + f * (fMax - fMin);
}


pair<vector<float>,vector<float>> Algoritmo::operadorCruceBLX(vector<float> & P1, vector<float> & P2)
{
	//calculamos maximo y minimo del padre p1
	float *maxP1 = max_element(&P1[0],&(P1[0])+P1.size());
	float *minP1 = min_element(&P1[0],&(P1[0])+P1.size());

	//calculamos maximo y minumo del padre p2
	float *maxP2 = max_element(&P2[0],&(P2[0])+P2.size());
	float *minP2 = min_element(&P2[0],&(P2[0])+P2.size());

	//obtenemos el maximo total y el minimo total
	float MAX = (*maxP1>*maxP2)? *maxP1 : *maxP2;
	float MIN = (*minP1<*minP2)? *minP1 : *minP2;

	//iniciamos alpha
	float alpha = 0.3;

	//iniciamos el valor I
	float I = MAX - MIN;

	//cout << MIN-I*alpha << " " << MAX+I*alpha << endl;

	//creamos los hijos
	vector<float> hijo1, hijo2;
	float random = 0;
	for(int i = 0; i<P1.size(); i++)
	{
		random = fRand(MIN-I*alpha,MAX+I*alpha);
		random = (random > 1)? 1 : random;//truncamos en caso de que se pase de 1
		random = (random < 0)? 0 : random;//truncamos en caso de que se pase de 0
		hijo1.push_back(random);
		random = fRand(MIN-I*alpha,MAX+I*alpha);
		random = (random > 1)? 1 : random;//truncamos en caso de que se pase de 1
		random = (random < 0)? 0 : random;//truncamos en caso de que se pase de 0
		hijo2.push_back(random);
	}

	//creamos los hijos
	pair<vector<float>, vector<float>> hijos(hijo1,hijo2);

	return hijos;
}

vector<int> Algoritmo::torneoBinario(vector<float> & tasasPoblacion, int cantidad, int tamanioPoblacion)
{

	vector<int> padres;
	int padre1 = -1;
	float tasaIndividuo2,tasaIndividuo1;
	int random1,random2;
	for(int i = 0; i<cantidad; i++)
	{
		//cogemos el individuo1
		random1 = rand()%(tamanioPoblacion);
		tasaIndividuo1 = tasasPoblacion[random1];

		//cogemos individuo2
		random2 = rand()%(tamanioPoblacion);
		//evitamos que sea el mismo que el individuo1
		while(random1 == random2)
			random2 = rand()%(tamanioPoblacion);
		tasaIndividuo2 = tasasPoblacion[random2];

		//tomamos el primer padre comparando las tasas de los dos individuos elegidos aleatoriamente
		padre1 = (tasaIndividuo2 > tasaIndividuo1)? random2 : random1;

		padres.push_back(padre1);
	}

 	return padres;
}

void Algoritmo::operadorCruceCA(vector<float> & P1, vector<float> & P2, vector<float> & hijo)
{
	for(int i = 0; i<P1.size(); i++)
		hijo.push_back((P1[i]+P2[i]) / 2);
}

vector<float> Algoritmo::AGGCA(vector<int> indicesMatrizDatos)
{
	float probabilidadMutacion = 0.001;
	float probabilidadCruce = 0.7;
	int cantidadEvaluaciones = 0;
	//inicializamos el vector de pesos con valores aleatorios en el intervalo [0,1]
	float num = 0;
	vector<vector<float>> poblacion;
	vector<float> individuo;


	for(int i = 0; i < 30; i++)
	{
		for(int e = 0; e < matrizDatos[0].size(); e++)
		{
			num=1+rand()%(1001-1);
			num = num/1000.0;
			individuo.push_back(num);
		}
		poblacion.push_back(individuo);
		individuo.clear();
	}


	// evaluamos cada uno de los individuos de la poblacion
	// y almacenamos cada una de las tasas en el vector
	vector<float> tasasIndividuos;
	for(int i = 0; i<poblacion.size(); i++)
	{
		tasasIndividuos.push_back(KNNClasif(indicesMatrizDatos,indicesMatrizDatos,poblacion[i]));
		cantidadEvaluaciones++;
	}

	//cout << " dedo "  << poblacion.size()<< " " << tasasIndividuos.size() << endl;
	//vector con los indices de los mejores padres calculados con el torneo binario
	vector<int> mejoresPadres;
	vector<float> hijo;
	int cantidadCruces = 60/2*probabilidadCruce;
	vector<vector<float>> nuevaPoblacion;
	vector<float> tasasIndividuosNuevaPoblacion;
	bool mejorPadrePerdido = true;
	int s = 0;

	while(cantidadEvaluaciones < 15000)
	{
		//calculamos el mejor de los padres
		int mejorPadre = 0;
		for(int i = 0; i<tasasIndividuos.size();i++)
		{
			mejorPadre = (tasasIndividuos[mejorPadre] < tasasIndividuos[i])? i : mejorPadre;
		}
		//recogemos los mejores padres
		mejoresPadres = torneoBinario(tasasIndividuos,60,30);
		//cout << "mejores padres: " << mejoresPadres.size() << endl;

		// for(int i = 0; i<mejoresPadres.size();i++)
		// 	cout << "MEJOR PAPA " << i << ": " << mejoresPadres[i] << endl;
		//generamos los hijos de los mejores padres teniendo en cuenta la cantidad de cruces calculada
		for(int i = 0; i < cantidadCruces*2; i+=2)
		{
			vector<float> i1 = poblacion[mejoresPadres[i]];
			vector<float> i2 = poblacion[mejoresPadres[i+1]];
			operadorCruceCA(i1, i2, hijo);
			nuevaPoblacion.push_back(hijo);
			hijo.clear();
		}

		// cout << "TAMANIO NUEVA POBLACION: " << nuevaPoblacion.size() << endl;

		
		//rellenamos la poblacion con el resto de mejores padres que no se hayan cruzado
		//j es el indice por el que nos habiamos quedado en el vector de mejores padres
		int j = cantidadCruces*2;
		for(int i = nuevaPoblacion.size(); i < poblacion.size(); i++)
		{
			nuevaPoblacion.push_back(poblacion[mejoresPadres[j]]);
			if(i == mejorPadre)
				mejorPadrePerdido = false;
			j++;
		}

		// cout << "TAMANIO NUEVA POBLACION: " << nuevaPoblacion.size() << endl;

		//MUTAMOS
		int cantidadMutaciones = probabilidadMutacion * poblacion.size() * poblacion[0].size();
		cantidadMutaciones++;
		int individuoAMutar = rand()%poblacion.size();
		int genAMutar = rand()%poblacion[0].size();
		nuevaPoblacion[individuoAMutar] = generarVecino(nuevaPoblacion[individuoAMutar], genAMutar);
		int peorIndividuo = 0;
		//volvemos a realizar las evaluaciones de la nueva poblacion
		for(int i = 0; i<poblacion.size(); i++)
		{
			tasasIndividuosNuevaPoblacion.push_back(KNNClasif(indicesMatrizDatos,indicesMatrizDatos,nuevaPoblacion[i]));
			cantidadEvaluaciones++;
			//calculamos el peor individuo
			peorIndividuo = (tasasIndividuosNuevaPoblacion[peorIndividuo] < tasasIndividuosNuevaPoblacion[i])? i : peorIndividuo;
		}

		//cogemos el mejor de la poblacion anterior y lo sustituimos por el peor de la nueva poblacion
		if(mejorPadrePerdido)
		{
			nuevaPoblacion[peorIndividuo] = poblacion[mejorPadre];
			tasasIndividuosNuevaPoblacion[peorIndividuo] = tasasIndividuos[mejorPadre];
		}
		
		//actualizamos la nueva poblacion
		poblacion = nuevaPoblacion;
		tasasIndividuos = tasasIndividuosNuevaPoblacion;

		nuevaPoblacion.clear();
		tasasIndividuosNuevaPoblacion.clear();
		mejorPadrePerdido = true;
		mejoresPadres.clear();

		//cout << cantidadEvaluaciones << endl;
	}

	
	int mejorPadre = 0;
	for(int i = 0; i<tasasIndividuos.size();i++)
	{
		mejorPadre = (tasasIndividuos[mejorPadre] < tasasIndividuos[i])? i : mejorPadre;
	}

	vector<float> mejorIndividuo = poblacion[mejorPadre];
	// vector<float> mejorIndividuo = poblacion[0];

	return mejorIndividuo;

}

vector<float> Algoritmo::AGEBLX(vector<int> indicesMatrizDatos)
{
	float probabilidadMutacion = 0.001;
	float probabilidadCruce = 0.7;
	int cantidadEvaluaciones = 0;
	//inicializamos el vector de pesos con valores aleatorios en el intervalo [0,1]
	float num = 0;
	vector<vector<float>> poblacion;
	vector<float> individuo;


	for(int i = 0; i < 30; i++)
	{
		for(int e = 0; e < matrizDatos[0].size(); e++)
		{
			num=1+rand()%(1001-1);
			num = num/1000.0;
			individuo.push_back(num);
		}
		poblacion.push_back(individuo);
		individuo.clear();
	}

	// evaluamos cada uno de los individuos de la poblacion
	// y almacenamos cada una de las tasas en el vector
	vector<float> tasasIndividuos;
	for(int i = 0; i<poblacion.size(); i++)
	{
		tasasIndividuos.push_back(KNNClasif(indicesMatrizDatos,indicesMatrizDatos,poblacion[i]));
		cantidadEvaluaciones++;
	}

	//cout << " dedo "  << poblacion.size()<< " " << tasasIndividuos.size() << endl;
	//vector con los indices de los mejores padres calculados con el torneo binario
	vector<int> mejoresPadres;
	int s = 0;

	while(cantidadEvaluaciones < 15000)
	{
		//calculamos los 2 peores padres
		int peorPadre = 0;
		int peorPadre2 = 0;
		for(int i = 0; i<tasasIndividuos.size();i++)
		{
			if(tasasIndividuos[peorPadre] > tasasIndividuos[i] && tasasIndividuos[peorPadre2] > tasasIndividuos[i])
				peorPadre = i;
			else if(tasasIndividuos[peorPadre] < tasasIndividuos[i] && tasasIndividuos[peorPadre2] > tasasIndividuos[i])
				peorPadre2 = i;
		}
		//recogemos los 2 mejores padres
		mejoresPadres = torneoBinario(tasasIndividuos,2,30); 
		// cout << "mejores padres: " << mejoresPadres.size() << endl;

		// for(int i = 0; i<mejoresPadres.size();i++)
		// 	cout << "MEJOR PAPA " << i << ": " << mejoresPadres[i] << endl;
		//generamos un único cruce
		pair<vector<float>, vector<float>> hijos;
			vector<float> i1 = poblacion[mejoresPadres[0]];
			vector<float> i2 = poblacion[mejoresPadres[1]];
			hijos=operadorCruceBLX(i1, i2);

		// cout << "TAMANIO NUEVA POBLACION: " << nuevaPoblacion.size() << endl;

		//calculamos las tasas de los dos nuevos hijos
		float tasaHijo1 = KNNClasif(indicesMatrizDatos,indicesMatrizDatos,hijos.first);
		float tasaHijo2 = KNNClasif(indicesMatrizDatos,indicesMatrizDatos,hijos.second);
		//en caso de que los hijos sean mejor que los peores padres los sustituimos
		if(tasaHijo1 > tasaHijo2)
		{
			if(tasaHijo1 > tasasIndividuos[peorPadre])
			{
				poblacion[peorPadre] = hijos.first;
				tasasIndividuos[peorPadre] = tasaHijo1;
			}
			if(tasaHijo2 > tasasIndividuos[peorPadre2])
			{
				poblacion[peorPadre2] = hijos.second;
				tasasIndividuos[peorPadre2] = tasaHijo2;
			}
		}
		else
		{
			if(tasaHijo2 > tasasIndividuos[peorPadre])
			{
				poblacion[peorPadre] = hijos.first;
				tasasIndividuos[peorPadre] = tasaHijo2;
			}
			if(tasaHijo1 > tasasIndividuos[peorPadre2])
			{
				poblacion[peorPadre2] = hijos.second;
				tasasIndividuos[peorPadre2] = tasaHijo1;
			}
		}

		 //cout << "TAMANIO NUEVA POBLACION: " << poblacion.size() << endl;

		//MUTAMOS
		int cantidadMutaciones = probabilidadMutacion * poblacion.size() * poblacion[0].size();
		cantidadMutaciones++;
		int peorIndividuo = 0;
		//volvemos a realizar las evaluaciones de los individuos mutados
		for(int i = 0; i<cantidadMutaciones; i++)
		{
			int individuoAMutar = rand()%poblacion.size();
			int genAMutar = rand()%poblacion[0].size();	
			poblacion[individuoAMutar] = generarVecino(poblacion[individuoAMutar], genAMutar);
			tasasIndividuos[individuoAMutar] = KNNClasif(indicesMatrizDatos,indicesMatrizDatos,poblacion[individuoAMutar]);
			cantidadEvaluaciones++;
		}

		// cout << cantidadEvaluaciones << endl;
	}

	
	int mejorPadre = 0;
	for(int i = 0; i<tasasIndividuos.size();i++)
	{
		mejorPadre = (tasasIndividuos[mejorPadre] < tasasIndividuos[i])? i : mejorPadre;
	}

	vector<float> mejorIndividuo = poblacion[mejorPadre];

	return mejorIndividuo;

}

vector<float> Algoritmo::AGECA(vector<int> indicesMatrizDatos)
{
	float probabilidadMutacion = 0.001;
	float probabilidadCruce = 0.7;
	int cantidadEvaluaciones = 0;
	//inicializamos el vector de pesos con valores aleatorios en el intervalo [0,1]
	float num = 0;
	vector<vector<float>> poblacion;
	vector<float> individuo;

	for(int i = 0; i < 30; i++)
	{
		for(int e = 0; e < matrizDatos[0].size(); e++)
		{
			num=1+rand()%(1001-1);
			num = num/1000.0;
			individuo.push_back(num);
		}
		poblacion.push_back(individuo);
		individuo.clear();
	}

	// evaluamos cada uno de los individuos de la poblacion
	// y almacenamos cada una de las tasas en el vector
	vector<float> tasasIndividuos;
	for(int i = 0; i<poblacion.size(); i++)
	{
		tasasIndividuos.push_back(KNNClasif(indicesMatrizDatos,indicesMatrizDatos,poblacion[i]));
		cantidadEvaluaciones++;
	}

	//cout << " dedo "  << poblacion.size()<< " " << tasasIndividuos.size() << endl;
	//vector con los indices de los mejores padres calculados con el torneo binario
	vector<int> mejoresPadres;
	int s = 0;

	while(cantidadEvaluaciones < 15000)
	{
		//calculamos los 2 peores padres
		int peorPadre = 0;
		int peorPadre2 = 0;
		for(int i = 0; i<tasasIndividuos.size();i++)
		{
			if(tasasIndividuos[peorPadre] > tasasIndividuos[i] && tasasIndividuos[peorPadre2] > tasasIndividuos[i])
				peorPadre = i;
			else if(tasasIndividuos[peorPadre] < tasasIndividuos[i] && tasasIndividuos[peorPadre2] > tasasIndividuos[i])
				peorPadre2 = i;
		}
		//recogemos los 2 mejores padres
		mejoresPadres = torneoBinario(tasasIndividuos,4,30); 
		// cout << "mejores padres: " << mejoresPadres.size() << endl;

		// for(int i = 0; i<mejoresPadres.size();i++)
		// 	cout << "MEJOR PAPA " << i << ": " << mejoresPadres[i] << endl;

		//generamos dos cruce
		vector<float> hijo1, hijo2;
		vector<float> i1 = poblacion[mejoresPadres[0]];
		vector<float> i2 = poblacion[mejoresPadres[1]];
		vector<float> i3 = poblacion[mejoresPadres[2]];
		vector<float> i4 = poblacion[mejoresPadres[3]];
		operadorCruceCA(i1, i2, hijo1);
		operadorCruceCA(i3, i4, hijo2);

		// cout << "TAMANIO NUEVA POBLACION: " << nuevaPoblacion.size() << endl;

		//calculamos las tasas de los dos nuevos hijos
		float tasaHijo1 = KNNClasif(indicesMatrizDatos,indicesMatrizDatos,hijo1);
		float tasaHijo2 = KNNClasif(indicesMatrizDatos,indicesMatrizDatos,hijo2);
		//en caso de que los hijos sean mejor que los peores padres los sustituimos
		if(tasaHijo1 > tasaHijo2)
		{
			if(tasaHijo1 > tasasIndividuos[peorPadre])
			{
				poblacion[peorPadre] = hijo1;
				tasasIndividuos[peorPadre] = tasaHijo1;
			}
			if(tasaHijo2 > tasasIndividuos[peorPadre2])
			{
				poblacion[peorPadre2] = hijo2;
				tasasIndividuos[peorPadre2] = tasaHijo2;
			}
		}
		else
		{
			if(tasaHijo2 > tasasIndividuos[peorPadre])
			{
				poblacion[peorPadre] = hijo1;
				tasasIndividuos[peorPadre] = tasaHijo2;
			}
			if(tasaHijo1 > tasasIndividuos[peorPadre2])
			{
				poblacion[peorPadre2] = hijo2;
				tasasIndividuos[peorPadre2] = tasaHijo1;
			}
		}

		 //cout << "TAMANIO NUEVA POBLACION: " << poblacion.size() << endl;

		//MUTAMOS
		int cantidadMutaciones = probabilidadMutacion * poblacion.size() * poblacion[0].size();
		cantidadMutaciones++;
		int peorIndividuo = 0;
		//volvemos a realizar las evaluaciones de los individuos mutados
		for(int i = 0; i<cantidadMutaciones; i++)
		{
			int individuoAMutar = rand()%poblacion.size();
			int genAMutar = rand()%poblacion[0].size();	
			poblacion[individuoAMutar] = generarVecino(poblacion[individuoAMutar], genAMutar);
			tasasIndividuos[individuoAMutar] = KNNClasif(indicesMatrizDatos,indicesMatrizDatos,poblacion[individuoAMutar]);
			cantidadEvaluaciones++;
		}

		// cout << cantidadEvaluaciones << endl;
	}

	
	int mejorPadre = 0;
	for(int i = 0; i<tasasIndividuos.size();i++)
	{
		mejorPadre = (tasasIndividuos[mejorPadre] < tasasIndividuos[i])? i : mejorPadre;
	}

	vector<float> mejorIndividuo = poblacion[mejorPadre];

	return mejorIndividuo;

}

vector<float> Algoritmo::AMCA1(vector<int> indicesMatrizDatos)
{
	float probabilidadMutacion = 0.001;
	float probabilidadCruce = 0.7;
	int cantidadEvaluaciones = 0;
	//inicializamos el vector de pesos con valores aleatorios en el intervalo [0,1]
	float num = 0;
	vector<vector<float>> poblacion;
	vector<float> individuo;


	for(int i = 0; i < 10; i++)
	{
		for(int e = 0; e < matrizDatos[0].size(); e++)
		{
			num=1+rand()%(1001-1);
			num = num/1000.0;
			individuo.push_back(num);
		}
		poblacion.push_back(individuo);
		individuo.clear();
	}

	//cout << "HOLA: " << poblacion.size() << " " << poblacion[poblacion.size()-1].size() << endl;


	// evaluamos cada uno de los individuos de la poblacion
	// y almacenamos cada una de las tasas en el vector
	vector<float> tasasIndividuos;
	for(int i = 0; i<poblacion.size(); i++)
	{
		tasasIndividuos.push_back(KNNClasif(indicesMatrizDatos,indicesMatrizDatos,poblacion[i]));
		cantidadEvaluaciones++;
	}

	//cout << " dedo "  << poblacion.size()<< " " << tasasIndividuos.size() << endl;
	//vector con los indices de los mejores padres calculados con el torneo binario
	vector<int> mejoresPadres;
	vector<float> hijo;
	int cantidadCruces = 20/2*probabilidadCruce;
	vector<vector<float>> nuevaPoblacion;
	vector<float> tasasIndividuosNuevaPoblacion;
	bool mejorPadrePerdido = true;
	int s = 0;
	int generacion = 0;
	pair<vector<float>,float> solucionLocalSearch;

	while(cantidadEvaluaciones < 15000)
	{
		
		//aumentamos el numero de generación
		generacion++;
		//calculamos el mejor de los padres
		int mejorPadre = 0;
		for(int i = 0; i<tasasIndividuos.size();i++)
		{
			mejorPadre = (tasasIndividuos[mejorPadre] < tasasIndividuos[i])? i : mejorPadre;
		}
		//recogemos los mejores padres
		mejoresPadres = torneoBinario(tasasIndividuos,20,10);
		//cout << "mejores padres: " << mejoresPadres.size() << endl;

		// for(int i = 0; i<mejoresPadres.size();i++)
		// 	cout << "MEJOR PAPA " << i << ": " << mejoresPadres[i] << endl;
		//generamos los hijos de los mejores padres teniendo en cuenta la cantidad de cruces calculada
		for(int i = 0; i < cantidadCruces*2; i+=2)
		{
			vector<float> i1 = poblacion[mejoresPadres[i]];
			vector<float> i2 = poblacion[mejoresPadres[i+1]];
			operadorCruceCA(i1, i2, hijo);
			nuevaPoblacion.push_back(hijo);
			hijo.clear();
		}

		// cout << "TAMANIO NUEVA POBLACION: " << nuevaPoblacion.size() << endl;

		
		//rellenamos la poblacion con el resto de mejores padres que no se hayan cruzado
		//j es el indice por el que nos habiamos quedado en el vector de mejores padres
		int j = cantidadCruces*2;
		for(int i = nuevaPoblacion.size(); i < poblacion.size(); i++)
		{
			nuevaPoblacion.push_back(poblacion[mejoresPadres[j]]);
			if(i == mejorPadre)
				mejorPadrePerdido = false;
			j++;
		}

		// cout << "TAMANIO NUEVA POBLACION: " << nuevaPoblacion.size() << endl;

		//MUTAMOS
		int cantidadMutaciones = probabilidadMutacion * poblacion.size() * poblacion[0].size();
		cantidadMutaciones++;
		int individuoAMutar = rand()%poblacion.size();
		int genAMutar = rand()%poblacion[0].size();
		nuevaPoblacion[individuoAMutar] = generarVecino(nuevaPoblacion[individuoAMutar], genAMutar);
		int peorIndividuo = 0;
		//volvemos a realizar las evaluaciones de la nueva poblacion
		for(int i = 0; i<poblacion.size(); i++)
		{
			tasasIndividuosNuevaPoblacion.push_back(KNNClasif(indicesMatrizDatos,indicesMatrizDatos,nuevaPoblacion[i]));
			cantidadEvaluaciones++;
			//calculamos el peor individuo
			peorIndividuo = (tasasIndividuosNuevaPoblacion[peorIndividuo] < tasasIndividuosNuevaPoblacion[i])? i : peorIndividuo;
		}

		//cada 10 iteraciones realizamos explotación con la búsqueda local
		if(generacion == 10)
		{
			generacion = 0;
			for(int i = 0; i<poblacion.size(); i++)
			{
				//llamamos a la local search
				solucionLocalSearch = localSearch(indicesMatrizDatos,2,poblacion[i]);
				poblacion[i] = solucionLocalSearch.first;
				//aumentamos la cantidad de llamadas al 1_nn que se realizan
				//dentro de la propia busqueda local
				//que en este caso son el número de atributos de los individuos por 2, mas una llamada.
				cantidadEvaluaciones += (2*poblacion[0].size()+1);
			}
		}

		//cogemos el mejor de la poblacion anterior y lo sustituimos por el peor de la nueva poblacion
		if(mejorPadrePerdido)
		{
			nuevaPoblacion[peorIndividuo] = poblacion[mejorPadre];
			tasasIndividuosNuevaPoblacion[peorIndividuo] = tasasIndividuos[mejorPadre];
		}
		
		//actualizamos la nueva poblacion
		poblacion = nuevaPoblacion;
		tasasIndividuos = tasasIndividuosNuevaPoblacion;

		nuevaPoblacion.clear();
		tasasIndividuosNuevaPoblacion.clear();
		mejorPadrePerdido = true;
		mejoresPadres.clear();

		// cout << cantidadEvaluaciones << endl;
	}

	
	int mejorPadre = 0;
	for(int i = 0; i<tasasIndividuos.size();i++)
	{
		mejorPadre = (tasasIndividuos[mejorPadre] < tasasIndividuos[i])? i : mejorPadre;
	}

	vector<float> mejorIndividuo = poblacion[mejorPadre];
	// vector<float> mejorIndividuo = poblacion[0];

	return mejorIndividuo;

}

vector<float> Algoritmo::AMCA2(vector<int> indicesMatrizDatos)
{
	float probabilidadMutacion = 0.001;
	float probabilidadCruce = 0.7;
	float probabilidadLocalSearch = 0.1;
	int cantidadEvaluaciones = 0;
	//inicializamos el vector de pesos con valores aleatorios en el intervalo [0,1]
	float num = 0;
	vector<vector<float>> poblacion;
	vector<float> individuo;

	for(int i = 0; i < 10; i++)
	{
		for(int e = 0; e < matrizDatos[0].size(); e++)
		{
			num=1+rand()%(1001-1);
			num = num/1000.0;
			individuo.push_back(num);
		}
		poblacion.push_back(individuo);
		individuo.clear();
	}


	//nos creamos un vector con los indices de cada individuo de la poblacion
	//para usarlo al decidir a quien s ele aplica la localSearch
	vector<int> vectorPoblacion;
	for(int i = 0; i<poblacion.size(); i++)
	{
		vectorPoblacion.push_back(i);
	}


	// evaluamos cada uno de los individuos de la poblacion
	// y almacenamos cada una de las tasas en el vector
	vector<float> tasasIndividuos;
	for(int i = 0; i<poblacion.size(); i++)
	{
		tasasIndividuos.push_back(KNNClasif(indicesMatrizDatos,indicesMatrizDatos,poblacion[i]));
		cantidadEvaluaciones++;
	}

	//cout << " dedo "  << poblacion.size()<< " " << tasasIndividuos.size() << endl;
	//vector con los indices de los mejores padres calculados con el torneo binario
	vector<int> mejoresPadres;
	vector<float> hijo;
	int cantidadCruces = 20/2*probabilidadCruce;
	vector<vector<float>> nuevaPoblacion;
	vector<float> tasasIndividuosNuevaPoblacion;
	bool mejorPadrePerdido = true;
	int s = 0;
	int individuoAleatorio = 0;
	int generacion = 0;
	int cantidadIndividuosLocalSearch = probabilidadLocalSearch * poblacion.size();
	vector<int> individuosParaLocalSearch;
	pair<vector<float>,float> solucionLocalSearch;


	while(cantidadEvaluaciones < 15000)
	{
		//aumentamos el numero de generación
		generacion++;
		//calculamos el mejor de los padres
		int mejorPadre = 0;
		for(int i = 0; i<tasasIndividuos.size();i++)
		{
			mejorPadre = (tasasIndividuos[mejorPadre] < tasasIndividuos[i])? i : mejorPadre;
		}
		//recogemos los mejores padres
		mejoresPadres = torneoBinario(tasasIndividuos,20,10);
		//cout << "mejores padres: " << mejoresPadres.size() << endl;

		// for(int i = 0; i<mejoresPadres.size();i++)
		// 	cout << "MEJOR PAPA " << i << ": " << mejoresPadres[i] << endl;
		//generamos los hijos de los mejores padres teniendo en cuenta la cantidad de cruces calculada
		for(int i = 0; i < cantidadCruces*2; i+=2)
		{
			vector<float> i1 = poblacion[mejoresPadres[i]];
			vector<float> i2 = poblacion[mejoresPadres[i+1]];
			operadorCruceCA(i1, i2, hijo);
			nuevaPoblacion.push_back(hijo);
			hijo.clear();
		}

		// cout << "TAMANIO NUEVA POBLACION: " << nuevaPoblacion.size() << endl;

		
		//rellenamos la poblacion con el resto de mejores padres que no se hayan cruzado
		//j es el indice por el que nos habiamos quedado en el vector de mejores padres
		int j = cantidadCruces*2;
		for(int i = nuevaPoblacion.size(); i < poblacion.size(); i++)
		{
			nuevaPoblacion.push_back(poblacion[mejoresPadres[j]]);
			if(i == mejorPadre)
				mejorPadrePerdido = false;
			j++;
		}

		// cout << "TAMANIO NUEVA POBLACION: " << nuevaPoblacion.size() << endl;

		//MUTAMOS
		int cantidadMutaciones = probabilidadMutacion * poblacion.size() * poblacion[0].size();
		cantidadMutaciones++;
		int individuoAMutar = rand()%poblacion.size();
		int genAMutar = rand()%poblacion[0].size();
		nuevaPoblacion[individuoAMutar] = generarVecino(nuevaPoblacion[individuoAMutar], genAMutar);
		int peorIndividuo = 0;
		//volvemos a realizar las evaluaciones de la nueva poblacion
		for(int i = 0; i<poblacion.size(); i++)
		{
			tasasIndividuosNuevaPoblacion.push_back(KNNClasif(indicesMatrizDatos,indicesMatrizDatos,nuevaPoblacion[i]));
			cantidadEvaluaciones++;
			//calculamos el peor individuo
			peorIndividuo = (tasasIndividuosNuevaPoblacion[peorIndividuo] < tasasIndividuosNuevaPoblacion[i])? i : peorIndividuo;
		}

		//cada 10 iteraciones realizamos explotación con la búsqueda local
		if(generacion == 10)
		{
			random_shuffle(vectorPoblacion.begin(),vectorPoblacion.end());
			generacion = 0;
			for(int i = 0; i<cantidadIndividuosLocalSearch; i++)
			{
				//llamamos a la local search
				solucionLocalSearch = localSearch(indicesMatrizDatos,2,poblacion[vectorPoblacion[i]]);
				poblacion[vectorPoblacion[i]] = solucionLocalSearch.first;
				//aumentamos la cantidad de llamadas al 1_nn que se realizan
				//dentro de la propia busqueda local
				//que en este caso son el número de atributos de los individuos por 2, mas una llamada.
				cantidadEvaluaciones += (2*poblacion[0].size()+1);
			}
		}

		//cogemos el mejor de la poblacion anterior y lo sustituimos por el peor de la nueva poblacion
		if(mejorPadrePerdido)
		{
			nuevaPoblacion[peorIndividuo] = poblacion[mejorPadre];
			tasasIndividuosNuevaPoblacion[peorIndividuo] = tasasIndividuos[mejorPadre];
		}
		
		//actualizamos la nueva poblacion
		poblacion = nuevaPoblacion;
		tasasIndividuos = tasasIndividuosNuevaPoblacion;

		nuevaPoblacion.clear();
		tasasIndividuosNuevaPoblacion.clear();
		mejorPadrePerdido = true;
		mejoresPadres.clear();

		// cout << cantidadEvaluaciones << endl;
	}

	
	int mejorPadre = 0;
	for(int i = 0; i<tasasIndividuos.size();i++)
	{
		mejorPadre = (tasasIndividuos[mejorPadre] < tasasIndividuos[i])? i : mejorPadre;
	}

	vector<float> mejorIndividuo = poblacion[mejorPadre];
	// vector<float> mejorIndividuo = poblacion[0];

	return mejorIndividuo;

}


vector<float> Algoritmo::AMCA3(vector<int> indicesMatrizDatos)
{
	float probabilidadMutacion = 0.001;
	float probabilidadCruce = 0.7;
	float probabilidadLocalSearch = 0.1;
	int cantidadEvaluaciones = 0;

	set<pair<float,int>> poblacionOrdenadaPorTasas;
	//inicializamos el vector de pesos con valores aleatorios en el intervalo [0,1]
	float num = 0;
	vector<vector<float>> poblacion;
	vector<float> individuo;

	for(int i = 0; i < 10; i++)
	{
		for(int e = 0; e < matrizDatos[0].size(); e++)
		{
			num=1+rand()%(1001-1);
			num = num/1000.0;
			individuo.push_back(num);
		}
		poblacion.push_back(individuo);
		individuo.clear();
	}


	//nos creamos un vector con los indices de cada individuo de la poblacion
	//para usarlo al decidir a quien s ele aplica la localSearch
	vector<int> vectorPoblacion;
	for(int i = 0; i<poblacion.size(); i++)
	{
		vectorPoblacion.push_back(i);
	}


	// evaluamos cada uno de los individuos de la poblacion
	// y almacenamos cada una de las tasas en el vector
	vector<float> tasasIndividuos;
	for(int i = 0; i<poblacion.size(); i++)
	{
		tasasIndividuos.push_back(KNNClasif(indicesMatrizDatos,indicesMatrizDatos,poblacion[i]));
		cantidadEvaluaciones++;
	}

	//cout << " dedo "  << poblacion.size()<< " " << tasasIndividuos.size() << endl;
	//vector con los indices de los mejores padres calculados con el torneo binario
	vector<int> mejoresPadres;
	vector<float> hijo;
	int cantidadCruces = 20/2*probabilidadCruce;
	vector<vector<float>> nuevaPoblacion;
	vector<float> tasasIndividuosNuevaPoblacion;
	bool mejorPadrePerdido = true;
	int s = 0;
	int individuoAleatorio = 0;
	int generacion = 0;
	int cantidadIndividuosLocalSearch = probabilidadLocalSearch * poblacion.size();
	vector<int> individuosParaLocalSearch;
	pair<vector<float>,int> solucionLocalSearch;


	while(cantidadEvaluaciones < 15000)
	{
		
		//aumentamos el numero de generación
		generacion++;
		//calculamos el mejor de los padres
		int mejorPadre = 0;
		for(int i = 0; i<tasasIndividuos.size();i++)
		{
			mejorPadre = (tasasIndividuos[mejorPadre] < tasasIndividuos[i])? i : mejorPadre;
		}
		//recogemos los mejores padres
		mejoresPadres = torneoBinario(tasasIndividuos,20,10);
		//cout << "mejores padres: " << mejoresPadres.size() << endl;

		// for(int i = 0; i<mejoresPadres.size();i++)
		// 	cout << "MEJOR PAPA " << i << ": " << mejoresPadres[i] << endl;
		//generamos los hijos de los mejores padres teniendo en cuenta la cantidad de cruces calculada
		for(int i = 0; i < cantidadCruces*2; i+=2)
		{
			vector<float> i1 = poblacion[mejoresPadres[i]];
			vector<float> i2 = poblacion[mejoresPadres[i+1]];
			operadorCruceCA(i1, i2, hijo);
			nuevaPoblacion.push_back(hijo);
			hijo.clear();
		}

		// cout << "TAMANIO NUEVA POBLACION: " << nuevaPoblacion.size() << endl;

		
		//rellenamos la poblacion con el resto de mejores padres que no se hayan cruzado
		//j es el indice por el que nos habiamos quedado en el vector de mejores padres
		int j = cantidadCruces*2;
		for(int i = nuevaPoblacion.size(); i < poblacion.size(); i++)
		{
			nuevaPoblacion.push_back(poblacion[mejoresPadres[j]]);
			if(i == mejorPadre)
				mejorPadrePerdido = false;
			j++;
		}

		// cout << "TAMANIO NUEVA POBLACION: " << nuevaPoblacion.size() << endl;

		//MUTAMOS
		int cantidadMutaciones = probabilidadMutacion * poblacion.size() * poblacion[0].size();
		cantidadMutaciones++;
		int individuoAMutar = rand()%poblacion.size();
		int genAMutar = rand()%poblacion[0].size();
		nuevaPoblacion[individuoAMutar] = generarVecino(nuevaPoblacion[individuoAMutar], genAMutar);
		int peorIndividuo = 0;
		pair<float,int> tasa_indice;
		float tasa = 0;
		//volvemos a realizar las evaluaciones de la nueva poblacion
		for(int i = 0; i<poblacion.size(); i++)
		{
			cantidadEvaluaciones++;
			tasa = KNNClasif(indicesMatrizDatos,indicesMatrizDatos,nuevaPoblacion[i]);
			tasasIndividuosNuevaPoblacion.push_back(tasa);
			tasa_indice.first = tasa;
			tasa_indice.second = i;
			//ordenamos en el set los individuos por tasas
			poblacionOrdenadaPorTasas.insert(tasa_indice);
			//calculamos el peor individuo
			peorIndividuo = (tasasIndividuosNuevaPoblacion[peorIndividuo] < tasasIndividuosNuevaPoblacion[i])? i : peorIndividuo;
		}

		//cada 10 iteraciones realizamos explotación con la búsqueda local
		set<pair<float,int>>::iterator it = poblacionOrdenadaPorTasas.end();
		it--;
		if(generacion == 10)
		{
			generacion = 0;
			for(int i = 0; i<cantidadIndividuosLocalSearch; i++)
			{
				//llamamos a la local search
				solucionLocalSearch = localSearch(indicesMatrizDatos,2,poblacion[it->second]);
				poblacion[it->second] = solucionLocalSearch.first;
				it--;
				//aumentamos la cantidad de llamadas al 1_nn que se realizan
				//dentro de la propia busqueda local
				//que en este caso son el número de atributos de los individuos por 2, mas una llamada.
				cantidadEvaluaciones += (2*poblacion[0].size()+1);
			}
		}

		//cogemos el mejor de la poblacion anterior y lo sustituimos por el peor de la nueva poblacion
		if(mejorPadrePerdido)
		{
			nuevaPoblacion[peorIndividuo] = poblacion[mejorPadre];
			tasasIndividuosNuevaPoblacion[peorIndividuo] = tasasIndividuos[mejorPadre];
		}
		
		//actualizamos la nueva poblacion
		poblacion = nuevaPoblacion;
		tasasIndividuos = tasasIndividuosNuevaPoblacion;

		nuevaPoblacion.clear();
		tasasIndividuosNuevaPoblacion.clear();
		mejorPadrePerdido = true;
		mejoresPadres.clear();

		// cout << cantidadEvaluaciones << endl;
	}

	
	int mejorPadre = 0;
	for(int i = 0; i<tasasIndividuos.size();i++)
	{
		mejorPadre = (tasasIndividuos[mejorPadre] < tasasIndividuos[i])? i : mejorPadre;
	}

	vector<float> mejorIndividuo = poblacion[mejorPadre];
	// vector<float> mejorIndividuo = poblacion[0];

	return mejorIndividuo;

}
/*
 * algoritmo_carros.c
 * 
 * 
 */


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

typedef struct car{
	
	short int length;
	short int direction;
	short int position;
	long long int speed;
	long int time;
} Car;

void times_in_grid(int gridPos, Car c, long int seconds_since_sim_start, double *time_arrival, double *time_leaving)
{
	// Presume-se que a posição do carro é o quadrado mais pro seu fundo, de forma que ele ocupa
	// as posições [position, position + length]
	if (c.speed > 0) // Os calculos sao meio diferentes
	{
		int distToGrid 	= gridPos - c.position;
		distToGrid 		-= c.length - 1;
		
		double timeIn = seconds_since_sim_start;
		*time_arrival = timeIn + (float)distToGrid / c.speed;
		*time_leaving = *time_arrival + ((float)(c.length + 1)/(float)c.speed); 
	}
	
	else if (c.speed < 0) // Pra velocidades positivas e negativas, ja q a posicao é sempre a "bunda" do carro
	{
		int distToGrid 	= c.position - (gridPos + 1);
		
		double timeIn = seconds_since_sim_start;
		*time_arrival = timeIn - (float)distToGrid / c.speed;
		*time_leaving = *time_arrival - ((float)(c.length + 1)/(float)c.speed); 
	}
	
	else // velocidade = 0
	{
		*time_arrival = -1;
		*time_leaving = -1; // Vai ter codigo para, se o tempo for negativo, mandar chamar a ambulancia
	}
}

void accelerate(Car *c)
{
	if (c->speed < 0)
	{
		c->speed -= 2;
	}
	
	else // Velocidade nunca vai ser 0; accelerate só é chamada depois de times_in_grid, que já checa isso
	{
		c->speed += 2; // Mas se for, aceleramos de qq forma
	}
}

void read_client_message(char* msg, short int* len, short int* dir, short int* pos, long long int* spd)
{
	// Sabemos que a estrutura da mensagem é "%d;%d;%d;%d;%d", então os dois primeiros caracteres já terão sido
	// lidos quando esta função for chamada. Lemos então a partir de i = 2.
	int ind_0 = 0;
	int ind_1 = 0;
	long long int vals[4];
	for (int i = 2; msg[i] != '\0'; i++)
	{
		char substr[20];
		if (msg[i] != ';')
		{
			substr[ind_0] = msg[i];
			ind_0++;
		}
		else
		{
			substr[ind_0] = '\0';
			ind_0 = 0;
			vals[ind_1] = atoi(substr);
			ind_1++;
		}
	}
	
	*len = vals[0];
	*dir = vals[1];
	*pos = vals[2];
	*spd = vals[3];
}
int main(int argc, char **argv)
{
	// Digamos que os carros se movimentam discretamente; a cada unidade de tempo qualquer,
	// eles andam uma distância igual a Vel. E só pra ser extra seguro, o servidor manda os
	// carros aumentarem a velocidade de tal forma que ***nunca existe mais de um carro em 
	// qualquer um dos espaços do cruzamento ao mesmo tempo***.
	
	// Suponhamos que o grid seja as posições 23 e 24 de ambas as ruas.
	
	time_t startTime;
	startTime = time(NULL);
	
	Car horizontal[] = {
		{3, 1, 23, -3},
		{3, 1, 15, 3},
		{3, 1, 3, 3},
		{3, 1, 3, 3},
		{3, 1, 3, 3},
		{3, 1, 3, 3},
		{3, 1, 3, 3},
		};
	Car vertical[] = {
		{2, 0, 3, 3},
		{3, 0, 3, 3},
		{3, 0, 3, 3},
		{3, 0, 3, 3},
		{3, 0, 3, 3},
		{3, 0, 3, 3},
		{3, 0, 3, 3},
		};
/*	Ex.: Movimento de horz[0]
t0:	---------------GG------XXX----
	012345678901234567890123456789
t1:	---------------GG-XXX---------
	012345678901234567890123456789
t2:	---------------XXX------------
	012345678901234567890123456789
t3:	---------------GG----XXX------
* 
* 
* 	Ex2: Movimento de horz[1]
	012345678901234567890123456789
t0:	---------------XXX-----GG-----
	012345678901234567890123456789
t1:	------------------XXX--GG-----
	012345678901234567890123456789
t2:	---------------------XXXG-----
	012345678901234567890123456789
t3:	-----------------------GXXX---
	012345678901234567890123456789
*/


	double time_a_horz, time_l_horz;
	double time_a_vert, time_l_vert;

	printf("Before we have:\nhorz: ");
	for (int i = 0; i < 7; i++)
	{
		times_in_grid(15, horizontal[i], startTime - time(NULL), &time_a_horz, &time_l_horz);
		printf("[%f,%f] ", time_a_horz, time_l_horz);
	}
	printf("\nvert: ");
	for (int i = 0; i < 7; i++)
	{
		times_in_grid(15, vertical[i], startTime - time(NULL), &time_a_horz, &time_l_horz);
		printf("[%f,%f] ", time_a_horz, time_l_horz);
	}
	printf("\n\n");
	

		
	for (int i = 0; i < 7; i++)
	{
		times_in_grid(15, horizontal[i], startTime - time(NULL), &time_a_horz, &time_l_horz);
		for (int j = 0; j < 7; j++)
		{
			times_in_grid(15, vertical[j], startTime - time(NULL), &time_a_vert, &time_l_vert);
			if ((time_a_horz > time_a_vert && time_a_horz < time_l_vert) ||
				(time_l_horz > time_a_vert && time_l_horz < time_l_vert))
			{
				printf("horz car %d [%f,%f] is crashing into vert car %d [%f,%f]\n", i, time_a_horz, time_l_horz, j, time_a_vert, time_l_vert);
				accelerate(&(horizontal[i]));
				i = -1;
				break;
			}	
		}
	}
	printf("\nNow we have:\nhorz: ");
	for (int i = 0; i < 7; i++)
	{
		times_in_grid(15, horizontal[i], startTime - time(NULL), &time_a_horz, &time_l_horz);
		printf("[%f,%f] ", time_a_horz, time_l_horz);
	}
	printf("\nvert: ");
	for (int i = 0; i < 7; i++)
	{
		times_in_grid(15, vertical[i], startTime - time(NULL), &time_a_vert, &time_l_vert);
		printf("[%f,%f] ", time_a_vert, time_l_vert);
	}
	printf("\n");
	
	printf("Let's make a 'message'. ");
	char msg[20] = "2;1;23;4;5;";
	printf("da message: %s\nNow make a car get them values\n", msg);
	Car kekmobile;
	read_client_message(msg, &(kekmobile.length), &(kekmobile.direction), &(kekmobile.position), &(kekmobile.speed));
	printf("print dat kekmobile: len = %d, dir = %d, pos = %d, spd = %ld\n", kekmobile.length, kekmobile.direction, kekmobile.position, kekmobile.speed);
	
	return 0;
}


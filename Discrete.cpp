// Discrete.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <float.h>  
#include <fstream>  
#include <iostream>
#include <sstream>

using namespace std;

#define dip 10000
#define fill 10000000
#define knot_num 1000
#define test_size 240669

///function list
void read_file(struct N N[]);
void initialize_N(struct N N[]);
void initialize_Temp(struct Temp Temp[]);
void initialize_x(struct Dip_x Dip_x[]);
void initialize_y(struct Dip_y Dip_y[]);
void init_fill_x(struct Fill_x Fill_x[]);
void init_fill_y(struct Fill_y Fill_y[]);
void init_Fin(struct Fin Fin[]);
void scaling(struct N N[]);
void adjust(struct N N[]);
void adj_order(struct N N[]);
void kill_redundant(struct N N[]);
int up_num(struct N N[]);

void corp_array(struct N N[], int q);

int min_Dip_x(struct Dip_x Dip_x[], int dip_size);
int max_Dip_x(struct Dip_x Dip_x[], int dip_size);
int min_Dip_y(struct Dip_x Dip_x[], int dip_size);
int max_Dip_y(struct Dip_x Dip_x[], int dip_size);
int min_Dip_y_x(struct Dip_y Dip_y[], int dip_size);
int max_Dip_y_x(struct Dip_y Dip_y[], int dip_size);
int min_Dip_y_y(struct Dip_y Dip_y[], int dip_size);
int max_Dip_y_y(struct Dip_y Dip_y[], int dip_size);
void zero_a(double p[], int q);
void make_order(double p[], int q, int r);
void corp_knot(double p[], int q);
int knot_length(double p[], int q);

struct N {
	double x, y, z;
};

struct Temp{
	double x, y, z;
};

struct temp {
	double x, y, z;
};

struct Dip_x {
	double x, y, z;
};

struct Dip_y {
	double x, y, z;
};

struct Fill_x {
	double x, y, z;
};

struct Fill_y {
	double x, y, z;
};

struct Fin {
	double x, y, z;
};

int main()
{
	struct N *N;
	struct Dip_x *Dip_x;
	struct Dip_y *Dip_y;
	struct Fill_x *Fill_x;
	struct Fill_y *Fill_y;
	struct Fin *Fin;
	struct Temp *Temp;
	int i, j, l, s, t, k, m, p, q, dip_size_x, dip_size_y, fill_num_x, fill_num_y, Fin_num;
	int test_count, fill_count, array_size;
	double tempx_min, tempx_max, tempy_min, tempy_max; //tempz_min, tempz_max;
	double slope, slope_abs, floor_temp, ceil_temp;
	double dis, dis_gap, dis_range;
	int x_min, x_max, y_min, y_max;
	double z_initial;
	double *knot_x;
	double *knot_y;
	char buffer[50];
	ofstream output;

	N = (struct N *)malloc(test_size * sizeof(struct N));
	Temp = (struct Temp*)malloc(test_size * sizeof(struct Temp));
	Dip_x = (struct Dip_x *)malloc(dip * sizeof(struct Dip_x));
	Dip_y = (struct Dip_y *)malloc(dip * sizeof(struct Dip_y));
	Fill_x = (struct Fill_x*)malloc(fill * sizeof(struct Fill_x));
	Fill_y = (struct Fill_y*)malloc(fill * sizeof(struct Fill_y));
	Fin = (struct Fin*)malloc(fill * sizeof(struct Fin));
	knot_x = (double*)malloc(knot_num * sizeof(double));
	knot_y = (double*)malloc(knot_num * sizeof(double));

	zero_a(knot_x, 0);
	zero_a(knot_y, 0);

	initialize_N(N);
	initialize_Temp(Temp);

	//read raw file
	read_file(N);

	//scaling the z coordinate resolution is 1
	scaling(N);

	//rounding the coordinates to their closest integer
	adjust(N);

	//adjust the order of points (z from small to large)
	adj_order(N);

	//delete the redundant
	kill_redundant(N);

	//get the number of the lines
	test_count = up_num(N);
	
	
	for (i = 0; i < test_count; i++) {
		Temp[i].x = N[i].x;
		Temp[i].y = N[i].y;
		Temp[i].z = N[i].z;
	}

	sprintf_s(buffer, "test_final.txt", i);
	output.open(buffer);
	for (l = 0; l < test_count; l++)
	{
		output << N[l].x << " " << N[l].y << " " << N[l].z << endl;
	}
	output.close();

	//skrskr

	for (s = 0; s < 152; s++) {
		corp_array(N, s);
		array_size = up_num(N);
		initialize_x(Dip_x);
		initialize_y(Dip_y);
		init_fill_x(Fill_x);
		init_fill_y(Fill_y);
		init_Fin(Fin);

	

		Dip_x[0].x = N[0].x;
		Dip_x[0].y = N[0].y;
		Dip_x[0].z = N[0].z;
		t = 0;
		//Fill gap in x-direction
		for (i = 1; i < array_size; i++) {
			if (abs(N[i].y - N[i - 1].y) + abs(N[i].x - N[i - 1].x) <= 50) {
				if ((N[i].y == N[i - 1].y) && (N[i].x == N[i - 1].x)) {
					t = t; //t pointer holds, i++
				}

				else if (abs(N[i].y - N[i - 1].y) == 1.0) {
					Dip_x[t].x = N[i].x;
					Dip_x[t].y = N[i].y;
					Dip_x[t].z = N[i].z;
					t++; //adjacent points
				}

				else if ((N[i].y == N[i - 1].y) && (abs(N[i].x - N[i - 1].x) == 1.0)) {
					Dip_x[t].x = N[i].x;
					Dip_x[t].y = N[i].y;
					Dip_x[t].z = N[i].z;
					t++; //adjacent points
				}

				else if ((N[i].y == N[i - 1].y) && (abs(N[i].x - N[i - 1].x) > 1.0)) {
					dis = N[i].x - N[i - 1].x;
					dis_gap = dis / abs(dis);
					dis_range = abs(dis);
					for (j = 1; j < dis_range + 1; j++) {
						Dip_x[t].x = N[i - 1].x + dis_gap * j;
						Dip_x[t].y = N[i - 1].y;
						Dip_x[t].z = N[i - 1].z;
						t++;
					}
				}

				else {
					slope = (N[i].x - N[i - 1].x) / (N[i].y - N[i - 1].y);
					slope_abs = abs((N[i].x - N[i - 1].x) / (N[i].y - N[i - 1].y));
					floor_temp = floor(slope_abs);
					ceil_temp = ceil(slope_abs);

					if (N[i].y < N[i - 1].y) {
						if (slope_abs - floor_temp <= ceil_temp - slope_abs) {
							Dip_x[t].x = floor(N[i - 1].x - slope);
							Dip_x[t].y = N[i - 1].y - 1;
							Dip_x[t].z = N[i - 1].z;
						}
						else {
							Dip_x[t].x = ceil(N[i - 1].x - slope);
							Dip_x[t].y = N[i - 1].y - 1;
							Dip_x[t].z = N[i - 1].z;
						}
					}

					else {
						if (slope_abs - floor_temp <= ceil_temp - slope_abs) {
							Dip_x[t].x = floor(N[i - 1].x + slope);
							Dip_x[t].y = N[i - 1].y + 1;
							Dip_x[t].z = N[i - 1].z;
						}
						else {
							Dip_x[t].x = ceil(N[i - 1].x + slope);
							Dip_x[t].y = N[i - 1].y + 1;
							Dip_x[t].z = N[i - 1].z;
						}
					}
					N[i - 1].x = Dip_x[t].x;
					N[i - 1].y = Dip_x[t].y;
					N[i - 1].z = Dip_x[t].z;
					i = i - 1;
					t++;
				}
			}

			else {
				i = i;
				t = t;
			}

		}
		dip_size_x = t;

		//clear those duplicates

		for (i = 0; i < dip_size_x; i++) {
			if ((Dip_x[i].x == Dip_x[i + 1].x) && (Dip_x[i].y == Dip_x[i + 1].y) && (Dip_x[i].z == Dip_x[i + 1].z)) {
				for (j = i; j < dip_size_x - 1; j++) {
					Dip_x[j].x = Dip_x[j + 1].x;
					Dip_x[j].y = Dip_x[j + 1].y;
					Dip_x[j].z = Dip_x[j + 1].z;
				}
				dip_size_x = dip_size_x - 1;
			}
		}

		for (i = 0; i < dip_size_x; i++) {
			Dip_x[i].x = int(Dip_x[i].x);
			Dip_x[i].y = int(Dip_x[i].y);
			Dip_x[i].z = int(Dip_x[i].z);
		}

		x_min = min_Dip_x(Dip_x, dip_size_x);
		x_max = max_Dip_x(Dip_x, dip_size_x);

		y_min = min_Dip_y(Dip_x, dip_size_x);
		y_max = max_Dip_y(Dip_x, dip_size_x);

		k = 0;
		l = 0;

		i = y_min;

		for (i = y_min; i < y_max + 1; i++) {
			for (m = 0; m < dip_size_x; m++) {
				if (Dip_x[m].y == i) {
					knot_x[k] = Dip_x[m].x;
					knot_x[k] = int(knot_x[k]);
					k++;
				}
			}
			if (k > 1) {
				//make the knot_x in order
				make_order(knot_x, k, x_max);

				//find the filling starting and ending points
				corp_knot(knot_x, k);

				//get the total number of knot points
				fill_count = knot_length(knot_x, k);

				//not singular-tangential boarder
				for (q = 0; q < fill_count; q++) {
					Fill_x[l].x = knot_x[q];
					Fill_x[l].y = i;
					Fill_x[l].z = s;
					l++;
					for (p = knot_x[q]; p < knot_x[q + 1]; p++) {
						Fill_x[l].x = Fill_x[l - 1].x + 1;
						Fill_x[l].y = i;
						Fill_x[l].z = s;
						l++;
					}
					q++;
				}
			}

			k = 0;
			zero_a(knot_x, 0);
		}

		fill_num_x = l;


		Dip_y[0].x = N[0].x;
		Dip_y[0].y = N[0].y;
		Dip_y[0].z = N[0].z;

		//fill gap in y-direction

		t = 0;
		for (i = 1; i < array_size; i++) {
			if (abs(N[i].y - N[i - 1].y) + abs(N[i].x - N[i - 1].x) <= 50) {
				if ((N[i].y == N[i - 1].y) && (N[i].x == N[i - 1].x)) {
					t = t; //t pointer holds, i++
				}

				else if (abs(N[i].x - N[i - 1].x) == 1.0) {
					Dip_y[t].x = N[i].x;
					Dip_y[t].y = N[i].y;
					Dip_y[t].z = N[i].z;
					t++; //adjacent points
				}

				else if ((N[i].x == N[i - 1].x) && (abs(N[i].y - N[i - 1].y) == 1.0)) {
					Dip_y[t].x = N[i].x;
					Dip_y[t].y = N[i].y;
					Dip_y[t].z = N[i].z;
					t++; //adjacent points
				}

				else if ((N[i].x == N[i - 1].x) && (abs(N[i].y - N[i - 1].y) > 1.0)) {
					dis = N[i].y - N[i - 1].y;
					dis_gap = dis / abs(dis);
					dis_range = abs(dis);
					for (j = 1; j < dis_range + 1; j++) {
						Dip_y[t].x = N[i - 1].x;
						Dip_y[t].y = N[i - 1].y + dis_gap * j;
						Dip_y[t].z = N[i - 1].z;
						t++;
					}
				}

				else {
					slope = (N[i].y - N[i - 1].y) / (N[i].x - N[i - 1].x);
					slope_abs = abs((N[i].y - N[i - 1].y) / (N[i].x - N[i - 1].x));
					floor_temp = floor(slope_abs);
					ceil_temp = ceil(slope_abs);

					if (N[i].x < N[i - 1].x) {
						if (slope_abs - floor_temp <= ceil_temp - slope_abs) {
							Dip_y[t].x = N[i - 1].x - 1;
							Dip_y[t].y = floor(N[i - 1].y - slope);
							Dip_y[t].z = N[i - 1].z;
						}
						else {
							Dip_y[t].x = N[i - 1].x - 1;
							Dip_y[t].y = ceil(N[i - 1].y - slope);
							Dip_y[t].z = N[i - 1].z;
						}
					}

					else {
						if (slope_abs - floor_temp <= ceil_temp - slope_abs) {
							Dip_y[t].x = N[i - 1].x + 1;
							Dip_y[t].y = floor(N[i - 1].y + slope);
							Dip_y[t].z = N[i - 1].z;
						}
						else {
							Dip_y[t].x = N[i - 1].x + 1;
							Dip_y[t].y = ceil(N[i - 1].y + slope);
							Dip_y[t].z = N[i - 1].z;
						}
					}
					N[i - 1].x = Dip_y[t].x;
					N[i - 1].y = Dip_y[t].y;
					N[i - 1].z = Dip_y[t].z;
					i = i - 1;
					t++;
				}
			}

			else {
				i = i;
				t = t;
			}

		}
		dip_size_y = t;

		//discretization in y-direction

		for (i = 0; i < dip_size_y; i++) {
			Dip_y[i].x = int(Dip_y[i].x);
			Dip_y[i].y = int(Dip_y[i].y);
			Dip_y[i].z = int(Dip_y[i].z);
		}

		x_min = min_Dip_y_x(Dip_y, dip_size_y);
		x_max = max_Dip_y_x(Dip_y, dip_size_y);

		y_min = min_Dip_y_y(Dip_y, dip_size_y);
		y_max = max_Dip_y_y(Dip_y, dip_size_y);

		k = 0;
		l = 0;

		i = y_min;

		for (i = x_min; i < x_max + 1; i++) {
			for (m = 0; m < dip_size_y; m++) {
				if (Dip_y[m].x == i) {
					knot_y[k] = Dip_y[m].y;
					knot_y[k] = int(knot_y[k]);
					k++;
				}
			}
			if (k > 1) {
				//make the knot_y in order
				make_order(knot_y, k, y_max);
				corp_knot(knot_y, k);
				fill_count = knot_length(knot_y, k);

				//not singular-tangential boarder

				for (q = 0; q < fill_count; q++) {
					Fill_y[l].x = i;
					Fill_y[l].y = knot_y[q];
					Fill_y[l].z = s;
					l++;
					for (p = knot_y[q]; p < knot_y[q + 1]; p++) {
						Fill_y[l].x = i;
						Fill_y[l].y = Fill_y[l - 1].y + 1;
						Fill_y[l].z = s;
						l++;
					}
					q++;
				}
			}
			else {
				Fill_y[l].x = i;
				Fill_y[l].y = knot_y[0];
				Fill_y[l].z = s;
				l++;
			}

			k = 0;
			zero_a(knot_y, 0);
		}

		fill_num_y = l;

		sprintf_s(buffer, "trial_fill_gap_new_x_slice_%d.txt", s);
		output.open(buffer);
		for (l = 1; l < dip_size_x; l++)
		{
			output << Dip_x[l].x << " " << Dip_x[l].y << " " << Dip_x[l].z << endl; // print_out entire outline
		}
		output.close();

		sprintf_s(buffer, "trial_fill_gap_new_y_slice_%d.txt", s);
		output.open(buffer);
		for (l = 1; l < dip_size_y; l++)
		{
			output << Dip_y[l].x << " " << Dip_y[l].y << " " << Dip_y[l].z << endl; // print_out entire outline
		}
		output.close();

		sprintf_s(buffer, "trial_final_x_slice_%d.txt", s);
		output.open(buffer);
		for (l = 0; l < fill_num_x; l++)
		{
			output << Fill_x[l].x << " " << Fill_x[l].y << " " << Fill_x[l].z << endl;
		}
		output.close();

		sprintf_s(buffer, "trial_final_y_slice_%d.txt", s);
		output.open(buffer);
		for (l = 0; l < fill_num_y; l++)
		{
			output << Fill_y[l].x << " " << Fill_y[l].y << " " << Fill_y[l].z << endl;
		}
		output.close();

		//compare two sets, and write down all the common lines
		l = 0;
		if (fill_num_x <= fill_num_y) {
			for (i = 0; i < fill_num_x; i++) {
				for (j = 0; j < fill_num_y; j++) {
					if ((Fill_y[j].x == Fill_x[i].x) && (Fill_y[j].y == Fill_x[i].y) && (Fill_y[j].z == Fill_x[i].z)) {
						Fin[l].x = Fill_x[i].x;
						Fin[l].y = Fill_x[i].y;
						Fin[l].z = Fill_x[i].z;
						l++;
					}
				}
			}
		}
		else {
			for (i = 0; i < fill_num_y; i++) {
				for (j = 0; j < fill_num_x; j++) {
					if ((Fill_x[j].x == Fill_y[i].x) && (Fill_x[j].y == Fill_y[i].y) && (Fill_x[j].z == Fill_y[i].z)) {
						Fin[l].x = Fill_y[i].x;
						Fin[l].y = Fill_y[i].y;
						Fin[l].z = Fill_y[i].z;
						l++;
					}
				}
			}
		}
		Fin_num = l;

		sprintf_s(buffer, "trial_final_Fin_slice_%d.txt", s);
		output.open(buffer);
		for (l = 0; l < Fin_num; l++)
		{
			output << Fin[l].x << " " << Fin[l].y << " " << Fin[l].z << endl;
		}
		output.close();

		for (i = 0; i < test_count; i++) {
			N[i].x = Temp[i].x;
			N[i].y = Temp[i].y;
			N[i].z = Temp[i].z;
		}
	}
    return 0;
}

void read_file(struct N N[])
{
	double temp;
	int i;
	ifstream in;
	char buffer[50];

	sprintf_s(buffer, "test.txt");
	in.open(buffer);
	for (i = 0; i < test_size; i++) {
		in >> temp;
		N[i].x = temp;

		in >> temp;
		N[i].y = temp;

		in >> temp;
		N[i].z = temp;
	}
	in.close();
	return;
}

void initialize_N(struct N N[])
{
	int i;
	for (i = 0; i < test_size; i++)
	{
		N[i].x = 0;
		N[i].y = 0;
		N[i].z = 0;
	}
	return;
}

void initialize_Temp(struct Temp Temp[]) {
	int i;
	for (i = 0; i < test_size; i++) {
		Temp[i].x = 0;
		Temp[i].y = 0;
		Temp[i].z = 0;
	}
	return;
}

void initialize_x(struct Dip_x Dip_x[])
{
	int i;
	for (i = 0; i < dip; i++)
	{
		Dip_x[i].x = 0;
		Dip_x[i].y = 0;
		Dip_x[i].z = 0;
	}
	return;
}

void initialize_y(struct Dip_y Dip_y[])
{
	int i;
	for (i = 0; i < dip; i++)
	{
		Dip_y[i].x = 0;
		Dip_y[i].y = 0;
		Dip_y[i].z = 0;
	}
	return;
}

void init_fill_x(struct Fill_x Fill_x[])
{
	int i;
	for (i = 0; i < fill; i++)
	{
		Fill_x[i].x = 0;
		Fill_x[i].y = 0;
		Fill_x[i].z = 0;
	}
	return;
}

void init_fill_y(struct Fill_y Fill_y[])
{
	int i;
	for (i = 0; i < fill; i++)
	{
		Fill_y[i].x = 0;
		Fill_y[i].y = 0;
		Fill_y[i].z = 0;
	}
	return;
}

void init_Fin(struct Fin Fin[]) {
	int i;
	for (i = 0; i < fill; i++)
	{
		Fin[i].x = 0;
		Fin[i].y = 0;
		Fin[i].z = 0;
	}
	return;
}

int min_Dip_x(struct Dip_x Dip_x[], int dip_size)
{
	int i;
	int min_Dip_xvalue;
	min_Dip_xvalue = Dip_x[1].x;
	for (i = 1; i < dip_size; i++) {
		if (Dip_x[i].x <= min_Dip_xvalue) {
			min_Dip_xvalue = Dip_x[i].x;
		}
	}
	return min_Dip_xvalue;
}

int max_Dip_x(struct Dip_x Dip_x[], int dip_size)
{
	int i;
	int max_Dip_xvalue;
	max_Dip_xvalue = Dip_x[1].x;
	for (i = 1; i < dip_size; i++) {
		if (Dip_x[i].x >= max_Dip_xvalue) {
			max_Dip_xvalue = Dip_x[i].x;
		}
	}
	return max_Dip_xvalue;
}

int min_Dip_y(struct Dip_x Dip_x[], int dip_size)
{
	int i;
	int min_Dip_yvalue;
	min_Dip_yvalue = Dip_x[1].y;
	for (i = 1; i < dip_size; i++) {
		if (Dip_x[i].y <= min_Dip_yvalue) {
			min_Dip_yvalue = Dip_x[i].y;
		}
	}
	return min_Dip_yvalue;
}

int max_Dip_y(struct Dip_x Dip_x[], int dip_size)
{
	int i;
	int max_Dip_yvalue;
	max_Dip_yvalue = Dip_x[1].y;
	for (i = 1; i < dip_size; i++) {
		if (Dip_x[i].y >= max_Dip_yvalue) {
			max_Dip_yvalue = Dip_x[i].y;
		}
	}
	return max_Dip_yvalue;
}

int min_Dip_y_x(struct Dip_y Dip_y[], int dip_size)
{
	int i;
	int min_Dip_xvalue;
	min_Dip_xvalue = Dip_y[1].x;
	for (i = 1; i < dip_size; i++) {
		if (Dip_y[i].x <= min_Dip_xvalue) {
			min_Dip_xvalue = Dip_y[i].x;
		}
	}
	return min_Dip_xvalue;
}

int max_Dip_y_x(struct Dip_y Dip_y[], int dip_size)
{
	int i;
	int max_Dip_xvalue;
	max_Dip_xvalue = Dip_y[1].x;
	for (i = 1; i < dip_size; i++) {
		if (Dip_y[i].x >= max_Dip_xvalue) {
			max_Dip_xvalue = Dip_y[i].x;
		}
	}
	return max_Dip_xvalue;
}

int min_Dip_y_y(struct Dip_y Dip_y[], int dip_size)
{
	int i;
	int min_Dip_yvalue;
	min_Dip_yvalue = Dip_y[1].y;
	for (i = 1; i < dip_size; i++) {
		if (Dip_y[i].y <= min_Dip_yvalue) {
			min_Dip_yvalue = Dip_y[i].y;
		}
	}
	return min_Dip_yvalue;
}

int max_Dip_y_y(struct Dip_y Dip_y[], int dip_size)
{
	int i;
	int max_Dip_yvalue;
	max_Dip_yvalue = Dip_y[1].y;
	for (i = 1; i < dip_size; i++) {
		if (Dip_y[i].y >= max_Dip_yvalue) {
			max_Dip_yvalue = Dip_y[i].y;
		}
	}
	return max_Dip_yvalue;
}

void zero_a(double p[], int q)
{
	int i;
	for (i = 0; i<knot_num; i++)
	{
		p[i] = q;
	}
	return;
}

void make_order(double p[], int q, int r)
{
	int i,j,min;
	double *after_order;
	double *order_temp;

	after_order = (double*)malloc(q * sizeof(double));
	order_temp = (double*)malloc(q * sizeof(double));
	//extract non-zero components in p[]
	for (i = 0; i < q; i++) {
		after_order[i] = p[i];
	}

	for (i = 0; i < q; i++) {
		min = 0;
		for (j = 0; j < q; j++) {
			if (after_order[j] < after_order[min]) {
				min = j;
			}
		}
		order_temp[i] = after_order[min];
		after_order[min] = r;
	}

	for (i = 0; i < knot_num; i++) {
		p[i] = 0;
	}

	for (i = 0; i < q; i++) {
		p[i] = order_temp[i];
	}
	return;
}

void corp_knot(double p[], int q)
{
	int i, j, start_count, end_count, com_count, total_count, com_tot_count;
	double *start_knot;
	double *end_knot;

	i = 0;
	j = 0;

	start_knot = (double*)malloc(q * sizeof(double));
	end_knot = (double*)malloc(q * sizeof(double));

	for (i = 0; i < q; i++) {
		start_knot[i] = 0;
	}
	for (i = 0; i < q; i++) {
		end_knot[i] = 0;
	}

	com_count = 0;

	end_count = -1;

	for (i = 0; i < q; i++) {
		if (i == 0) {
			start_count = 0;
			start_knot[0] = p[0];
			com_count++;
		}
		else if (i == q - 1) {
			if ((p[i] - p[i - 1] > 1) && (start_count != end_count)) {
				com_count++;
				end_count++;
				end_knot[end_count] = p[i];
			}
			else if (p[i] - p[i - 1] == 1) {
				end_count++;
				end_knot[end_count] = p[i];
			}
			else {
				com_count++;
				start_count++;
				start_knot[start_count] = p[i];
			}
		}
		else {
			if ((p[i] - p[i - 1] > 1) && (p[i + 1] - p[i] == 1)) {
				if ((com_count % 2 != 0) && (start_count != end_count)) {
					end_count = start_count;
					end_knot[end_count] = p[i];
				}
				start_count++;
				start_knot[start_count] = p[i];
				com_count++;
			}
			else if ((p[i] - p[i - 1] == 1) && (p[i + 1] - p[i] > 1)) {
				end_count = start_count;
				end_knot[end_count] = p[i];
			}
			else if ((p[i] - p[i - 1] > 1) && (p[i + 1] - p[i] > 1)) {
				com_count++;
				if (start_count == end_count) {
					start_count++;
					start_knot[start_count] = p[i];
				}
				else {
					end_count = start_count;
					end_knot[end_count] = p[i];
				}
			}
			else {
				i = i;
			}
		}
	}

	
	total_count = start_count + 1;

	for (j = 0; j < knot_num; j++) {
		p[j] = 0;
	}

	i = 0;

	if (start_count == end_count) {
		for (j = 0; j < total_count; j++) {
			p[i] = start_knot[j];
			i++;
			p[i] = end_knot[j];
			i++;
		}
	}
	else {
		for (j = 0; j < total_count - 1; j++) {
			p[i] = start_knot[j];
			i++;
			p[i] = end_knot[j];
			i++;
		}
		p[i] = start_knot[total_count - 1];
	}
	

	com_tot_count = i;

	return;
}

int knot_length(double p[], int q) {
	int i, j;
	int counter_knot;
	counter_knot = 0;
	for (i = 0; i < q; i++) {
		if (p[i] != 0) {
			counter_knot++;
		}
	}
	return counter_knot;
}

void scaling(struct N N[]) {
	int i;
	for (i = 0; i < test_size; i++) {
		N[i].x = 0.1*(N[i].x);
		N[i].y = 0.1*(N[i].y);
		N[i].z = 0.1*(N[i].z);
	}
	return;
}

void adjust(struct N N[]) {
	int i;
	for (i = 0; i < test_size; i++) {
		N[i].x = round(N[i].x);
		N[i].y = round(N[i].y);
		N[i].z = round(N[i].z);
	}
	return;
}

void adj_order(struct N N[]) {
	int i, j, l;
	struct temp *temp;
	temp = (struct temp *)malloc(test_size * sizeof(struct temp));
	l = 0;
	for (i = 0; i < 152; i++) {
		for (j = 0; j < test_size; j++) {
			if (N[j].z == i) {
				temp[l].x = N[j].x;
				temp[l].y = N[j].y;
				temp[l].z = N[j].z;
				l++;
			}
		}
	}

	for (i = 0; i < test_size; i++) {
		N[i].x = temp[i].x;
		N[i].y = temp[i].y;
		N[i].z = temp[i].z;

	}
	return;
}

void kill_redundant(struct N N[]) {
	int i, j, l;
	l = test_size;
	for (i = 0; i < l - 1; i++) {
		if ((N[i].x == N[i + 1].x) && (N[i].y == N[i + 1].y) && (N[i].z == N[i + 1].z)) {
			for (j = i; j < l - 1; j++) {
				N[j].x = N[j + 1].x;
				N[j].y = N[j + 1].y;
				N[j].z = N[j + 1].z;
			}
			l--;
			i--;
		}
	}
	if (l != test_size) {
		for (i = l - 1; i < test_size; i++) {
			N[i].x = 0;
			N[i].y = 0;
			N[i].z = 0;
		}
	}
	return;
}

int up_num(struct N N[]) {
	int i;
	int up_num;
	up_num = 0;
	for (i = 0; i < test_size; i++) {
		if ((N[i].x != 0) || (N[i].y != 0) || (N[i].z != 0)) {
			up_num++;
		}
	}
	return up_num;
}

void corp_array(struct N N[], int q) {
	int i, j, l;

	l = 0;

	struct temp *temp;

	temp = (struct temp *)malloc(test_size * sizeof(struct temp));

	for (i = 0; i < test_size; i++) {
		temp[i].x = 0;
		temp[i].y = 0;
		temp[i].z = 0;
	}
	l = 0;
	for (i = 0; i < test_size; i++) {
		if (N[i].z == q) {
			temp[l].x = N[i].x;
			temp[l].y = N[i].y;
			temp[l].z = N[i].z;
			l++;
		}
	}

	for (j = 0; j < l; j++) {
		N[j].x = temp[j].x;
		N[j].y = temp[j].y;
		N[j].z = temp[j].z;
	}
	for (j = l; j < test_size; j++) {
		N[j].x = 0;
		N[j].y = 0;
		N[j].z = 0;
	}

	return;
}

////rounding coordinate
//for (i = 0; i < slice_size; i++) {
//	tempx_min = N[i].x - floor(N[i].x);
//	tempx_max = ceil(N[i].x) - N[i].x;
//
//	if (tempx_min <= tempx_max) {
//		N[i].x = floor(N[i].x);
//	}
//	else {
//		N[i].x = ceil(N[i].x);
//	}
//}
//for (i = 0; i < slice_size; i++) {
//	tempy_min = N[i].y - floor(N[i].y);
//	tempy_max = ceil(N[i].y) - N[i].y;
//	if (tempy_min <= tempy_max) {
//		N[i].y = floor(N[i].y);
//	}
//
//	else {
//		N[i].y = ceil(N[i].y);
//	}
//}
//
//for (i = 0; i < slice_size; i++) {
//	tempy_min = N[i].z - floor(N[i].z);
//	tempy_max = ceil(N[i].z) - N[i].z;
//	if (tempy_min <= tempy_max) {
//		N[i].z = floor(N[i].z);
//	}
//	else {
//		N[i].z = ceil(N[i].z);
//	}
//}




////rounding coordinate
//for (i = 0; i < n; i++) {
//	tempx_min = N[i].x - floor(N[i].x);
//	tempx_max = ceil(N[i].x) - N[i].x;
//	if (tempx_min <= tempx_max) {
//		N[i].x = floor(N[i].x);
//	}
//	else {
//		N[i].x = ceil(N[i].x);
//	}
//}
//for (i = 0; i < n; i++) {
//	tempy_min = N[i].y - floor(N[i].y);
//	tempy_max = ceil(N[i].y) - N[i].y;
//	if (tempy_min <= tempy_max) {
//		N[i].y = floor(N[i].y);
//	}
//	else {
//		N[i].y = ceil(N[i].y);
//	}
//}
//
//for (i = 0; i < n; i++) {
//	tempy_min = N[i].z - floor(N[i].z);
//	tempy_max = ceil(N[i].z) - N[i].z;
//	if (tempy_min <= tempy_max) {
//		N[i].z = floor(N[i].z);
//	}
//	else {
//		N[i].z = ceil(N[i].z);
//	}
//}
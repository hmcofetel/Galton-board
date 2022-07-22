#ifndef __BALL_H__
#define __BALL_H__

#include <vector>
#include <cstdlib>
#include <ctime>

#include "config.h"
#include "num.h"
using namespace std;

class Ball
{
private:
	float a[2] = {0,1000};  
	float error = 0.5;
	float pos[2] ;
	float pos0[2];
	float delta_T;
	float v[2] = {0,0};
	float v0[2] = {0,0};
	vector <float> E_ARR;

	const int sample = 100;
	float calc_E()
	{
		return HEIGHT - pos[1] - r + v[0]*v[0] + v[1]*v[1];
	}

public:
	float r;
	bool is_stability = false;

	Ball(float position[] , float rad, float time_delay )
	{
		pos[0] = position[0];
		pos[1] = position[1];
		pos0[0] = position[0];
		pos0[1] = position[1];
		r = rad;
		delta_T = time_delay;
		srand(time(NULL)); 
	}

	float *getPos(){return pos;}
	float *getSpeed(){ return v;}

	

	void run(float T)
	{
		v[0] = v0[0] + a[0]*(T- delta_T);
		v[1] = v0[1] + a[1]*(T- delta_T);

		pos[0] = pos0[0] + v0[0]*(T- delta_T) + 0.5*a[0]*(T- delta_T)*(T- delta_T) ;
		pos[1] = pos0[1] + v0[1]*(T- delta_T) + 0.5*a[1]*(T- delta_T)*(T- delta_T) ;


		if (pos[0] + r >= WIDTH)
		{
			delta_T = T;
			v0[0] = -v[0]*error;
			v0[1] = v[1]*error;
			pos0[0] = WIDTH - r;
			pos0[1] = pos[1];
		}

		if (pos[0] - r < 0.0)
		{
			delta_T = T;
			v0[0] = -v[0]*error;
			v0[1] = v[1]*error;
			pos0[0] = r;
			pos0[1] = pos[1];
		}


		if (pos[1] + r >= HEIGHT)
		{
			delta_T = T;
			v0[0] = v[0]*error;
			v0[1] = -v[1]*error;
			pos0[0] = pos[0];
			pos0[1] = HEIGHT - r;
		}

		if (pos[1] - r < 0)
		{
			delta_T = T;
			v0[0] = v[0]*error;
			v0[1] = -v[1]*error;
			pos0[0] = pos[0];
			pos0[1] =  r;
		}
	}

	void collide(float T, float map[][2])
	{
		for (int i = 0; i < NUMP; ++i)
		{
			if(distance(pos, map[i]) <= BALL_R + POINT_R)
			{
				float dis = distance(pos, map[i]);
				float jet_v[2] = {pos[0] - map[i][0],pos[1] - map[i][1] } ;
				float *jet_pr = get_projection_vect(v,jet_v );

				v0[0] = v[0]  + jet_pr[0]*(1 + error) + rand() % (10 - 0 + 1) + 0;
				v0[1] = v[1] + jet_pr[1]*(1 + error)+ rand() % (10 - 0 + 1) + 0;

				delta_T = T;
				float dis_jet = magnitude(jet_v);

				pos0[0] = map[i][0] + jet_v[0]*(BALL_R + POINT_R + 1)/dis_jet;
				pos0[1] = map[i][1] + jet_v[1]*(BALL_R + POINT_R + 1)/dis_jet;

			}
		}
	}

	void collide_ball(float T, vector <Ball*> ball)
	{
		for (int i = 0; i < ball.size(); ++i)
		{
			if(distance(pos, ball[i] -> getPos()) <= BALL_R + BALL_R)
			{

				float *oth_pos = ball[i] -> getPos();
				float dis = distance(pos, oth_pos);
				
				if (dis <= 1){continue;}
				float jet_v[2] = {pos[0] - oth_pos[0],pos[1] - oth_pos[1] } ;
				float *jet_pr = get_projection_vect(v,jet_v );
				float *jet_oth_pr = get_projection_vect(ball[i] -> getSpeed(),jet_v );

				v0[0] = v[0]  + jet_pr[0]*error + jet_oth_pr[0]*error ;
				v0[1] = v[1] + jet_pr[1]*error + jet_oth_pr[1]*error;

				
				float dis_jet = magnitude(jet_v);
				delta_T = T;
				pos0[0] = oth_pos[0] + jet_v[0]*(BALL_R + BALL_R + 0.2)/dis_jet;
				pos0[1] = oth_pos[1] + jet_v[1]*(BALL_R + BALL_R + 0.2)/dis_jet;

			}
		}
	}

	void collide_columns(float T, float columns[])
	{
		for (int i = 0; i < NUMC && pos[1] >= HEIGHT - HEIGHTC ; ++i)
		{

			if (columns[i] - pos[0] <= r + WIDTHC/2  && columns[i] - pos[0] > 0)
			{	
				
				delta_T = T;
				v0[0] = -v[0]*error;
				v0[1] = v[1]*error;
				pos0[0] = columns[i] - WIDTHC/2 - r;
				pos0[1] = pos[1];
			}

			if (  pos[0] - columns[i] <= r + WIDTHC/2 && pos[0] - columns[i] > 0  )
			{	
				
				delta_T = T;
				v0[0] = -v[0]*error;
				v0[1] = v[1]*error;
				pos0[0] = columns[i] + WIDTHC/2 + r;
				pos0[1] = pos[1];
			}
		}
	}

	void stability()
	{
		if (E_ARR.size() < sample )
		{
			E_ARR.push_back(calc_E());
		}
		else
		{
			E_ARR.erase(E_ARR.begin());
			E_ARR.push_back(calc_E());	
		}

		float avg = 0;
		float sigma = 0;

		for (int i = 0; i < E_ARR.size(); ++i)
		{
			avg += E_ARR[i];

		}	
		avg = avg /E_ARR.size();

		for (int i = 0; i < E_ARR.size(); ++i)
		{
			sigma += (E_ARR[i] - avg)*(E_ARR[i] - avg);

		}

		sigma =  sigma/E_ARR.size();

		if (sigma < 2e+6 && E_ARR.size() == sample && pos[1] > HEIGHT - HEIGHTC)
		{
			is_stability = true;
		}

	}

};

#endif
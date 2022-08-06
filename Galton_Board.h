#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <thread>
#include <iostream>
#include <vector>

#include "Config.h"
#include "Ball.h"
#include "QuadTree.h"
using namespace std;


class GaltonBoard
{
private:
	bool RUN = true;
	float T = 0.0;
	int x_click,y_click;
	float MAP[NUMP][2];
	float COLUMN[NUMC];


	SDL_Window *window;
	SDL_Renderer *renderer;
	SDL_Event event;
	SDL_Texture *texBall = nullptr;
	SDL_Texture *texMap = nullptr;
	SDL_Texture *texBackGround = nullptr;

	vector <vector<float>> POINTS; 
	vector <Ball*> BALLS;
	SDL_Rect destRect, srcRect;
	vector <thread> THREADS;
	Uint32 frameStart, frameTime;

	QuadTree *qtree; 
	
	void createMap()
	{
		int cnt = 0;
		for (int i = 100; i < HEIGHT; i+=50)
		{
			if (i%100 == 0)
			{
				for (int j = 50;j < WIDTH ; j+=50)
				{
					if (cnt >= NUMP){
						break;
					}
					MAP[cnt][1] =  i;
					MAP[cnt][0] =  j;
					cnt++;
				}
			}
			else 
			{
				for (int j = 25;j < WIDTH ; j+=50)
				{
					if (cnt >= NUMP){
						break;
					}
					MAP[cnt][1] =  i;
					MAP[cnt][0] =  j;
					cnt++;
				}
			}
			
		}
	}	

	void createColumns()
	{
		for (int i = 0;  i < NUMC; ++i)
		{
			COLUMN[i] =  25*i;
		}
	}


	void drawIMG(SDL_Texture *tex ,int x_src, int y_src, int width_src, int height_src,int x, int y, int width, int height)
	{

		srcRect.w = width_src;
		srcRect.h = height_src;
		srcRect.x = x_src;
		srcRect.y = y_src;

		destRect.h = height;
		destRect.w = width;
		destRect.x = x - destRect.w /2  ;
		destRect.y = y - destRect.h/2 ;

		SDL_RenderCopy(renderer,tex,&srcRect, &destRect);
	}	

	void drawRect(int x, int y , int width, int height, Uint8 r, Uint8 g, Uint8 b, Uint8 a)
	{
		SDL_Rect Rect;
		Rect.w = width;
	    Rect.h = height;
	    Rect.x = x - width/2;
	    Rect.y = y;
	    SDL_SetRenderDrawColor( renderer, r, g, b, a );
	    SDL_RenderFillRect( renderer, &Rect );
	}

	void runTime(){
		while (RUN)
		{
			T+= 0.012;
			SDL_Delay(1000*0.012);
		}
	}

	void runQuadTree()
	{
		while(RUN)
		{
			qtree -> run();
			qtree -> freeOverflow();
			qtree -> checkDeadEnd();
			SDL_Delay(1000/((float)FPS+100));
		}
	}

	void runBall(Ball *ball)
	{
		while (RUN && !ball-> is_stability )
		{
			ball -> run(T);
			SDL_Delay(1000/((float)FPS+30));
		}
	}



	void runCollide(Ball *ball)
	{
		while (RUN && !ball-> is_stability )
		{
			ball -> collide(T, MAP);
			SDL_Delay(1000/((float)FPS+100));
		}

	}

	void runCollideBall(Ball *ball)
	{
		while (RUN && !ball-> is_stability)
		{
			ball -> collide_ball(T,qtree-> query(ball));
			SDL_Delay(1000/((float)FPS+100));
		}

	}

	void runCollideColumns(Ball *ball)
	{
		while (RUN && !ball-> is_stability)
		{
			ball -> collide_columns(T, COLUMN);
			SDL_Delay(1000/((float)FPS+100));
		}
	}

	void runCheckStable(Ball *ball)
	{
		while (RUN && !ball-> is_stability )
		{
			ball -> stability();
			SDL_Delay(1000/((float)FPS+100));
		}
	}

	

	void drawBackGround()
	{
		drawIMG(texBackGround, 0,0,2000, 989, WIDTH/2,HEIGHT/2,WIDTH,HEIGHT);
	}

	void drawBalls(){
		for (int i = 0; i < BALLS.size(); ++i)
		{
			drawIMG(texBall,0,0,500,500,BALLS[i] -> getPos()[0],BALLS[i] -> getPos()[1],  2*BALL_R,2*BALL_R);
		}
		
	}

	void drawMap(){
		for (int i = 0; i < NUMP; i++)
		{
			drawIMG(texMap,0,0,500,500,MAP[i][0],MAP[i][1],  2*POINT_R,2*POINT_R);
		}
		
	}

	void drawColumns()
	{
		for (int i = 0; i < NUMC; ++i)
		{
			drawRect(COLUMN[i], HEIGHT - HEIGHTC, WIDTHC, HEIGHTC, 255,255,255,255);
		}
	}

	void appendBall(int x, int y)
	{
		float pos[2] = {(float) x,(float)y};
		Ball* ball = new Ball(pos, BALL_R, T);
		BALLS.push_back(ball);
		qtree -> add(ball);

		THREADS.push_back(thread(runBall,this ,ball));
		THREADS.push_back(thread(runCollide, this,ball));
		THREADS.push_back(thread(runCollideBall, this,ball));
		THREADS.push_back(thread(runCollideColumns,this ,ball));
		THREADS.push_back(thread(runCheckStable,this ,ball));
	}

public:

	void init(int x, int y)
	{
		
		SDL_Init(SDL_INIT_EVERYTHING);
		window = SDL_CreateWindow("Galton Board", x, y, WIDTH, HEIGHT, 0);
		renderer = SDL_CreateRenderer(window, -1, 0);
		
		createMap();
		createColumns();

		texBall =  SDL_CreateTextureFromSurface(renderer,IMG_Load(PTH_BALL));
		texMap = SDL_CreateTextureFromSurface(renderer,IMG_Load(PTH_POINT));
		texBackGround = SDL_CreateTextureFromSurface(renderer, IMG_Load(PTH_BACKGROUND));

		qtree = new QuadTree(new Rectangle(0,0,WIDTH, HEIGHT),30);

		THREADS.push_back(thread(&GaltonBoard::runTime, this));
		THREADS.push_back(thread(&GaltonBoard::runQuadTree, this));

	}

	void loop()
	{

		while (RUN)
		{
			frameStart = SDL_GetTicks();

			SDL_RenderClear(renderer);
			drawBackGround();
			drawBalls();
			drawMap();
			drawColumns();

			SDL_RenderPresent(renderer);
			SDL_PollEvent(&event);
			switch (event.type)
			{
				case SDL_MOUSEBUTTONDOWN:
					SDL_GetMouseState(&x_click, &y_click);
					appendBall(x_click, y_click);
					break;

				case SDL_QUIT:
					RUN = false;
					break;

			}

			frameTime = SDL_GetTicks() - frameStart;

			if (1000/FPS > frameTime)
			{
				SDL_Delay(1000/FPS - frameTime);
			}
			
		}
	}

	void clean()
	{
		SDL_DestroyTexture(texMap);
		SDL_DestroyTexture(texBall);
		SDL_DestroyTexture(texBackGround);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		for (auto i:BALLS)
			delete i;
		delete qtree;

	}

};



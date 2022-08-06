#ifndef __QUAD_TREE__
#define __QUAD_TREE__

#include "Ball.h"


class Rectangle
{

private:
	float  shape[4];
public:
	Rectangle(float x, float y, float w, float h)
	{
		this -> shape[0] =  x ;
		this -> shape[1] =  y ;
		this -> shape[2] =  w ;
		this -> shape[3] =  h ;
	}

	float *getShape()
	{
		return shape;
	}
};

class QuadTree
{
private:
	int capacity;
	bool divided;
	bool emptyHole;

	float *shape;
	Rectangle *rect;
	vector <Ball *> balls;
	QuadTree *tl = nullptr, *tr = nullptr, *bl =nullptr, *br = nullptr; 
	static vector <Ball*> overflow;
	

public:
	QuadTree(Rectangle *rect, int cap)
	{
		this -> rect = rect;
		this -> capacity = cap;
		this -> divided = false;
		this -> shape = rect -> getShape();

	}

	~QuadTree()
	{
		
		delete rect;
		delete shape;
		for (auto i : balls)
			delete i;
		delete tl, tr, bl, br;

	}

	void add(Ball *ball)
	{

		if ((int)balls.size() < capacity  &&  !divided)
			this -> balls.push_back(ball);
		else if (balls.size() ==  capacity &&  !divided)
		{
			for (auto i : balls)
				addSubDiv(i);

			balls.clear();
			divided = true;
			addSubDiv(ball);			
		}
		else if (divided)
			addSubDiv(ball);	
	}

	void addSubDiv(Ball *ball)
	{
		switch(findLocation(ball))
		{
			case 0:
				if (!tl)
					tl = new QuadTree(new Rectangle(shape[0], shape[1], shape[2]/2, shape[3]/2), capacity);
				tl -> add(ball);
				break;

			case 1:
				if (!tr)
					tr = new QuadTree(new Rectangle(shape[0] + shape[2]/2, shape[1], shape[2]/2, shape[3]/2),capacity);
				tr -> add(ball);
				break;

			case 2:
				if (!bl)
					bl = new QuadTree(new Rectangle(shape[0], shape[1] + shape[3]/2, shape[2]/2, shape[3]/2),capacity);
				bl -> add(ball);
				break;

			case 3:
				if (!br)
					br = new QuadTree(new Rectangle(shape[0] + shape[2]/2, shape[1] + shape[3]/2, shape[2]/2, shape[3]/2),capacity);
				br -> add(ball);
				break;	
		}
	}

	int findLocation(Ball *ball)
	{	
		float *pos = ball -> getPos();

		if (pos[0] >= shape[0] && pos[0] <= shape[0] + shape[2]/2)
		{
			if (pos[1] >= shape[1] && pos[1] <= shape[1] + shape[3]/2)
			{
				return 0;
			}
			if (pos[1] > shape[1]  + shape[3]/2 && pos[1] <= shape[1] + shape[3])
			{
				return 2;
			}

		}
		if (pos[0] > shape[0] + shape[2]/2 && pos[0] <= shape[0] + shape[2])
		{
			if (pos[1] >= shape[1] && pos[1] <= shape[1] + shape[3]/2)
			{	
				return 1;
			}

			if (pos[1] > shape[1]  + shape[3]/2 && pos[1] <= shape[1] + shape[3])
			{
				return 3;
			}

		}
		return -1;
	}



	void freeOverflow()
	{
		for (int i = 0 ; i < overflow.size(); i++)
		{
			add(overflow[i]);
		}

		overflow.clear();
		
	}

	bool is_divided()
	{
		return divided;
	}

	void checkDeadEnd()
	{
		if (tl)
		{
			if(!tl-> is_divided() && tl -> sizeCap() == 0)
				tl = nullptr;
			else 
				tl -> checkDeadEnd();		
		}
		if (tr)
		{
			if(!tr-> is_divided() && tr -> sizeCap() == 0)
				tr = nullptr;
			else 
				tr -> checkDeadEnd();
		}
		if (bl)
		{
			if(!bl-> is_divided() && bl -> sizeCap() == 0)
				bl = nullptr;
			else 
				bl -> checkDeadEnd();
		}

		if (br)
		{
			if(!br-> is_divided() && br -> sizeCap() == 0)
				br = nullptr;
			else 
				br -> checkDeadEnd();
		}
		
		if(!br&&!tr&&!bl&&!tl)
			divided = false;

		
	}

	vector <Ball *> query(Ball* ball)
	{
		vector <Ball*> result;
		if (!divided)
		{
			for (auto i: balls)
				if (i != ball)
				{	
					result.push_back(i);
				}
			return result;
		}
		else
		{
			switch(findLocation(ball))
			{
				case 0:
					if (tl)
						result = tl -> query(ball);
					break;
				case 1:
					if (tr)
						result = tr -> query(ball);
					break;
				case 2:
					if (bl)
						result = bl -> query(ball);
					break;
				case 3:
					if (br)
						result = br -> query(ball);
					break;

			}
			return result;
		}
			

	}


	void run()
	{
		for(int i = 0; i < balls.size(); i++)
		{	
			if (findLocation(balls[i]) == -1)
			{
				overflow.push_back(balls[i]);
				balls.erase(balls.begin()+i);

			}	
		}
	
		if(tl)
			tl -> run();
		if (tr)
			tr -> run();
		if (bl)
			bl -> run();
		if(br)
			br -> run() ;
	}

	int sizeCap()
	{
		return balls.size();
	}
};

vector <Ball *> QuadTree:: overflow;




#endif
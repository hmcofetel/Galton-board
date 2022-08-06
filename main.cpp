#include "Galton_Board.h"

int main(int argc, char *argv[])
{
	GaltonBoard *galtonboard = new GaltonBoard();
	galtonboard -> init(0,25);
	galtonboard -> loop();
	galtonboard -> clean();
	return 0;
}
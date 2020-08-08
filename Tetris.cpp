#include <iostream>
#include <Windows.h>
#include <thread>

// 31:03 / 36:19
// Code - It - Yourself!Tetris - Programming from Scratch(Quick and Simple C++)
// Bugs -> Border null check. Down arrow key do not force current piece down.

using namespace std;

wstring tetromino[7];

int fieldWidth = 12;
int fieldHeight = 18;
unsigned char* field = nullptr;

int screenWidth = 80;
int screenHeight = 30;

int Rotate(int pX, int pY, int r);
void CreateAssets();
bool IsValidPlacement(int tetromino, int currentRotation, int position_X, int position_Y);

int main()
{
	// Create assets
	CreateAssets();

	field = new unsigned char[fieldWidth * fieldHeight];

	for (int x = 0; x < fieldWidth; x++)
		for (int y = 0; y < fieldHeight; y++)
		{
			// i = y * width + x
			field[y * fieldWidth + x] = (x == 0 || x == fieldWidth - 1 || y == 0 || y == fieldHeight - 1) ? 9 : 0;
		}

	wchar_t* screen = new wchar_t[screenWidth * screenHeight];
	for (int i = 0; i < screenWidth * screenHeight; i++) screen[i] = L' ';
	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(hConsole);
	DWORD dwByteWritten = 0;

	bool isGameOver = false;

	int currentPiece = 1;
	int currentRotation = 0;
	int current_X = fieldWidth / 2;
	int current_Y = 1;

	bool keys[4];
	bool isPressingHoldDown = false;

	int speed = 20;
	int speedCounter = 0;
	bool isForcingDown = false;

	while (isGameOver == false)
	{
		// GAME TIMING
		std::this_thread::sleep_for(50ms);

		speedCounter++;

		isForcingDown = (speedCounter == speed);

		// INPUT
		for (int k = 0; k < 4; k++)
		{														// R	L	DZ
			keys[k] = (0x8000 & GetAsyncKeyState((unsigned char)("\x27\x25\x28Z"[k]))) != 0;
		}

		// GAME LOGIC
		current_X += (keys[0] && IsValidPlacement(currentPiece, currentRotation, current_X + 1, current_Y) ? 1 : 0);
		current_X -= (keys[1] && IsValidPlacement(currentPiece, currentRotation, current_X - 1, current_Y) ? 1 : 0);
		current_X += (keys[2] && IsValidPlacement(currentPiece, currentRotation, current_X, current_Y + 1) ? 1 : 0);

		if(keys[3])     
		{
			currentRotation += (isPressingHoldDown == false && IsValidPlacement(currentPiece, currentRotation + 1, current_X, current_Y)) ? 1 : 0;
			isPressingHoldDown = true;
		}
		else
		{
			isPressingHoldDown = false;
		}

		if(isForcingDown)
		{
			if(IsValidPlacement(currentPiece, currentRotation, current_X, current_Y + 1))
			{
				current_Y++;
			}
			else
			{
				for (int pX = 0; pX < 4; pX++)
					for (int pY = 0; pY < 4; pY++)
					{
						if (tetromino[currentPiece][Rotate(pX, pY, currentRotation)] == L'X')
						{
							field[(current_Y + pY) * fieldWidth + (current_X + pX)] = currentPiece + 1;
						}
					}

				currentPiece = rand() % 7;
				currentRotation = 0;
				current_X = fieldWidth / 2;
				current_Y = 1;
				
				isGameOver = IsValidPlacement(currentPiece, currentRotation, current_X, current_Y);
			}

			speedCounter = 0;
		}

		// RENDER OUTPUT

		int xOffset = 2;
		int yOffset = 2;

		// Draw Field
		for (int x = 0; x < fieldWidth; x++)
			for (int y = 0; y < fieldHeight; y++)
			{
				screen[(y + yOffset) * screenWidth + (x + xOffset)] = L" ABCDEFG=#"[field[y * fieldWidth + x]];
			}

		// Draw Current Piece
		for (int pX = 0; pX < 4; pX++)
			for (int pY = 0; pY < 4; pY++)
			{
				if(tetromino[currentPiece][Rotate(pX, pY, currentRotation)] == L'X')
				{
					screen[(current_Y + pY + yOffset) * screenWidth + (current_X + pX + xOffset)] = currentPiece + 65;
				}
			}

		// Display Frame
		WriteConsoleOutputCharacter(hConsole, screen, screenWidth * screenHeight, { 0, 0 }, &dwByteWritten);
	}



	return 0;
}

int Rotate(int pX, int pY, int r)
{
	switch (r % 4)
	{
	case 0: return pY * 4 + pX;
	case 1: return 12 + pY - (pX * 4);
	case 2: return 15 - (pY * 4) - pX;
	case 3: return 3 - pY + (pX * 4);
	default: break;
	}
}

void CreateAssets()
{
	tetromino[0].append(L"..X.");
	tetromino[0].append(L"..X.");
	tetromino[0].append(L"..X.");
	tetromino[0].append(L"..X.");

	tetromino[1].append(L"..X.");
	tetromino[1].append(L".XX.");
	tetromino[1].append(L".X..");
	tetromino[1].append(L"....");

	tetromino[2].append(L".X..");
	tetromino[2].append(L".XX.");
	tetromino[2].append(L"..X.");
	tetromino[2].append(L"....");

	tetromino[3].append(L"....");
	tetromino[3].append(L".XX.");
	tetromino[3].append(L".XX.");
	tetromino[3].append(L"....");

	tetromino[4].append(L"..X.");
	tetromino[4].append(L".XX.");
	tetromino[4].append(L"..X.");
	tetromino[4].append(L"....");

	tetromino[5].append(L"....");
	tetromino[5].append(L".XX.");
	tetromino[5].append(L"..X.");
	tetromino[5].append(L"..X.");

	tetromino[6].append(L"....");
	tetromino[6].append(L".XX.");
	tetromino[6].append(L".X..");
	tetromino[6].append(L".X..");
}

bool IsValidPlacement(int _tetromino, int currentRotation, int position_X, int position_Y)
{
	for (int pX = 0; pX < 4; pX++)
		for (int pY = 0; pY < 4; pY++)
		{
			// Get index into piece
			int pI = Rotate(pX, pY, currentRotation);

			// Get index into field
			int fI = (position_Y + pY) * fieldWidth + (position_X + pX);

			if (position_X + pX >= 0 && position_X + pX < fieldWidth)
				if (position_Y + pY >= 0 && position_Y + pY < fieldHeight)
				{
					if(tetromino[_tetromino][pI] == L'X' && field[fI] != 0)
					{
						return false;
					}
				}
		}

	return true;
}

#pragma region DEFAULT COMMENTS
// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
#pragma endregion
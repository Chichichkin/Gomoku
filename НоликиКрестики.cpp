#define GLEW_STATIC
#include <stdio.h>
#include <conio.h>
#include <math.h>
#include <ctime>
#include <malloc.h>
#include <Windows.h>
#include <GL/glew.h>
#include <Gl/glut.h>
#include <GLFW/glfw3.h>



int N, M;//размер поля
int scale = 35;
int W, H;//ширина и Высота
int MaxW = GetSystemMetrics(SM_CXSCREEN),MaxH = GetSystemMetrics(SM_CYSCREEN);//Максимальные Ширина и Высота
bool Gamer = 0;
int count = 0;
int GameMode = 1;
bool GameStat = 1;
int NumPotenCell = 0;
int PlayerLastX, PlayerLastY, AILastX, AILastY;
int WinX1, WinX2, WinY1, WinY2;


struct Card
{
	int x;
	int y;
	int value;
	int potential;
};
struct Card **cell;
struct Card2
{
	int x;
	int y;
	int potential;
};
struct Card2 *Poten;
const char *PlayerPatern[] = {
	"22222", // пять в ряд (финальная выигрышная линия) 99999
	"022220", // Открытая четверка 7000
	"02222", // Полузакрытая четверка. Один ход до победы, но соперник может заблокировать 4000
	"22220", // Полузакрытая четверка. Один ход до победы, но соперник может заблокировать 4000
	"02220", // Открытая тройка (как 2 полузакрытых) 3000
	"020222", // Четверка с брешью. Один ход до победы, но соперник может заблокировать 2000
	"022022", // Четверка с брешью. Один ход до победы, но соперник может заблокировать 2000 
	"022202", // Четверка с брешью. Один ход до победы, но соперник может заблокировать 2000 
	"222020", // Четверка с брешью. Один ход до победы, но соперник может заблокировать 2000
	"220220", // Четверка с брешью. Один ход до победы, но соперник может заблокировать 2000
	"202220", // Четверка с брешью. Один ход до победы, но соперник может заблокировать 2000
	"0222", // Полузакрытая тройка 1500
	"2220", // Полузакрытая тройка 1500
	"02202", // Тройка с брешью 800
	"02022", // Тройка с брешью 800
	"22020", // Тройка с брешью 800
	"20220", // Тройка с брешью 800
	"0220", // Открытая двойка 200
	"220", //Полузакрытая двойка 100
	"022" //Полузакрытая двойка 100
};
const char *AIPatern[] = {
	"11111", // пять в ряд (финальная выигрышная линия) 99999
	"011110", // Открытая четверка 15000
	"01111", // Полузакрытая четверка. Один ход до победы, но соперник может заблокировать 7000
	"11110", // Полузакрытая четверка. Один ход до победы, но соперник может заблокировать 7000
	"01110", // Открытая тройка (как 2 полузакрытых) 3000
	"010111", // Четверка с брешью. Один ход до победы, но соперник может заблокировать 2000
	"011011", // Четверка с брешью. Один ход до победы, но соперник может заблокировать 2000 
	"011101", // Четверка с брешью. Один ход до победы, но соперник может заблокировать 2000 
	"111010", // Четверка с брешью. Один ход до победы, но соперник может заблокировать 2000
	"110110", // Четверка с брешью. Один ход до победы, но соперник может заблокировать 2000
	"101110", // Четверка с брешью. Один ход до победы, но соперник может заблокировать 2000
	"0111", // Полузакрытая тройка 1500
	"1110", // Полузакрытая тройка 1500
	"01101", // Тройка с брешью 800
	"01011", // Тройка с брешью 800
	"11010", // Тройка с брешью 800
	"10110", // Тройка с брешью 800
	"0110", // Открытая двойка 200
	"110", // Полузакрытая двойка 100
	"011", // Полузакрытая двойка 100
};
int weight[20] = { 99999,15000,7000,7000,3000,2000,2000,2000,2000,2000,2000,1500,1500,800,800,800,800,200, 100,100 };

void reshape(int w, int h)
{
	glViewport(0, 0, w, h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, w, h, 0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}
void display();
void DrawField();
void drawCircle(float x, float y, float r, int amountSegments);
void timer(int);
void MousePressed(int button, int state, int x, int y);
void Check(int row, int col);
void CheckVertical(int row, int col);
void CheckHorizontal(int row, int col);
void CheckMainDiagonal(int row, int col);
void CheckBackDiagonal(int row, int col);
void CheckAll();
void DrawEndLine();
void AI();
void addPoten(int row, int col);
void CheckPoten(int row, int col);
void FindMove();
void FindEnemyMove();
void ChooseMove();
int CompareVertical(int Number, int player);
int CompareHorizontal(int Number, int player);
int CompareDiagonal(int Number, int player);
int CompareBackDiagonal(int Number, int player);
void quickSort(struct Card2 * numbers, int left, int right);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char ** argv)
{
	int i, j;
	system("chcp 1251");
	system("cls");
	printf("Выберите режим игры:\n");
	printf("1 - игрок против игрока\n");
	printf("2 - игра против компьютера\n");
	scanf("%d", &GameMode);
	printf("Введите количество клеток в высоту и в ширину\n");
	scanf("%d%d", &M, &N);
	W = N * scale;
	H = M * scale;
	Poten = (struct Card2 *)calloc(1, sizeof(struct Card2));
	cell = (struct Card **)calloc(M, sizeof(struct Card *));
	for (i = 0; i < M; i++)
		cell[i] = (struct Card*)calloc(N, sizeof(struct Card));
	for (i = 0; i < M; i++)
		for (j = 0; j < N; j++)
		{
			cell[i][j].x = i;
			cell[i][j].y = j;
			cell[i][j].value = -1;
			cell[i][j].potential = -100;
		}
	if (GameMode == 2)
	{
		cell[M / 2][N / 2].x = N / 2;
		cell[M / 2][N / 2].y = M / 2;
		cell[M / 2][N / 2].value = Gamer;
		AILastX = N / 2;
		AILastY = M / 2;

		if (N/2 == N - 1 && W + 35 < MaxW)
		{
			N++;
			W = N * scale;
			for (i = 0; i < M; i++)
				cell[i] = (struct Card*)realloc(cell[i], N * sizeof(struct Card));
			for (i = 0; i < M; i++)
			{
				cell[i][N - 1].x = i;
				cell[i][N - 1].y = N - 1;
				cell[i][N - 1].value = -1;
				cell[i][N - 1].potential = -100;
			}
		}
		if (M/2 == M - 1 && H + 35 < MaxH)
		{
			M++;
			H = M * scale;
			cell = (struct Card **)realloc(cell, M * sizeof(struct Card *));
			cell[M - 1] = (struct Card*)calloc(N, sizeof(struct Card));
			for (i = 0; i < N; i++)
			{
				cell[M - 1][i].x = M - 1;
				cell[M - 1][i].y = i;
				cell[M - 1][i].value = -1;
				cell[M - 1][i].potential = -100;
			}
		}

		if (N/2-1 == 0 && W + 35 < MaxW)
		{
			N++;
			W = N * scale;
			for (i = 0; i < M; i++)
				cell[i] = (struct Card*)realloc(cell[i], N * sizeof(struct Card));
			for (i = 0; i < M; i++)
				for (j = N - 1; j > 0; j--)
				{
					cell[i][j].x = i;
					cell[i][j].y = j;
					cell[i][j].value = cell[i][j - 1].value;
					cell[i][j].potential = cell[i][j - 1].potential;
				}
			for (i = 0; i < M; i++)
			{
				cell[i][0].x = i;
				cell[i][0].y = 0;
				cell[i][0].value = -1;
				cell[i][0].potential = -100;
			}
		}
		if (M/2-1 == 0 && H + 35 < MaxH)
		{
			M++;
			H = M * scale;
			cell = (struct Card **)realloc(cell, M * sizeof(struct Card *));
			cell[M - 1] = (struct Card*)calloc(N, sizeof(struct Card));
			for (i = M - 1; i > 0; i--)
				for (j = 0; j < N; j++)
				{
					cell[i][j].x = i;
					cell[i][j].y = j;
					cell[i][j].value = cell[i - 1][j].value;
					cell[i][j].potential = cell[i - 1][j].potential;
				}
			for (i = 0; i < N; i++)
			{
				cell[0][i].x = 0;
				cell[0][i].y = j;
				cell[0][i].value = -1;
				cell[0][i].potential = -100;
			}

		}
		Gamer = !Gamer;
		addPoten(AILastY, AILastX);

	}
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(W + 25,H + 25);
	glutCreateWindow("Крестик & Нолик");

	glClearColor(0, 0, 0, 1);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0,W,H,0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glutReshapeFunc(reshape);

	glutDisplayFunc(display);
	glutMouseFunc(MousePressed);
	glutTimerFunc(100, timer, 0);


	glutMainLoop();

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void DrawField()
{
	int i;
	glColor3f(100, 0, 0);
	glLineWidth(3);
	glBegin(GL_LINES);
	
	for (i = 0; i < W; i += scale)
	{
		glVertex2i(i, 0);
		glVertex2i(i, H);
	}
	for (i = 0; i < H; i += scale)
	{
		glVertex2i(0, i);
		glVertex2i(W, i);
	}
	glEnd();
}
void drawCircle(float x, float y, float r, int amountSegments)
{
	glColor3f(0, 0, 100);
	glLineWidth(4);
	glBegin(GL_LINE_LOOP);

	for (int i = 0; i < amountSegments; i++)
	{
		float angle = 2.0 * 3.1415926 * float(i) / float(amountSegments);

		float dx = r * cosf(angle);
		float dy = r * sinf(angle);

		glVertex2f(x + dx, y + dy);
	}

	glEnd();
} 
void DrawCards()
{
	for (int i = 0; i < M; i++)
	{
		for (int j = 0; j < N; j++)
		{
			if (cell[i][j].value != -1)
			{
				glLineWidth(2);
				if (cell[i][j].value == 0)
				{
					glColor3f(50, 255, 0);
					glLineWidth(4);
					glBegin(GL_LINES);
					glVertex2i(j * scale + 5, i * scale + 5);
					glVertex2i(j * scale + scale - 5, i * scale + scale - 5);
					glVertex2i(j * scale + 5 , i * scale + scale - 5);
					glVertex2i(j * scale + scale - 5, i * scale + 5);
					glEnd();
				}
				if (cell[i][j].value == 1)
				{
					drawCircle((j * scale + (scale / 2)) - 1, (i * scale + (scale / 2)), 12, 20);
				}
			}
		}
	}
	
	{											//Рамки
		{
			glBegin(GL_QUADS);
			glColor3f(100, 0, 0);
			glVertex2i(0, H);
			glVertex2i(0, H + 25);
			glVertex2i(W, H + 25);
			glVertex2i(W, H);
			glEnd();
		}
		{
			glBegin(GL_QUADS);
			glColor3f(150, 0, 200);
			glVertex2i(5, H+5);
			glVertex2i(5, H + 20);
			glVertex2i(W, H + 20);
			glVertex2i(W, H+5);
			glEnd();
		}
		{
			glBegin(GL_QUADS);
			glColor3f(100, 0, 0);
			glVertex2i(W, 0);
			glVertex2i(W, H);
			glVertex2i(W+25, H);
			glVertex2i(W+25, 0);
			glEnd();
		}
		{
			glBegin(GL_QUADS);
			glColor3f(150, 0, 200);
			glVertex2i(W+5, 0);
			glVertex2i(W+5, H);
			glVertex2i(W + 20, H);
			glVertex2i(W + 20, 0);
			glEnd();
		}
		{
			glBegin(GL_QUADS);
			glColor3f(100, 0, 0);
			glVertex2i(W, H);
			glVertex2i(W, H+25);
			glVertex2i(W + 25, H+25);
			glVertex2i(W + 25, H);
			glEnd();
		}
		{
			glBegin(GL_QUADS);
			glColor3f(150, 0, 200);
			glVertex2i(W+5, H + 5);
			glVertex2i(W+5, H + 20);
			glVertex2i(W + 20, H + 20);
			glVertex2i(W + 20, H+5);
			glEnd();
		}
	}
}
void display()
{
	glClear(GL_COLOR_BUFFER_BIT);
	if (GameStat)
	{
		DrawField();
		DrawCards();
	}
	else
	{
		DrawField();
		DrawCards();
		DrawEndLine();
	}
	glutSwapBuffers();
}
void timer(int)
{
	display();
	glutTimerFunc(100, timer, 0);
}
void MousePressed(int button, int state, int x, int y)
{
	bool check1 = 1, check2 = 1;
	int i,j,k;
	if (state == GLUT_DOWN && GameStat)
	{
		if ((Gamer == 1 && GameMode == 2) || GameMode == 1)
		{
			int row = y / scale;
			int col = x / scale;
			

			if (cell[row][col].value == -1)
			{
				cell[row][col].value = Gamer;
				PlayerLastX = col;
				PlayerLastY = row;
				if (cell[row][col].potential == 0)
					CheckPoten(row, col);
			}
			else
				return;
			Check(row, col);
			if (GameStat != 0)
			{
				if (col == N - 1 && W + 35 < MaxW)
				{
					N++;
					W = N * scale;
					for (i = 0; i < M; i++)
						cell[i] = (struct Card*)realloc(cell[i], N * sizeof(struct Card));
					for (i = 0; i < M; i++)
					{
						cell[i][N - 1].x = i;
						cell[i][N - 1].y = N - 1;
						cell[i][N - 1].value = -1;
						cell[i][N - 1].potential = -100;
					}
				}
				if (row == M - 1 && H + 35 < MaxH)
				{
					M++;
					H = M * scale;
					cell = (struct Card **)realloc(cell, M * sizeof(struct Card *));
					cell[M - 1] = (struct Card*)calloc(N, sizeof(struct Card));
					for (i = 0; i < N; i++)
					{
						cell[M - 1][i].x = M - 1;
						cell[M - 1][i].y = i;
						cell[M - 1][i].value = -1;
						cell[M - 1][i].potential = -100;
					}
				}
				if (col == 0 && W + 35 < MaxW)
				{
					N++;
					W = N * scale;
					for (i = 0; i < M; i++)
						cell[i] = (struct Card*)realloc(cell[i], N * sizeof(struct Card));
					for (i = 0; i < M; i++)
						for (j = N - 1; j > 0; j--)
						{
							cell[i][j].x = i;
							cell[i][j].y = j;
							cell[i][j].value = cell[i][j - 1].value;
							cell[i][j].potential = cell[i][j - 1].potential;
						}
					for (i = 0; i < M; i++)
					{
						cell[i][0].x = i;
						cell[i][0].y = 0;
						cell[i][0].value = -1;
						cell[i][0].potential = -100;
					}
					for (i = 0; i < NumPotenCell; i++)
						Poten[i].x++;
					if (GameMode == 2)
					{
						addPoten(row, col + 1);
						check1 = 0;
					}
				}
				if (row == 0 && H + 35 < MaxH)
				{
					M++;
					H = M * scale;
					cell = (struct Card **)realloc(cell, M * sizeof(struct Card *));
					cell[M - 1] = (struct Card*)calloc(N, sizeof(struct Card));
					for (i = M - 1; i > 0; i--)
						for (j = 0; j < N; j++)
						{
							cell[i][j].x = i;
							cell[i][j].y = j;
							cell[i][j].value = cell[i - 1][j].value;
							cell[i][j].potential = cell[i - 1][j].potential;
						}
					for (i = 0; i < N; i++)
					{
						cell[0][i].x = 0;
						cell[0][i].y = j;
						cell[0][i].value = -1;
						cell[0][i].potential = -100;
					}
					for (i = 0; i < NumPotenCell; i++)
						Poten[i].y++;
					if (GameMode == 2)
					{
						addPoten(row + 1, col);
						check2 = 0;
					}

				}
				if (check1 && check2)
					addPoten(row, col);
				for (i = 0; i < NumPotenCell; i++)
					printf("(%d, %d) Potential - %d\n", Poten[i].x, Poten[i].y, Poten[i].potential);
				printf("\n");
			}
		}
		glutReshapeWindow(W+25, H+25);
	}
	
	check1 = 1;
	check2 = 1;
	
	if (GameMode == 2 && GameStat && Gamer == 0)
	{
		AI();
		Check(AILastY,AILastX);
		if (GameStat != 0)
		{
			if (AILastX == N - 1 && W + 35 < MaxW)
			{
				N++;
				W = N * scale;
				for (i = 0; i < M; i++)
					cell[i] = (struct Card*)realloc(cell[i], N * sizeof(struct Card));
				for (i = 0; i < M; i++)
				{
					cell[i][N - 1].x = i;
					cell[i][N - 1].y = N - 1;
					cell[i][N - 1].value = -1;
					cell[i][N - 1].potential = -100;
				}
			}
			if (AILastY == M - 1 && H + 35 < MaxH)
			{
				M++;
				H = M * scale;
				cell = (struct Card **)realloc(cell, M * sizeof(struct Card *));
				cell[M - 1] = (struct Card*)calloc(N, sizeof(struct Card));
				for (i = 0; i < N; i++)
				{
					cell[M - 1][i].x = M - 1;
					cell[M - 1][i].y = i;
					cell[M - 1][i].value = -1;
					cell[M - 1][i].potential = -100;
				}
			}
			if (AILastX == 0 && W + 35 < MaxW)
			{
				N++;
				W = N * scale;
				for (i = 0; i < M; i++)
					cell[i] = (struct Card*)realloc(cell[i], N * sizeof(struct Card));
				for (i = 0; i < M; i++)
					for (j = N - 1; j > 0; j--)
					{
						cell[i][j].x = i;
						cell[i][j].y = j;
						cell[i][j].value = cell[i][j - 1].value;
						cell[i][j].potential = cell[i][j - 1].potential;
					}
				for (i = 0; i < M; i++)
				{
					cell[i][0].x = i;
					cell[i][0].y = 0;
					cell[i][0].value = -1;
					cell[i][0].potential = -100;
				}
				for (i = 0; i < NumPotenCell; i++)
					Poten[i].x++;
				addPoten(AILastY, AILastX +1);
				check1 = 0;
			}
			if (AILastY == 0 && H + 35 < MaxH)
			{
				M++;
				H = M * scale;
				cell = (struct Card **)realloc(cell, M * sizeof(struct Card *));
				cell[M - 1] = (struct Card*)calloc(N, sizeof(struct Card));
				for (i = M - 1; i > 0; i--)
					for (j = 0; j < N; j++)
					{
						cell[i][j].x = i;
						cell[i][j].y = j;
						cell[i][j].value = cell[i - 1][j].value;
						cell[i][j].potential = cell[i - 1][j].potential;
					}
				for (i = 0; i < N; i++)
				{
					cell[0][i].x = 0;
					cell[0][i].y = j;
					cell[0][i].value = -1;
					cell[0][i].potential = -100;
				}
				for (i = 0; i < NumPotenCell; i++)
					Poten[i].y++;
				addPoten(AILastY + 1, AILastX);
				check2 = 0;
			}
			if (check1 && check2)
				addPoten(AILastY, AILastX);
		}
		glutReshapeWindow(W + 25, H + 25);

	}
}
void Check(int row, int col)
{
	CheckVertical(row, col);
	CheckHorizontal(row, col);
	CheckMainDiagonal(row, col);
	CheckBackDiagonal(row, col);
	CheckAll();
	Gamer = !Gamer;
	++count;
}
void CheckVertical(int row, int col)
{
	int i,gear = 0,StartPos = row;
	bool Win = 0;
	for (i = row; i < M; i++)
	{
		gear++;
		if (gear == 5 && cell[i][col].value == Gamer)
		{
			Win = 1;
			WinX1 = col * scale + 17;
			WinY1 = StartPos * scale;
			WinX2 = col * scale + 17;
			WinY2 = (StartPos + 4)* scale + 35;
			break;
		}
		if (cell[i][col].value != Gamer)
		{
			StartPos = i-1;
			break;
		}
	}
	gear = 0;
	for (i = StartPos; i > 0; i--)
	{
		gear++;
		if (gear == 5 && cell[i][col].value == Gamer)
		{
			Win = 1;
			WinX1 = col * scale + 17;
			WinY1 = StartPos * scale + 35;
			WinX2 = col * scale + 17;
			WinY2 = (StartPos - 4)* scale;
			break;
		}
		if (cell[i][col].value != Gamer || i == 0)
			break;
	}

	if (Win)
	{
		printf("Вертикальный выйгрыш у игрока %d\n", Gamer+1);
		GameStat = 0;
	}
}
void CheckHorizontal(int row, int col)
{
	int i, gear = 0, StartPos = col;
	bool Win = 0;
	for (i = col; i < N; i++)
	{
		gear++;
		if (gear == 5 && cell[row][i].value == Gamer)
		{
			Win = 1;
			WinX1 = StartPos * scale;
			WinY1 = row * scale + 17;
			WinX2 = (StartPos + 4)* scale + 35;
			WinY2 = row * scale + 17;
			break;
		}
		if (cell[row][i].value != Gamer)
		{
			StartPos = i-1;
			break;
		}

	}
	gear = 0;
	for (i = StartPos; i > 0; i--)
	{
		gear++;
		if (gear == 5 && cell[row][i].value == Gamer)
		{
			Win = 1;
			WinX1 = StartPos * scale + 35;
			WinY1 = row * scale + 17;
			WinX2 = (StartPos - 4)* scale;
			WinY2 = row * scale + 17;
			break;
		}
		if (cell[row][i].value != Gamer || i == 0)
			break;
	}
	if (Win)
	{
		printf("Горизонтальный выйгрыш у игрока %d\n", Gamer+1);
		GameStat = 0;
		
	}
}
void CheckMainDiagonal(int row, int col)
{
	int i, j, gear = 0, StartPosI = row, StartPosJ = col;
	bool Win = 0;
	for (i = row,j = col; i < M && j < N; i++,j++)
	{
		gear++;
		if (gear == 5 && cell[i][j].value == Gamer)
		{
			Win = 1;
			WinX1 = StartPosJ * scale;
			WinY1 = StartPosI * scale;
			WinX2 = (StartPosJ + 4) * scale + 35;
			WinY2 = (StartPosI + 4) * scale + 35;
			break;
		}
		if (cell[i][j].value != Gamer)
		{
			StartPosI = i-1;
			StartPosJ = j-1;
			break;
		}
	}
	gear = 0;
	for (i = StartPosI, j = StartPosJ; i > 0 && j > 0; i--, j--)
	{
		gear++;
		if (gear == 5 && cell[i][j].value == Gamer)
		{
			Win = 1;
			WinX1 = StartPosJ * scale + 35;
			WinY1 = StartPosI * scale + 35;
			WinX2 = (StartPosJ - 4) * scale;
			WinY2 = (StartPosI - 4) * scale;
			break;
		}
		if (cell[i][j].value != Gamer)
			break;
	}
	
	if (Win)
	{
		printf("Диагональный выйгрыш у %d\n", Gamer + 1);
		GameStat = 0;
	}
}
void CheckBackDiagonal(int row, int col)
{
	int i, j, gear = 0, StartPosI = row, StartPosJ = col;
	bool Win = 0;
	for (i = row, j = col; i < M && j > 0; i++, j--)
	{
		gear++;
		if (gear == 5 && cell[i][j].value == Gamer)
		{
			Win = 1;
			WinX1 = StartPosJ * scale + 35;
			WinY1 = StartPosI * scale;
			WinX2 = (StartPosJ - 4) * scale;
			WinY2 = (StartPosI + 4)* scale + 35;
			break;
		}
		if (cell[i][j].value != Gamer)
		{
			StartPosI = i - 1;
			StartPosJ = j + 1;
			break;
		}
	}
	gear = 0;
	for (i = StartPosI, j = StartPosJ; i > 0 && j < N; i--, j++)
	{
		gear++;
		if (gear == 5 && cell[i][j].value == Gamer)
		{
			Win = 1;
			WinX1 = StartPosJ * scale;
			WinY1 = StartPosI * scale + 35;
			WinX2 = (StartPosJ + 4) * scale + 35;
			WinY2 = (StartPosI - 4)* scale;
			break;
		}
		if (cell[i][j].value != Gamer)
			break;
	}

	if (Win)
	{
		printf("Диагональный выйгрыш у %d\n", Gamer + 1);
		GameStat = 0;
		
	}
}
void CheckAll()
{
	if (count >= N * M - 1)
	{
		printf("Ничья!");
		GameStat = 0;
	}
}
void DrawEndLine()
{
	glLineWidth(4);
	glColor3f(0.9, 0 , 0.9);
	glBegin(GL_LINES);
	glVertex2i(WinX1, WinY1);
	glVertex2i(WinX2, WinY2);
	glEnd();

}
void AI()
{
	FindMove();
	FindEnemyMove();
	ChooseMove();
}
void addPoten(int row,int col)
{
	int i, k, LocalRow = row , LocalCol = col;
	if (row == 0)
		LocalRow++;
	if (col == 0)
		LocalCol++;
	for (i = 0; i < 3; i++)
		for (k = 0; k < 3; k++)
		{
			if (cell[LocalRow  - 1 + i][LocalCol - 1 + k].value ==-1 && cell[LocalRow - 1 + i][LocalCol - 1 + k].potential < 0 && (LocalRow - 1 + i != row || LocalCol - 1 + k != col))
			{
				Poten[NumPotenCell].x = LocalCol - 1 + k;
				Poten[NumPotenCell].y = LocalRow - 1 + i;
				Poten[NumPotenCell].potential = 0;
				cell[LocalRow - 1 + i][LocalCol - 1 + k].potential = 0;
				NumPotenCell++;
				Poten = (struct Card2*)realloc(Poten, (NumPotenCell + 1) * sizeof(struct Card2));
			}
				
		}

}
void CheckPoten(int row, int col)
{
	int i=0;
	for (i = 0; i < NumPotenCell; i++)
		if (row == Poten[i].y && col == Poten[i].x)
			break;
	for (i; i < NumPotenCell - 1; i++)
		Poten[i] = Poten[i + 1];
	Poten = (struct Card2*)realloc(Poten, NumPotenCell * sizeof(struct Card2));
	NumPotenCell--;
}
void FindMove()
{
	int i;
	for (i = 0; i < NumPotenCell; i++)
	{
		Poten[i].potential += CompareVertical(i, 1);
		Poten[i].potential += CompareHorizontal(i, 1);
		Poten[i].potential += CompareDiagonal(i, 1);
		Poten[i].potential += CompareBackDiagonal(i, 1);
	}
}
void FindEnemyMove()
{
	int i;
	for (i = 0; i < NumPotenCell; i++)
	{
		Poten[i].potential += 1.1*CompareVertical(i, 2);
		Poten[i].potential += 1.1*CompareHorizontal(i, 2);
		Poten[i].potential += 1.1*CompareDiagonal(i, 2);
		Poten[i].potential += 1.1*CompareBackDiagonal(i, 2);
	}
}
void ChooseMove()
{
	int i,max,j,gear = 0;
	quickSort(Poten, 0, NumPotenCell-1);
	j = NumPotenCell-2;
	max = Poten[NumPotenCell-1].potential;
	while (Poten[j].potential == max)
	{
		gear++;
		j--;
	}
	srand(time(NULL));              

	int RandMax = rand() % (gear+1) ;
	cell[Poten[NumPotenCell - 1 - RandMax].y][Poten[NumPotenCell - 1 - RandMax].x].value = Gamer;
	AILastX = Poten[NumPotenCell - 1 - RandMax].x;
	AILastY = Poten[NumPotenCell - 1 - RandMax].y;
	for (j = 0; j < NumPotenCell; j++)
		Poten[j].potential = 0;
	CheckPoten(AILastY, AILastX);

}
int CompareVertical(int Number, int player)
{
	int min = Poten[Number].y, max = Poten[Number].y, i, j = 0, col = Poten[Number].x, k;
	char *string;
	while (min > 0 && j < 5)
	{
		min--;
		j++;
	}
	j = 0;
	while (max < M-1 && j < 5)
	{
		max++;
		j++;
	}
	j = 0;
	string = (char*)calloc((max - min) + 2,sizeof(char));
	if (player == 1)
	{
		for (i = min; i < max + 1; i++)
		{
			if (i != Poten[Number].y)
			{
				if (cell[i][col].value == -1)
					string[j++] = '0';
				if (cell[i][col].value == 0)
					string[j++] = '1';
				if (cell[i][col].value == 1)
					string[j++] = '2';
			}
			else
				string[j++] = '1';
		}
	}
	else
	{
		for (i = min; i < max + 1; i++)
		{
			if (i != Poten[Number].y)
			{
				if (cell[i][col].value == -1)
					string[j++] = '0';
				if (cell[i][col].value == 0)
					string[j++] = '1';
				if (cell[i][col].value == 1)
					string[j++] = '2';
			}
			else
				string[j++] = '2';
		}
	}
	string[j] = '\0';
	j = 0;
	for (k = 0; k < 20; k++)
	{
		for (i = 0; i < strlen(string) + 1; i++)
		{
			if (player == 1)
			{
				if (string[i] == AIPatern[k][0])
					while (string[i + j] == AIPatern[k][j])
					{
						j++;
						if (strlen(AIPatern[k]) == j)
						{
							if (k == 0)
								return (weight[k]+50000);
							return weight[k];
						}
					}
				j = 0;
			}
			else
			{
				if (string[i] == PlayerPatern[k][0])
					while (string[i + j] == PlayerPatern[k][j])
					{
						j++;
						if (strlen(PlayerPatern[k]) == j)
						{
							return weight[k];
						}
					}
				j = 0;
			}
		}

	}
	free(string);
}
int CompareHorizontal(int Number, int player)
{
	int min = Poten[Number].x, max = Poten[Number].x, i, j = 0, row = Poten[Number].y, k;
	char *string;
	while (min > 0 && j < 5)
	{
		min--;
		j++;
	}
	j = 0;
	while (max < N-1 && j < 5)
	{
		max++;
		j++;
	}
	j = 0;
	string = (char*)calloc((max-min) + 2, sizeof(char));
	for (i = min; i < max+1; i++)
	{
		if (player == 1)
		{
			if (i != Poten[Number].x)
			{
				if (cell[row][i].value == -1)
					string[j++] = '0';
				if (cell[row][i].value == 0)
					string[j++] = '1';
				if (cell[row][i].value == 1)
					string[j++] = '2';
			}
			else
			{
				string[j++] = '1';
			}	
		}
		else
		{
			if (i != Poten[Number].x)
			{
				if (cell[row][i].value == -1)
					string[j++] = '0';
				if (cell[row][i].value == 0)
					string[j++] = '1';
				if (cell[row][i].value == 1)
					string[j++] = '2';
			}
			else
			{
				string[j++] = '2';
			}
		}
	
	}
	string[j] = '\0';
	j = 0;
	for (k = 0; k < 20; k++) /////////////////////////////////////////////////
	{
		for (i = 0; i < strlen(string) + 1; i++)
		{
			if (player == 1)
			{
				if (string[i] == AIPatern[k][0])
					while (string[i + j] == AIPatern[k][j])
					{
						j++;
						if (strlen(AIPatern[k]) == j)
						{
							if (k == 0)
								return (weight[k] + 50000);
							return weight[k];
						}
					}
				j = 0;
			}
			else
			{
				if (string[i] == PlayerPatern[k][0])
					while (string[i + j] == PlayerPatern[k][j])
					{
						j++;
						if (strlen(PlayerPatern[k]) == j)
						{
							return weight[k];
						}
					}
				j = 0;
			}
		}

	}
	free(string);
}
int CompareDiagonal(int Number, int player)
{
	int l, minCol = Poten[Number].x, minRow = Poten[Number].y, maxCol = Poten[Number].x, maxRow = Poten[Number].y, i, j = 0, row = Poten[Number].y, k,firstPart=0,secondPart=0;
	char *string;
	while (minCol > 0 && minRow > 0 && j < 5)
	{
		minCol--;
		minRow--;
		j++;
		firstPart = j;
	}
	j = 0;
	while (maxCol < N-1 && maxRow < M-1 && j < 5)
	{
		maxCol++;
		maxRow++;
		j++;
		secondPart = j;
	}
	j = 0;
	string = (char*)calloc((firstPart+secondPart) + 2, sizeof(char));
	for (i = minRow,k = minCol; i < maxRow + 1 && k < maxCol + 1; i++,k++)
	{
		if (player == 1)
		{
			if (i != Poten[Number].y || k != Poten[Number].x)
			{
				if (cell[i][k].value == -1)
					string[j++] = '0';
				if (cell[i][k].value == 0)
					string[j++] = '1';
				if (cell[i][k].value == 1)
					string[j++] = '2';
			}
			else
				string[j++] = '1';
				
		}
		else
		{
			if (i != Poten[Number].y || k != Poten[Number].x)
			{
				if (cell[i][k].value == -1)
					string[j++] = '0';
				if (cell[i][k].value == 0)
					string[j++] = '1';
				if (cell[i][k].value == 1)
					string[j++] = '2';
			}
			else
				string[j++] = '2';
		}
	}
	string[j] = '\0';
	j = 0;
	for (k = 0; k < 20; k++)
	{
		for (i = 0; i < strlen(string) + 1; i++)
		{
			if (player == 1)
			{
				if (string[i] == AIPatern[k][0])
					while (string[i + j] == AIPatern[k][j])
					{
						j++;
						if (strlen(AIPatern[k]) == j)
						{
							if (k == 0)
								return (weight[k] + 50000);
							return weight[k];
						}
					}
				j = 0;
			}
			else
			{
				if (string[i] == PlayerPatern[k][0])
					while (string[i + j] == PlayerPatern[k][j])
					{
						j++;
						if (strlen(PlayerPatern[k]) == j)
						{
							return weight[k];
						}
					}
				j = 0;
			}
		}

	}
	free(string);
}
int CompareBackDiagonal(int Number,int player)
{
	int minCol = Poten[Number].x, minRow = Poten[Number].y, maxCol = Poten[Number].x, maxRow = Poten[Number].y, i, j = 0, row = Poten[Number].y, k, firstPart = 0, secondPart = 0;
	char *string;
	while (minCol < N-1 && minRow > 0 && j < 5)
	{
		minCol++;
		minRow--;
		j++;
		firstPart = j;
	}
	j = 0;
	while (maxCol > 0 && maxRow < M-1 && j < 5)
	{
		maxCol--;
		maxRow++;
		j++;
		secondPart = j;
	}
	j = 0;
	string = (char*)calloc((firstPart + secondPart) + 2, sizeof(char));
	for (i = minRow, k = minCol; i < maxRow + 1 && k > maxCol - 1; i++, k--)
	{
		if (player == 1)
		{
			if (i != Poten[Number].y || k != Poten[Number].x)
			{
				if (cell[i][k].value == -1)
					string[j++] = '0';
				if (cell[i][k].value == 0)
					string[j++] = '1';
				if (cell[i][k].value == 1)
					string[j++] = '2';
			}
			else
				string[j++] = '1';
		}
		else
		{
			if (i != Poten[Number].y && k != Poten[Number].x)
			{
				if (cell[i][k].value == -1)
					string[j++] = '0';
				if (cell[i][k].value == 0)
					string[j++] = '1';
				if (cell[i][k].value == 1)
					string[j++] = '2';
			}
			else
				string[j++] = '2';
		}
	}
	string[j] = '\0';
	j = 0;
	for (k = 0; k < 20; k++)
	{
		for (i = 0; i < strlen(string) + 1; i++)
		{
			if (player == 1)
			{
				if (string[i] == AIPatern[k][0])
					while (string[i + j] == AIPatern[k][j])
					{

						j++;
						if (strlen(AIPatern[k]) == j)
						{
							if (k == 0)
								return (weight[k] + 50000);
							return weight[k];
						}
					}
				j = 0;
			}
			else
			{
				if (string[i] == PlayerPatern[k][0])
					while (string[i + j] == PlayerPatern[k][j])
					{
						j++;
						if (strlen(PlayerPatern[k]) == j)
						{
							return weight[k];
						}
					}
				j = 0;
			}
		}

	}
	free(string);
}
void quickSort(struct Card2 * numbers, int left, int right)
{
	struct Card2 change;
	struct Card2 pivot; 
	int l_hold = left;
	int r_hold = right;
	int pivot2;
	pivot = numbers[left];
	while (left < right)
	{
		while ((numbers[right].potential >= pivot.potential) && (left < right))
			right--;
		if (left != right)
		{
			numbers[left] = numbers[right];
			left++;
		}
		while ((numbers[left].potential <= pivot.potential) && (left < right))
			left++;
		if (left != right)
		{
			numbers[right] = numbers[left];
		}
	}
	numbers[left] = pivot;
	pivot2 = left;
	left = l_hold;
	right = r_hold;
	if (left < pivot2)
		quickSort(numbers, left, pivot2 - 1);
	if (right > pivot2)
		quickSort(numbers, pivot2 + 1, right);
}

#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <math.h>
#include <ctime>
#include <malloc.h>
#include <fstream>
#include <SDL_ttf.h>
#include "stack.h"
using namespace std;
#define gravity 20.1
int sum;
int attack_speed=300;
int time_dash = 500;
int stanlock = 300;
int immortal_time_hero = 700;
int wave_survive = 0;

 #pragma region kadrovka
int x_hero_sprite = 0;
int y_hero_sprite = 0;
int w_hero_sprite = 80;
int frame = 0;
int frame_count = 9;
int frame_for_sword = 0;
#pragma endregion
enum idblocks{PLATFORM=20,WALL,THORNS};
enum direction { UP = 100, DOWN, LEFT,RIGHT };
enum IDenemy {TUCHKA=-6,LIFESTEALB=-5,BIGEB,DAMAGEB, HILKA,MONETKA, BASIC = 1,FLAING, RUSHER, BOMBER,LASER};
enum statusenemy { IMMORTAL = 1, FALLING, STAY, GOING, RUSH };
int wig = 800, heg = 600;
int size_w_hero;
int size_h_hero;
SDL_Window* win = NULL;
SDL_Renderer* ren = NULL;


int DeInit(int error)
{
	if (ren != NULL) SDL_DestroyRenderer(ren);
	if (win != NULL) SDL_DestroyWindow(win);
	SDL_Quit();
	IMG_Quit();
	TTF_Quit();
	exit(error);

}
void Init()
{
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		printf("Couldn't init SDL! Error: %s", SDL_GetError());
		system("pause");
		DeInit(1);
	}

	int res;
	if ((res = IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG)) == 0)
	{
		printf("Couldn't init SDL_image! Error: %s", SDL_GetError());
		system("pause");
		DeInit(1);
	}

	if (TTF_Init())
	{
		printf("Couldn't init TTF! Error: %s", SDL_GetError());
		system("pause");
		DeInit(1);
	}
	win = SDL_CreateWindow("Window",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, wig, heg,
		SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	if (win == NULL)
	{
		printf("Couldn't create window! Error: %s", SDL_GetError());
		system("pause");
		DeInit(1);
	}

	ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
	if (ren == NULL)
	{
		printf("Couldn't create renderer! Error: %s", SDL_GetError());
		system("pause");
		DeInit(1);
	}

}

int random(int a, int b)
{
	return rand() % (b - a + 1) + a;
}
float *platforms;
int sumplat = 0;
struct enemy
{
	int id;
	int status;
	int gainmoney;
	float x;
	float y;
	float timex;
	float timey;
	bool flag;
	int size;
	int max_hp;
	int cor_hp;
	int damage;
	float speed;
	float vy;
	float vx;
	int attack_delay;
	int cor_delay;
	int cor_immortal_time;
	int animation;
	SDL_Rect enemy_texture;
};



void createplatform()
{
	int randi = random(1,4);
		char str[100];
		//float h0, x0,h,x1;
	switch(randi)
	{
	case 1:
		strcpy_s(str, "levels\\platform.txt");
		break;
	case 2:
		strcpy_s(str, "level1.txt");
	
		break;
	case 3:
		strcpy_s(str, "levels\\level2.txt");

		break;
	case 4:
		strcpy_s(str, "levels\\level3.txt");

		break;
	}
	sum = 0;
	FILE* f;
	fopen_s(&f, str, "rt");
	while (!feof(f))
	{
		fscanf(f,"%*f");
		sum++;
	}
	fclose(f);
	fopen_s(&f, str, "rt");
	platforms = (float*)malloc(sum * sizeof(float));
	for (int i = 0; i < sum; i++)
	{
		fscanf(f, "%f", &platforms[i]);
		if (platforms[i] >= PLATFORM)
			sumplat++;
	}
	fclose(f);
	//plat - float h, x0,x1;
	//wall - float h0, x0,h,x1;
}

void renderplatform(SDL_Texture* pl, SDL_Texture* wal)
{
	for (int i = 0; i < sum; i++)
	{
		if (platforms[i] == PLATFORM)
		{
			float h, x0, x1;
			float  coordx0, coordx1;
			int coordh;
			h = platforms[i + 1]; x0 = platforms[i + 2]; x1 = platforms[i + 3];
			coordh = heg * (1 - h);
			coordx0 = wig * x0;
			coordx1 = wig * x1- coordx0;
			SDL_Rect plata = { coordx0,coordh,coordx1,10 };
			SDL_RenderCopy(ren, pl, NULL, &plata);
		}
		if (platforms[i] == WALL)
		{
			float h0, x0, h, x1;
			float  leftline, rightline, upline, downline;
			h0 = platforms[i + 1]; x0 = platforms[i + 2]; h = platforms[i + 3]; x1 = platforms[i + 4];
			
			SDL_Rect wala = { wig * x0,heg * h0,wig * x1,heg * h };
			SDL_RenderCopy(ren, wal, NULL, &wala);
		}
	}


}

bool in_wall(float x, float y,int size,float &x1,float &y1)
{
	for (int i = 0; i < sum ; i++)
	{
		if (platforms[i] == WALL)
		{
			float xw1 = platforms[i + 2] * wig;
			float xw2 = (platforms[i + 2] + platforms[i + 4]) * wig;
			float yw1 = platforms[i + 1] * heg;
			float yw2 = (platforms[i + 1] + platforms[i + 3]) * heg;
			if ((x + size > xw1 )and (x <xw2))
			{

				if ((y1 + size >yw1) and (y1 < yw2))
				{
					if (y1 + size < yw1 and y1>yw2 )
					{
						if (abs(xw1 - x) < abs(xw2 - x - size))
							x1 = xw1 - size;

						else
							x1 = xw2;
					}

					if(x1+size>xw1 and x1<xw2 )
					{
						if (abs(yw1 - y) < abs(yw2 - y - size))
							y1 = yw1 - size;
						else
							y1 = yw2;
					}
					//printf("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\n");
					return true;
				}
			}

		}
	}
	
	return false;
}
bool in_wall_fly(enemy& en)
{
	for (int i = 0; i < sum; i++)
	{
		if (platforms[i] == WALL)
		{
			float xw1 = platforms[i + 2] * wig;
			float xw2 = (platforms[i + 2] + platforms[i + 4]) * wig;
			float yw1 = platforms[i + 1] * heg;
			float yw2 = (platforms[i + 1] + platforms[i + 3]) * heg;
			if(en.y+en.size>yw1 and en.y<yw2)
			{
				if ((en.x + en.size + en.vx > xw1) and (en.x + en.size < xw1))
				{
					en.x = xw1- en.size-1;
				
					en.vx = 0;
				

				}
				else if ((en.x +en.vx < xw2) and (en.x > xw2))
				{
					en.x = xw2+1;
					
					en.vx = 0;
			
				}
			}
			if (en.x + en.size > xw1 and en.x < xw2)
			{
				if ((en.y + en.size + en.vy > yw1) and (en.y + en.size < yw1))
				{
					en.y = yw1-1-en.size;
					
					en.vy = 0;
				

				}
				else if ((en.y + en.vy < yw2) and (en.y > yw2))
				{
					en.y = yw2+1;
					
					en.vy = 0;
				

				}
				
			}
		}
	}
	en.y += en.vy;
	en.x += en.vx;
	return false;
}
bool on_platform(float x, float &y, float vy, int dt, int size)
{
	float h, x0, x1;
	float  coordx0, coordx1;
	int coordh;
	for (int i = 0; i < sum; i++)
	{
		if (platforms[i] == PLATFORM)
		{
			h = platforms[i + 1]; x0 = platforms[i + 2]; x1 = platforms[i + 3];
			coordh = heg * (1 - h);
			coordx0 = wig * x0;
			coordx1 = wig * x1;
		
		
			if ((x > coordx0 - size) and (x < coordx1))
				if ((int(y) <= (coordh - size)) and (int(y) + vy * dt / 16 >= (coordh - size)) and vy >= 0)
				{
					y = coordh - size;
					//printf("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\n");
					return true;
				}
		}
		if (platforms[i] == WALL)
		{
			h = platforms[i + 1]; x0 = platforms[i + 2]; x1 = platforms[i + 4];
			coordh = heg *  h;
			coordx0 = wig * x0;
			coordx1 = wig * x1+ coordx0;


			if ((x > coordx0 - size) and (x < coordx1))
				if ((int(y) <= (coordh - size)) and (int(y) + vy * dt / 16 >= (coordh - size)) and vy >= 0)
				{
					y = coordh - size;
					//printf("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\n");
					return true;
				}
		}
	
	}


	return false;
}


int touch_enemy_hero(float x, float y, enemy en)
{
	if ((en.x <= x and en.x + en.size >= x + size_w_hero) or (en.x >= x and en.x <= x + size_w_hero) or (en.x + en.size >= x and en.x + en.size <= x + size_w_hero))
		if ((en.y <= y and en.y + en.size >= y + size_h_hero) or (en.y >= y and en.y <= y + size_h_hero) or (en.y + en.size >= y and en.y + en.size <= y + size_h_hero))
		{
			if ((x + size_w_hero / 2) < (en.x + en.size))
				return LEFT;
			else
				return RIGHT;
		}
	return false;
}

int touch_laser(float x, float y,int enx,int eny, int enx1, int eny1)
{
	if ((enx <= x and enx + enx1 >= x + size_w_hero) or (enx >= x and enx <= x + size_w_hero) or (enx + enx1 >= x and enx + enx1 <= x + size_w_hero))
		if ((eny <= y and eny + eny1 >= y + size_h_hero) or (eny >= y and eny <= y + size_h_hero) or (eny + eny1 >= y and eny + eny1 <= y + size_h_hero))
		{
			if ((x + size_w_hero / 2) < (enx + eny1))
				return LEFT;
			else
				return RIGHT;
		}
	return false;
}




void spawn_enemy_air(enemy& en,float x, float y)
{
	en.x = random(0, wig - en.size);
	en.y = random(0, heg - en.size);
				do{
					en.y += 100;
				}while ((sqrt(((x - en.x) * (x - en.x)) + ((y - en.y) * (y - en.y))) < 200));
				
				in_wall(en.x, en.y, en.size, en.x, en.y);
		
		
	
}
void spawn_enemy_ground(enemy& en,int* count,bool flagg)
{
	bool flag = false;
	int n;
	do
	{
		 n = random(1, sumplat);
		 for (int i = 0; i < sumplat; i++)
		 {
			 if (count[i] == n)
				 break;
			 if (count[i] == 0)
			 {
				 count[i] = n;
				 flag = true;
				 break;
			 }
		 }
	} while (!flag or flagg);
		int plat = 0;
	for (int i = 0; i < sum; i++)
	{
		if (platforms[i] >= PLATFORM)
			plat++;
		if (plat == n)
		{
			
			do
			{
				flag = true;

				if (platforms[i] == PLATFORM)
				{

					en.x = random(max(0, int(platforms[i + 2] * wig)), min(wig, int(platforms[i + 3] * wig)) - en.size);
					//en.x = platforms[i + 2] + rand() % (int(platforms[i + 3] - platforms[i + 2]));

					en.y = (1 - platforms[i + 1]) * heg - en.size - 10;
					
				}
				if (platforms[i] == WALL)
				{

					en.x = random(max(0, int(platforms[i + 2] * wig)), min(wig, int((platforms[i + 4] + platforms[i + 2]) * wig) - en.size));
					//en.x = platforms[i + 2] + rand() % int(platforms[i + 4]);
					en.y = platforms[i + 1] * heg - en.size - 10;
				}
				if (in_wall(en.x, en.y, en.size, en.x, en.y))
				{
					printf("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaZ\n");
					flag = false;
				}
			} while (!flag);
			break;
		}
	}
}


bool logicplatform(float &vy, float &y,float &x,int dt)
{
	float h, x0,x1;
	float  coordx0, coordx1;
	int coordh;
	for (int i = 0; i < sum - 3; i ++)
	{
		if (platforms[i] == PLATFORM)
		{
			h = platforms[i + 1]; x0 = platforms[i + 2]; x1 = platforms[i + 3];
			coordh = heg * (1 - h);
			coordx0 = wig * x0;
			coordx1 = wig * x1;
			SDL_SetRenderDrawColor(ren, 200, 0, 0, 255);
			SDL_RenderDrawLine(ren, coordx0, coordh, coordx1, coordh);
			if ((x > coordx0 - size_w_hero) and (x < coordx1))
				if ((int(y) <= coordh - size_h_hero) and (int(y) + vy * dt/16 >= (coordh - size_h_hero)) and vy>=0)
				{
					y = coordh - size_h_hero;
					//printf("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\n");
						return true;
				}
		}
	}
	

	return false;
}
int logicwall(float& vy, float& vx, float& y, float& x,int dt)
{
	
	float h0, x0,h,x1;
	float  leftline, rightline, upline, downline;
	bool flag = false;
	for (int i = 0; i < sum - 4; i ++)
	{
		if (platforms[i] == WALL)
		{
			h0 = platforms[i + 1]; x0 = platforms[i + 2]; h = platforms[i + 3];x1 = platforms[i + 4];
			leftline = x0 * wig; rightline = (x0 + x1) * wig; upline = h0 * heg; downline=heg*(h0+h);
			SDL_SetRenderDrawColor(ren, 200, 0, 200, 255);
			SDL_Rect r = { wig *x0,heg*h0,wig*x1,heg*h};
			SDL_RenderDrawRect(ren, &r);

			if (x + vx * dt / 16 + size_w_hero >= leftline and int(x) + size_w_hero <= leftline and int(y) >= upline- size_h_hero and int(y) <= downline)
			{
				vx = 0;
				x = leftline - size_w_hero;
				flag = 1;
			}
			 if (int(x) + vx * dt / 16 <= rightline and x >= int(rightline) and int(y) >= upline - size_h_hero and int(y) <= downline)
			{
				//printf("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
				vx = 0;
				x= rightline;
				flag = 1;
			}
			 if (int(y) >= downline and int(y) +vy * dt / 16 <= downline)
				 if((int(x) < rightline and int(x) > leftline - size_w_hero)or (int(x) + size_w_hero <= leftline and int(x) + size_w_hero + vx * dt / 16 > leftline) or (int(x) >= rightline and int(x) + vx * dt / 16 < rightline))
			{
					//printf("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
				vy = 0;
				y = downline;
				flag = 1;
			}
			 if (int(y) <= upline- size_h_hero +5 and int(y) + vy * dt / 16 >= upline- size_h_hero -1 )
				 if((int(x) < rightline and int(x) > leftline - size_w_hero)or (int(x) + size_w_hero <= leftline and int(x) + size_w_hero + vx * dt / 16 > leftline) or (int(x) >= rightline and int(x) + vx * dt / 16 < rightline))
			{
			/*	printf("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");*/
				vy = 0;
				y = upline - size_h_hero;
				return 2;
			}
		}
	}

		return flag;
}


bool keep_dist_rusher(float x, float y, float min, float max, enemy& en)
{
	float disty = (y + size_h_hero / 2) - (en.y + en.size);
	float distx = (x + size_w_hero / 2) - (en.x + en.size);
	float dist = sqrt(disty * disty + distx * distx);

	 if (dist > max)
	{
		en.vx = distx / dist * en.speed;
		en.vy = (disty / dist+0.2) * en.speed;
	//	printf("AAAAAAAAAA");
	}
	else if (dist < min)
	{
		if (disty < 0)
		{
			if (distx < 0)
				en.vx = -disty / dist * en.speed;
			else
				en.vx = disty / dist * en.speed;
			en.vy = -abs(distx) / dist * en.speed;
		}
		else
		{
			/*en.vx = -distx / dist * en.speed;
			en.vy = -disty / dist * en.speed;*/

			if (distx < 0)
				en.vx = -disty / dist * en.speed;
			else
				en.vx = disty / dist * en.speed;
			en.vy = -abs(distx) / dist * en.speed;
			if (abs(distx) > abs(disty))
			{
				en.vx /= -2;
			
			}
		}
	}
	else
		return true;
	return false;

}

bool keep_dist_laser(float x, float y, float min, float max, enemy& en)
{
	float disty = (y + size_h_hero / 2) - (en.y + en.size);
	float distx = (x + size_w_hero / 2) - (en.x + en.size);
	float dist = sqrt(disty * disty + distx * distx);

	if (dist > max)
	{
		en.vx = distx / dist * en.speed;
		en.vy = (disty / dist + 0.2) * en.speed;
		//	printf("AAAAAAAAAA");
	}
	else if (dist < min)
	{
		if (disty < 0)
		{
			if (distx < 0)
				en.vx = -disty / dist * en.speed;
			else
				en.vx = disty / dist * en.speed;
			en.vy = -abs(distx) / dist * en.speed;
		}
		else
		{
			/*en.vx = -distx / dist * en.speed;
			en.vy = -disty / dist * en.speed;*/

			if (distx < 0)
				en.vx = -disty / dist * en.speed;
			else
				en.vx = disty / dist * en.speed;
			en.vy = -abs(distx) / dist * en.speed-3;
		}
	}
	else
		return true;
	return false;

}
int check_out_dist(enemy& en)
{
	//printf("wwwwwwwwwwwwwwwww");
	bool flagx = false;
	bool flagy = false;
	if (en.vx > 0 and en.x >= en.timex)
		flagx = true;
	if (en.vx < 0 and en.x <= en.timex)
		flagx = true;
	
	return (flagx + flagy);
}

bool sword_enemy(int x_sword,int y_sword,int x_size_sword, int y_size_sword, enemy &en, int dt,int direct,float &vy)
{
	
	float h0, x0,h,x1;
	float  leftline, rightline, upline, downline;
	bool flag = false;
	
	if(en.cor_immortal_time <= 0)
	{
		switch (direct)
		{
		case RIGHT:
			if (y_sword+ y_size_sword >= int(en.y) and y_sword <= int(en.y) + en.size)
				if ((x_sword <= int(en.x) and x_sword + x_size_sword >= int(en.x)) or (x_sword >= int(en.x) and x_sword<= int(en.x) + en.size))
				{
					en.vx = 3;
					en.vy = -7;
					return true;
					//printf("\aAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\n");
				}
			break;
		case LEFT:
			if (y_sword + y_size_sword >= int(en.y) and y_sword <= int(en.y) + en.size)
					if ((x_sword <= int(en.x) + en.size and x_sword + x_size_sword >= int(en.x) + en.size) or (x_sword + x_size_sword >= int(en.x) and x_sword +x_size_sword <= int(en.x) + en.size))
					{
						en.vx = -3;
						en.vy = -7;
						return true;
						//printf("\aAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\n");
					}
				break;
		case UP:
			if (x_sword+ x_size_sword >= int(en.x) and x_sword <= int(en.x) + en.size)
				if ((y_sword >= int(en.y) and y_sword <= int(en.y) + en.size) or (y_sword+ y_size_sword >= int(en.y) and y_sword+ y_size_sword <= int(en.y) + en.size)or(y_sword + y_size_sword >= int(en.y) + en.size and y_sword<= int(en.y)))
				{
				
					
					en.vy = -7;
					return true;
					//printf("\aAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\n");
				}
			break;
		case DOWN:
			if (x_sword + x_size_sword >= int(en.x) and x_sword <= int(en.x) + en.size)
				if ((y_sword >= int(en.y) and y_sword <= int(en.y) + en.size) or (y_sword + y_size_sword >= int(en.y) and y_sword + y_size_sword <= int(en.y) + en.size) or (y_sword + y_size_sword >= int(en.y) + en.size and y_sword <= int(en.y)))
				{
					
					
					//printf("\aAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\n");
					vy = -8;
					return true;
				}
			break;
			
		}
	}
	return false;
	
	
}
void anime_data(int f, int f_count, int x_sprite, int y_sprite, int w_sprite)
{
	if (f > f_count-1)
		frame = 0;
	frame_count=f_count;
	x_hero_sprite=x_sprite;
	y_hero_sprite=y_sprite;
	w_hero_sprite=w_sprite;

}


SDL_Texture* loadtexturefromfile(const char* filename, SDL_Rect* rect)
{
	SDL_Surface* surface = IMG_Load(filename);
	if (surface == 0)
	{
		printf("Couldn't load image %s! Error %s", filename, SDL_GetError());
		system("pause");
		DeInit(1);
	}
		SDL_Texture* texture = SDL_CreateTextureFromSurface(ren, surface);
		*rect = { 0,0,surface->w,surface->h };
		SDL_FreeSurface(surface);
		return texture;
}

void saverecords(  int wave,int kills,int time, int choice)
{
	FILE* fp = fopen("record.txt", "rt");
	if (!fp)
		exit(0);

	int mas[3];
	for (int i = 0; i < 3; i++)
		fscanf_s(fp, "%d", &mas[i]);
	fclose(fp);
	switch (choice)
	{
	case 1:
		mas[0]=max(mas[0],wave);
		break;
	case 2:
		mas[1] = max(mas[1], kills);
		break;
	case 3:
		mas[2] = min(mas[2], time);
		break;
	}
	fp = fopen("record.txt", "wt");
	if (!fp)
		exit(0);
	for (int i = 0; i < 3; i++)
		fprintf(fp, "%d ", mas[i]);
	fclose(fp);
}
void loadrecords(int &wave, int &kills, int &time)
{
	FILE* fp = fopen("record.txt", "rt");
	if (!fp)
		exit(0);

		fscanf_s(fp, "%d", &wave);
		fscanf_s(fp, "%d", &kills);
		fscanf_s(fp, "%d", &time);

	fclose(fp);
}
//SDL_Texture* genertext(const char* str, TTF_Font* font, SDL_Rect* rect, SDL_Color fg)
//{
//	SDL_Surface* surface = TTF_RenderText_Blended(font, str, fg);
//	*rect = { 0,0,surface->w,surface->h };
//
//	SDL_Texture* texture = SDL_CreateTextureFromSurface(ren, surface);
//	SDL_FreeSurface(surface);
//
//}
bool tap_on_but(int x,int y, int butx0, int buty0, int butx1, int buty1 )
{
	if (x > butx0 and x< butx1+ butx0 and y>buty0 and y < buty1+ buty0)
	{
		return true;
	}
	return false;
} 

void shop(int &max_hp,int &hp,int &damage,bool &djump,int &coin)
{
	int timerm = 0;
	int timerh = 0;
	int timerd = 0;
	int timerj = 0;
	int timerg = 0;
	SDL_Rect hero_rect;

	char costdamage[10] = "cost: 40";
	char costmaxhp[10] = "cost: 20";
	char costheal[10] = "cost: 10";
	char costjump[15]; 
	if (djump) strcpy(costjump, "complete");
	else strcpy(costjump, "cost: 100");
	
	TTF_Font* pixelfont = TTF_OpenFont("fonts\\pixel.fon", 100);
	SDL_Texture* damage_tex = loadtexturefromfile("hero\\damage.png", &hero_rect);
	SDL_Texture* max_hp_tex = loadtexturefromfile("hero\\max_hp.png", &hero_rect);
	SDL_Texture* hp_tex = loadtexturefromfile("hero\\heal.png", &hero_rect);
	SDL_Texture* dj_tex = loadtexturefromfile("hero\\doublejump.jpg", &hero_rect);
	SDL_Texture* nextlvl_tex = loadtexturefromfile("hero\\50621.png", &hero_rect);
	

	char strhp[20] = {};
	char strmaxhp[20] = {};
	char havecoin[25] = {};
	char strkill[20] = {};
	char strcoin[20] = {}; sprintf_s(strcoin, "%i", coin);

	SDL_Event ev;
	bool runing = true;

	SDL_Surface* str_surf = TTF_RenderText_Blended(pixelfont, costdamage, { 200,170,0,255 });
	SDL_Rect str_rect = { 10,10,str_surf->w,str_surf->h };
	SDL_Texture* str_costd = SDL_CreateTextureFromSurface(ren, str_surf);
	SDL_FreeSurface(str_surf);

	str_surf = TTF_RenderText_Blended(pixelfont, costmaxhp, { 200,170,0,255 });
	SDL_Texture* str_costm = SDL_CreateTextureFromSurface(ren, str_surf);
	SDL_FreeSurface(str_surf);

	str_surf = TTF_RenderText_Blended(pixelfont, costjump, { 200,170,0,255 });
	SDL_Texture* str_jump = SDL_CreateTextureFromSurface(ren, str_surf);
	SDL_FreeSurface(str_surf);

	str_surf = TTF_RenderText_Blended(pixelfont, costheal, { 200,170,0,255 });
	SDL_Texture* str_costh = SDL_CreateTextureFromSurface(ren, str_surf);
	SDL_FreeSurface(str_surf);


	while (runing)
	{
		SDL_SetRenderDrawColor(ren, 200, 200, 200, 255);
		SDL_RenderClear(ren);
	 sprintf_s(strkill, "Damage:%i", damage);
	  str_surf = TTF_RenderText_Blended(pixelfont, strkill, { 0,0,0,255 });
	
	SDL_Texture* str_textst = SDL_CreateTextureFromSurface(ren, str_surf);
	SDL_FreeSurface(str_surf);

	 sprintf_s(strmaxhp, "MaxHp:%i", max_hp);
	 str_surf = TTF_RenderText_Blended(pixelfont, strmaxhp, { 200,0,0,255 });
	SDL_Texture* str_textstmaxhp = SDL_CreateTextureFromSurface(ren, str_surf);
	SDL_FreeSurface(str_surf);

	 sprintf_s(strhp, "CorHp:%i", hp);
	str_surf = TTF_RenderText_Blended(pixelfont, strhp, { 20,200,20,255 });
	SDL_Texture* str_textsthp = SDL_CreateTextureFromSurface(ren, str_surf);
	SDL_FreeSurface(str_surf);

	sprintf_s(havecoin, "coin$: %i", coin);
	str_surf = TTF_RenderText_Blended(pixelfont, havecoin, { 0,0,0,255 });
	SDL_Texture* str_money = SDL_CreateTextureFromSurface(ren, str_surf);
	SDL_FreeSurface(str_surf);

	


		int x = 0, y = 0;
		while (SDL_PollEvent(&ev))
		{
			switch (ev.type)
			{
			case SDL_QUIT:
				runing = false;
			case SDL_KEYDOWN:
				switch (ev.key.keysym.scancode)
				{
					break;
				case SDL_SCANCODE_ESCAPE:
					runing = false;
					break;
				}
			case SDL_WINDOWEVENT:
				if (ev.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
				{
					wig = ev.window.data1;
					heg = ev.window.data2;
				}
				break;

			case SDL_MOUSEBUTTONDOWN:
				if (ev.button.button == SDL_BUTTON_LEFT)
				{

					x = ev.button.x;
					y = ev.button.y;
				}

			}

		}

		

		int killx0 = 50, killy0 = 50, killx1 = 90, killy1 = 90;
		if (tap_on_but(x, y, 10, heg - 50, 40, 40))
		{
			runing = false;
			break;
		}
		if (tap_on_but(x, y, killx0, killy0, killx1, killy1))
		{
			if(coin<40)
			timerd = 200;
			else
			{
				damage += 10;
				coin -= 40;
			}
		}
		if (tap_on_but(x, y, killx0 + killx1 + 40, killy0, killx1, killy1))
		{
			if (coin < 20)
				timerm = 200;
			else
			{
				max_hp += 10;
				hp += 10;
				coin -= 20;
			}
		}
		if (tap_on_but(x, y, killx0 + 2 * killx1 + 80, killy0, killx1, killy1))
		{
			if (coin < 10 or hp == max_hp)
				timerh = 200;
			else
			{
				hp = max_hp;
				coin -= 10;
			}
		}
		if (tap_on_but(x, y, killx0 + 3 * killx1 + 120, killy0, killx1, killy1))
		{
			if (coin < 100 or djump)
				timerj = 200;
			else
			{
				djump = true;
				coin -= 100;
				strcpy(costjump, "complete");
				str_surf = TTF_RenderText_Blended(pixelfont, costjump, { 200,170,0,255 });
				str_jump = SDL_CreateTextureFromSurface(ren, str_surf);
				SDL_FreeSurface(str_surf);
			}
		}


		SDL_Rect money = { wig-250,30,200,40 };
		SDL_Rect next_lvl = { 10,heg - 50,40,40 };


		SDL_Rect cost_kill = { killx0,killy0-50,80,40 };
		SDL_Rect but_kill = { killx0,killy0,killx1,killy1 };
		SDL_Rect text_kill = { killx0,killy0+ killy1+10,killx1,20 };

		SDL_Rect cost_maxhp = { killx0 + killx1 + 40,killy0 - 50,80,40 };
		SDL_Rect but_maxhp = { killx0+ killx1+40,killy0,killx1,killy1 };
		SDL_Rect text_maxhp = { killx0 + killx1 + 40,killy0 + killy1 + 10,killx1,20 };

		SDL_Rect cost_heal = { killx0 + 2 * killx1 + 80,killy0 - 50,80,40 };
		SDL_Rect but_heal = { killx0 + 2*killx1 + 80,killy0,killx1,killy1 };
		SDL_Rect text_heal = { killx0 + 2*killx1 + 80,killy0 + killy1 + 10,killx1,20 };

		SDL_Rect cost_dj = { killx0 + 3 * killx1 + 120,killy0 - 50,80,40 };
		SDL_Rect but_dj = { killx0 + 3 * killx1 + 120,killy0,killx1,killy1 };

		SDL_SetRenderDrawColor(ren, 180, 30, 30, 255);
		if (timerd>0)
		{
			timerd -= 30;
			SDL_RenderFillRect(ren, &but_kill);
		}
		if (timerm > 0)
		{
			timerm -= 30;
			SDL_RenderFillRect(ren, &but_maxhp);
		}
		if (timerh > 0)
		{
			timerh -= 30;
			SDL_RenderFillRect(ren, &but_heal);
		}
		if (timerg > 0)
		{
			timerh -= 30;
			SDL_RenderFillRect(ren, &but_dj);
		}
		SDL_RenderCopy(ren, nextlvl_tex, NULL, &next_lvl);

		SDL_RenderCopy(ren, str_costd, NULL, &cost_kill);
		SDL_RenderCopy(ren, str_costm, NULL, &cost_maxhp);
		SDL_RenderCopy(ren, str_costh, NULL, &cost_heal);
		SDL_RenderCopy(ren, str_jump, NULL, &cost_dj);

		SDL_RenderCopy(ren, str_money, NULL, &money);

		SDL_RenderCopy(ren, dj_tex, NULL, &but_dj);
		SDL_RenderCopy(ren, damage_tex, NULL, &but_kill);
		SDL_RenderCopy(ren, max_hp_tex, NULL, &but_maxhp);
		SDL_RenderCopy(ren, hp_tex, NULL, &but_heal);

		SDL_RenderCopy(ren, str_textst, NULL, &text_kill);
		SDL_RenderCopy(ren, str_textstmaxhp, NULL, &text_maxhp);
		SDL_RenderCopy(ren, str_textsthp, NULL, &text_heal);

		SDL_RenderPresent(ren);
		SDL_Delay(30);
		SDL_DestroyTexture(str_textst);
		SDL_DestroyTexture(str_textstmaxhp);
		SDL_DestroyTexture(str_textsthp);
		SDL_DestroyTexture(str_money);
	}
	TTF_CloseFont(pixelfont);
	SDL_DestroyTexture(damage_tex);
	SDL_DestroyTexture(dj_tex);
	SDL_DestroyTexture(max_hp_tex);
	SDL_DestroyTexture(hp_tex);
	SDL_DestroyTexture(nextlvl_tex);
	
}



void game(int gamemod)
{
	int lifesteal = 0;
	int buffbig = 0;
	int buffdamage = 0;
	wave_survive = 0;
	int damage_hero ;
	int max_hp_hero ;
	bool havedoublejump=false;
	float length_sword = 0.08;
	int coins = 200;
	if (gamemod == 1)
	{
		damage_hero = 30;
		max_hp_hero = 200;
	}
	if (gamemod == 2)
	{
		damage_hero = 100;
		max_hp_hero = 1000;
	}
	if (gamemod == 3)
	{
		damage_hero = 60;
		max_hp_hero = 90;
	}
	int hp_hero = max_hp_hero;
	
	int speed = 8;
	srand(time(0));
	int sword_delay = 0;
	int timer = 0;
	int cur_frametime = 0;
	int max_frametime = 60;
	int dt;
	bool survive = true;
	SDL_Event ev;

	//SDL_Surface* win_surf = SDL_GetWindowSurface(win);

	SDL_Rect back_rect;
	//SDL_Texture* back = loadtexturefromfile("levels\\level1.png",&back_rect);
#pragma region png
	TTF_Font* basicfont = TTF_OpenFont("fonts\\basic.TTF", 25);
	TTF_Font* pixelfont = TTF_OpenFont("fonts\\pixel.fon", 50);

	char strtimer[10] = "00";
	
	SDL_Surface* str_surf = TTF_RenderText_Blended(pixelfont, strtimer,{100,100,150,255});
	SDL_Rect str_rect = { wig / 2,30,str_surf->w,str_surf->h };
	SDL_Texture* str_text = SDL_CreateTextureFromSurface(ren, str_surf);

	SDL_Texture* platforma = loadtexturefromfile("levels\\plat.png", &back_rect);
	SDL_Texture* walla = loadtexturefromfile("levels\\wall.png", &back_rect);

	SDL_Rect hero_rect;

	SDL_Texture* tuchka_tex = loadtexturefromfile("buffs\\tuchka.jpg", &hero_rect);
	SDL_Texture* ls_tex  = loadtexturefromfile("buffs\\lifesteal.png", &hero_rect);
	SDL_Texture* dm_tex  = loadtexturefromfile("buffs\\buffdamage.png", &hero_rect);
	SDL_Texture* big_tex = loadtexturefromfile("buffs\\buffbig.png", &hero_rect);
	SDL_Texture* mon_tex = loadtexturefromfile("buffs\\monetka.png", &hero_rect);
	SDL_Texture* heal_tex = loadtexturefromfile("buffs\\hilka.png", &hero_rect);



	SDL_Texture* hero_tex = loadtexturefromfile("hero\\main_hero.png", &hero_rect);
	SDL_Texture* basic_tex = loadtexturefromfile("enemy\\basic.png", &hero_rect);
	SDL_Texture* fly_tex = loadtexturefromfile("enemy\\fly.png", &hero_rect);
	SDL_Texture* rasher_tex = loadtexturefromfile("enemy\\rusher.png", &hero_rect);
	SDL_Texture* bomber_tex = loadtexturefromfile("enemy\\bomber.png", &hero_rect);
	SDL_Texture* ufo_tex  = loadtexturefromfile("enemy\\ufo.png", &hero_rect);
	SDL_Texture* laser_tex = loadtexturefromfile("enemy\\laser.png", &hero_rect);
#pragma endregion
	int count_kills;
	int lasttime = SDL_GetTicks();
	int newtime = SDL_GetTicks();
	//...............................................................................до этого момента создается на 1 игру
	enemy** enemys = NULL;
	while (survive)
	{
#pragma region keylogic
		bool jump = false;
		bool secondjump = false;
		bool sword = false;
		bool attackleft = false;
		bool pressu = false;
		bool pressd = false;
		bool pressr = false;
		bool pressl = false;
		bool runing = true;
		bool dash = false;
		bool round = true;
		int choice_direction_attack = RIGHT;
		int choice_dash = RIGHT;
#pragma endregion
		hp_hero = min(max_hp_hero, hp_hero + max_hp_hero/10+1);
		 count_kills = 200;
		int wave_cost = 3 + wave_survive;
		float vx = 0, vy = 0;
		int count_enemy_wave=0;
		int dash_cooldown = 0;
		int cor_immortal_time_hero = 3000;
		int cor_stanlock = 0;
		float x = wig / 2 - 100;
		float y = heg / 2;
		timer = 0;
		createplatform();
		newtime = SDL_GetTicks();
		lasttime = SDL_GetTicks();
		//...............................................................................создается каждый уровень
		while (round)
		{
		
			#pragma region create_enemy
			enemys = (enemy**)realloc(enemys,(count_enemy_wave + 1) * sizeof(enemy*));

			enemys[count_enemy_wave] = (enemy*)malloc(wave_cost * sizeof(enemy));

				bool basicflag=false;
			int* basic_count = (int*)malloc(sumplat * sizeof(int));
			for (int i = 0; i < sumplat; i++)basic_count[i] = 0;
			for (int i = 0; i < wave_cost; i++)
			{  
				if (basicflag or basic_count[sumplat - 1] != 0)
				{
					enemys[count_enemy_wave][i].id = random(FLAING, LASER);  //random(FLAING,RUSHER);//RUSHER - последний
					basicflag = true;
				}
				else
					enemys[count_enemy_wave][i].id = random(FLAING, LASER);   //random(BASIC, RUSHER);//RUSHER - последний
					
				
				enemys[count_enemy_wave][i].cor_immortal_time = random(700, 1200);
				enemys[count_enemy_wave][i].animation = 0;
				enemys[count_enemy_wave][i].vy = 0;
				enemys[count_enemy_wave][i].vx = 0;
				enemys[count_enemy_wave][i].cor_delay = 1000;
				switch (enemys[count_enemy_wave][i].id)
				{
				case BASIC:
					enemys[count_enemy_wave][i].gainmoney = 10 + wave_survive * 4;
					enemys[count_enemy_wave][i].status = FALLING;
					enemys[count_enemy_wave][i].max_hp = 100 + wave_survive * 10;
					enemys[count_enemy_wave][i].cor_hp = enemys[count_enemy_wave][i].max_hp;
					enemys[count_enemy_wave][i].damage = 10 + wave_survive * 5;
					enemys[count_enemy_wave][i].size = 45;
					enemys[count_enemy_wave][i].speed = 6 + 0.5 * wave_survive;
					enemys[count_enemy_wave][i].attack_delay = max(0, 600 - 80 * wave_survive);

					spawn_enemy_ground(enemys[count_enemy_wave][i], basic_count,true);
					/*enemys[j][i].x = random(0, wig-enemys[j][i].size);
					enemys[j][i].y = random(0, heg);*/
					break;
				case FLAING:
					enemys[count_enemy_wave][i].gainmoney = 5 + wave_survive * 3;
					enemys[count_enemy_wave][i].flag = false;
					enemys[count_enemy_wave][i].status = STAY;
					enemys[count_enemy_wave][i].max_hp = 60 + wave_survive * 5;
					enemys[count_enemy_wave][i].cor_hp = enemys[count_enemy_wave][i].max_hp;
					enemys[count_enemy_wave][i].damage = 12 + wave_survive * 6;
					enemys[count_enemy_wave][i].size = 35;
					enemys[count_enemy_wave][i].speed = 4 + 0.5 * wave_survive;
					enemys[count_enemy_wave][i].attack_delay = max(0, 400 - 80 * wave_survive);


					spawn_enemy_air(enemys[count_enemy_wave][i], x, y);
					/*enemys[j][i].x = random(0, wig-enemys[j][i].size);
					enemys[j][i].y = random(0, heg);*/
					break;
				case RUSHER:
					enemys[count_enemy_wave][i].gainmoney = 10 + wave_survive * 8;
					enemys[count_enemy_wave][i].status = STAY;
					enemys[count_enemy_wave][i].max_hp = 70 + wave_survive * 7;
					enemys[count_enemy_wave][i].cor_hp = enemys[count_enemy_wave][i].max_hp;
					enemys[count_enemy_wave][i].damage = 18 + wave_survive * 8;
					enemys[count_enemy_wave][i].size = 40;
					enemys[count_enemy_wave][i].speed = 3 + 0.5 * wave_survive;
					enemys[count_enemy_wave][i].attack_delay = max(0, 1000 - 60 * wave_survive);


					spawn_enemy_air(enemys[count_enemy_wave][i], x, y);

					break;
				case LASER:
					enemys[count_enemy_wave][i].gainmoney = 10 + wave_survive * 8;
					enemys[count_enemy_wave][i].status = STAY;
					enemys[count_enemy_wave][i].max_hp = 70 + wave_survive * 7;
					enemys[count_enemy_wave][i].cor_hp = enemys[count_enemy_wave][i].max_hp;
					enemys[count_enemy_wave][i].damage = 18 + wave_survive * 8;
					enemys[count_enemy_wave][i].size = 40;
					enemys[count_enemy_wave][i].speed = 3 + 0.5 * wave_survive;
					enemys[count_enemy_wave][i].attack_delay = max(0, 1000 - 60 * wave_survive);


					spawn_enemy_air(enemys[count_enemy_wave][i], x, y);

					break;
				case BOMBER:
					enemys[count_enemy_wave][i].gainmoney = 20 + wave_survive * 8;
					enemys[count_enemy_wave][i].status = FALLING;
					enemys[count_enemy_wave][i].max_hp = 80 + wave_survive * 10;
					enemys[count_enemy_wave][i].cor_hp = enemys[count_enemy_wave][i].max_hp;
					enemys[count_enemy_wave][i].damage = 30 + wave_survive * 40;
					enemys[count_enemy_wave][i].size = 60;
					enemys[count_enemy_wave][i].speed = 8 + 0.7 * wave_survive;
					enemys[count_enemy_wave][i].attack_delay = max(0, 1000 - 80 * wave_survive);

					spawn_enemy_ground(enemys[count_enemy_wave][i], basic_count,false);
					/*enemys[j][i].x = random(0, wig-enemys[j][i].size);
					enemys[j][i].y = random(0, heg);*/
					break;
				}
			}

			free(basic_count);

#pragma endregion
			count_enemy_wave++;
			runing = true;
			//...............................................................................на	каждую волну

			
			

			while (runing)
			{
				/*	printf("%d\n", sum);*/
					int direct;
					int enemy_counter = wave_cost * count_enemy_wave;
					float leng;
				size_w_hero = wig / 50;
				size_h_hero = heg / 15;
				if (buffbig > 0)
				{
					size_w_hero *= 2;
					size_h_hero *= 2;
					buffbig -= dt;
					if (buffbig <= 0)
					{
						max_hp_hero /= 2;
						hp_hero /= 2;
						length_sword /= 2;
					}
			/*		length_sword *= 2;
					hp_hero *= 2;
					max_hp_hero *= 2;*/
				}
				if (lifesteal > 0) lifesteal -= dt;
				if (buffdamage > 0) buffdamage -= dt;
				SDL_Rect cub = { x,y,size_w_hero,size_h_hero };

				bool changecolor = false;
				SDL_SetRenderDrawColor(ren, 200, 200, 200, 255);
				SDL_RenderClear(ren);
			
				while (SDL_PollEvent(&ev))
				{
					switch (ev.type)
					{
					case SDL_QUIT:
						runing = false;
						round = false;
						survive = false;
						break;

					case SDL_WINDOWEVENT:
						if (ev.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
						{
							wig = ev.window.data1;
							heg = ev.window.data2;
						}
						break;

					case SDL_MOUSEBUTTONDOWN:
						if (ev.button.button == SDL_BUTTON_LEFT)
						{

							x = ev.button.x;
							y = ev.button.y;
						}
					case SDL_KEYDOWN:
						switch (ev.key.keysym.scancode)
						{
						case SDL_SCANCODE_A:		if (sword_delay <= 0) sword = true;	break;
						case SDL_SCANCODE_UP:		pressu = true;	break;
						case SDL_SCANCODE_DOWN:		pressd = true;	break;
						case SDL_SCANCODE_RIGHT:	pressr = true; 	break;
						case SDL_SCANCODE_LEFT:		pressl = true;	break;

						case SDL_SCANCODE_ESCAPE:
							runing = false;
							round = false;
							survive = false;
							break;
						case SDL_SCANCODE_D:
							if (dash_cooldown <= 0)
							{
								cor_immortal_time_hero = time_dash;
								dash_cooldown = 1200;
								dash = true;
							}
							break;

						case SDL_SCANCODE_SPACE:
							if (jump)
							{
								vy = -10;
							}
							else if (secondjump and havedoublejump)
							{
									vy = -10;
								secondjump = false;
							}
							break;
						}
						break;
					case SDL_KEYUP:
						switch (ev.key.keysym.scancode)
						{
						case SDL_SCANCODE_UP:

							pressu = false;
							break;
						case SDL_SCANCODE_DOWN:

							pressd = false;
							break;
						case SDL_SCANCODE_RIGHT:

							pressr = false;
							break;
						case SDL_SCANCODE_LEFT:

							pressl = false;
							break;
						}
						break;
					}

				}
				bool fafasf = true;
				if (fafasf)
				{

#pragma region work_time


					newtime = SDL_GetTicks();
					dt = newtime - lasttime;
					if (dt < 16)
					{
						SDL_Delay(16 - dt);
						newtime = SDL_GetTicks();
						dt = newtime - lasttime;
					}
					lasttime = newtime;
					cur_frametime += dt;
					if (cur_frametime >= max_frametime)
					{
						cur_frametime -= max_frametime;
						frame = (frame + 1) % frame_count;
					}

#pragma endregion
					renderplatform(platforma, walla);
					//	SDL_RenderCopy(ren, back, NULL, NULL);

#pragma region animation_hero
					SDL_Rect dst_rect = { x_hero_sprite + 128.5 * frame,y_hero_sprite,w_hero_sprite,125 };
					//cub-hero-pixels= 130x130
					SDL_Rect cub_hero = { x,y - size_h_hero * 0.4,size_w_hero,size_h_hero * 1.4 };
					if (dash)
					{
						anime_data(frame, 4, 920, 520, 110);
						if (vx >= 0)
							SDL_RenderCopy(ren, hero_tex, &dst_rect, &cub_hero);
						else
							SDL_RenderCopyEx(ren, hero_tex, &dst_rect, &cub_hero, 0, NULL, SDL_FLIP_HORIZONTAL);
					}
					else if (vy < 0)
					{

						anime_data(frame, 3, 1180, 1150, 80);
						if (!attackleft)
							SDL_RenderCopy(ren, hero_tex, &dst_rect, &cub_hero);
						else
							SDL_RenderCopyEx(ren, hero_tex, &dst_rect, &cub_hero, 0, NULL, SDL_FLIP_HORIZONTAL);
					}
					else if (vy != 0 and pressl and !pressr)
					{
						anime_data(frame, 3, 660, 1150, 80);
						//void anime_data(int& frame, int& frame_count, int& x_hero_sprite, int& y_hero_sprite, int& w_hero_sprite)
						SDL_RenderCopyEx(ren, hero_tex, &dst_rect, &cub_hero, 0, NULL, SDL_FLIP_HORIZONTAL);
					}
					else if (vy != 0)
					{
						anime_data(frame, 3, 660, 1150, 80);
						SDL_RenderCopy(ren, hero_tex, &dst_rect, &cub_hero);
					}
					else if (pressr and !pressl)
					{
						anime_data(frame, 9, 27, -5, 80);
						SDL_RenderCopy(ren, hero_tex, &dst_rect, &cub_hero);
					}
					else if (pressl and !pressr)
					{
						anime_data(frame, 9, 27, -5, 80);
						SDL_RenderCopyEx(ren, hero_tex, &dst_rect, &cub_hero, 0, NULL, SDL_FLIP_HORIZONTAL);
					}
					else
					{
						anime_data(frame, 6, 790, 635, 80);
						SDL_RenderCopy(ren, hero_tex, &dst_rect, &cub_hero);
					}
#pragma endregion

					printf("%d\n", dt);
#pragma region basic_move
					vy += gravity / 1000 * float(dt);
					//if (pressu and y > 0) { vy = -(float(dt)/1000) * speed;  }
					if (logicplatform(vy, y, x, dt))
					{

						vy = 0;
						jump = true;
						//printf("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\n");
					}
					else { jump = false; }

					if (!dash and attackleft)choice_dash = LEFT;
					else if (!dash)choice_dash = RIGHT;
					if (cor_stanlock > 0)
					{

						cor_stanlock -= dt;
					}
					else if (dash)
					{
						if (cor_immortal_time_hero > 0)
						{
							cor_immortal_time_hero -= dt;
							if (choice_dash == RIGHT)
								vx = 2 * speed;
							if (choice_dash == LEFT)
								vx = -speed * 2;



							vy = 0;
						}
						else
							dash = false;
					}
					else
					{
						if (pressd) { if (vy == 0)y++; }
						if (pressr and !pressl) { vx = speed; attackleft = false;/*printf("%f     AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\n",vx);*/ }
						else if (pressl and !pressr) { vx = -speed; attackleft = true; }
						else { vx = 0; }
					}

					if (logicwall(vy, vx, y, x, dt) == 2)
					{

						//printf("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\n");
						jump = true;
					}
					x += vx * (float(dt) / 16);
					y += vy * (float(dt) / 16);
					if (sword and sword_delay <= 0 and cor_stanlock <= 0)
					{
						sword_delay = attack_speed;
						sword = false;
						frame_for_sword = 0;
						if (pressu)
							choice_direction_attack = UP;
						else if (pressd and vy != 0)
							choice_direction_attack = DOWN;
						else if (attackleft)
							choice_direction_attack = LEFT;
						else
							choice_direction_attack = RIGHT;
					}
#pragma endregion

#pragma region hit_box_sword

					if (sword_delay > 0)
					{
						int x_sword;
						int y_sword;
						int x_size_sword;
						int y_size_sword;

						SDL_Rect hit_box_sword;
						SDL_Rect dst_sword = { 1535 + 127.5 * frame_for_sword,1530,130,125 };
						frame_for_sword = (attack_speed - sword_delay) * 4 / (attack_speed);
						SDL_SetRenderDrawColor(ren, 0, 200, 0, 255);
						switch (choice_direction_attack)
						{
						case(UP):
							x_sword = int(x);
							y_sword = int(y) - int(heg * length_sword * 1.5);
							x_size_sword = size_w_hero;
							y_size_sword = int(heg * length_sword * 1.5);
							break;

						case(DOWN):
							x_sword = int(x);
							y_sword = int(y) + size_h_hero;
							x_size_sword = size_w_hero;
							y_size_sword = int(heg * length_sword * 1.2);
							break;

						case(LEFT):
							x_sword = int(x) - int(wig * length_sword);
							y_sword = int(y);
							x_size_sword = int(wig * length_sword) + size_w_hero;
							y_size_sword = size_h_hero;
							break;
						case(RIGHT):


							x_sword = int(x);
							y_sword = int(y);
							x_size_sword = int(wig * length_sword) + size_w_hero;
							y_size_sword = size_h_hero;
							break;
						}
						hit_box_sword = { x_sword ,y_sword,x_size_sword,y_size_sword };
						SDL_RenderDrawRect(ren, &hit_box_sword);
						SDL_SetRenderDrawColor(ren, 0, 200, 0, 255);
						switch (choice_direction_attack)
						{
						case RIGHT:
							SDL_RenderCopy(ren, hero_tex, &dst_sword, &hit_box_sword);
							break;
						case LEFT:
							SDL_RenderCopyEx(ren, hero_tex, &dst_sword, &hit_box_sword, 0, NULL, SDL_FLIP_HORIZONTAL);
							break;
						case UP:
							SDL_RenderCopyEx(ren, hero_tex, &dst_sword, &hit_box_sword, 270, NULL, SDL_FLIP_NONE);
							break;
						case DOWN:
							SDL_RenderCopyEx(ren, hero_tex, &dst_sword, &hit_box_sword, 90, NULL, SDL_FLIP_NONE);
							break;
						}
						for (int j = 0; j < count_enemy_wave; j++)
							for (int i = 0; i < wave_cost; i++)
							{
								if (enemys[j][i].id > 0)
								{
									if (sword_enemy(x_sword, y_sword, x_size_sword, y_size_sword, enemys[j][i], dt, choice_direction_attack, vy))
									{
										enemys[j][i].cor_immortal_time = attack_speed;
										if(buffdamage>0)
											enemys[j][i].cor_hp -= damage_hero;
										enemys[j][i].cor_hp -= damage_hero;
										if (enemys[j][i].id == FLAING or enemys[j][i].id == RUSHER)
										{
											enemys[j][i].status = FALLING;
											enemys[j][i].vx *= 2;

											if (choice_direction_attack == DOWN)
												enemys[j][i].vy = 6;
										}
										printf("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\n");
									}
								}
							}



						sword_delay -= dt;
					}
#pragma endregion
					SDL_SetRenderDrawColor(ren, 255, 0, 0, 255);
#pragma region enemys_move
					for (int j = 0; j < count_enemy_wave; j++)
						for (int i = 0; i < wave_cost; i++)
						{
							if (enemys[j][i].cor_immortal_time > 0)
								enemys[j][i].cor_immortal_time -= dt;
							else if (  enemys[j][i].id != 0 and (direct = touch_enemy_hero(x, y, enemys[j][i])) )
							{
								if(cor_immortal_time_hero <= 0 and enemys[j][i].id > 0)
								{
									if(enemys[j][i].id==BOMBER)
									{
										enemys[j][i].status = RUSH;
										enemys[j][i].vy = 0;
										enemys[j][i].cor_delay = 500;
									}
									if (direct == RIGHT)
										vx = 5;
									else
										vx = -5;
									cor_stanlock = stanlock;
									vy = -5;
									hp_hero -= enemys[j][i].damage;
									cor_immortal_time_hero = immortal_time_hero;
								}
								else if(enemys[j][i].id < 0)
								{
									switch (enemys[j][i].id)
									{
										//LIFESTEALB=-5,BIGEB,DAMAGEB, HILKA,MONETKA
									case  TUCHKA:
										hp_hero -= hp_hero * dt / 100 / 16;
										break;
									case  LIFESTEALB:
										lifesteal = 4000;
										break;
									case BIGEB:
										if (buffbig <= 0)
										{
											max_hp_hero *= 2;
											hp_hero *= 2;
											length_sword *= 2;
										}
										buffbig = 5000;
										break;
									case DAMAGEB:
										buffdamage = 5000;
										break;
									case HILKA:
										hp_hero = min(max_hp_hero, hp_hero+max_hp_hero / 3);
										break;
									case MONETKA:
										coins += 40;
										break;
									}
									if(enemys[j][i].id!=TUCHKA) enemys[j][i].id = 0;
								}
							}
							if (enemys[j][i].cor_hp <= 0)
							{
								if (lifesteal > 0)
									hp_hero = min(max_hp_hero,hp_hero+max_hp_hero / 20);
								
								if (random(1, 3) == 1)
								enemys[j][i].id = random(LIFESTEALB, MONETKA);
								else
								{
									enemys[j][i].id = 0;
									enemys[j][i].cor_hp = 1;
								}

								if (enemys[j][i].id < 0)
								
								enemys[j][i].cor_hp = 1;
								coins+=enemys[j][i].gainmoney ;
								count_kills++;
								
							}
							if (enemys[j][i].id <= 0)
							{
								enemy_counter--;
							}
							SDL_Rect hit_box_enemy = { enemys[j][i].x,enemys[j][i].y,enemys[j][i].size,enemys[j][i].size };
							switch (enemys[j][i].id)
							{
							case BASIC:

#pragma region movement_basic
								if (enemys[j][i].vy != 0)
									enemys[j][i].status = FALLING;
								switch (enemys[j][i].status)
								{

#pragma region going
								case GOING:
									if (enemys[j][i].vx > 0)
									{
										if (on_platform(enemys[j][i].x + enemys[j][i].vx * float(dt) / 16 + enemys[j][i].size, enemys[j][i].y, enemys[j][i].vy, dt, enemys[j][i].size)
											and !in_wall(enemys[j][i].x + enemys[j][i].vx * float(dt) / 16, enemys[j][i].y, enemys[j][i].size, enemys[j][i].x, enemys[j][i].y)
											and enemys[j][i].x + enemys[j][i].vx * float(dt) / 16 < wig - enemys[j][i].size)
											enemys[j][i].x += enemys[j][i].vx * float(dt) / 16;
										else { enemys[j][i].status = STAY;	enemys[j][i].cor_delay = enemys[j][i].attack_delay; enemys[j][i].vx = -enemys[j][i].vx; }

									}
									if (enemys[j][i].vx <= 0)
									{
										if (on_platform(enemys[j][i].x + enemys[j][i].vx * float(dt) / 16 - enemys[j][i].size, enemys[j][i].y, enemys[j][i].vy, dt, enemys[j][i].size)
											and !in_wall(enemys[j][i].x + enemys[j][i].vx * float(dt) / 16, enemys[j][i].y, enemys[j][i].size, enemys[j][i].x, enemys[j][i].y)
											and enemys[j][i].x + enemys[j][i].vx * float(dt) / 16 > 0)
											enemys[j][i].x += enemys[j][i].vx * float(dt) / 16;
										else { enemys[j][i].status = STAY;	enemys[j][i].cor_delay = enemys[j][i].attack_delay; enemys[j][i].vx = -enemys[j][i].vx; }
									}

									enemys[j][i].animation = (enemys[j][i].animation + dt) % (6 * 60);
									enemys[j][i].enemy_texture = { 3 + 111 * (enemys[j][i].animation / 60),174,107,127 };
									break;
#pragma endregion

#pragma region stay
								case STAY:

									//printf("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\n");
									if (enemys[j][i].cor_immortal_time > 0 and choice_direction_attack != DOWN)
									{
										enemys[j][i].x -= enemys[j][i].vx;
									}
									else if (enemys[j][i].cor_delay <= 0)
									{
										enemys[j][i].status = GOING;

										if (enemys[j][i].vx < 0)
											enemys[j][i].vx = -enemys[j][i].speed;
										else
											enemys[j][i].vx = enemys[j][i].speed;

										enemys[j][i].animation = 0;
									}
									else
										enemys[j][i].cor_delay -= dt;
									enemys[j][i].animation = (enemys[j][i].animation + dt) % (6 * 60);
									enemys[j][i].enemy_texture = { 145 + 106 * (enemys[j][i].animation / 60),23,100,127 };
									break;


#pragma endregion

#pragma region falling
								case FALLING:

									enemys[j][i].vy += gravity / 1000 * float(dt);
									enemys[j][i].y += enemys[j][i].vy * float(dt) / 16;
									if (in_wall(enemys[j][i].x + enemys[j][i].vx * float(dt) / 16, enemys[j][i].y - 10, enemys[j][i].size, enemys[j][i].x, enemys[j][i].y))
									{
										enemys[j][i].vx *= -1;
										enemys[j][i].vy = 0;
									}
									if (enemys[j][i].x + enemys[j][i].vx * float(dt) / 16 <= 0)
									{
										enemys[j][i].x = 0;
										enemys[j][i].vx = -enemys[j][i].vx;
									}
									else if (enemys[j][i].x + enemys[j][i].vx * float(dt) / 16 >= wig - enemys[j][i].size)
									{
										enemys[j][i].x = wig - enemys[j][i].size;
										enemys[j][i].vx = -enemys[j][i].vx;
									}
									else
										enemys[j][i].x += enemys[j][i].vx * float(dt) / 16;

									if (enemys[j][i].y > heg)
										enemys[j][i].cor_hp = 0;

									if (on_platform(enemys[j][i].x, enemys[j][i].y, enemys[j][i].vy, dt, enemys[j][i].size))
									{
										enemys[j][i].vy = 0;
										enemys[j][i].status = STAY;
										enemys[j][i].cor_delay = enemys[j][i].attack_delay;
										enemys[j][i].animation = 0;
									}

#pragma endregion

								}



								if (enemys[j][i].status == FALLING)
									enemys[j][i].enemy_texture = { 3,920,128,108 };
								if (enemys[j][i].vx > 0)
									SDL_RenderCopy(ren, basic_tex, &enemys[j][i].enemy_texture, &hit_box_enemy);
								else
									SDL_RenderCopyEx(ren, basic_tex, &enemys[j][i].enemy_texture, &hit_box_enemy, 0, NULL, SDL_FLIP_HORIZONTAL);




#pragma endregion	

								break;

							case FLAING:

#pragma region fly_movement
								switch (enemys[j][i].status)
								{
								case STAY:
									if (enemys[j][i].cor_delay <= 0)
									{
										enemys[j][i].status = GOING;
										enemys[j][i].animation = random(0, 60);
										enemys[j][i].cor_delay = random(7, 12);
									}
									else
										enemys[j][i].cor_delay -= dt;

									break;
								case GOING:
									leng = sqrt(((x - enemys[j][i].x) * (x - enemys[j][i].x)) + ((y - enemys[j][i].y) * (y - enemys[j][i].y)));
									enemys[j][i].vx = (x - enemys[j][i].x) / leng * enemys[j][i].speed * enemys[j][i].cor_delay / 10;
									enemys[j][i].vy = (y - enemys[j][i].y) / leng * enemys[j][i].speed * enemys[j][i].cor_delay / 10;
									in_wall_fly(enemys[j][i]);

									//	enemys[j][i].x += enemys[j][i].vx;
									//	enemys[j][i].y += enemys[j][i].vy;

									//in_wall(enemys[j][i].x+ enemys[j][i].vx, enemys[j][i].y+ enemys[j][i].vy, enemys[j][i].size, enemys[j][i].x, enemys[j][i].y);
									break;
								case FALLING:
									if (enemys[j][i].cor_immortal_time > 0)
									{

										in_wall_fly(enemys[j][i]);
										enemys[j][i].vy *= 0.9 / 16 * dt;
										enemys[j][i].vx *= 0.9 / 16 * dt;

									}
									else
									{
										enemys[j][i].cor_delay = enemys[j][i].attack_delay;
										enemys[j][i].status = STAY;
										enemys[j][i].animation = 0;
									}
									break;
								}
								if (enemys[j][i].status == FALLING)
									enemys[j][i].enemy_texture = { 5,162,90,90 };
								else
								{
									enemys[j][i].animation = (enemys[j][i].animation + dt) % (4 * 60);
									enemys[j][i].enemy_texture = { 5 + 102 * (enemys[j][i].animation / 60),25,95,115 };
								}
								if (x - enemys[j][i].x < 0)
									SDL_RenderCopy(ren, fly_tex, &enemys[j][i].enemy_texture, &hit_box_enemy);
								else
									SDL_RenderCopyEx(ren, fly_tex, &enemys[j][i].enemy_texture, &hit_box_enemy, 0, NULL, SDL_FLIP_HORIZONTAL);
#pragma endregion

								break;

							case RUSHER:

#pragma region rusher_movement
								switch (enemys[j][i].status)
								{
								case STAY:

									if (enemys[j][i].cor_delay <= 0)
									{
										enemys[j][i].status = GOING;
										enemys[j][i].enemy_texture = { 5,25,128,145 };


										enemys[j][i].animation = random(0, 60);
										enemys[j][i].cor_delay = 600;

									}
									else
									{
										enemys[j][i].cor_delay -= dt;
										enemys[j][i].animation = (enemys[j][i].animation + dt) % (3 * 60);
										enemys[j][i].enemy_texture = { 20 + 155 * (enemys[j][i].animation / 60),525,135,140 };

									}
									break;

								case GOING:

									if (keep_dist_laser(x, y, enemys[j][i].cor_delay, 650, enemys[j][i]))
									{
										//printf("VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV");
										enemys[j][i].status = RUSH;
										enemys[j][i].enemy_texture = { 5,845,210,60 };
										enemys[j][i].timex = x;
										enemys[j][i].timey = y;
										enemys[j][i].vx = (x - enemys[j][i].x) / sqrt((x - enemys[j][i].x) * (x - enemys[j][i].x) + (y - enemys[j][i].y) * (y - enemys[j][i].y)) * enemys[j][i].speed * 5;
										enemys[j][i].vy = (y - enemys[j][i].y) / sqrt((x - enemys[j][i].x) * (x - enemys[j][i].x) + (y - enemys[j][i].y) * (y - enemys[j][i].y)) * enemys[j][i].speed * 5;
										enemys[j][i].cor_delay = 120;
										enemys[j][i].flag = false;
										break;
									}
									enemys[j][i].cor_delay -= 3 * dt / 16;
									enemys[j][i].animation = (enemys[j][i].animation + dt) % (4 * 60);
									enemys[j][i].enemy_texture = { 5 + 133 * (enemys[j][i].animation / 60),25,125,145 };
									in_wall_fly(enemys[j][i]);
									break;

								case RUSH:
									if (!enemys[j][i].flag)
									{
										enemys[j][i].flag = check_out_dist(enemys[j][i]);

									}
									else
									{
										if (enemys[j][i].cor_delay > 0)
										{
											enemys[j][i].cor_delay -= dt;

											//printf("VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV");
										}
										else
										{
											enemys[j][i].status = STAY;
											enemys[j][i].cor_delay = enemys[j][i].attack_delay;
										}
									}
									enemys[j][i].animation = (enemys[j][i].animation + dt) % (3 * 60);
									enemys[j][i].enemy_texture = { 5 + 215 * (enemys[j][i].animation / 60),845,210,60 };
									enemys[j][i].x += enemys[j][i].vx;
									enemys[j][i].y += enemys[j][i].vy;
									break;

								case FALLING:
									if (enemys[j][i].cor_immortal_time > 0)
									{
										enemys[j][i].animation = 0;
										enemys[j][i].enemy_texture = { 5 + 125 * (enemys[j][i].animation / 60),930,120,125 };
										in_wall_fly(enemys[j][i]);
										enemys[j][i].vy *= 0.9 / 16 * dt;
										enemys[j][i].vx *= 0.9 / 16 * dt;

									}
									else
									{
										enemys[j][i].cor_delay = enemys[j][i].attack_delay;
										enemys[j][i].status = STAY;
										enemys[j][i].enemy_texture = { 5 + 102 * (enemys[j][i].animation / 60),25,100,115 };
										enemys[j][i].animation = 0;
									}
								}
								if (enemys[j][i].vx < 0)
									SDL_RenderCopy(ren, rasher_tex, &enemys[j][i].enemy_texture, &hit_box_enemy);
								else
									SDL_RenderCopyEx(ren, rasher_tex, &enemys[j][i].enemy_texture, &hit_box_enemy, 0, NULL, SDL_FLIP_HORIZONTAL);
#pragma endregion

								break;
							case LASER:
#pragma region laser_movement
								SDL_Rect laser;
								switch (enemys[j][i].status)
								{
								case STAY:

									if (enemys[j][i].cor_delay <= 0)
									{
										enemys[j][i].status = GOING;
										enemys[j][i].enemy_texture = { 5,25,128,145 };


										enemys[j][i].animation = random(0, 60);
										enemys[j][i].cor_delay = 600;

									}
									else
									{
										enemys[j][i].cor_delay -= dt;
										enemys[j][i].animation = (enemys[j][i].animation + dt) % (4 * 60);
										enemys[j][i].enemy_texture = { 11 + 64 * (enemys[j][i].animation / 60),48,33,22 };

									}
									break;

								case GOING:

									if (keep_dist_laser(x, y, enemys[j][i].cor_delay, 650, enemys[j][i]) and abs(enemys[j][i].x-x)<100)
									{
										//printf("VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV");
										enemys[j][i].status = RUSH;
										enemys[j][i].enemy_texture = { 5,845,210,60 };
										enemys[j][i].cor_delay = 600;
										enemys[j][i].flag = false;
										break;
									}
									enemys[j][i].cor_delay -= 3 * dt / 16;
									enemys[j][i].animation = (enemys[j][i].animation + dt) % (4 * 60);
									enemys[j][i].enemy_texture = { 11 + 64 * (enemys[j][i].animation / 60),48,33,22 };
									if (enemys[j][i].y < 0)
										enemys[j][i].y = 0;
									in_wall_fly(enemys[j][i]);
									break;

								case RUSH:
									laser = { int(enemys[j][i].x),int(enemys[j][i].y) + enemys[j][i].size,enemys[j][i].size,heg };
									SDL_RenderCopy(ren, laser_tex, NULL, &laser);
									if (touch_laser(x, y, int(enemys[j][i].x), int(enemys[j][i].y) + enemys[j][i].size, enemys[j][i].size, heg))
										hp_hero -= 2 * dt / 16;
									enemys[j][i].cor_delay -=dt;
									if (enemys[j][i].cor_delay <= 0)
									{
										enemys[j][i].status = FALLING;
										enemys[j][i].vy = enemys[j][i].speed / 2;
										enemys[j][i].vx = enemys[j][i].speed *2*(1-rand()%2*2);
										enemys[j][i].cor_hp -= enemys[j][i].max_hp / 5;
										enemys[j][i].cor_immortal_time = 1200;
									}
									enemys[j][i].enemy_texture = { 140,113,33,28 };
									break;

								case FALLING:
									if (enemys[j][i].cor_immortal_time > 0)
									{
										enemys[j][i].animation = 0;
										enemys[j][i].enemy_texture = { 5,192,33,19 };
										in_wall_fly(enemys[j][i]);
										enemys[j][i].vy *= 0.9 / 16 * dt;
										enemys[j][i].vx *= 0.9 / 16 * dt;

									}
									else
									{
										enemys[j][i].cor_delay = enemys[j][i].attack_delay;
										enemys[j][i].status = STAY;
										enemys[j][i].enemy_texture = { 5 + 102 * (enemys[j][i].animation / 60),25,100,115 };
										enemys[j][i].animation = 0;
									}
								}
							
									SDL_RenderCopy(ren, ufo_tex, &enemys[j][i].enemy_texture, &hit_box_enemy);
							
#pragma endregion
									break;
							case BOMBER:
								if (enemys[j][i].vy != 0)
									enemys[j][i].status = FALLING;
								switch (enemys[j][i].status)
								{
								case RUSH:
									enemys[j][i].cor_delay -= dt;
									enemys[j][i].animation = (enemys[j][i].animation + dt) % (3 * 60);
									enemys[j][i].enemy_texture = { 3 + 153 * (enemys[j][i].animation / 60),1000,149,120 };
									if (enemys[j][i].cor_delay <= 0)
									{
										enemys[j][i].id = TUCHKA;
										enemys[j][i].size = 130;
										enemys[j][i].cor_delay = 2000;
									}
									break;
								case FALLING:

									enemys[j][i].vy += gravity / 1000 * float(dt);
									enemys[j][i].y += enemys[j][i].vy * float(dt) / 16;

									enemys[j][i].animation = (enemys[j][i].animation + dt) % (3 * 60);
									enemys[j][i].enemy_texture = { 3 + 134 * (enemys[j][i].animation / 60),834,130,140 };

									if (in_wall(enemys[j][i].x + enemys[j][i].vx * float(dt) / 16, enemys[j][i].y - 10, enemys[j][i].size, enemys[j][i].x, enemys[j][i].y))
									{
										enemys[j][i].vx *= -1;
										enemys[j][i].vy = 0;
									}
									if (enemys[j][i].x + enemys[j][i].vx * float(dt) / 16 <= 0)
									{
										enemys[j][i].x = 0;
										enemys[j][i].vx = -enemys[j][i].vx;
									}
									else if (enemys[j][i].x + enemys[j][i].vx * float(dt) / 16 >= wig - enemys[j][i].size)
									{
										enemys[j][i].x = wig - enemys[j][i].size;
										enemys[j][i].vx = -enemys[j][i].vx;
									}
									else
										enemys[j][i].x += enemys[j][i].vx * float(dt) / 16;

									if (enemys[j][i].y > heg)
										enemys[j][i].cor_hp = 0;

									if (on_platform(enemys[j][i].x, enemys[j][i].y, enemys[j][i].vy, dt, enemys[j][i].size))
									{
										enemys[j][i].vy = 0;
										if(enemys[j][i].cor_immortal_time>0)
										enemys[j][i].status = STAY;
										else
											enemys[j][i].status = GOING;
										enemys[j][i].cor_delay = enemys[j][i].attack_delay;
										enemys[j][i].animation = 0;
									}
									break;
								case STAY:
									

									//printf("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\n");
									if (enemys[j][i].cor_immortal_time > 0 and choice_direction_attack != DOWN)
									{
										enemys[j][i].x -= enemys[j][i].vx;
									}
									else if (enemys[j][i].cor_delay <= 0)
									{
										enemys[j][i].status = GOING;

										if (enemys[j][i].vx < 0)
											enemys[j][i].vx = -enemys[j][i].speed;
										else
											enemys[j][i].vx = enemys[j][i].speed;

										enemys[j][i].animation = 0;
									}
									else
										enemys[j][i].cor_delay -= dt;
									enemys[j][i].animation = (enemys[j][i].animation + dt) % (6 * 60);
									enemys[j][i].enemy_texture = { 3 + 125 * (enemys[j][i].animation / 60),23,120,150 };
									break;


								case GOING:

									if (enemys[j][i].y + enemys[j][i].size < y)
									{

										enemys[j][i].vy = -1;
									}
									else if (enemys[j][i].y > y + size_h_hero)
									{
										enemys[j][i].vy = -10;

									}
									else
									{
										enemys[j][i].vy = -3;
									}
									if (enemys[j][i].x < x + size_w_hero / 2)
										enemys[j][i].vx = enemys[j][i].speed;
									else
										enemys[j][i].vx = -enemys[j][i].speed;
									//if ()
										enemys[j][i].y += enemys[j][i].vy;
									enemys[j][i].status = FALLING;
								}
								if(enemys[j][i].vx<0)
								SDL_RenderCopy(ren, bomber_tex, &enemys[j][i].enemy_texture, &hit_box_enemy);
								else
									SDL_RenderCopyEx(ren, bomber_tex, &enemys[j][i].enemy_texture, &hit_box_enemy, 0, NULL, SDL_FLIP_HORIZONTAL);
								break;

							case TUCHKA:
								enemys[j][i].cor_delay -= dt;
								if (enemys[j][i].cor_delay <= 0)
									enemys[j][i].id = 0;
								SDL_RenderCopy(ren, tuchka_tex, NULL, &hit_box_enemy);
								break;
							case LIFESTEALB:
								SDL_RenderCopy(ren, ls_tex, NULL, &hit_box_enemy);
								break;
							case BIGEB:
								SDL_RenderCopy(ren, big_tex, NULL, &hit_box_enemy);
								break;
							case DAMAGEB:
								SDL_RenderCopy(ren, dm_tex, NULL, &hit_box_enemy);
								break;
							case HILKA:
								SDL_RenderCopy(ren, heal_tex, NULL, &hit_box_enemy);
								break;
							case MONETKA:
								SDL_RenderCopy(ren, mon_tex, NULL, &hit_box_enemy);
								break;


								break;
							}
							if (enemys[j][i].id > 0)
							{
								SDL_SetRenderDrawColor(ren, 180, 0, 0, 255);
								SDL_Rect hpbar_enemy = { enemys[j][i].x,enemys[j][i].y - 13,max(0,enemys[j][i].size * enemys[j][i].cor_hp / enemys[j][i].max_hp),8 };
								SDL_RenderFillRect(ren, &hpbar_enemy);
								//SDL_RenderDrawRect(ren, &hit_box_enemy);
								SDL_SetRenderDrawColor(ren, 255, 0, 0, 255);
								SDL_Rect max_hpbar_enemy = { enemys[j][i].x,enemys[j][i].y - 13,enemys[j][i].size,8 };
								SDL_RenderDrawRect(ren, &max_hpbar_enemy);
							}
						}
					printf("        %d   %d  ", enemy_counter, hp_hero);
#pragma endregion
#pragma region exit_from_screen
					{
						if (y > heg + 100)
						{
							y = 0 - size_h_hero - 10;
							vy = 1;
							hp_hero -= 10;
							cor_immortal_time_hero = 700;
						}

						if (y <-300)
						{
							y = heg/2;
							x = wig / 2;
						}
					}
					if (x > wig + size_w_hero)
						x = -size_w_hero;
					if (x < -size_w_hero)
						x = wig + size_w_hero;
#pragma endregion
					dash_cooldown -= dt;

					if (jump)
						secondjump = true;
					if (cor_immortal_time_hero > 0)
						cor_immortal_time_hero -= dt;
					if (hp_hero < 0)
					{
						round = false;
						runing = false;
						survive = false;
					}
					SDL_SetRenderDrawColor(ren, 0, 0, 200, 255);

					//SDL_RenderDrawRect(ren, &cub);
					SDL_SetRenderDrawColor(ren, 180, 0, 0, 255);
					SDL_Rect hpbar = { 30,30,max(0,300 * hp_hero / max_hp_hero),30 };
					SDL_RenderFillRect(ren, &hpbar);

					SDL_SetRenderDrawColor(ren, 255, 0, 0, 255);
					SDL_Rect max_hpbar = { 30,30,300,30 };
					SDL_RenderDrawRect(ren, &max_hpbar);


					int timetimer = timer;
					timer += dt;
					SDL_Rect timer_rect = { wig / 2,20,80,40 };
					SDL_Rect coins_rect = { wig -100,20,80,40 };
					//if(timer%1000< timetimer %1000)
					//{
					if (gamemod == 1)
						sprintf_s(strtimer, "%02i", (20000 - timer) / 1000);
					if (gamemod == 2)
						sprintf_s(strtimer, "%i", count_kills);
					str_surf = TTF_RenderText_Blended(pixelfont, strtimer, { 100,100,150,255 });
					str_rect = { 10,10,str_surf->w,str_surf->h };
					if (str_text != NULL) SDL_DestroyTexture(str_text);
					str_text = SDL_CreateTextureFromSurface(ren, str_surf);
					SDL_FreeSurface(str_surf);

					SDL_RenderCopy(ren, str_text, NULL, &timer_rect);

					sprintf_s(strtimer, "%i", coins);
					str_surf = TTF_RenderText_Blended(pixelfont, strtimer, { 180,170,0,255 });
					
					if (str_text != NULL) SDL_DestroyTexture(str_text);
					str_text = SDL_CreateTextureFromSurface(ren, str_surf);
					SDL_FreeSurface(str_surf);

					//}
					SDL_RenderCopy(ren, str_text, NULL, &coins_rect);
					SDL_RenderPresent(ren);
					//if(timer>1000)



					#pragma region work_with_timer
					if (gamemod == 2)
						if (timer > 1000)
							runing = false;

					if (gamemod == 1)
					{
						if (timer % 4000 < timetimer % 4000)
						{

							runing = false;
						}

						if (timer >= 20000)
						{
							runing = false;
							round = false;
							if (buffbig > 0)
							{
								max_hp_hero /= 2;
								hp_hero /= 2;
								length_sword /= 2;
								buffbig = 0;
							}
							buffdamage = 0;
							lifesteal = 0;
							
						}
						

					}
					if (gamemod == 3)
					{
						if (timer % 6000 < timetimer % 6000 or !enemy_counter)
						{

							runing = false;
						}

						if (timer >= 30000 or(count_enemy_wave>=4 and enemy_counter<(count_enemy_wave* wave_cost+3)))
						{
							runing = false;
							round = false;
						}
					}
#pragma endregion
					
				}

			/*	str_surf = TTF_RenderText_Blended(pixelfont, strtimer, { 100,100,150,255 });
				str_rect = { 10,10,str_surf->w,str_surf->h };
				str_text = SDL_CreateTextureFromSurface(ren, str_surf);*/
			}
			printf("aohipav;eovnilweb");
			
			//.............................................................................................
		}
		wave_survive++;
		saverecords(wave_survive, count_kills, 100000, gamemod);
		for (int i = 0; i < count_enemy_wave; i++)	
				free(enemys[i]);


		if (survive)
			shop(max_hp_hero,hp_hero,damage_hero,havedoublejump,coins);


		//free(enemys);
		free(platforms);
		//free_matrix(enemys, count_enemy_wave);
		//.............................................................................................
	}
	//SDL_DestroyTexture(back);
	free(enemys);
	TTF_CloseFont(basicfont);
	TTF_CloseFont(pixelfont);
	SDL_DestroyTexture(platforma);
	SDL_DestroyTexture(walla);
	SDL_DestroyTexture(hero_tex);
	SDL_DestroyTexture(basic_tex);
	SDL_DestroyTexture(fly_tex);
	SDL_DestroyTexture(bomber_tex);
	SDL_DestroyTexture(rasher_tex);
	SDL_DestroyTexture(ls_tex);
	SDL_DestroyTexture(dm_tex);
	SDL_DestroyTexture(big_tex);
	SDL_DestroyTexture(mon_tex);
	SDL_DestroyTexture(heal_tex);
	SDL_DestroyTexture(str_text);
	SDL_DestroyTexture(tuchka_tex);
	SDL_DestroyTexture(ufo_tex);
	SDL_DestroyTexture(laser_tex);
	
		
	
		
}


//SDL_Texture* genertext(const char* str, TTF_Font* font, SDL_Rect* rect, SDL_Color fg)
//{
//	SDL_Surface* surface = TTF_RenderText_Blended(font, str, fg);
//	*rect = { 0,0,surface->w,surface->h };
//
//	SDL_Texture* texture = SDL_CreateTextureFromSurface(ren, surface);
//	SDL_FreeSurface(surface);
//
//}

void menu()
{
	int w, k, t;
	
	loadrecords(w, k, t);
	char strk[200] = {}; sprintf_s(strk, "%i", k);
	char strw[200] = {}; sprintf_s(strw, "%i", w);
	char strt[200] = "DONTWORKED";// sprintf_s(strt, "%i", t);
	char strstart[] = "Start";
	char strshell[] = "Dont touch";
	char strsspeed[] = "speedrun";
	TTF_Font* basicfont = TTF_OpenFont("fonts\\basic.TTF", 75);

	SDL_Surface* str_surf = TTF_RenderText_Blended(basicfont, strstart, { 100,100,150,255 });
	SDL_Rect str_rect = { 10,10,str_surf->w,str_surf->h };
	SDL_Texture* str_textst = SDL_CreateTextureFromSurface(ren, str_surf);
	SDL_FreeSurface(str_surf);
	str_surf = TTF_RenderText_Blended(basicfont, strshell, { 0,0,0,255 });
	SDL_Texture* str_textdt = SDL_CreateTextureFromSurface(ren, str_surf);
	SDL_FreeSurface(str_surf);
	str_surf = TTF_RenderText_Blended(basicfont, strsspeed, { 220,220,220,255 });
	SDL_Texture* str_textsr = SDL_CreateTextureFromSurface(ren, str_surf);
	SDL_FreeSurface(str_surf);
	
	
	str_surf = TTF_RenderText_Blended(basicfont, strt, { 0,0,0,255 });
	SDL_Texture* str_t = SDL_CreateTextureFromSurface(ren, str_surf);
	SDL_FreeSurface(str_surf);
	bool runing = true;
	SDL_Event ev;
	while (runing)
	{
		str_surf = TTF_RenderText_Blended(basicfont, strk, { 0,0,0,255 });
		SDL_Texture* str_k = SDL_CreateTextureFromSurface(ren, str_surf);
		SDL_FreeSurface(str_surf);
		str_surf = TTF_RenderText_Blended(basicfont, strw, { 0,0,0,255 });
		SDL_Texture* str_w = SDL_CreateTextureFromSurface(ren, str_surf);
		SDL_FreeSurface(str_surf);
		loadrecords(w, k, t);
		 sprintf_s(strk, "%i", k);
		 sprintf_s(strw, "%i", w);
	int startx0 = wig/2-100, starty0 = heg/2-30, startx1 = 200, starty1 = 60;
	int hellx0 = wig / 2 - 100, helly0 = heg / 2 + 40, hellx1 = 200, helly1 = 60;
	int speedx0 = wig / 2 - 100, speedy0 = heg / 2 + 110, speedx1 = 200, speedy1 = 60;
	SDL_Rect but_start = { startx0,starty0,startx1,starty1 };
	SDL_Rect rec_start = { startx0+ startx1+10,starty0,40,starty1 };
	SDL_Rect but_hell = { hellx0,helly0,hellx1,helly1 };
	SDL_Rect rec_hell = { hellx0+ hellx1+10,helly0,60,helly1 };
	SDL_Rect but_speed = { speedx0,speedy0,speedx1,speedy1 };
	SDL_Rect rec_speed = { speedx0+ speedx1+10,speedy0,200,speedy1 };
		int x = 0, y = 0;
		while (SDL_PollEvent(&ev))
		{
			switch (ev.type)
			{
			case SDL_QUIT:
				runing = false;

				break;

			case SDL_WINDOWEVENT:
				if (ev.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
				{
					wig = ev.window.data1;
					heg = ev.window.data2;
				}
				break;

			case SDL_MOUSEBUTTONDOWN:
				if (ev.button.button == SDL_BUTTON_LEFT)
				{

					x = ev.button.x;
					y = ev.button.y;
				}
			case SDL_KEYDOWN:
				switch (ev.key.keysym.scancode)
				{
					break;
				case SDL_SCANCODE_ESCAPE:
					runing = false;
					break;
				}
		
			}

		}
		int moddd = 0;
		SDL_SetRenderDrawColor(ren, 200, 200, 200, 255);
		SDL_RenderClear(ren);
		if(tap_on_but(x, y, startx0, starty0, startx1, starty1))
			moddd=1;
		if (tap_on_but(x, y, hellx0, helly0, hellx1, helly1))
			moddd = 2;
	
	//	if (tap_on_but(x, y, speedx0, speedy0, speedx1, speedy1))
	//		moddd = 3;
		if(moddd)
			game(moddd);

		SDL_SetRenderDrawColor(ren, 200, 0, 200, 255);
		SDL_RenderFillRect(ren, &but_start);
		SDL_SetRenderDrawColor(ren, 200, 0, 0, 255);
		SDL_RenderFillRect(ren, &but_hell);
		SDL_SetRenderDrawColor(ren, 20, 20, 20, 255);
		SDL_RenderFillRect(ren, &but_speed);



		SDL_RenderCopy(ren, str_textst, NULL, &but_start);
		SDL_RenderCopy(ren, str_textsr, NULL, &but_speed);
		SDL_RenderCopy(ren, str_textdt, NULL, &but_hell);

		SDL_SetRenderDrawColor(ren, 0, 0, 200, 255);
		SDL_RenderCopy(ren, str_k, NULL, &rec_hell);
		SDL_RenderCopy(ren, str_w, NULL, &rec_start);
		SDL_RenderCopy(ren, str_t, NULL, &rec_speed);

		SDL_RenderPresent(ren);
		SDL_Delay(30);
	
		
	SDL_DestroyTexture(str_k);
	SDL_DestroyTexture(str_w);
	}
}

int main(int argc, char* argv[])
{
	Init();
	
	menu();
	//int maxhp = 100, hp = 70, damage=20 , coins=200;
	//bool flag = false;
	//shop(maxhp, hp, damage, flag, coins);
	//game();
	DeInit(0);
	return 0;	
}

/* 
* SDL_Rect r = { wig/2,heg/2,200,50 };
SDL_SetRenderDrawColor(ren, 200, 200, 200, 255);//выбор цвета
SDL_RenderFillRect(ren, &r);
SDL_RenderDrawRect(ren, &r);//нарисовать периметр прямоугольника
SDL_RenderFillRect(ren, &r);//нарисовать заполненный прямоугольник
SDL_Delay(size_cub00);//задержка в мс
SDL_RenderPresent(ren);//вывести на экран то что мы сделали(вывести из буфера)
SDL_RenderDrawLine(ren, 40-i, 60+i, 300-i, 40+i);//ну это линия написаны координаты начала и конца
SDL_RenderClear(ren);//изменить цвет окна
SDL_RenderDrawLine(ren, size_cub, heg/2, wig*2/3, 10);
*/
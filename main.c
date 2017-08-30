#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#define MAX_AFISAT 25
char* TEXT_PRINCIPAL = NULL;
char* TEXT_GEN = NULL;
char* TEXT_ACTORI = NULL;
char* TEXT_REGIZOR = NULL;
char* TEXT_USER = NULL;
char* TEXT_COMENT = NULL;
int NUMAR_AN = 0, NUMAR_MIN = 0, NUMAR_STELE = 0;
LPCTSTR WindowCaption = L"";

//Functii algoritm///////////////////////////////////
typedef struct FILM;
typedef struct user {
	char*        nume;
	struct user* next;
};
typedef struct lista_filme {
	struct FILM*		film;//referinta la un film existent
	struct lista_filme* next_list;
};
typedef struct lista_cautare {
	struct FILM*		film;//referinta la un film existent
	int                 potriviri;
	struct lista_cautare* next_list;
};
typedef struct actor {
	char*				nume;
	struct lista_filme* first_lista_filme;//personale
	struct actor*       next_actor;//GLOBAL
};
typedef struct lista_actori {
	struct actor*		 actor;//referinta la un actor existent
	struct lista_actori* next_list;
};
typedef struct comentariu {
	char*		       coment;
	struct comentariu* next_com;
};
typedef struct rating {
	struct user*        useri_votat;//evidenta pentru votat
	int                 nr_voturi;
	double              medie_voturi;
	struct comentariu*  first_list_com;
};
typedef struct regizor {
	char*	             nume;
	struct lista_filme*  first_lista_filme;//personale
	struct regizor*      next_regizor;//GLOBAL
};
typedef struct FILM {
	char*                titlu;
	int                  an;
	int                  durata;
	struct rating*		 rating;
	struct lista_actori* first_list_actori;
	struct regizor*		 regizor; //referinta la un regizor existent
	struct FILM*		 prec_film;
	struct FILM*		 next_film;
};
typedef struct GEN {
	char*          nume;
	struct FILM*   first_film;
	struct FILM*   last_film;
	struct GEN*    prec_gen;//GLOBAL
	struct GEN*    next_gen;//GLOBAL
};
struct GEN*     FIRST_GEN = NULL;
struct regizor* FIRST_REG = NULL;
struct actor*   FIRST_ACTOR = NULL;

int existaFilm(const char* titlu) {
	for (struct GEN* g = FIRST_GEN; g != NULL; g = g->next_gen)
		for (struct FILM* f = g->first_film; f != NULL; f = f->next_film)
			if (_stricmp(f->titlu, titlu) == 0)
				return 1;
	return 0;
}
int addGen(const char* nume) {
	for (struct GEN* g = FIRST_GEN; g != NULL; g = g->next_gen)
		if (_stricmp(g->nume, nume) == 0)
			return 0;

	struct GEN* newG = (struct GEN*)malloc(sizeof(struct GEN));
	newG->nume = (char*)malloc((strlen(nume) + 1) * sizeof(char)); strcpy(newG->nume, nume);
	newG->first_film = NULL;
	newG->last_film = NULL;
	if (FIRST_GEN == NULL) {
		FIRST_GEN = newG;
		FIRST_GEN->next_gen = NULL;
		FIRST_GEN->prec_gen = NULL;
		return 1;
	}
	else {
		int adaugat = 0;
		for (struct GEN* g = FIRST_GEN; g != NULL; g = g->next_gen) {
			if (_stricmp(g->nume, nume) > 0 && g == FIRST_GEN) {//PRIMUL
				g->prec_gen = newG;
				newG->next_gen = g;
				newG->prec_gen = NULL;
				FIRST_GEN = newG;
				return 1;
			}
			if (_stricmp(g->nume, nume) < 0) {
				if (g->next_gen != NULL) {
					g->next_gen->prec_gen = newG;
					newG->next_gen = g->next_gen;
				}
				else newG->next_gen = NULL; //ULTIMUL
				newG->prec_gen = g;
				g->next_gen = newG;
				return 1;
			}
		}
	}
}
int addVot(const char* titlu_film, const char* user, const int nr_stele) {
	if (nr_stele < 1 || nr_stele>10)
		return 11;

	for (struct GEN* g = FIRST_GEN; g != NULL; g = g->next_gen)
		for (struct FILM* f = g->first_film; f != NULL; f = f->next_film)
			if (_stricmp(titlu_film, f->titlu) == 0) {

				if (f->rating->useri_votat == NULL) {
					f->rating->useri_votat = (struct user*)malloc(sizeof(struct user));
					f->rating->useri_votat->nume = (char*)malloc((strlen(user) + 1) * sizeof(char)); strcpy(f->rating->useri_votat->nume, user);
					f->rating->useri_votat->next = NULL;
				}
				else {
					if (f->rating->useri_votat != NULL)
						for (struct user* u = f->rating->useri_votat; u != NULL; u = u->next)
							if (_stricmp(user, u->nume) == 0)
								return 2;

					struct user* u = (struct user*)malloc(sizeof(struct user));
					u->nume = (char*)malloc((strlen(user) + 1) * sizeof(char)); strcpy(u->nume, user);
					u->next = f->rating->useri_votat;
					f->rating->useri_votat = u;
				}

				f->rating->nr_voturi++;
				f->rating->medie_voturi = (double)(f->rating->medie_voturi*(f->rating->nr_voturi - 1) + nr_stele) / (f->rating->nr_voturi);
				return 1;
			}

	return 0;
}
int addComentariu(const char* titlu_film, const char* user, const char* coment) {
	for (struct GEN* g = FIRST_GEN; g != NULL; g = g->next_gen)
		for (struct FILM* f = g->first_film; f != NULL; f = f->next_film)
			if (_stricmp(titlu_film, f->titlu) == 0) {
				if (f->rating->first_list_com == NULL) {
					f->rating->first_list_com = (struct comentariu*)malloc(sizeof(struct comentariu));
					f->rating->first_list_com->coment = (char*)malloc((strlen(coment) + strlen(user) + 2 + 1) * sizeof(char));
					strcpy(f->rating->first_list_com->coment, user); strcat(f->rating->first_list_com->coment, ": "); strcat(f->rating->first_list_com->coment, coment);
					f->rating->first_list_com->next_com = NULL;
				}
				else {
					struct comentariu* com = (struct comentariu*)malloc(sizeof(struct comentariu));
					com->coment = (char*)malloc((strlen(coment) + strlen(user) + 2 + 1) * sizeof(char));
					strcpy(com->coment, user); strcat(com->coment, ": "); strcat(com->coment, coment);
					com->next_com = f->rating->first_list_com;
					f->rating->first_list_com = com;//devine primul si cel mai recent adaugat
				}
				return 1;
			}

	return 0;
}
int addRegizor(const char* nume_regizor) {
	for (struct GEN* g = FIRST_GEN; g != NULL; g = g->next_gen) 																		    //
		for (struct FILM* f = g->first_film; f != NULL; f = f->next_film) 																	//
			if (f->regizor != NULL)																												//
				if (_stricmp(f->regizor->nume, nume_regizor) == 0)
					return 0;

	struct regizor* reg = (struct regizor*)malloc(sizeof(struct regizor));//adaugam noul regizor
	reg->nume = (char*)malloc((strlen(nume_regizor) + 1) * sizeof(char)); strcpy(reg->nume, nume_regizor);
	reg->first_lista_filme = NULL;
	if (FIRST_REG != NULL)
		reg->next_regizor = FIRST_REG;																										     //
	else reg->next_regizor = NULL;																											     //
	FIRST_REG = reg;																															 //
	return 1;
}
int addActor(const char* nume_actor) {
	for (struct actor* actor = FIRST_ACTOR; actor != NULL; actor = actor->next_actor)
		if (_stricmp(actor->nume, nume_actor) == 0)
			return 0;

	struct actor* a = (struct actor*)malloc(sizeof(struct actor));
	a->nume = (char*)malloc((strlen(nume_actor) + 1) * sizeof(char));
	strcpy(a->nume, nume_actor);
	a->first_lista_filme = NULL;
	if (FIRST_ACTOR != NULL) {
		a->next_actor = FIRST_ACTOR;
		FIRST_ACTOR = a;
	}
	else {
		a->next_actor = NULL;
		FIRST_ACTOR = a;
	}
	return 1;
}
int addFilm(const char* nume_gen, const char* titlu, const int durata, const int an, char* actori, const char* nume_regizor) {
	addGen(nume_gen);
	for (struct GEN* g = FIRST_GEN; g != NULL; g = g->next_gen)
		for (struct FILM* f = g->first_film; f != NULL; f = f->next_film)
			if (_stricmp(f->titlu, titlu) == 0)
				return 0;

	int RET = 0;
	struct FILM* newF = (struct FILM*)malloc(sizeof(struct FILM));
	newF->titlu = (char*)malloc((strlen(titlu) + 1) * sizeof(char));
	strcpy(newF->titlu, titlu);

	newF->durata = durata;
	newF->an = an;
	newF->rating = (struct rating*)malloc(sizeof(struct rating));
	newF->rating->nr_voturi = 0;
	newF->rating->medie_voturi = 0.0;
	newF->rating->first_list_com = NULL;
	newF->rating->useri_votat = NULL;
	newF->regizor = NULL;
	newF->first_list_actori = NULL;

	//add regizor sau la regizor existent;
	if (1 == 1) {
		// ADD REGIZOR ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		int gata_reg = 0;																														    //
		for (struct GEN* g = FIRST_GEN; g != NULL; g = g->next_gen) {																			    //
			for (struct FILM* f = g->first_film; f != NULL; f = f->next_film) {																		//
				if (f->regizor != NULL)																												//
					if (_stricmp(f->regizor->nume, nume_regizor) == 0) {//l-am gasit, exista deja, doar ii adaugam in lista proprie noul film
						struct lista_filme* LIST_ITEM = (struct lista_filme*)malloc(sizeof(struct lista_filme));
						LIST_ITEM->film = newF;
						LIST_ITEM->next_list = f->regizor->first_lista_filme;
						f->regizor->first_lista_filme = LIST_ITEM;//devine primul in lista
						newF->regizor = f->regizor;
						RET = 2;//ASOCIAT
						gata_reg = 1;
						break;
					}
			}
			if (gata_reg == 1)
				break;
		}
		if (gata_reg == 0) {//il aduagam
			struct regizor* reg = (struct regizor*)malloc(sizeof(struct regizor));//adaugam noul regizor
			reg->nume = (char*)malloc((strlen(nume_regizor) + 1) * sizeof(char)); strcpy(reg->nume, nume_regizor);
			reg->first_lista_filme = (struct lista_filme*)malloc(sizeof(struct lista_filme));
			reg->first_lista_filme->film = newF;
			reg->first_lista_filme->next_list = NULL;
			if (FIRST_REG != NULL)
				reg->next_regizor = FIRST_REG;																										     //
			else reg->next_regizor = NULL;																											     //
			FIRST_REG = reg;																															 //
			newF->regizor = reg;																														 //
			RET = 1;	        																														 //
		}	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	}

	RET *= 10;
	//add sau asociere actori
	// ADD ACTORI ////////////////////////////////////////////////////////////////////////////////////////////////////////
	char* LISTA = (char*)malloc((strlen(actori) + 1) * sizeof(char)); strcpy(LISTA, actori);							//
	char* ACT = strtok(LISTA, ",;");																				    //
	while (ACT != NULL) {																								//																											
		struct lista_actori* ELEM_LISTA = (struct lista_actori*)malloc(sizeof(struct lista_actori));
		struct actor* ACT_EXISTENT = NULL;
		for (struct actor* actor = FIRST_ACTOR; actor != NULL; actor = actor->next_actor)
			if (_stricmp(actor->nume, ACT) == 0) {//l-am gasit
				ACT_EXISTENT = actor;
				break;
			}

		if (ACT_EXISTENT != NULL) {//_______________________________________________________________________
			int are_film_trecut = 0;
			for (struct lista_filme* lf = ACT_EXISTENT->first_lista_filme; lf != NULL; lf = lf->next_list)
				if (_stricmp(lf->film->titlu, newF->titlu) == 0) {
					are_film_trecut = 1;
					break;
				}
			if (are_film_trecut == 0) {//trebuie sa il punem
				struct lista_filme* list_item = (struct lista_filme*)malloc(sizeof(struct lista_filme));
				list_item->film = newF;
				if (ACT_EXISTENT->first_lista_filme != NULL) {
					list_item->next_list = ACT_EXISTENT->first_lista_filme;
					ACT_EXISTENT->first_lista_filme = list_item;
				}
				else {
					list_item->next_list = NULL;
					ACT_EXISTENT->first_lista_filme = list_item;
				}
			}//___________________________________________________________________________________________
			ELEM_LISTA->actor = ACT_EXISTENT;
			RET += 2;//ASOCIAT
		}
		else {//il cream----------------------------------------------------------------------------------
			struct actor* a = (struct actor*)malloc(sizeof(struct actor));
			a->nume = (char*)malloc((strlen(ACT) + 1) * sizeof(char));
			strcpy(a->nume, ACT);
			a->first_lista_filme = (struct lista_filme*)malloc(sizeof(struct lista_filme));
			a->first_lista_filme->film = newF;
			a->first_lista_filme->next_list = NULL;
			if (FIRST_ACTOR != NULL) {
				a->next_actor = FIRST_ACTOR;
				FIRST_ACTOR = a;
			}
			else {
				a->next_actor = NULL;
				FIRST_ACTOR = a;
			}
			ELEM_LISTA->actor = a;
			RET += 1;
		}//-----------------------------------------------------------------------------------------------

		ELEM_LISTA->next_list = NULL; ///adaugam in lista filmului
		if (newF->first_list_actori == NULL)
			newF->first_list_actori = ELEM_LISTA;
		else for (struct lista_actori* la = newF->first_list_actori; la != NULL; la = la->next_list)
			if (la->next_list == NULL) {
				la->next_list = ELEM_LISTA;
				break;
			}

		ACT = strtok(NULL, ",;");
		RET *= 10;//
	}																													//
	free(LISTA);																										//
																														//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	for (struct GEN* g = FIRST_GEN; g != NULL; g = g->next_gen)
		if (_stricmp(g->nume, nume_gen) == 0) {

			if (g->first_film == NULL) { //primul film al genului------------------------------------------
				newF->next_film = NULL;
				newF->prec_film = NULL;
				g->first_film = newF;
				return RET; //---------------------------------------------------------------------------------
			}
			else {
				for (struct FILM* f = g->first_film; f != NULL; f = f->next_film) {//.......................
					if (f->an >= an && f->next_film == NULL) {//ultimul (dupa f)
						newF->next_film = NULL;
						newF->prec_film = f;
						f->next_film = newF;
						return RET;
					}
					if (f->an < an) {
						newF->next_film = f;
						if (f == g->first_film) {//il punem primul (inaintea lui f)
							newF->prec_film = NULL;
							g->first_film = newF;//PRIMUL in lista
						}
						else {
							newF->prec_film = f->prec_film;
							newF->prec_film->next_film = newF;
						}
						f->prec_film = newF;
						return RET;
					}
				}//...........................................................................................
				return RET;
			}
		}
	return RET;
}
int addFilmFromFile(char* nume_fisier) {
	FILE* file = fopen(nume_fisier, "r");
	if (file == NULL)
		return -1;

	char gen[20], titlu[80], actori[200], regizor[30];
	int an, minute;

	fgets(gen, 20, file);
	if (TEXT_GEN != NULL)
		free(TEXT_GEN);
	TEXT_GEN = (char*)malloc((strlen(gen) + 1) * sizeof(char)); strcpy(TEXT_GEN, gen);
	while (!(TEXT_GEN[strlen(TEXT_GEN) - 1] >= 'a'&&TEXT_GEN[strlen(TEXT_GEN) - 1] <= 'z'))
		TEXT_GEN[strlen(TEXT_GEN) - 1] = NULL;

	fgets(titlu, 80, file);
	if (TEXT_PRINCIPAL != NULL)
		free(TEXT_PRINCIPAL);
	TEXT_PRINCIPAL = (char*)malloc((strlen(titlu) + 1) * sizeof(char)); strcpy(TEXT_PRINCIPAL, titlu);
	while (!(TEXT_PRINCIPAL[strlen(TEXT_PRINCIPAL) - 1] >= 'a'&&TEXT_PRINCIPAL[strlen(TEXT_PRINCIPAL) - 1] <= 'z'))
		TEXT_PRINCIPAL[strlen(TEXT_PRINCIPAL) - 1] = NULL;

	fgets(actori, 200, file);
	if (TEXT_ACTORI != NULL)
		free(TEXT_ACTORI);
	TEXT_ACTORI = (char*)malloc((strlen(actori) + 1) * sizeof(char)); strcpy(TEXT_ACTORI, actori);
	while (!(TEXT_ACTORI[strlen(TEXT_ACTORI) - 1] >= 'a'&&TEXT_ACTORI[strlen(TEXT_ACTORI) - 1] <= 'z'))
		TEXT_ACTORI[strlen(TEXT_ACTORI) - 1] = NULL;

	fgets(regizor, 30, file);
	if (TEXT_REGIZOR != NULL)
		free(TEXT_REGIZOR);
	TEXT_REGIZOR = (char*)malloc((strlen(regizor) + 1) * sizeof(char)); strcpy(TEXT_REGIZOR, regizor);
	while (!(TEXT_REGIZOR[strlen(TEXT_REGIZOR) - 1] >= 'a'&&TEXT_REGIZOR[strlen(TEXT_REGIZOR) - 1] <= 'z'))
		TEXT_REGIZOR[strlen(TEXT_REGIZOR) - 1] = NULL;

	fscanf(file, "%d %d", &an, &minute); NUMAR_MIN = minute; NUMAR_AN = an;

	int RET = addFilm(TEXT_GEN, TEXT_PRINCIPAL, NUMAR_MIN, NUMAR_AN, TEXT_ACTORI, TEXT_REGIZOR);//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

	if (file != NULL)
		fclose(file);
	return RET;
}
void printNoutati(HDC hDC, RECT rc, HWND FEREASTRA) {
	int XXX = 30;
	int gasit = 0;
	/*PAINTSTRUCT Ps;
	hDC = BeginPaint(FEREASTRA, &Ps);*/
	int max = 0, min = 3000;
	for (struct GEN* g = FIRST_GEN; g != NULL; g = g->next_gen)
		for (struct FILM* f = g->first_film; f != NULL; f = f->next_film) {
			if (f->an > max)
				max = f->an;
			if (f->an < min)
				min = f->an;
		}

	int COUNT = MAX_AFISAT;
	while (max >= min) {
		for (struct GEN* g = FIRST_GEN; g != NULL; g = g->next_gen)
			for (struct FILM* f = g->first_film; f != NULL; f = f->next_film)
				if (f->an == max) {
					gasit = 1;
					SetRect(&rc, 30, XXX, 400, 800); DrawText(hDC, "'", 1, &rc, DT_LEFT | DT_EXTERNALLEADING | DT_WORDBREAK);
					SetRect(&rc, 30 + 1 * 8, XXX, 400, 800); DrawText(hDC, f->titlu, strlen(f->titlu), &rc, DT_LEFT | DT_EXTERNALLEADING | DT_WORDBREAK);
					SetRect(&rc, 30 + (1 + strlen(f->titlu)) * 8, XXX, 400, 800); DrawText(hDC, "' (", 3, &rc, DT_LEFT | DT_EXTERNALLEADING | DT_WORDBREAK);
					char buff[5]; itoa(f->an, buff, 10);
					for (int i = 0; i<strlen(buff); i++)
						if (!(buff[i] >= '0'&&buff[i] <= '9')) {
							if (i == strlen(buff) - 1)
								buff[i] = NULL;
							else {
								int j;
								for (j = i + 1; j < strlen(buff); j++)
									buff[j - 1] = buff[j];
								buff[j] = NULL;
							}
						}
					SetRect(&rc, 30 + (4 + strlen(f->titlu)) * 8, XXX, 400, 800); DrawText(hDC, buff, strlen(buff), &rc, DT_LEFT | DT_EXTERNALLEADING | DT_WORDBREAK);
					SetRect(&rc, 30 + (8 + strlen(f->titlu)) * 8, XXX, 400, 800); DrawText(hDC, "),", 2, &rc, DT_LEFT | DT_EXTERNALLEADING | DT_WORDBREAK);
					int gen_gata = 0;
					for (struct GEN* gg = FIRST_GEN; gg != NULL; gg = gg->next_gen) {
						for (struct FILM* ff = gg->first_film; ff != NULL; ff = ff->next_film)
							if (_stricmp(ff->titlu, f->titlu) == 0) {
								SetRect(&rc, 30 + (10 + strlen(f->titlu)) * 8, XXX, 400, 800); DrawText(hDC, gg->nume, strlen(gg->nume), &rc, DT_LEFT | DT_EXTERNALLEADING | DT_WORDBREAK);
								COUNT--;
								if (COUNT == 0)//afisam maxim 15 noutati
									return;
								gen_gata = 1;
								break;
							}
						if (gen_gata == 1) {
							break;
						}
					}
					XXX += 16;
				}
		max--;
	}
	if (gasit == 0) {
		SetRect(&rc, 30, XXX, 400, 800); DrawText(hDC, "Nu s-au gasit rezultate.", 23, &rc, DT_LEFT | DT_EXTERNALLEADING | DT_WORDBREAK);
	}
	/*EndPaint(FEREASTRA, &Ps);*/
}
void printFilmePeGen(HDC hDC, RECT rc, HWND FEREASTRA, char* nume_gen) {
	int XXX = 30;
	int gasit = 0;
	/*PAINTSTRUCT Ps;
	hDC = BeginPaint(FEREASTRA, &Ps);*/

	for (struct GEN* g = FIRST_GEN; g != NULL; g = g->next_gen)
		if (_stricmp(g->nume, nume_gen) == 0) {
			gasit = 1;
			SetRect(&rc, 30, XXX, 500, 700); DrawText(hDC, "##### ", 6, &rc, DT_LEFT | DT_EXTERNALLEADING | DT_WORDBREAK);
			SetRect(&rc, 30 + 6 * 8, XXX, 500, 700); DrawText(hDC, g->nume, strlen(g->nume), &rc, DT_LEFT | DT_EXTERNALLEADING | DT_WORDBREAK);
			SetRect(&rc, 30 + (6 + strlen(g->nume)) * 8, XXX, 500, 700); DrawText(hDC, " #####", 6, &rc, DT_LEFT | DT_EXTERNALLEADING | DT_WORDBREAK);
			XXX += 45;

			int are = 0;
			for (struct FILM* f = g->first_film; f != NULL; f = f->next_film) {
				are = 1;
				SetRect(&rc, 30, XXX, 500, 700); DrawText(hDC, "'", 1, &rc, DT_LEFT | DT_EXTERNALLEADING | DT_WORDBREAK);
				SetRect(&rc, 30 + 1 * 8, XXX, 500, 700); DrawText(hDC, f->titlu, strlen(f->titlu), &rc, DT_LEFT | DT_EXTERNALLEADING | DT_WORDBREAK);
				SetRect(&rc, 30 + (1 + strlen(f->titlu)) * 8, XXX, 500, 700); DrawText(hDC, "' (", 3, &rc, DT_LEFT | DT_EXTERNALLEADING | DT_WORDBREAK);
				char buff[5]; itoa(f->an, buff, 10);
				for (int i = 0; i<strlen(buff); i++)
					if (!(buff[i] >= '0'&&buff[i] <= '9')) {
						if (i == strlen(buff) - 1)
							buff[i] = NULL;
						else {
							int j;
							for (j = i + 1; j < strlen(buff); j++)
								buff[j - 1] = buff[j];
							buff[j] = NULL;
						}
					}
				SetRect(&rc, 30 + (4 + strlen(f->titlu)) * 8, XXX, 500, 700); DrawText(hDC, buff, strlen(buff), &rc, DT_LEFT | DT_EXTERNALLEADING | DT_WORDBREAK);
				SetRect(&rc, 30 + (8 + strlen(f->titlu)) * 8, XXX, 500, 700); DrawText(hDC, "),", 2, &rc, DT_LEFT | DT_EXTERNALLEADING | DT_WORDBREAK);

				char buf[4];
				itoa(f->durata, buf, 10);
				for (int i = 0; i<strlen(buf); i++)
					if (!(buf[i] >= '0'&&buf[i] <= '9')) {
						if (i == strlen(buf) - 1)
							buf[i] = NULL;
						else {
							int j;
							for (j = i + 1; j < strlen(buf); j++)
								buf[j - 1] = buf[j];
							buf[j] = NULL;
						}
					}
				SetRect(&rc, 30 + (10 + strlen(f->titlu)) * 8, XXX, 500, 700); DrawText(hDC, buf, strlen(buf), &rc, DT_LEFT | DT_EXTERNALLEADING | DT_WORDBREAK);
				SetRect(&rc, 30 + (10 + strlen(f->titlu) + strlen(buf)) * 8, XXX, 500, 700); DrawText(hDC, " min", 4, &rc, DT_LEFT | DT_EXTERNALLEADING | DT_WORDBREAK);
				XXX += 16;
			}
			if (are == 0) {
				SetRect(&rc, 30, 75, 500, 700); DrawText(hDC, "Nu exista filme asociate.", 25, &rc, DT_LEFT | DT_EXTERNALLEADING | DT_WORDBREAK);
			}
			break;
		}
	if (gasit == 0) {
		SetRect(&rc, 30, XXX, 500, 700); DrawText(hDC, "Nu s-au gasit rezultate", 23, &rc, DT_LEFT | DT_EXTERNALLEADING | DT_WORDBREAK);
	}
	/*EndPaint(FEREASTRA, &Ps);*/
}
void printListaGenuri(HDC hDC, RECT rc, HWND FEREASTRA) {
	int XXX = 20;
	int YYY = 30;
	int count = 0;
	int gasit = 0;
	for (struct GEN* g = FIRST_GEN; g != NULL; g = g->next_gen) {
		gasit = 1;
		SetRect(&rc, YYY, XXX, 300, 100); DrawText(hDC, g->nume, strlen(g->nume), &rc, DT_LEFT | DT_EXTERNALLEADING | DT_WORDBREAK);
		XXX += 16;
		count++;
		if (count == 9) {
			count = 0;
			YYY += 120;
			XXX = 30;
		}
	}
	if (gasit == 0) {
		SetRect(&rc, 30, XXX, 300, 100); DrawText(hDC, "Nu exista genuri adaugate.", 26, &rc, DT_LEFT | DT_EXTERNALLEADING | DT_WORDBREAK);
	}
}
void printListaRegizori(HDC hDC, RECT rc, HWND FEREASTRA) {
	int XXX = 20;
	int YYY = 30;
	int count = 0;
	int gasit = 0;
	for (struct regizor* r = FIRST_REG; r != NULL; r = r->next_regizor) {
		gasit = 1;
		SetRect(&rc, YYY, XXX, 300, 100); DrawText(hDC, r->nume, strlen(r->nume), &rc, DT_LEFT | DT_EXTERNALLEADING | DT_WORDBREAK);
		XXX += 16;
		count++;
		if (count == 9) {
			count = 0;
			YYY += 120;
			XXX = 30;
		}
	}
	if (gasit == 0) {
		SetRect(&rc, 30, XXX, 300, 100); DrawText(hDC, "Nu exista regizori adaugati.", 28, &rc, DT_LEFT | DT_EXTERNALLEADING | DT_WORDBREAK);
	}
}
void printListaActori(HDC hDC, RECT rc, HWND FEREASTRA) {
	int XXX = 20;
	int YYY = 20;
	int count = 0;
	int gasit = 0;
	for (struct actor* a = FIRST_ACTOR; a != NULL; a = a->next_actor) {
		gasit = 1;
		SetRect(&rc, YYY, XXX, 600, 400); DrawText(hDC, a->nume, strlen(a->nume), &rc, DT_LEFT | DT_EXTERNALLEADING | DT_WORDBREAK);
		XXX += 16;
		count++;
		if (count == 15) {
			count = 0;
			YYY += 180;
			XXX = 20;
		}
	}
	if (gasit == 0) {
		SetRect(&rc, 30, XXX, 600, 400); DrawText(hDC, "Nu exista actori adaugati.", 26, &rc, DT_LEFT | DT_EXTERNALLEADING | DT_WORDBREAK);
	}
}
void printDetaliiActor(HDC hDC, RECT rc, HWND FEREASTRA, char* nume_actor) {
	int XXX = 30;
	/*PAINTSTRUCT Ps;
	hDC = BeginPaint(FEREASTRA, &Ps);*/

	for (struct actor* a = FIRST_ACTOR; a != NULL; a = a->next_actor)
		if (_stricmp(a->nume, nume_actor) == 0) {
			SetRect(&rc, 30, XXX, 480, 250); DrawText(hDC, "ACTORUL ", 8, &rc, DT_LEFT | DT_EXTERNALLEADING | DT_WORDBREAK);
			SetRect(&rc, 30 + 9 * 8, XXX, 480, 250); DrawText(hDC, nume_actor, strlen(nume_actor), &rc, DT_LEFT | DT_EXTERNALLEADING | DT_WORDBREAK);
			SetRect(&rc, 30 + (9 + strlen(nume_actor)) * 8, XXX, 480, 250); DrawText(hDC, ":", 1, &rc, DT_LEFT | DT_EXTERNALLEADING | DT_WORDBREAK);
			XXX += 45;

			int are = 0;
			for (struct lista_filme* lf = a->first_lista_filme; lf != NULL; lf = lf->next_list) {
				are = 1;
				SetRect(&rc, 30, XXX, 480, 250); DrawText(hDC, "'", 1, &rc, DT_LEFT | DT_EXTERNALLEADING | DT_WORDBREAK);
				SetRect(&rc, 30 + 1 * 8, XXX, 480, 250); DrawText(hDC, lf->film->titlu, strlen(lf->film->titlu), &rc, DT_LEFT | DT_EXTERNALLEADING | DT_WORDBREAK);
				SetRect(&rc, 30 + (1 + strlen(lf->film->titlu)) * 8, XXX, 480, 250); DrawText(hDC, "' (", 3, &rc, DT_LEFT | DT_EXTERNALLEADING | DT_WORDBREAK);
				char buff[5]; itoa(lf->film->an, buff, 10);
				for (int i = 0; i<strlen(buff); i++)
					if (!(buff[i] >= '0'&&buff[i] <= '9')) {
						if (i == strlen(buff) - 1)
							buff[i] = NULL;
						else {
							int j;
							for (j = i + 1; j < strlen(buff); j++)
								buff[j - 1] = buff[j];
							buff[j] = NULL;
						}
					}
				SetRect(&rc, 30 + (4 + strlen(lf->film->titlu)) * 8, XXX, 480, 250); DrawText(hDC, buff, strlen(buff), &rc, DT_LEFT | DT_EXTERNALLEADING | DT_WORDBREAK);
				SetRect(&rc, 30 + (8 + strlen(lf->film->titlu)) * 8, XXX, 480, 250); DrawText(hDC, "),", 2, &rc, DT_LEFT | DT_EXTERNALLEADING | DT_WORDBREAK);
				itoa(lf->film->durata, buff, 10);
				for (int i = 0; i<strlen(buff); i++)
					if (!(buff[i] >= '0'&&buff[i] <= '9')) {
						if (i == strlen(buff) - 1)
							buff[i] = NULL;
						else {
							int j;
							for (j = i + 1; j < strlen(buff); j++)
								buff[j - 1] = buff[j];
							buff[j] = NULL;
						}
					}
				SetRect(&rc, 30 + (10 + strlen(lf->film->titlu)) * 8, XXX, 480, 250); DrawText(hDC, buff, strlen(buff), &rc, DT_LEFT | DT_EXTERNALLEADING | DT_WORDBREAK);
				SetRect(&rc, 30 + (13 + strlen(lf->film->titlu)) * 8, XXX, 480, 250); DrawText(hDC, " min, ", 6, &rc, DT_LEFT | DT_EXTERNALLEADING | DT_WORDBREAK);

				int gen_gata = 0;
				for (struct GEN* gg = FIRST_GEN; gg != NULL; gg = gg->next_gen) {
					for (struct FILM* ff = gg->first_film; ff != NULL; ff = ff->next_film)
						if (_stricmp(ff->titlu, lf->film->titlu) == 0) {
							SetRect(&rc, 30 + (19 + strlen(lf->film->titlu)) * 8, XXX, 480, 250); DrawText(hDC, gg->nume, strlen(gg->nume), &rc, DT_LEFT | DT_EXTERNALLEADING | DT_WORDBREAK);
							gen_gata = 1;
							break;
						}
					if (gen_gata == 1) {
						break;
					}
				}
				XXX += 32;
			}
			if (are == 0) {
				SetRect(&rc, 30, 30, 480, 250); DrawText(hDC, "Nu exista filme asociate.", 25, &rc, DT_LEFT | DT_EXTERNALLEADING | DT_WORDBREAK);
			}
			return;
		}

	SetRect(&rc, 30, 30, 480, 250); DrawText(hDC, "Nu s-au gasit rezultate pentru actorul ", 39, &rc, DT_LEFT | DT_EXTERNALLEADING | DT_WORDBREAK);
	SetRect(&rc, 30 + 39 * 8, 30, 480, 250); DrawText(hDC, nume_actor, strlen(nume_actor), &rc, DT_LEFT | DT_EXTERNALLEADING | DT_WORDBREAK);
	//EndPaint(FEREASTRA, &Ps);
}
void printDetaliiRegizor(HDC hDC, RECT rc, HWND FEREASTRA, char* nume_regizor) {
	int XXX = 30;
	/*PAINTSTRUCT Ps;
	hDC = BeginPaint(FEREASTRA, &Ps);*/
	for (struct regizor* r = FIRST_REG; r != NULL; r = r->next_regizor)
		if (_stricmp(r->nume, nume_regizor) == 0) {
			SetRect(&rc, 30, XXX, 480, 250); DrawText(hDC, "REGIZORUL ", 10, &rc, DT_LEFT | DT_EXTERNALLEADING | DT_WORDBREAK);
			SetRect(&rc, 30 + 11 * 8, XXX, 480, 250); DrawText(hDC, nume_regizor, strlen(nume_regizor), &rc, DT_LEFT | DT_EXTERNALLEADING | DT_WORDBREAK);
			SetRect(&rc, 30 + (11 + strlen(nume_regizor)) * 8, XXX, 480, 250); DrawText(hDC, ":", 1, &rc, DT_LEFT | DT_EXTERNALLEADING | DT_WORDBREAK);
			XXX += 45;

			int are = 0;
			for (struct lista_filme* lf = r->first_lista_filme; lf != NULL; lf = lf->next_list) {
				are = 1;
				SetRect(&rc, 30, XXX, 480, 250); DrawText(hDC, "'", 1, &rc, DT_LEFT | DT_EXTERNALLEADING | DT_WORDBREAK);
				SetRect(&rc, 30 + 1 * 8, XXX, 480, 250); DrawText(hDC, lf->film->titlu, strlen(lf->film->titlu), &rc, DT_LEFT | DT_EXTERNALLEADING | DT_WORDBREAK);
				SetRect(&rc, 30 + (1 + strlen(lf->film->titlu)) * 8, XXX, 480, 250); DrawText(hDC, "' (", 3, &rc, DT_LEFT | DT_EXTERNALLEADING | DT_WORDBREAK);
				char buff[5]; itoa(lf->film->an, buff, 10);
				for (int i = 0; i<strlen(buff); i++)
					if (!(buff[i] >= '0'&&buff[i] <= '9')) {
						if (i == strlen(buff) - 1)
							buff[i] = NULL;
						else {
							int j;
							for (j = i + 1; j < strlen(buff); j++)
								buff[j - 1] = buff[j];
							buff[j] = NULL;
						}
					}
				SetRect(&rc, 30 + (4 + strlen(lf->film->titlu)) * 8, XXX, 480, 250); DrawText(hDC, buff, strlen(buff), &rc, DT_LEFT | DT_EXTERNALLEADING | DT_WORDBREAK);
				SetRect(&rc, 30 + (8 + strlen(lf->film->titlu)) * 8, XXX, 480, 250); DrawText(hDC, "),", 2, &rc, DT_LEFT | DT_EXTERNALLEADING | DT_WORDBREAK);
				itoa(lf->film->durata, buff, 10);
				for (int i = 0; i<strlen(buff); i++)
					if (!(buff[i] >= '0'&&buff[i] <= '9')) {
						if (i == strlen(buff) - 1)
							buff[i] = NULL;
						else {
							int j;
							for (j = i + 1; j < strlen(buff); j++)
								buff[j - 1] = buff[j];
							buff[j] = NULL;
						}
					}
				SetRect(&rc, 30 + (10 + strlen(lf->film->titlu)) * 8, XXX, 480, 250); DrawText(hDC, buff, strlen(buff), &rc, DT_LEFT | DT_EXTERNALLEADING | DT_WORDBREAK);
				SetRect(&rc, 30 + (13 + strlen(lf->film->titlu)) * 8, XXX, 480, 250); DrawText(hDC, " min, ", 6, &rc, DT_LEFT | DT_EXTERNALLEADING | DT_WORDBREAK);

				int gen_gata = 0;
				for (struct GEN* gg = FIRST_GEN; gg != NULL; gg = gg->next_gen) {
					for (struct FILM* ff = gg->first_film; ff != NULL; ff = ff->next_film)
						if (_stricmp(ff->titlu, lf->film->titlu) == 0) {
							SetRect(&rc, 30 + (19 + strlen(lf->film->titlu)) * 8, XXX, 480, 250); DrawText(hDC, gg->nume, strlen(gg->nume), &rc, DT_LEFT | DT_EXTERNALLEADING | DT_WORDBREAK);
							gen_gata = 1;
							break;
						}
					if (gen_gata == 1) {
						break;
					}
				}
				XXX += 32;
			}
			if (are == 0) {
				SetRect(&rc, 30, 30, 480, 250); DrawText(hDC, "Nu exista filme asociate.", 25, &rc, DT_LEFT | DT_EXTERNALLEADING | DT_WORDBREAK);
			}
			return;
		}

	SetRect(&rc, 30, 30, 480, 250); DrawText(hDC, "Nu s-au gasit rezultate pentru regizorul ", 41, &rc, DT_LEFT | DT_EXTERNALLEADING | DT_WORDBREAK);
	SetRect(&rc, 30 + 41 * 8, 30, 480, 250); DrawText(hDC, nume_regizor, strlen(nume_regizor), &rc, DT_LEFT | DT_EXTERNALLEADING | DT_WORDBREAK);
	//EndPaint(FEREASTRA, &Ps);
}
void printDetaliiFilm(HDC hDC, RECT rc, HWND FEREASTRA, char* nume_film) {
	struct lista_cautare* ListaCuRezultateleCautariiDeFilme = NULL;
	int dimF = 0, dimT = 0, di = 0, dj = 0, max = 0, nr_rezultate = 0, nr_cuv_scris = 0;;
	char* NUME_F = NULL;
	char* NUME_TITLU = NULL;

	NUME_F = (char*)malloc((strlen(nume_film) + 1) * sizeof(char)); strcpy(NUME_F, nume_film);
	dimF = strlen(NUME_F);
	for (int i = 0; i < dimF; i++) {
		int ok = 0;
		if ((NUME_F[i] >= 'a' && NUME_F[i] <= 'z') || (NUME_F[i] >= 'A' && NUME_F[i] <= 'Z') || (NUME_F[i] >= '0' && NUME_F[i] <= '9') || NUME_F[i] == '-')
			ok = 1;
		if (ok == 0)
			NUME_F[i] = '#';
		if (i == dimF - 1)
			break;
	}
	char* temp = (char*)malloc((dimF + 1) * sizeof(char)); strcpy(temp, NUME_F);
	char* p = strtok(temp, "#");
	while (p != NULL) {
		nr_cuv_scris++;
		p = strtok(NULL, "#");
	}
	free(temp); temp = NULL;

	for (struct GEN* g = FIRST_GEN; g != NULL; g = g->next_gen)
		for (struct FILM* f = g->first_film; f != NULL; f = f->next_film)
		{//0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
			int COUNTER = 0;
			NUME_TITLU = (char*)malloc((strlen(f->titlu) + 1) * sizeof(char)); strcpy(NUME_TITLU, f->titlu);
			dimT = strlen(NUME_TITLU);
			for (int i = 0; i < dimT; i++) {
				int ok = 0;
				if ((NUME_TITLU[i] >= 'a' && NUME_TITLU[i] <= 'z') || (NUME_TITLU[i] >= 'A' && NUME_TITLU[i] <= 'Z') || (NUME_TITLU[i] >= '0' && NUME_TITLU[i] <= '9') || NUME_TITLU[i] == '-')
					ok = 1;
				if (ok == 0)
					NUME_TITLU[i] = '#';
				if (i == dimT - 1)
					break;
			}

			int pozitie = 0;
			while (1 == 1) { //_________________________________________________________________________________________________________________
				char* cuv_t = NULL;
				int i = 0;
				for (i = pozitie; i < dimT; i++) {
					if (i == pozitie && NUME_TITLU[i] == '#') {//doua spatii consecutive
						pozitie++;
						i = pozitie;
						continue;
					}
					if (i == pozitie && (NUME_TITLU[i + 1] == '#' || i == dimT - 1)) {//cuvant o litera
						cuv_t = (char*)malloc(2 * sizeof(char));
						cuv_t[0] = NUME_TITLU[i];
						cuv_t[1] = NULL;
						pozitie += 2;
						break;
					}
					if (NUME_TITLU[i + 1] == '#' || i == dimT - 1) { //AM PARSAT UN CUVANT
						cuv_t = (char*)malloc((i - pozitie + 2) * sizeof(char));
						int ii = 0;
						for (int k = pozitie; k <= i; k++, ii++)
							cuv_t[ii] = NUME_TITLU[k];
						cuv_t[ii] = NULL;
						di = ii;
						pozitie = i + 2;//urmatorul cuvant de la pozitia viitoare
						break;
					}
				}

				int poz = 0;
				while (2 == 2) { //-----------------------------------------------------------------------------
					char* cuv_f = NULL;
					int j = 0;
					for (j = poz; j < dimF; j++) {
						if (j == poz && NUME_F[j] == '#') {//doua spatii consecutive
							poz++;
							j = poz;
							continue;
						}
						if (j == poz && (NUME_F[j + 1] == '#' || j == dimF - 1)) {//cuvant o litera
							cuv_f = (char*)malloc(2 * sizeof(char));
							cuv_f[0] = NUME_F[j];
							cuv_f[1] = NULL;
							poz += 2;
							break;
						}
						if (NUME_F[j + 1] == '#' || j == dimF - 1) { //AM PARSAT UN CUVANT
							cuv_f = (char*)malloc((j - poz + 2) * sizeof(char));
							int jj = 0;
							for (int k = poz; k <= j; k++, jj++)
								cuv_f[jj] = NUME_F[k];
							cuv_f[jj] = NULL;
							dj = jj;
							poz = j + 2;//urmatorul cuvant de la pozitia viitoare
							break;
						}
					}

					if (di == dj && cuv_f != NULL && cuv_t != NULL) {
						int ok = 0;
						for (int k = 0; k <di; k++)
							if (cuv_f[k] == cuv_t[k] || cuv_f[k] == cuv_t[k] + 32 || cuv_f[k] == cuv_t[k] - 32)
								ok = 1;
							else {
								ok = 0;
								break;
							}
							if (ok == 1)
								COUNTER++;
					}
					free(cuv_f); cuv_f = NULL;
					if (j >= dimF)
						break;
				} //-----------------------------------------------------------------------------------------
				free(cuv_t); cuv_t = NULL;
				if (i >= dimT)
					break;
			}//__________________________________________________________________________________________________________________________________

			if (COUNTER > max)
				max = COUNTER;
			if (COUNTER > 0) {//adaugam in lista de rezultate////////////////////////////////////////////////////
				nr_rezultate++;
				struct lista_cautare* LF = (struct lista_cautare*)malloc(sizeof(struct lista_cautare));
				LF->film = f;
				LF->potriviri = COUNTER;
				if (nr_rezultate == 1)
					LF->next_list = NULL;
				else LF->next_list = ListaCuRezultateleCautariiDeFilme;
				ListaCuRezultateleCautariiDeFilme = LF;
			}//gata adaugarea///////////////////////////////////////////////////////////////////////////////

		}//000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000

		 /*PAINTSTRUCT Ps;
		 hDC = BeginPaint(FEREASTRA, &Ps);*/
	int XXX = 30;
	char tx[100];
	if (nr_rezultate == 0) {
		strcpy(tx, "Nu exista rezultate pentru cautarea filmului "); strcat(tx, nume_film);
		SetRect(&rc, 30, XXX, 500, 700); DrawText(hDC, tx, strlen(tx), &rc, DT_LEFT | DT_EXTERNALLEADING | DT_WORDBREAK);
	}
	else {
		strcpy(tx, "S-A(U) GASIT ");
		char buff[3]; itoa(nr_rezultate, buff, 10);
		for (int i = 0; i<strlen(buff); i++)
			if (!(buff[i] >= '0'&&buff[i] <= '9')) {
				if (i == strlen(buff) - 1)
					buff[i] = NULL;
				else {
					int j;
					for (j = i + 1; j < strlen(buff); j++)
						buff[j - 1] = buff[j];
					buff[j] = NULL;
				}
			}
		strcat(tx, buff);
		strcat(tx, " REZULTAT(E) PENTRU '");
		strcat(tx, nume_film);
		strcat(tx, "' :");
		SetRect(&rc, 30, XXX, 500, 700); DrawText(hDC, tx, strlen(tx), &rc, DT_LEFT | DT_EXTERNALLEADING | DT_WORDBREAK);
		XXX += 45;

		int i = 0;
		while (max > 0) {
			for (struct lista_cautare* lf = ListaCuRezultateleCautariiDeFilme; lf != NULL; lf = lf->next_list, i++)
				if (lf->potriviri == max) {
					SetRect(&rc, 30, XXX, 500, 700); DrawText(hDC, "  (", 3, &rc, DT_LEFT | DT_EXTERNALLEADING | DT_WORDBREAK);
					char buf[3]; itoa(lf->potriviri, buf, 10);
					for (int i = 0; i<strlen(buf); i++)
						if (!(buf[i] >= '0'&&buf[i] <= '9')) {
							if (i == strlen(buf) - 1)
								buf[i] = NULL;
							else {
								int j;
								for (j = i + 1; j < strlen(buf); j++)
									buf[j - 1] = buf[j];
								buf[j] = NULL;
							}
						}
					SetRect(&rc, 30 + 3 * 8, XXX, 500, 700); DrawText(hDC, buf, strlen(buf), &rc, DT_LEFT | DT_EXTERNALLEADING | DT_WORDBREAK);
					SetRect(&rc, 30 + 6 * 8, XXX, 500, 700); DrawText(hDC, "/", 1, &rc, DT_LEFT | DT_EXTERNALLEADING | DT_WORDBREAK);
					char fub[3]; itoa(nr_cuv_scris, fub, 10);
					for (int i = 0; i<strlen(fub); i++)
						if (!(fub[i] >= '0'&&fub[i] <= '9')) {
							if (i == strlen(fub) - 1)
								fub[i] = NULL;
							else {
								int j;
								for (j = i + 1; j < strlen(fub); j++)
									fub[j - 1] = fub[j];
								fub[j] = NULL;
							}
						}
					SetRect(&rc, 30 + 7 * 8, XXX, 500, 700); DrawText(hDC, fub, strlen(fub), &rc, DT_LEFT | DT_EXTERNALLEADING | DT_WORDBREAK);
					SetRect(&rc, 30 + 10 * 8, XXX, 500, 700); DrawText(hDC, ")", 1, &rc, DT_LEFT | DT_EXTERNALLEADING | DT_WORDBREAK);
					XXX += 16;

					SetRect(&rc, 30, XXX, 500, 700); DrawText(hDC, "'", 1, &rc, DT_LEFT | DT_EXTERNALLEADING | DT_WORDBREAK);
					SetRect(&rc, 30 + 1 * 8, XXX, 500, 700); DrawText(hDC, lf->film->titlu, strlen(lf->film->titlu), &rc, DT_LEFT | DT_EXTERNALLEADING | DT_WORDBREAK);
					SetRect(&rc, 30 + (1 + strlen(lf->film->titlu)) * 8, XXX, 500, 700); DrawText(hDC, "' (", 3, &rc, DT_LEFT | DT_EXTERNALLEADING | DT_WORDBREAK);
					char buff[5]; itoa(lf->film->an, buff, 10);
					for (int i = 0; i<strlen(buff); i++)
						if (!(buff[i] >= '0'&&buff[i] <= '9')) {
							if (i == strlen(buff) - 1)
								buff[i] = NULL;
							else {
								int j;
								for (j = i + 1; j < strlen(buff); j++)
									buff[j - 1] = buff[j];
								buff[j] = NULL;
							}
						}
					SetRect(&rc, 30 + (4 + strlen(lf->film->titlu)) * 8, XXX, 500, 700); DrawText(hDC, buff, strlen(buff), &rc, DT_LEFT | DT_EXTERNALLEADING | DT_WORDBREAK);
					SetRect(&rc, 30 + (8 + strlen(lf->film->titlu)) * 8, XXX, 500, 700); DrawText(hDC, "),", 2, &rc, DT_LEFT | DT_EXTERNALLEADING | DT_WORDBREAK);
					itoa(lf->film->durata, buff, 10);
					for (int i = 0; i<strlen(buff); i++)
						if (!(buff[i] >= '0'&&buff[i] <= '9')) {
							if (i == strlen(buff) - 1)
								buff[i] = NULL;
							else {
								int j;
								for (j = i + 1; j < strlen(buff); j++)
									buff[j - 1] = buff[j];
								buff[j] = NULL;
							}
						}
					SetRect(&rc, 30 + (10 + strlen(lf->film->titlu)) * 8, XXX, 500, 700); DrawText(hDC, buff, strlen(buff), &rc, DT_LEFT | DT_EXTERNALLEADING | DT_WORDBREAK);
					SetRect(&rc, 30 + (13 + strlen(lf->film->titlu)) * 8, XXX, 500, 700); DrawText(hDC, " min", 4, &rc, DT_LEFT | DT_EXTERNALLEADING | DT_WORDBREAK);
					XXX += 16;

					int gen_gata = 0;
					for (struct GEN* gg = FIRST_GEN; gg != NULL; gg = gg->next_gen) {
						for (struct FILM* ff = gg->first_film; ff != NULL; ff = ff->next_film)
							if (_stricmp(ff->titlu, lf->film->titlu) == 0) {
								SetRect(&rc, 30, XXX, 500, 700); DrawText(hDC, "  Gen: ", 7, &rc, DT_LEFT | DT_EXTERNALLEADING | DT_WORDBREAK);
								SetRect(&rc, 30 + 7 * 8, XXX, 500, 700); DrawText(hDC, gg->nume, strlen(gg->nume), &rc, DT_LEFT | DT_EXTERNALLEADING | DT_WORDBREAK);
								gen_gata = 1;
								break;
							}
						if (gen_gata == 1) {
							break;
						}
					}
					XXX += 16;

					SetRect(&rc, 30, XXX, 500, 700); DrawText(hDC, "  Regizor: ", 11, &rc, DT_LEFT | DT_EXTERNALLEADING | DT_WORDBREAK);
					SetRect(&rc, 30 + 11 * 8, XXX, 500, 700); DrawText(hDC, lf->film->regizor->nume, strlen(lf->film->regizor->nume), &rc, DT_LEFT | DT_EXTERNALLEADING | DT_WORDBREAK);
					XXX += 16;

					if (nr_rezultate == 1) {//detalii mai multe doar la o cautare exacta
						SetRect(&rc, 30, XXX, 500, 700); DrawText(hDC, "  Rating: ", 10, &rc, DT_LEFT | DT_EXTERNALLEADING | DT_WORDBREAK);
						FILE* file = fopen("a.txt", "w");
						fprintf(file, "%.1f", lf->film->rating->medie_voturi);
						fclose(file);
						file = fopen("a.txt", "r");
						fgets(buff, 4, file);
						fclose(file);
						SetRect(&rc, 30 + 10 * 8, XXX, 500, 700); DrawText(hDC, buff, 4, &rc, DT_LEFT | DT_EXTERNALLEADING | DT_WORDBREAK);
						SetRect(&rc, 30 + 14 * 8, XXX, 500, 700); DrawText(hDC, " (", 2, &rc, DT_LEFT | DT_EXTERNALLEADING | DT_WORDBREAK);
						itoa(lf->film->rating->nr_voturi, buf, 10);
						for (int i = 0; i<strlen(buf); i++)
							if (!(buf[i] >= '0'&&buf[i] <= '9')) {
								if (i == strlen(buf) - 1)
									buf[i] = NULL;
								else {
									int j;
									for (j = i + 1; j < strlen(buf); j++)
										buf[j - 1] = buf[j];
									buf[j] = NULL;
								}
							}
						SetRect(&rc, 30 + 16 * 8, XXX, 500, 700); DrawText(hDC, buf, strlen(buf), &rc, DT_LEFT | DT_EXTERNALLEADING | DT_WORDBREAK);
						SetRect(&rc, 30 + 20 * 8, XXX, 500, 700); DrawText(hDC, " voturi)", 8, &rc, DT_LEFT | DT_EXTERNALLEADING | DT_WORDBREAK);
						XXX += 16;

						SetRect(&rc, 30, XXX, 500, 700); DrawText(hDC, "  Actori: ", 10, &rc, DT_LEFT | DT_EXTERNALLEADING | DT_WORDBREAK);
						for (struct lista_actori* a = lf->film->first_list_actori; a != NULL; a = a->next_list) {
							SetRect(&rc, 30 + 10 * 8, XXX, 500, 700); DrawText(hDC, a->actor->nume, strlen(a->actor->nume), &rc, DT_LEFT | DT_EXTERNALLEADING | DT_WORDBREAK);
							XXX += 16;
						}

						SetRect(&rc, 30, XXX, 500, 700); DrawText(hDC, "  Comentarii: ", 13, &rc, DT_LEFT | DT_EXTERNALLEADING | DT_WORDBREAK);
						XXX += 16;
						if (lf->film->rating->first_list_com != NULL) {
							for (struct comentariu* com = lf->film->rating->first_list_com; com != NULL; com = com->next_com) {
								SetRect(&rc, 30 + 6 * 8, XXX, 500, 700); DrawText(hDC, com->coment, strlen(com->coment), &rc, DT_LEFT | DT_EXTERNALLEADING | DT_WORDBREAK);
								XXX += 16;
							}
						}
						else {
							SetRect(&rc, 30 + 6 * 8, XXX, 500, 700); DrawText(hDC, "Nu sunt comentarii de afisat.", 29, &rc, DT_LEFT | DT_EXTERNALLEADING | DT_WORDBREAK);
						}
						XXX += 32; //urm film
					}
				}
			max--;
		}
	}
	/*EndPaint(FEREASTRA, &Ps);*/

	if (nr_rezultate > 0) //clear
		if (ListaCuRezultateleCautariiDeFilme != NULL)
			for (struct lista_cautare* lf = ListaCuRezultateleCautariiDeFilme; lf != NULL; ) {
				struct lista_cautare* LF_ITEM = lf;
				lf = lf->next_list;
				free(LF_ITEM); LF_ITEM = NULL;
			}
}
void printFilmePeAn(HDC hDC, RECT rc, HWND FEREASTRA, const int an) {
	char buf[5]; itoa(an, buf, 10);
	for (int i = 0; i<strlen(buf); i++)
		if (!(buf[i] >= '0'&&buf[i] <= '9')) {
			if (i == strlen(buf) - 1)
				buf[i] = NULL;
			else {
				int j;
				for (j = i + 1; j < strlen(buf); j++)
					buf[j - 1] = buf[j];
				buf[j] = NULL;
			}
		}
	int XXX = 30;
	/*PAINTSTRUCT Ps;
	hDC = BeginPaint(FEREASTRA, &Ps);*/

	SetRect(&rc, 30, XXX, 500, 700); DrawText(hDC, "FILME DIN ANUL ", 15, &rc, DT_LEFT | DT_EXTERNALLEADING | DT_WORDBREAK);
	SetRect(&rc, 30 + 15 * 8, XXX, 500, 700); DrawText(hDC, buf, strlen(buf), &rc, DT_LEFT | DT_EXTERNALLEADING | DT_WORDBREAK);
	SetRect(&rc, 30 + 19 * 8, XXX, 500, 700); DrawText(hDC, ":", 1, &rc, DT_LEFT | DT_EXTERNALLEADING | DT_WORDBREAK);
	XXX += 45;

	int gasit = 0;
	for (struct GEN* g = FIRST_GEN; g != NULL; g = g->next_gen)
		for (struct FILM* f = g->first_film; f != NULL; f = f->next_film)
			if (f->an == an)
			{
				gasit = 1;
				SetRect(&rc, 30, XXX, 500, 700); DrawText(hDC, "'", 1, &rc, DT_LEFT | DT_EXTERNALLEADING | DT_WORDBREAK);
				SetRect(&rc, 30 + 1 * 8, XXX, 500, 700); DrawText(hDC, f->titlu, strlen(f->titlu), &rc, DT_LEFT | DT_EXTERNALLEADING | DT_WORDBREAK);
				SetRect(&rc, 30 + (1 + strlen(f->titlu)) * 8, XXX, 500, 700); DrawText(hDC, "' (", 3, &rc, DT_LEFT | DT_EXTERNALLEADING | DT_WORDBREAK);
				char buff[5]; itoa(f->an, buff, 10);
				for (int i = 0; i<strlen(buff); i++)
					if (!(buff[i] >= '0'&&buff[i] <= '9')) {
						if (i == strlen(buff) - 1)
							buff[i] = NULL;
						else {
							int j;
							for (j = i + 1; j < strlen(buff); j++)
								buff[j - 1] = buff[j];
							buff[j] = NULL;
						}
					}
				SetRect(&rc, 30 + (4 + strlen(f->titlu)) * 8, XXX, 500, 700); DrawText(hDC, buff, strlen(buff), &rc, DT_LEFT | DT_EXTERNALLEADING | DT_WORDBREAK);
				SetRect(&rc, 30 + (8 + strlen(f->titlu)) * 8, XXX, 500, 700); DrawText(hDC, "),", 2, &rc, DT_LEFT | DT_EXTERNALLEADING | DT_WORDBREAK);
				int gen_gata = 0;
				for (struct GEN* gg = FIRST_GEN; gg != NULL; gg = gg->next_gen) {
					for (struct FILM* ff = gg->first_film; ff != NULL; ff = ff->next_film)
						if (_stricmp(ff->titlu, f->titlu) == 0) {
							SetRect(&rc, 30 + (10 + strlen(f->titlu)) * 8, XXX, 500, 700); DrawText(hDC, gg->nume, strlen(gg->nume), &rc, DT_LEFT | DT_EXTERNALLEADING | DT_WORDBREAK);
							gen_gata = 1;
							break;
						}
					if (gen_gata == 1) {
						break;
					}
				}
				XXX += 32;
			}
	if (gasit == 0) {
		SetRect(&rc, 30, XXX, 500, 700); DrawText(hDC, "Nu s-au gasit rezultate", 23, &rc, DT_LEFT | DT_EXTERNALLEADING | DT_WORDBREAK);
	}
	/*EndPaint(FEREASTRA, &Ps);*/
}
int save() {
	int ok = 0;
	FILE* ForLoad = fopen("load.txt", "w");
	for (struct GEN* g = FIRST_GEN; g != NULL; g = g->next_gen)
		for (struct FILM* f = g->first_film; f != NULL; f = f->next_film) {
			ok = 1;
			char* nume = (char*)malloc((strlen(f->titlu) + 5) * sizeof(char));
			strcpy(nume, f->titlu); strcat(nume, ".txt");

			fputs(nume, ForLoad); fputs("\n", ForLoad);//se salveaza in lista de load
			FILE* file = fopen(nume, "w");

			fputs(g->nume, file); fputs("\n", file);
			fputs(f->titlu, file); fputs("\n", file);
			for (struct lista_actori* la = f->first_list_actori; la != NULL; la = la->next_list) {
				fputs(la->actor->nume, file);
				if (la->next_list != NULL)
					fputs(",", file);
			}
			fputs("\n", file);
			fputs(f->regizor->nume, file); fputs("\n", file);
			char an[5], min[4];
			itoa(f->an, an, 10); itoa(f->durata, min, 10);
			for (int i = 0; i<strlen(an); i++)
				if (!(an[i] >= '0'&&an[i] <= '9')) {
					if (i == strlen(an) - 1)
						an[i] = NULL;
					else {
						int j;
						for (j = i + 1; j < strlen(an); j++)
							an[j - 1] = an[j];
						an[j] = NULL;
					}
				}
			for (int i = 0; i<strlen(min); i++)
				if (!(min[i] >= '0'&&min[i] <= '9')) {
					if (i == strlen(min) - 1)
						min[i] = NULL;
					else {
						int j;
						for (j = i + 1; j < strlen(min); j++)
							min[j - 1] = min[j];
						min[j] = NULL;
					}
				}
			fputs(an, file); fputs(" ", file);
			fputs(min, file);

			fclose(file);
			free(nume); nume = NULL;
		}
	fclose(ForLoad);
	return ok;
}
int load() {
	FILE* file = fopen("load.txt", "r");
	if (file == NULL)
		return 0;

	int ok = -1;
	int count = 0;
	char doc[30];
	while (fgets(doc, 30, file)) {
		int i = addFilmFromFile(doc);
		if (i == -1) {
			char text[100]; strcpy(text, "Eroare deschidere fisier "); strcat(text, doc);
			MessageBox(NULL, text, WindowCaption, MB_OK);
		}
		else if (i == 0) {
			char text[100]; strcpy(text, "Filmul din fisierul "); strcat(text, doc); strcat(text, " exista deja in site.");
			MessageBox(NULL, text, WindowCaption, MB_OK);
		}
		else count++;
	}

	char tex[100];
	char buff[4];
	itoa(count, buff, 10);
	strcpy(tex, "S-a(u) adaugat "); strcat(tex, buff); strcat(tex, "  film(e).");
	MessageBox(NULL, tex, WindowCaption, MB_OK);
	if (count == 0)
		return -1;
	return 1;
	fclose(file);
}
void delAll() {
	struct FILM* FILMUL = NULL;
	for (struct GEN* g = FIRST_GEN; g != NULL; ) {
		struct GEN* GEN = g;
		g = g->next_gen;

		free(GEN->nume); GEN->nume = NULL;
		for (struct FILM* f = GEN->first_film; f != NULL; ) {
			FILMUL = f;
			f = f->next_film;

			free(FILMUL->titlu); FILMUL->titlu = NULL;
			for (struct user* USR = FILMUL->rating->useri_votat; USR != NULL; ) {
				struct user* u = USR;
				USR = USR->next;
				free(u->nume); u->nume = NULL;
				free(u); u = NULL;
			}
			FILMUL->rating->useri_votat = NULL;
			for (struct comentariu* COM = FILMUL->rating->first_list_com; COM != NULL; ) {
				struct comentariu* c = COM;
				COM = COM->next_com;
				free(c->coment); c->coment = NULL;
				free(c); c = NULL;
			}
			FILMUL->rating->first_list_com = NULL;
			free(FILMUL->rating); FILMUL->rating = NULL;
			for (struct lista_actori* ACT = FILMUL->first_list_actori; ACT != NULL; ) {
				struct lista_actori* la = ACT;
				ACT = ACT->next_list;
				free(la); la = NULL;
			}
			FILMUL->first_list_actori = NULL;
			free(FILMUL); FILMUL = NULL;
		}
		free(GEN); GEN = NULL;
	}

	struct regizor* REG = NULL;
	for (struct regizor* reg = FIRST_REG; reg != NULL; ) {
		REG = reg;
		reg = reg->next_regizor;

		free(REG->nume); REG->nume = NULL;
		for (struct lista_filme* lf = REG->first_lista_filme; lf != NULL; ) {
			struct lista_filme* LF_ITEM = lf;
			lf = lf->next_list;
			free(LF_ITEM); LF_ITEM = NULL;
		}
		REG->first_lista_filme = NULL;
		free(REG); REG = NULL;
	}

	struct actor* ACT = NULL;
	for (struct actor* act = FIRST_ACTOR; act != NULL; ) {
		ACT = act;
		act = act->next_actor;

		free(ACT->nume); ACT->nume = NULL;
		for (struct lista_filme* lf = ACT->first_lista_filme; lf != NULL; ) {
			struct lista_filme* LF_ITEM = lf;
			lf = lf->next_list;
			free(LF_ITEM); LF_ITEM = NULL;
		}
		ACT->first_lista_filme = NULL;
		free(ACT); ACT = NULL;
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


//Variabile globale WIN32//////////////////////////////////////////
HWND FEREASTRA01, FEREASTRA_RezCautareFilme, FEREASTRA_AddGen, FEREASTRA_AddVot, FEREASTRA_AddComentariu, FEREASTRA_AddFilm, FEREASTRA_AddFilmFromFile, FEREASTRA_AddActor, FEREASTRA_AddRegizor;
HWND FEREASTRA_CautaGen, FEREASTRA_CautaAn, FEREASTRA_CautaActor, FEREASTRA_CautaRegizor, FEREASTRA_Noutati, FEREASTRA_RezCautareGen, FEREASTRA_RezCautareAn, FEREASTRA_RezCautareActor, FEREASTRA_RezCautareRegizor;
HWND label01, label02, label03, label04, label05, label06, label07, label08, label09, label10, label11, label12, label13, label14, label15, label16, label17, label18, label19, label20, label21;
HBITMAP hBitmap01 = NULL;

HINSTANCE gINST, gPrevINST;
PWSTR gCMD;
BOOL WINAPI GetMessage(
	_Out_    LPMSG lpMsg,
	_In_opt_ HWND  hWnd,
	_In_     UINT  wMsgFilterMin,
	_In_     UINT  wMsgFilterMax
);

//Declaratii functii callback////////////////////////////////////////
LRESULT CALLBACK WndProc01(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK WndProcRezCautareFilme(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK WndProcAddGen(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK WndProcAddVot(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK WndProcAddComent(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK WndProcAddFilm(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK WndProcAddFilmFromFile(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK WndProcAddActor(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK WndProcAddRegizor(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK WndProcCautaGen(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK WndProcCautaAn(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK WndProcCautaActor(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK WndProcCautaRegizor(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK WndProcRezCautaGen(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK WndProcRezCautaAn(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK WndProcRezCautaActor(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK WndProcRezCautaRegizor(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK WndProcNoutati(HWND, UINT, WPARAM, LPARAM);
void AddMenus(HWND);

//Butoane meniu/////////////////////////////////////////////////////
#define IDM_ADD_FILM 1
#define IDM_ADD_FILM_FROM_FILE 2
#define IDM_ADD_GEN 3
#define IDM_ADD_REGIZOR 4
#define IDM_ADD_ACTOR 5
#define IDM_SEARCH_TITLU_FILM 6
#define IDM_SEARCH_GEN 7
#define IDM_SEARCH_REGIZOR 8
#define IDM_SEARCH_AN 9
#define IDM_SEARCH_ACTOR 10
#define IDM_ADD_VOT 11
#define IDM_ADD_COMMENT 12
#define IDM_SAVE 13
#define IDM_EXIT 14
#define IDM_LOAD 15

//Butoane ferestre///////////////////////////////////////////////////
#define IDM_BUTON_CAUTA_FILME 21
#define IDM_CAMP_CAUTA_FILME 22
#define IDM_BUTON_VOT 23
#define IDM_BUTON_COMMENT 24
#define IDM_BUTON_NOUTATI 25
#define IDM_CAMP_AFISARE 26
#define IDM_BUTON_FIN_ADD_GEN 27
#define IDM_BUTON_FIN_ADD_VOT 28
#define IDM_CAMP_VOT_FILM 29
#define IDM_CAMP_NUME_USER_VOT 30
#define IDM_CAMP_VOT_USER 31
#define IDM_CAMP_NUME_USER_COMENT 32
#define IDM_CAMP_COMENTARIU_USER 33
#define IDM_CAMP_COMENTARIU_FILM 34
#define IDM_BUTON_FIN_ADD_COMENTARIU 35
#define IDM_CAMP_ADD_FILM_GEN 36
#define IDM_CAMP_ADD_FILM_TITLU 37
#define IDM_CAMP_ADD_FILM_AN 38
#define IDM_CAMP_ADD_FILM_MINUTE 39
#define IDM_CAMP_ADD_FILM_REGIZOR 40
#define IDM_CAMP_ADD_FILM_ACTORI 41
#define IDM_BUTON_FIN_ADD_FILM 42
#define IDM_CAMP_ADD_FILM_FROM_FILE 43
#define IDM_BUTON_FIN_ADD_FILM_FROM_FILE 44
#define IDM_CAMP_ADD_ACTOR 45
#define IDM_BUTON_FIN_ADD_ACTOR 46
#define IDM_CAMP_ADD_REGIZOR 47
#define IDM_BUTON_FIN_ADD_REGIZOR 48
#define IDM_CAMP_CAUTA_REGIZOR 49
#define IDM_BUTON_FIN_CAUTA_REGIZOR 50
#define IDM_CAMP_CAUTA_ACTOR 51
#define IDM_BUTON_FIN_CAUTA_ACTOR 52
#define IDM_CAMP_CAUTA_AN 53
#define IDM_BUTON_FIN_CAUTA_AN 54
#define IDM_CAMP_CAUTA_GEN 55
#define IDM_BUTON_FIN_CAUTA_GEN 56

#define IDC_MAIN_EDIT	101
#define IMAGE_VIEW 200

//FERESTRE//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WINAPI window_ADD_GEN(HINSTANCE hInstance_REZ, HINSTANCE hPrevInstance_REZ, PWSTR lpCmdLine_REZ, int nCmdShow_REZ) {
	MSG  msg_rez;
	WNDCLASSW wc_rez = { 0 };

	wc_rez.lpszClassName = TEXT("Adaugare gen");///////////////////////////////////////////
	wc_rez.hInstance = hInstance_REZ;
	wc_rez.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
	wc_rez.lpfnWndProc = WndProcAddGen;//fereastra de procedura//////////////////////////////
	wc_rez.hCursor = LoadCursor(0, IDC_ARROW);
	//wc_rez.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);////////////////////////
	RegisterClassW(&wc_rez);

	FEREASTRA_AddGen = CreateWindowW(wc_rez.lpszClassName, L"Adaugare gen",      /////////////////       /////////////////////
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		300, 300, 455, 100, 0, 0, hInstance_REZ, 0); //////////////////////////////////////

	while (GetMessage(&msg_rez, NULL, 0, 0)) {
		TranslateMessage(&msg_rez);
		DispatchMessage(&msg_rez);
	}
}
void WINAPI window_REZ_CAUTARE(HINSTANCE hInstance_REZ, HINSTANCE hPrevInstance_REZ, PWSTR lpCmdLine_REZ, int nCmdShow_REZ) {
	MSG  msg_rez;
	WNDCLASSW wc_rez = { 0 };

	wc_rez.lpszClassName = TEXT("Cautare filme");//////////////////////////////////////////////////////
	wc_rez.hInstance = hInstance_REZ;
	wc_rez.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
	wc_rez.lpfnWndProc = WndProcRezCautareFilme;//fereastra de procedura///////////////////////////////
	wc_rez.hCursor = LoadCursor(0, IDC_ARROW);
	wc_rez.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);/////////////////////////////////////
	RegisterClassW(&wc_rez);

	FEREASTRA_RezCautareFilme = CreateWindowW(wc_rez.lpszClassName, L"Cautare filme",////////////////////
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		300, 100, 500, 700, 0, 0, hInstance_REZ, 0);////////////////////////////////////////////////////

	while (GetMessage(&msg_rez, NULL, 0, 0)) {
		TranslateMessage(&msg_rez);
		DispatchMessage(&msg_rez);
	}
}
void WINAPI window_ADD_VOT(HINSTANCE hInstance_REZ, HINSTANCE hPrevInstance_REZ, PWSTR lpCmdLine_REZ, int nCmdShow_REZ) {
	MSG  msg_rez;
	WNDCLASSW wc_rez = { 0 };

	wc_rez.lpszClassName = TEXT("Adaugare vot");///////////////////////////////////////////
	wc_rez.hInstance = hInstance_REZ;
	wc_rez.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
	wc_rez.lpfnWndProc = WndProcAddVot;//fereastra de procedura//////////////////////////////
	wc_rez.hCursor = LoadCursor(0, IDC_ARROW);
	//wc_rez.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);////////////////////////
	RegisterClassW(&wc_rez);

	FEREASTRA_AddVot = CreateWindowW(wc_rez.lpszClassName, L"Adaugare vot",/////////////////
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		300, 300, 455, 140, 0, 0, hInstance_REZ, 0); //////////////////////////////////////

	while (GetMessage(&msg_rez, NULL, 0, 0)) {
		TranslateMessage(&msg_rez);
		DispatchMessage(&msg_rez);
	}
}
void WINAPI window_ADD_COMENTARIU(HINSTANCE hInstance_REZ, HINSTANCE hPrevInstance_REZ, PWSTR lpCmdLine_REZ, int nCmdShow_REZ) {
	MSG  msg_rez;
	WNDCLASSW wc_rez = { 0 };

	wc_rez.lpszClassName = TEXT("Adaugare comentariu");///////////////////////////////////////////
	wc_rez.hInstance = hInstance_REZ;
	wc_rez.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
	wc_rez.lpfnWndProc = WndProcAddComent;//fereastra de procedura//////////////////////////////
	wc_rez.hCursor = LoadCursor(0, IDC_ARROW);
	//wc_rez.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);////////////////////////
	RegisterClassW(&wc_rez);

	FEREASTRA_AddComentariu = CreateWindowW(wc_rez.lpszClassName, L"Adaugare comentariu",/////////////////
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		300, 300, 560, 150, 0, 0, hInstance_REZ, 0); //////////////////////////////////////

	while (GetMessage(&msg_rez, NULL, 0, 0)) {
		TranslateMessage(&msg_rez);
		DispatchMessage(&msg_rez);
	}
}
void WINAPI window_ADD_FILM(HINSTANCE hInstance_REZ, HINSTANCE hPrevInstance_REZ, PWSTR lpCmdLine_REZ, int nCmdShow_REZ) {
	MSG  msg_rez;
	WNDCLASSW wc_rez = { 0 };

	wc_rez.lpszClassName = TEXT("Adaugare film");///////////////////////////////////////////
	wc_rez.hInstance = hInstance_REZ;
	wc_rez.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
	wc_rez.lpfnWndProc = WndProcAddFilm;//fereastra de procedura//////////////////////////////
	wc_rez.hCursor = LoadCursor(0, IDC_ARROW);
	//wc_rez.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);////////////////////////
	RegisterClassW(&wc_rez);

	FEREASTRA_AddFilm = CreateWindowW(wc_rez.lpszClassName, L"Adaugare film",/////////////////     ////////////////
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		300, 300, 820, 250, 0, 0, hInstance_REZ, 0); //////////////////////////////////////

	while (GetMessage(&msg_rez, NULL, 0, 0)) {
		TranslateMessage(&msg_rez);
		DispatchMessage(&msg_rez);
	}
}
void WINAPI window_ADD_FILM_FROM_FILE(HINSTANCE hInstance_REZ, HINSTANCE hPrevInstance_REZ, PWSTR lpCmdLine_REZ, int nCmdShow_REZ) {
	MSG  msg_rez;
	WNDCLASSW wc_rez = { 0 };

	wc_rez.lpszClassName = TEXT("Adaugare film din fisier existent");///////////////////////////////////////////
	wc_rez.hInstance = hInstance_REZ;
	wc_rez.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
	wc_rez.lpfnWndProc = WndProcAddFilmFromFile;//fereastra de procedura//////////////////////////////
	wc_rez.hCursor = LoadCursor(0, IDC_ARROW);
	//wc_rez.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);////////////////////////
	RegisterClassW(&wc_rez);

	FEREASTRA_AddFilmFromFile = CreateWindowW(wc_rez.lpszClassName, L"Adaugare film din fisier existent",/////////////////     ///////////////
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		300, 300, 520, 100, 0, 0, hInstance_REZ, 0); //////////////////////////////////////

	while (GetMessage(&msg_rez, NULL, 0, 0)) {
		TranslateMessage(&msg_rez);
		DispatchMessage(&msg_rez);
	}
}
void WINAPI window_ADD_ACTOR(HINSTANCE hInstance_REZ, HINSTANCE hPrevInstance_REZ, PWSTR lpCmdLine_REZ, int nCmdShow_REZ) {
	MSG  msg_rez;
	WNDCLASSW wc_rez = { 0 };

	wc_rez.lpszClassName = TEXT("Adaugare actor");///////////////////////////////////////////
	wc_rez.hInstance = hInstance_REZ;
	wc_rez.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
	wc_rez.lpfnWndProc = WndProcAddActor;//fereastra de procedura//////////////////////////////
	wc_rez.hCursor = LoadCursor(0, IDC_ARROW);
	//wc_rez.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);////////////////////////
	RegisterClassW(&wc_rez);

	FEREASTRA_AddActor = CreateWindowW(wc_rez.lpszClassName, L"Adaugare actor",/////////////////       //////////////////
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		300, 300, 455, 100, 0, 0, hInstance_REZ, 0); //////////////////////////////////////

	while (GetMessage(&msg_rez, NULL, 0, 0)) {
		TranslateMessage(&msg_rez);
		DispatchMessage(&msg_rez);
	}
}
void WINAPI window_ADD_REGIZOR(HINSTANCE hInstance_REZ, HINSTANCE hPrevInstance_REZ, PWSTR lpCmdLine_REZ, int nCmdShow_REZ) {
	MSG  msg_rez;
	WNDCLASSW wc_rez = { 0 };

	wc_rez.lpszClassName = TEXT("Adaugare regizor");///////////////////////////////////////////
	wc_rez.hInstance = hInstance_REZ;
	wc_rez.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
	wc_rez.lpfnWndProc = WndProcAddRegizor;//fereastra de procedura//////////////////////////////
	wc_rez.hCursor = LoadCursor(0, IDC_ARROW);
	//wc_rez.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);////////////////////////
	RegisterClassW(&wc_rez);

	FEREASTRA_AddRegizor = CreateWindowW(wc_rez.lpszClassName, L"Adaugare regizor",/////////////////    ///////////////
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		300, 300, 470, 100, 0, 0, hInstance_REZ, 0); //////////////////////////////////////

	while (GetMessage(&msg_rez, NULL, 0, 0)) {
		TranslateMessage(&msg_rez);
		DispatchMessage(&msg_rez);
	}
}
void WINAPI window_CAUTA_GEN(HINSTANCE hInstance_REZ, HINSTANCE hPrevInstance_REZ, PWSTR lpCmdLine_REZ, int nCmdShow_REZ) {
	MSG  msg_rez;
	WNDCLASSW wc_rez = { 0 };

	wc_rez.lpszClassName = TEXT("Cauta dupa gen");///////////////////////////////////////////
	wc_rez.hInstance = hInstance_REZ;
	wc_rez.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
	wc_rez.lpfnWndProc = WndProcCautaGen;//fereastra de procedura//////////////////////////////
	wc_rez.hCursor = LoadCursor(0, IDC_ARROW);
	//wc_rez.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);////////////////////////
	RegisterClassW(&wc_rez);

	FEREASTRA_CautaGen = CreateWindowW(wc_rez.lpszClassName, L"Cauta dupa gen",      /////////////////       /////////////////////
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		300, 300, 455, 250, 0, 0, hInstance_REZ, 0); //////////////////////////////////////

	while (GetMessage(&msg_rez, NULL, 0, 0)) {
		TranslateMessage(&msg_rez);
		DispatchMessage(&msg_rez);
	}
}
void WINAPI window_CAUTA_AN(HINSTANCE hInstance_REZ, HINSTANCE hPrevInstance_REZ, PWSTR lpCmdLine_REZ, int nCmdShow_REZ) {
	MSG  msg_rez;
	WNDCLASSW wc_rez = { 0 };

	wc_rez.lpszClassName = TEXT("Cauta an cu filme");///////////////////////////////////////////
	wc_rez.hInstance = hInstance_REZ;
	wc_rez.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
	wc_rez.lpfnWndProc = WndProcCautaAn;//fereastra de procedura//////////////////////////////
	wc_rez.hCursor = LoadCursor(0, IDC_ARROW);
	//wc_rez.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);////////////////////////
	RegisterClassW(&wc_rez);

	FEREASTRA_CautaAn = CreateWindowW(wc_rez.lpszClassName, L"Cauta an cu filme",      /////////////////       /////////////////////
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		300, 300, 455, 100, 0, 0, hInstance_REZ, 0); //////////////////////////////////////

	while (GetMessage(&msg_rez, NULL, 0, 0)) {
		TranslateMessage(&msg_rez);
		DispatchMessage(&msg_rez);
	}
}
void WINAPI window_CAUTA_ACTOR(HINSTANCE hInstance_REZ, HINSTANCE hPrevInstance_REZ, PWSTR lpCmdLine_REZ, int nCmdShow_REZ) {
	MSG  msg_rez;
	WNDCLASSW wc_rez = { 0 };

	wc_rez.lpszClassName = TEXT("Cauta actor");///////////////////////////////////////////
	wc_rez.hInstance = hInstance_REZ;
	wc_rez.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
	wc_rez.lpfnWndProc = WndProcCautaActor;//fereastra de procedura//////////////////////////////
	wc_rez.hCursor = LoadCursor(0, IDC_ARROW);
	//wc_rez.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);////////////////////////
	RegisterClassW(&wc_rez);

	FEREASTRA_CautaActor = CreateWindowW(wc_rez.lpszClassName, L"Cauta actor",      /////////////////       /////////////////////
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		300, 300, 600, 400, 0, 0, hInstance_REZ, 0); //////////////////////////////////////

	while (GetMessage(&msg_rez, NULL, 0, 0)) {
		TranslateMessage(&msg_rez);
		DispatchMessage(&msg_rez);
	}
}
void WINAPI window_CAUTA_REGIZOR(HINSTANCE hInstance_REZ, HINSTANCE hPrevInstance_REZ, PWSTR lpCmdLine_REZ, int nCmdShow_REZ) {
	MSG  msg_rez;
	WNDCLASSW wc_rez = { 0 };

	wc_rez.lpszClassName = TEXT("Cauta regizor");///////////////////////////////////////////
	wc_rez.hInstance = hInstance_REZ;
	wc_rez.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
	wc_rez.lpfnWndProc = WndProcCautaRegizor;//fereastra de procedura//////////////////////////////
	wc_rez.hCursor = LoadCursor(0, IDC_ARROW);
	//wc_rez.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);////////////////////////
	RegisterClassW(&wc_rez);

	FEREASTRA_CautaRegizor = CreateWindowW(wc_rez.lpszClassName, L"Cauta regizor",      /////////////////       /////////////////////
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		300, 300, 480, 250, 0, 0, hInstance_REZ, 0); //////////////////////////////////////

	while (GetMessage(&msg_rez, NULL, 0, 0)) {
		TranslateMessage(&msg_rez);
		DispatchMessage(&msg_rez);
	}
}
void WINAPI window_REZ_CAUTARE_GEN(HINSTANCE hInstance_REZ, HINSTANCE hPrevInstance_REZ, PWSTR lpCmdLine_REZ, int nCmdShow_REZ) {
	MSG  msg_rez;
	WNDCLASSW wc_rez = { 0 };

	wc_rez.lpszClassName = TEXT("Rezultate cautare gen");//////////////////////////////////////////////////////
	wc_rez.hInstance = hInstance_REZ;
	wc_rez.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
	wc_rez.lpfnWndProc = WndProcRezCautaGen;//fereastra de procedura///////////////////////////////
	wc_rez.hCursor = LoadCursor(0, IDC_ARROW);
	wc_rez.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);/////////////////////////////////////
	RegisterClassW(&wc_rez);

	FEREASTRA_RezCautareGen = CreateWindowW(wc_rez.lpszClassName, L"Rezultate cautare gen",////////////////////
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		300, 100, 500, 700, 0, 0, hInstance_REZ, 0);////////////////////////////////////////////////////

	while (GetMessage(&msg_rez, NULL, 0, 0)) {
		TranslateMessage(&msg_rez);
		DispatchMessage(&msg_rez);
	}
}
void WINAPI window_REZ_CAUTARE_AN(HINSTANCE hInstance_REZ, HINSTANCE hPrevInstance_REZ, PWSTR lpCmdLine_REZ, int nCmdShow_REZ) {
	MSG  msg_rez;
	WNDCLASSW wc_rez = { 0 };

	wc_rez.lpszClassName = TEXT("Rezultate cautare an");//////////////////////////////////////////////////////
	wc_rez.hInstance = hInstance_REZ;
	wc_rez.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
	wc_rez.lpfnWndProc = WndProcRezCautaAn;//fereastra de procedura///////////////////////////////
	wc_rez.hCursor = LoadCursor(0, IDC_ARROW);
	wc_rez.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);/////////////////////////////////////
	RegisterClassW(&wc_rez);

	FEREASTRA_RezCautareAn = CreateWindowW(wc_rez.lpszClassName, L"Rezultate cautare an",////////////////////
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		300, 100, 500, 700, 0, 0, hInstance_REZ, 0);////////////////////////////////////////////////////

	while (GetMessage(&msg_rez, NULL, 0, 0)) {
		TranslateMessage(&msg_rez);
		DispatchMessage(&msg_rez);
	}
}
void WINAPI window_REZ_CAUTARE_ACTOR(HINSTANCE hInstance_REZ, HINSTANCE hPrevInstance_REZ, PWSTR lpCmdLine_REZ, int nCmdShow_REZ) {
	MSG  msg_rez;
	WNDCLASSW wc_rez = { 0 };

	wc_rez.lpszClassName = TEXT("Rezultate cautare actor");//////////////////////////////////////////////////////
	wc_rez.hInstance = hInstance_REZ;
	wc_rez.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
	wc_rez.lpfnWndProc = WndProcRezCautaActor;//fereastra de procedura///////////////////////////////
	wc_rez.hCursor = LoadCursor(0, IDC_ARROW);
	wc_rez.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);/////////////////////////////////////
	RegisterClassW(&wc_rez);

	FEREASTRA_RezCautareActor = CreateWindowW(wc_rez.lpszClassName, L"Rezultate cautare actor",////////////////////
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		300, 100, 500, 700, 0, 0, hInstance_REZ, 0);////////////////////////////////////////////////////

	while (GetMessage(&msg_rez, NULL, 0, 0)) {
		TranslateMessage(&msg_rez);
		DispatchMessage(&msg_rez);
	}
}
void WINAPI window_REZ_CAUTARE_REGIZOR(HINSTANCE hInstance_REZ, HINSTANCE hPrevInstance_REZ, PWSTR lpCmdLine_REZ, int nCmdShow_REZ) {
	MSG  msg_rez;
	WNDCLASSW wc_rez = { 0 };

	wc_rez.lpszClassName = TEXT("Rezultate cautare regizor");//////////////////////////////////////////////////////
	wc_rez.hInstance = hInstance_REZ;
	wc_rez.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
	wc_rez.lpfnWndProc = WndProcRezCautaRegizor;//fereastra de procedura///////////////////////////////
	wc_rez.hCursor = LoadCursor(0, IDC_ARROW);
	wc_rez.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);/////////////////////////////////////
	RegisterClassW(&wc_rez);

	FEREASTRA_RezCautareRegizor = CreateWindowW(wc_rez.lpszClassName, L"Rezultate cautare regizor",////////////////////
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		300, 100, 500, 700, 0, 0, hInstance_REZ, 0);////////////////////////////////////////////////////

	while (GetMessage(&msg_rez, NULL, 0, 0)) {
		TranslateMessage(&msg_rez);
		DispatchMessage(&msg_rez);
	}
}
void WINAPI window_REZ_NOUTATI(HINSTANCE hInstance_REZ, HINSTANCE hPrevInstance_REZ, PWSTR lpCmdLine_REZ, int nCmdShow_REZ) {
	MSG  msg_rez;
	WNDCLASSW wc_rez = { 0 };

	wc_rez.lpszClassName = TEXT("Noutati filme");//////////////////////////////////////////////////////
	wc_rez.hInstance = hInstance_REZ;
	wc_rez.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
	wc_rez.lpfnWndProc = WndProcNoutati;//fereastra de procedura///////////////////////////////
	wc_rez.hCursor = LoadCursor(0, IDC_ARROW);
	wc_rez.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);/////////////////////////////////////
	RegisterClassW(&wc_rez);

	FEREASTRA_Noutati = CreateWindowW(wc_rez.lpszClassName, L"Noutati filme",////////////////////
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		300, 100, 400, 800, 0, 0, hInstance_REZ, 0);////////////////////////////////////////////////////

	while (GetMessage(&msg_rez, NULL, 0, 0)) {
		TranslateMessage(&msg_rez);
		DispatchMessage(&msg_rez);
	}
}

//CALLBACK&functii/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AddMenus(HWND FEREASTRA)
{
	HMENU hMenubar;
	HMENU h0Menu, h1Menu, h2Menu, h3Menu;
	hMenubar = CreateMenu();

	HBRUSH brush = CreateSolidBrush(RGB(134, 153, 255));
	SetClassLongPtr(FEREASTRA, GCLP_HBRBACKGROUND, (LONG)brush);

	h0Menu = CreateMenu();
	AppendMenuW(h0Menu, MF_STRING, IDM_SAVE, L"&Salveaza filme");
	AppendMenuW(h0Menu, MF_STRING, IDM_LOAD, L"&Importa filme");
	AppendMenuW(h0Menu, MF_SEPARATOR, 0, NULL);
	AppendMenuW(h0Menu, MF_STRING, IDM_EXIT, L"Iesire");

	h1Menu = CreateMenu();
	AppendMenuW(h1Menu, MF_STRING, IDM_ADD_FILM, L"&FILM NOU");
	AppendMenuW(h1Menu, MF_STRING, IDM_ADD_FILM_FROM_FILE, L"&FILM NOU din fisier text");
	AppendMenuW(h1Menu, MF_SEPARATOR, 0, NULL);
	AppendMenuW(h1Menu, MF_STRING, IDM_ADD_GEN, L"&GEN NOU");
	AppendMenuW(h1Menu, MF_SEPARATOR, 0, NULL);
	AppendMenuW(h1Menu, MF_STRING, IDM_ADD_REGIZOR, L"&REGIZOR NOU");
	AppendMenuW(h1Menu, MF_SEPARATOR, 0, NULL);
	AppendMenuW(h1Menu, MF_STRING, IDM_ADD_ACTOR, L"&ACTOR NOU");

	h2Menu = CreateMenu();
	AppendMenuW(h2Menu, MF_STRING, IDM_SEARCH_GEN, L"&Gen");
	AppendMenuW(h2Menu, MF_STRING, IDM_SEARCH_REGIZOR, L"&Regizor");
	AppendMenuW(h2Menu, MF_STRING, IDM_SEARCH_AN, L"&An");
	AppendMenuW(h2Menu, MF_STRING, IDM_SEARCH_ACTOR, L"&Actor");

	h3Menu = CreateMenu();
	AppendMenuW(h3Menu, MF_STRING, IDM_ADD_VOT, L"&Vot");
	AppendMenuW(h3Menu, MF_STRING, IDM_ADD_COMMENT, L"&Comentariu");

	AppendMenuW(hMenubar, MF_POPUP, (UINT_PTR)h0Menu, L"&File");
	AppendMenuW(hMenubar, MF_POPUP, (UINT_PTR)h1Menu, L"&Adauga");
	AppendMenuW(hMenubar, MF_POPUP, (UINT_PTR)h2Menu, L"&Cauta dupa");
	AppendMenuW(hMenubar, MF_POPUP, (UINT_PTR)h3Menu, L"&Review");

	SetMenu(FEREASTRA, hMenubar);
}

LRESULT CALLBACK WndProc01(HWND FEREASTRA, UINT msg, WPARAM ParamClickStanga, LPARAM ParamClickDreapta)
{
	PAINTSTRUCT   ps;
	HDC           hDC;
	int wmId, wmEvent;
	RECT rc;

	switch (msg)
	{
	case WM_COMMAND: {
		switch (LOWORD(ParamClickStanga))
		{
		case IDM_BUTON_CAUTA_FILME: {
			HINSTANCE* hInstance_r = (HINSTANCE*)malloc(sizeof(HINSTANCE));
			PWSTR lpCmdLine_r = gCMD;
			int nCmdShow_r = 10;

			if (TEXT_PRINCIPAL != NULL)
				free(TEXT_PRINCIPAL);
			TEXT_PRINCIPAL = (char*)malloc(100 * sizeof(char));
			GetWindowText(label01, TEXT_PRINCIPAL, 100);
			if (!(TEXT_PRINCIPAL[0] >= 'a' && TEXT_PRINCIPAL[0] <= 'z') && !(TEXT_PRINCIPAL[0] >= 'A' && TEXT_PRINCIPAL[0] <= 'Z') && !(TEXT_PRINCIPAL[0] >= '0' && TEXT_PRINCIPAL[0] <= '9'))
				break;

			window_REZ_CAUTARE(*hInstance_r, NULL, lpCmdLine_r, nCmdShow_r);//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< CAUTA FILM DUPA TITLU

			free(hInstance_r);
			break;
		}
		case IDM_BUTON_NOUTATI: {
			HINSTANCE* hInstance_r = (HINSTANCE*)malloc(sizeof(HINSTANCE));
			PWSTR lpCmdLine_r = gCMD;
			int nCmdShow_r = 10;

			window_REZ_NOUTATI(*hInstance_r, NULL, lpCmdLine_r, nCmdShow_r);//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< NOUTATI

			free(hInstance_r);
			break;
		}
		case IDM_ADD_FILM: {
			HINSTANCE* hInstance_r = (HINSTANCE*)malloc(sizeof(HINSTANCE));
			PWSTR lpCmdLine_r = gCMD;
			int nCmdShow_r = 10;

			window_ADD_FILM(*hInstance_r, NULL, lpCmdLine_r, nCmdShow_r);//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< ADD FILM

			free(hInstance_r);
			break;
		}
		case IDM_ADD_FILM_FROM_FILE: {
			HINSTANCE* hInstance_r = (HINSTANCE*)malloc(sizeof(HINSTANCE));
			PWSTR lpCmdLine_r = gCMD;
			int nCmdShow_r = 10;

			window_ADD_FILM_FROM_FILE(*hInstance_r, NULL, lpCmdLine_r, nCmdShow_r);//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< ADD FILM FROM FILE

			free(hInstance_r);
			break;
		}
		case IDM_ADD_GEN: {
			HINSTANCE* hInstance_r = (HINSTANCE*)malloc(sizeof(HINSTANCE));
			PWSTR lpCmdLine_r = gCMD;
			int nCmdShow_r = 10;

			window_ADD_GEN(*hInstance_r, NULL, lpCmdLine_r, nCmdShow_r);//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< ADD GEN

			free(hInstance_r);
			break;
		}
		case IDM_ADD_REGIZOR: {
			HINSTANCE* hInstance_r = (HINSTANCE*)malloc(sizeof(HINSTANCE));
			PWSTR lpCmdLine_r = gCMD;
			int nCmdShow_r = 10;

			window_ADD_REGIZOR(*hInstance_r, NULL, lpCmdLine_r, nCmdShow_r);//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< ADD REGIZOR

			free(hInstance_r);
			break;
		}
		case IDM_ADD_ACTOR: {
			HINSTANCE* hInstance_r = (HINSTANCE*)malloc(sizeof(HINSTANCE));
			PWSTR lpCmdLine_r = gCMD;
			int nCmdShow_r = 10;

			window_ADD_ACTOR(*hInstance_r, NULL, lpCmdLine_r, nCmdShow_r);//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< ADD ACTOR

			free(hInstance_r);
			break;
		}

		case IDM_ADD_VOT: {
			HINSTANCE* hInstance_r = (HINSTANCE*)malloc(sizeof(HINSTANCE));
			PWSTR lpCmdLine_r = gCMD;
			int nCmdShow_r = 10;

			window_ADD_VOT(*hInstance_r, NULL, lpCmdLine_r, nCmdShow_r);//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< ADD VOT

			free(hInstance_r);
			break;
		}
		case IDM_ADD_COMMENT: {
			HINSTANCE* hInstance_r = (HINSTANCE*)malloc(sizeof(HINSTANCE));
			PWSTR lpCmdLine_r = gCMD;
			int nCmdShow_r = 10;

			window_ADD_COMENTARIU(*hInstance_r, NULL, lpCmdLine_r, nCmdShow_r);//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< ADD COMENTARIU

			free(hInstance_r);
			break;
		}

		case IDM_SEARCH_GEN: {
			HINSTANCE* hInstance_r = (HINSTANCE*)malloc(sizeof(HINSTANCE));
			PWSTR lpCmdLine_r = gCMD;
			int nCmdShow_r = 10;

			window_CAUTA_GEN(*hInstance_r, NULL, lpCmdLine_r, nCmdShow_r);//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< CAUTA GEN

			free(hInstance_r);
			break;
		}
		case IDM_SEARCH_REGIZOR: {
			HINSTANCE* hInstance_r = (HINSTANCE*)malloc(sizeof(HINSTANCE));
			PWSTR lpCmdLine_r = gCMD;
			int nCmdShow_r = 10;

			window_CAUTA_REGIZOR(*hInstance_r, NULL, lpCmdLine_r, nCmdShow_r);//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< CAUTA REGIZOR

			free(hInstance_r);
			break;
		}
		case IDM_SEARCH_AN: {
			HINSTANCE* hInstance_r = (HINSTANCE*)malloc(sizeof(HINSTANCE));
			PWSTR lpCmdLine_r = gCMD;
			int nCmdShow_r = 10;

			window_CAUTA_AN(*hInstance_r, NULL, lpCmdLine_r, nCmdShow_r);//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< CAUTA AN

			free(hInstance_r);
			break;
		}
		case IDM_SEARCH_ACTOR: {
			HINSTANCE* hInstance_r = (HINSTANCE*)malloc(sizeof(HINSTANCE));
			PWSTR lpCmdLine_r = gCMD;
			int nCmdShow_r = 10;
			window_CAUTA_ACTOR(*hInstance_r, NULL, lpCmdLine_r, nCmdShow_r);//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< CAUTA ACTOR
			free(hInstance_r);
			break;
		}

		case IDM_SAVE: {
			int i = save(); //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< SAVE
			if (i == 1)
				MessageBox(NULL, "S-au salvat toate filmele site-ului in fisiere separate.", WindowCaption, MB_OK);
			else MessageBox(NULL, "Nu exista momentan filme de salvat.", WindowCaption, MB_OK);
			break;
		}
		case IDM_LOAD: {
			int i = load(); //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< SAVE
			if (i == 1)
				MessageBox(NULL, "S-au importat filme din fisierele mentionate in load.txt", WindowCaption, MB_OK);
			if (i == 0)
				MessageBox(NULL, "Nu exista filme de importat.", WindowCaption, MB_OK);
			break;
		}
		case IDM_EXIT: {
			SendMessage(FEREASTRA, WM_CLOSE, 0, 0);
			break;
		}
		}
		break;
	}

	case WM_CREATE: {
		AddMenus(FEREASTRA);
		HFONT hfDefault;
		HWND hEdit;

		label01 = CreateWindow(TEXT("Edit"), TEXT("Titlu film..."),
			WS_VISIBLE | WS_CHILD,
			20, 170, 300, 25,
			FEREASTRA, (HMENU)IDM_CAMP_CAUTA_FILME, NULL, NULL);
		CreateWindow(TEXT("button"), TEXT("Cauta"),
			WS_VISIBLE | WS_CHILD,
			325, 170, 95, 25,  //margine stanga, margine sus, lungime, latime
			FEREASTRA, (HMENU)IDM_BUTON_CAUTA_FILME, NULL, NULL);
		CreateWindow(TEXT("button"), TEXT("Noutati"),
			WS_VISIBLE | WS_CHILD,
			100, 210, 240, 25,  //margine stanga, margine sus, lungime, latime
			FEREASTRA, (HMENU)IDM_BUTON_NOUTATI, NULL, NULL);
		break;
	}
	case WM_SIZE: {
		HWND hEdit;
		RECT rcClient;
		GetClientRect(FEREASTRA, &rcClient);
		hEdit = GetDlgItem(FEREASTRA, IDC_MAIN_EDIT);
		SetWindowPos(hEdit, NULL, 0, 0, rcClient.right, rcClient.bottom, SWP_NOZORDER);
		break;
	}
	case WM_PAINT: {
		hDC = BeginPaint(FEREASTRA, &ps);
		GetClientRect(FEREASTRA, &rc);

		char l0[51] = "________________________________________________";
		char l1[51] = "_##########_##_##_______####_____####_##########";
		char l2[51] = "_##########_##_##_______######_######_##########";
		char l3[51] = "_##_________##_##_______##__#####__##_##________";
		char l4[51] = "_######_____##_##_______##____#____##_##########";
		char l5[51] = "_######_____##_##_______##_________##_##########";
		char l6[51] = "_##_________##_##_______##_________##_##________";
		char l7[51] = "_##_________##_########_##_________##_##########";
		char l8[73] = "  ##                 .##  ########  ##                 .##  ##########";
		char l9[98] = "                                                                                              ";

		SetRect(&rc, 20, 18, 450, 350); DrawText(hDC, l0, 50, &rc, DT_LEFT | DT_EXTERNALLEADING | DT_WORDBREAK);
		SetRect(&rc, 20, 30, 450, 350); DrawText(hDC, l1, 50, &rc, DT_LEFT | DT_EXTERNALLEADING | DT_WORDBREAK);
		SetRect(&rc, 20, 42, 450, 350); DrawText(hDC, l2, 50, &rc, DT_LEFT | DT_EXTERNALLEADING | DT_WORDBREAK);
		SetRect(&rc, 20, 54, 450, 350); DrawText(hDC, l3, 50, &rc, DT_LEFT | DT_EXTERNALLEADING | DT_WORDBREAK);
		SetRect(&rc, 20, 66, 450, 350); DrawText(hDC, l4, 50, &rc, DT_LEFT | DT_EXTERNALLEADING | DT_WORDBREAK);
		SetRect(&rc, 20, 78, 450, 350); DrawText(hDC, l5, 50, &rc, DT_LEFT | DT_EXTERNALLEADING | DT_WORDBREAK);
		SetRect(&rc, 20, 90, 450, 350); DrawText(hDC, l6, 50, &rc, DT_LEFT | DT_EXTERNALLEADING | DT_WORDBREAK);
		SetRect(&rc, 20, 102, 450, 350); DrawText(hDC, l7, 50, &rc, DT_LEFT | DT_EXTERNALLEADING | DT_WORDBREAK);
		SetRect(&rc, 20, 114, 450, 350); DrawText(hDC, l8, 72, &rc, DT_LEFT | DT_EXTERNALLEADING | DT_WORDBREAK);
		SetRect(&rc, 20, 126, 450, 350); DrawText(hDC, l9, 97, &rc, DT_LEFT | DT_EXTERNALLEADING | DT_WORDBREAK);

		ReleaseDC(0, hDC);
		/*LoadAndBlitBitmap( "d:/wood.bmp", hDC);*/
		EndPaint(FEREASTRA, &ps);
		break;
	}

				   //case WM_DRAWITEM: {
				   /*	LPDRAWITEMSTRUCT pDIS = (LPDRAWITEMSTRUCT)ParamClickStanga;
				   if (pDIS->CtlID == IDC_PNG)
				   {
				   HICON hIcon = (HICON)LoadImage(GetModuleHandle(0), MAKEINTRESOURCE(ICO_LOGO), IMAGE_ICON, 0, 0, LR_LOADTRANSPARENT);
				   DrawIconEx(pDIS->hDC, 0, 0, hIcon, 0, 0, 0, NULL, DI_NORMAL);
				   DestroyIcon(hIcon);
				   return TRUE;
				   }
				   break;
				   }*/

	case WM_DESTROY: {
		if (TEXT_PRINCIPAL != NULL) {
			free(TEXT_PRINCIPAL); TEXT_PRINCIPAL = NULL;
		}
		if (TEXT_GEN != NULL) {
			free(TEXT_GEN); TEXT_GEN = NULL;
		}
		if (TEXT_ACTORI != NULL) {
			free(TEXT_ACTORI); TEXT_ACTORI = NULL;
		}
		if (TEXT_REGIZOR != NULL) {
			free(TEXT_REGIZOR); TEXT_REGIZOR = NULL;
		}
		if (TEXT_USER != NULL) {
			free(TEXT_USER); TEXT_USER = NULL;
		}
		if (TEXT_COMENT != NULL) {
			free(TEXT_COMENT); TEXT_COMENT = NULL;
		}

		delAll();
		//3   DeleteObject(hBitmap);
		PostQuitMessage(0);
		break;
	}
	}
	return DefWindowProcW(FEREASTRA, msg, ParamClickStanga, ParamClickDreapta);
}

LRESULT CALLBACK WndProcRezCautareFilme(HWND FEREASTRA, UINT msg, WPARAM ParamClickStanga, LPARAM ParamClickDreapta)
{
	HDC hDC;
	PAINTSTRUCT Ps;
	RECT rc;
	switch (msg)
	{
	case WM_COMMAND:
		switch (LOWORD(ParamClickStanga))
		{
		case IDM_BUTON_VOT: {
			HINSTANCE* hInstance_r = (HINSTANCE*)malloc(sizeof(HINSTANCE));
			int nCmdShow_r = 10;
			window_ADD_VOT(*hInstance_r, NULL, NULL, nCmdShow_r);
			free(hInstance_r);
			break;
		}
		case IDM_BUTON_COMMENT: {
			HINSTANCE* hInstance_r = (HINSTANCE*)malloc(sizeof(HINSTANCE));
			int nCmdShow_r = 10;
			window_ADD_COMENTARIU(*hInstance_r, NULL, NULL, nCmdShow_r);
			free(hInstance_r);
			break;
		}
		}
		break;

	case WM_CREATE: {

		if (existaFilm(TEXT_PRINCIPAL) == 1) {
			CreateWindow(TEXT("button"), TEXT("Acorda Stele"),
				WS_VISIBLE | WS_CHILD,
				20, 500, 150, 25,
				FEREASTRA, (HMENU)IDM_BUTON_VOT, NULL, NULL);

			CreateWindow(TEXT("button"), TEXT("Comentariu"),
				WS_VISIBLE | WS_CHILD,
				200, 500, 150, 25,
				FEREASTRA, (HMENU)IDM_BUTON_COMMENT, NULL, NULL);
		}
		break;
	}
	case WM_SIZE: {
		HWND hEdit;
		GetClientRect(FEREASTRA, &rc);
		hEdit = GetDlgItem(FEREASTRA, IDC_MAIN_EDIT);
		SetWindowPos(hEdit, NULL, 0, 0, rc.right, rc.bottom, SWP_NOZORDER);
		break;
	}
	case WM_PAINT: {
		hDC = BeginPaint(FEREASTRA, &Ps);
		GetClientRect(FEREASTRA, &rc);
		printDetaliiFilm(hDC, rc, FEREASTRA, TEXT_PRINCIPAL);///////////////////////////////
		ReleaseDC(0, hDC);
		break;
	}
	case WM_DESTROY: {
		EndPaint(FEREASTRA, &Ps);
		PostQuitMessage(0);
		break;
	}
	}

	return DefWindowProcW(FEREASTRA, msg, ParamClickStanga, ParamClickDreapta);
}

LRESULT CALLBACK WndProcAddGen(HWND FEREASTRA, UINT msg, WPARAM ParamClickStanga, LPARAM ParamClickDreapta)
{
	switch (msg)
	{
	case WM_COMMAND:
		switch (LOWORD(ParamClickStanga))
		{
		case IDM_BUTON_FIN_ADD_GEN: {
			if (TEXT_PRINCIPAL != NULL)
				free(TEXT_PRINCIPAL);
			TEXT_PRINCIPAL = (char*)malloc(100 * sizeof(char));
			GetWindowText(label02, TEXT_PRINCIPAL, 100);//FEREASTRA_RezCautareFilme se va folosi de textul cautat
			if (!(TEXT_PRINCIPAL[0] >= 'a' && TEXT_PRINCIPAL[0] <= 'z') && !(TEXT_PRINCIPAL[0] >= 'A' && TEXT_PRINCIPAL[0] <= 'Z') && !(TEXT_PRINCIPAL[0] >= '0' && TEXT_PRINCIPAL[0] <= '9'))
				break;

			int ok = addGen(TEXT_PRINCIPAL);
			char text[100];
			switch (ok) {
			case 1:
				strcpy(text, "A fost adaugat cu succes genul "); strcat(text, TEXT_PRINCIPAL);
				MessageBox(NULL, text, WindowCaption, MB_OK);
				break;
			case 0:
				strcpy(text, "Genul "); strcat(text, TEXT_PRINCIPAL); strcat(text, " este deja asociat");
				MessageBox(NULL, text, WindowCaption, MB_OK);
				break;
			default:
				MessageBeep(MB_ICONERROR);
				MessageBox(NULL, "EROARE.Reincercati!", WindowCaption, MB_OK);
				break;
			}
			break;
		}
		}
		break;

	case WM_CREATE: {
		HFONT hfDefault;
		HWND hEdit;

		label02 = CreateWindow(TEXT("Edit"), TEXT(""),
			WS_VISIBLE | WS_CHILD,
			20, 20, 300, 25,
			FEREASTRA, (HMENU)IDM_CAMP_CAUTA_FILME, NULL, NULL);

		CreateWindow(TEXT("button"), TEXT("ADD GEN"),
			WS_VISIBLE | WS_CHILD,
			325, 20, 95, 25,  //margine stanga, margine sus, lungime, latime
			FEREASTRA, (HMENU)IDM_BUTON_FIN_ADD_GEN, NULL, NULL); //21 ESTE INDICATIVUL

		break;
	}
	case WM_SIZE: {
		HWND hEdit;
		RECT rcClient;
		GetClientRect(FEREASTRA, &rcClient);
		hEdit = GetDlgItem(FEREASTRA, IDC_MAIN_EDIT);
		SetWindowPos(hEdit, NULL, 0, 0, rcClient.right, rcClient.bottom, SWP_NOZORDER);

		break;
	}
	case WM_PAINT: {
		//aici imaginea
		break;
	}
	case WM_DESTROY: {
		if (TEXT_PRINCIPAL != NULL) {
			free(TEXT_PRINCIPAL); TEXT_PRINCIPAL = NULL;
		}
		PostQuitMessage(0);
		break;
	}
	}

	return DefWindowProcW(FEREASTRA, msg, ParamClickStanga, ParamClickDreapta);
}

LRESULT CALLBACK WndProcAddVot(HWND FEREASTRA, UINT msg, WPARAM ParamClickStanga, LPARAM ParamClickDreapta)
{
	switch (msg)
	{
	case WM_COMMAND:
		switch (LOWORD(ParamClickStanga))
		{
		case IDM_BUTON_FIN_ADD_VOT: {
			int k = 0;
			if (TEXT_PRINCIPAL != NULL)
				free(TEXT_PRINCIPAL);
			TEXT_PRINCIPAL = (char*)malloc(200 * sizeof(char));
			GetWindowText(label03, TEXT_PRINCIPAL, 200);
			if (!(TEXT_PRINCIPAL[0] >= 'a' && TEXT_PRINCIPAL[0] <= 'z') && !(TEXT_PRINCIPAL[0] >= 'A' && TEXT_PRINCIPAL[0] <= 'Z') && !(TEXT_PRINCIPAL[0] >= '0' && TEXT_PRINCIPAL[0] <= '9'))
				k = 1;

			if (TEXT_USER != NULL)
				free(TEXT_USER);
			TEXT_USER = (char*)malloc(30 * sizeof(char));
			GetWindowText(label04, TEXT_USER, 30);
			if (!(TEXT_USER[0] >= 'a' && TEXT_USER[0] <= 'z') && !(TEXT_USER[0] >= 'A' && TEXT_USER[0] <= 'Z') && !(TEXT_USER[0] >= '0' && TEXT_USER[0] <= '9'))
				k = 1;

			char nr[3];
			GetWindowText(label05, nr, 3);
			if (strlen(nr)>2 || (strlen(nr) == 1 && !(nr[0] >= '1' && nr[0] <= '9')) || (strlen(nr) == 2 && nr[0] != '1') || (strlen(nr) == 2 && nr[1] != '0'))
				k = 2;
			NUMAR_STELE = atoi(nr);

			if (k == 2) {
				MessageBeep(MB_ICONWARNING);
				MessageBox(NULL, "Introduceti un numar corect de stele.", WindowCaption, MB_OK);
				int i = 0;
			}
			if (k == 1) {
				MessageBeep(MB_ICONWARNING);
				MessageBox(NULL, "Toate campurile sunt obligatorii", WindowCaption, MB_OK);
			}
			if (k == 0) {
				int ok = addVot(TEXT_PRINCIPAL, TEXT_USER, NUMAR_STELE);
				char text[100];
				switch (ok) {
				case 0:
					strcpy(text, "Nu exista filmul "); strcat(text, TEXT_PRINCIPAL);
					MessageBeep(MB_ICONWARNING);
					MessageBox(NULL, text, WindowCaption, MB_OK);
					break;

				case 1:
					strcpy(text, "Au fost adaugate "); strcat(text, nr); strcat(text, "/10 stele pentru filmul ");  strcat(text, TEXT_PRINCIPAL);
					MessageBox(NULL, text, WindowCaption, MB_OK);
					break;

				case 2:
					strcpy(text, TEXT_USER); strcat(text, " a mai votat pentru acelasi film.Nu se poate adauga vot nou.");
					MessageBeep(MB_ICONERROR);
					MessageBox(NULL, text, WindowCaption, MB_OK);
					break;

				case 11:
					MessageBeep(MB_ICONWARNING);
					MessageBox(NULL, "Ratingul trebuie sa fie acordat in limita de 1-10 stele", WindowCaption, MB_OK);
					break;

				default:
					MessageBeep(MB_ICONERROR);
					MessageBox(NULL, "EROARE.Reincercati!", WindowCaption, MB_OK);
					break;
				}
			}
		}
		}
		break;

	case WM_CREATE: {
		HFONT hfDefault;
		HWND hEdit;

		if (TEXT_PRINCIPAL != NULL)
			label03 = CreateWindow(TEXT("Edit"), TEXT(TEXT_PRINCIPAL),
				WS_VISIBLE | WS_CHILD,
				20, 20, 300, 25,
				FEREASTRA, (HMENU)IDM_CAMP_VOT_FILM, NULL, NULL);
		else label03 = CreateWindow(TEXT("Edit"), TEXT("Titlu film"),
			WS_VISIBLE | WS_CHILD,
			20, 20, 300, 25,
			FEREASTRA, (HMENU)IDM_CAMP_VOT_FILM, NULL, NULL);

		CreateWindow(TEXT("button"), TEXT("ADD VOT"),
			WS_VISIBLE | WS_CHILD,
			325, 20, 95, 25,  //margine stanga, margine sus, lungime, latime
			FEREASTRA, (HMENU)IDM_BUTON_FIN_ADD_VOT, NULL, NULL); //21 ESTE INDICATIVUL

		label04 = CreateWindow(TEXT("Edit"), TEXT("Nume User"),
			WS_VISIBLE | WS_CHILD,
			20, 50, 150, 25,
			FEREASTRA, (HMENU)IDM_CAMP_NUME_USER_VOT, NULL, NULL);

		label05 = CreateWindow(TEXT("Edit"), TEXT("1,...,10"),
			WS_VISIBLE | WS_CHILD,
			180, 50, 50, 25,
			FEREASTRA, (HMENU)IDM_CAMP_VOT_USER, NULL, NULL);

		break;
	}
	case WM_SIZE: {
		HWND hEdit;
		RECT rcClient;
		GetClientRect(FEREASTRA, &rcClient);
		hEdit = GetDlgItem(FEREASTRA, IDC_MAIN_EDIT);
		SetWindowPos(hEdit, NULL, 0, 0, rcClient.right, rcClient.bottom, SWP_NOZORDER);
		break;
	}
	case WM_PAINT: {
		break;
	}
	case WM_DESTROY: {
		if (TEXT_USER != NULL) {
			free(TEXT_USER); TEXT_USER = NULL;
		}
		PostQuitMessage(0);
		break;
	}
	}

	return DefWindowProcW(FEREASTRA, msg, ParamClickStanga, ParamClickDreapta);
}

LRESULT CALLBACK WndProcAddComent(HWND FEREASTRA, UINT msg, WPARAM ParamClickStanga, LPARAM ParamClickDreapta)
{
	switch (msg)
	{
	case WM_COMMAND:
		switch (LOWORD(ParamClickStanga))
		{
		case IDM_BUTON_FIN_ADD_COMENTARIU: {
			int k = 0;
			if (TEXT_PRINCIPAL != NULL)
				free(TEXT_PRINCIPAL);
			TEXT_PRINCIPAL = (char*)malloc(100 * sizeof(char));
			GetWindowText(label08, TEXT_PRINCIPAL, 100);
			if (!(TEXT_PRINCIPAL[0] >= 'a' && TEXT_PRINCIPAL[0] <= 'z') && !(TEXT_PRINCIPAL[0] >= 'A' && TEXT_PRINCIPAL[0] <= 'Z') && !(TEXT_PRINCIPAL[0] >= '0' && TEXT_PRINCIPAL[0] <= '9')) {
				MessageBox(NULL, "Toate campurile sunt obligatorii", WindowCaption, MB_OK);
				k = 1;
			}

			if (TEXT_USER != NULL)
				free(TEXT_USER);
			TEXT_USER = (char*)malloc(30 * sizeof(char));
			GetWindowText(label06, TEXT_USER, 30);
			if (!(TEXT_USER[0] >= 'a' && TEXT_USER[0] <= 'z') && !(TEXT_USER[0] >= 'A' && TEXT_USER[0] <= 'Z') && !(TEXT_USER[0] >= '0' && TEXT_USER[0] <= '9')) {
				MessageBox(NULL, "Toate campurile sunt obligatorii", WindowCaption, MB_OK);
				k = 1;
			}
			if (TEXT_COMENT != NULL)
				free(TEXT_COMENT);
			TEXT_COMENT = (char*)malloc(200 * sizeof(char));
			GetWindowText(label07, TEXT_COMENT, 200);
			if (!(TEXT_COMENT[0] >= 'a' && TEXT_COMENT[0] <= 'z') && !(TEXT_COMENT[0] >= 'A' && TEXT_COMENT[0] <= 'Z') && !(TEXT_COMENT[0] >= '0' && TEXT_COMENT[0] <= '9')) {
				MessageBox(NULL, "Toate campurile sunt obligatorii", WindowCaption, MB_OK);
				k = 1;
			}

			if (k == 0) {
				int ok = addComentariu(TEXT_PRINCIPAL, TEXT_USER, TEXT_COMENT);
				char text[100];
				switch (ok) {
				case 0:
					strcpy(text, "Nu exista filmul "); strcat(text, TEXT_PRINCIPAL);
					MessageBeep(MB_ICONWARNING);
					MessageBox(NULL, text, WindowCaption, MB_OK);
					break;
				case 1:
					strcpy(text, "A fost adaugat un comentariu pentru filmul ");  strcat(text, TEXT_PRINCIPAL);
					MessageBox(NULL, text, WindowCaption, MB_OK);
					break;
				default:
					MessageBeep(MB_ICONERROR);
					MessageBox(NULL, "EROARE.Reincercati!", WindowCaption, MB_OK);
					break;
				}
			}
		}
		}
		break;

	case WM_CREATE: {
		HFONT hfDefault;
		HWND hEdit;

		if (TEXT_PRINCIPAL != NULL)
			label08 = CreateWindow(TEXT("Edit"), TEXT(TEXT_PRINCIPAL),
				WS_VISIBLE | WS_CHILD,
				20, 20, 300, 25,
				FEREASTRA, (HMENU)IDM_CAMP_COMENTARIU_FILM, NULL, NULL);
		else label08 = CreateWindow(TEXT("Edit"), TEXT("Titlu film"),
			WS_VISIBLE | WS_CHILD,
			20, 20, 300, 25,
			FEREASTRA, (HMENU)IDM_CAMP_COMENTARIU_FILM, NULL, NULL);

		CreateWindow(TEXT("button"), TEXT("ADD COMENTARIU"),
			WS_VISIBLE | WS_CHILD,
			325, 20, 200, 25,  //margine stanga, margine sus, lungime, latime
			FEREASTRA, (HMENU)IDM_BUTON_FIN_ADD_COMENTARIU, NULL, NULL); //21 ESTE INDICATIVUL

		label06 = CreateWindow(TEXT("Edit"), TEXT("Nume User"),
			WS_VISIBLE | WS_CHILD,
			20, 50, 150, 25,
			FEREASTRA, (HMENU)IDM_CAMP_NUME_USER_COMENT, NULL, NULL);

		label07 = CreateWindow(TEXT("Edit"), TEXT("comentariu.."),
			WS_VISIBLE | WS_CHILD,
			180, 50, 345, 40,
			FEREASTRA, (HMENU)IDM_CAMP_COMENTARIU_USER, NULL, NULL);

		break;
	}

	case WM_SIZE: {
		HWND hEdit;
		RECT rcClient;
		GetClientRect(FEREASTRA, &rcClient);
		hEdit = GetDlgItem(FEREASTRA, IDC_MAIN_EDIT);
		SetWindowPos(hEdit, NULL, 0, 0, rcClient.right, rcClient.bottom, SWP_NOZORDER);
		break;
	}

	case WM_DESTROY: {
		if (TEXT_USER != NULL) {
			free(TEXT_USER); TEXT_USER = NULL;
		}
		if (TEXT_COMENT != NULL) {
			free(TEXT_COMENT); TEXT_COMENT = NULL;
		}
		PostQuitMessage(0);
		break;
	}
	}

	return DefWindowProcW(FEREASTRA, msg, ParamClickStanga, ParamClickDreapta);
}

LRESULT CALLBACK WndProcAddFilm(HWND FEREASTRA, UINT msg, WPARAM ParamClickStanga, LPARAM ParamClickDreapta)
{
	switch (msg)
	{
	case WM_COMMAND:
		switch (LOWORD(ParamClickStanga))
		{
		case IDM_BUTON_FIN_ADD_FILM: {
			int k = 0;
			if (TEXT_GEN != NULL)
				free(TEXT_GEN);
			TEXT_GEN = (char*)malloc(100 * sizeof(char));
			GetWindowText(label09, TEXT_GEN, 100);
			if (!(TEXT_GEN[0] >= 'a' && TEXT_GEN[0] <= 'z') && !(TEXT_GEN[0] >= 'A' && TEXT_GEN[0] <= 'Z') && !(TEXT_GEN[0] >= '0' && TEXT_GEN[0] <= '9')) {
				MessageBox(NULL, "Toate campurile sunt obligatorii", WindowCaption, MB_OK);
				k = 1;
			}

			if (TEXT_PRINCIPAL != NULL)
				free(TEXT_PRINCIPAL);
			TEXT_PRINCIPAL = (char*)malloc(100 * sizeof(char));
			GetWindowText(label10, TEXT_PRINCIPAL, 100);
			if (!(TEXT_PRINCIPAL[0] >= 'a' && TEXT_PRINCIPAL[0] <= 'z') && !(TEXT_PRINCIPAL[0] >= 'A' && TEXT_PRINCIPAL[0] <= 'Z') && !(TEXT_PRINCIPAL[0] >= '0' && TEXT_PRINCIPAL[0] <= '9')) {
				MessageBox(NULL, "Toate campurile sunt obligatorii", WindowCaption, MB_OK);
				k = 1;
			}

			char text[100];
			char an[5];
			GetWindowText(label11, an, 5);
			if (!(an[0] == '1' || an[0] == '2') || strlen(an) != 4) {
				MessageBeep(MB_ICONERROR);
				MessageBox(NULL, "Introduceti un an corect!", WindowCaption, MB_OK);
				k = 2;
			}
			NUMAR_AN = atoi(an);
			char min[4];
			GetWindowText(label12, min, 4);
			if (!(min[0] >= '1' && min[0] <= '9')) {
				MessageBeep(MB_ICONERROR);
				MessageBox(NULL, "Introduceti un numar corect de minute!", WindowCaption, MB_OK);
				k = 3;
			}
			NUMAR_MIN = atoi(min);

			if (TEXT_REGIZOR != NULL)
				free(TEXT_REGIZOR);
			TEXT_REGIZOR = (char*)malloc(50 * sizeof(char));
			GetWindowText(label13, TEXT_REGIZOR, 50);
			if (!(TEXT_REGIZOR[0] >= 'a' && TEXT_REGIZOR[0] <= 'z') && !(TEXT_REGIZOR[0] >= 'A' && TEXT_REGIZOR[0] <= 'Z') && !(TEXT_REGIZOR[0] >= '0' && TEXT_REGIZOR[0] <= '9')) {
				MessageBox(NULL, "Toate campurile sunt obligatorii", WindowCaption, MB_OK);
				k = 1;
			}

			if (TEXT_ACTORI != NULL)
				free(TEXT_ACTORI);
			TEXT_ACTORI = (char*)malloc(300 * sizeof(char));
			GetWindowText(label14, TEXT_ACTORI, 300);
			if (!(TEXT_ACTORI[0] >= 'a' && TEXT_ACTORI[0] <= 'z') && !(TEXT_ACTORI[0] >= 'A' && TEXT_ACTORI[0] <= 'Z') && !(TEXT_ACTORI[0] >= '0' && TEXT_ACTORI[0] <= '9')) {
				MessageBox(NULL, "Toate campurile sunt obligatorii", WindowCaption, MB_OK);
				k = 1;
			}

			if (k == 0) {
				int STAT = addFilm(TEXT_GEN, TEXT_PRINCIPAL, NUMAR_MIN, NUMAR_AN, TEXT_ACTORI, TEXT_REGIZOR);
				if (STAT == 0) {
					strcpy(text, TEXT_PRINCIPAL); strcat(text, " exista deja in lista de filme a site-ului.");
					MessageBeep(MB_ICONWARNING);
					MessageBox(NULL, text, WindowCaption, MB_OK);
				}
				else {
					int S = 0;
					while (STAT != 0) {
						S = S * 10 + (STAT % 10);
						STAT /= 10;
					}
					if (S % 10 == 1)
						strcpy(text, "A fost adaugat regizorul ");
					else  strcpy(text, "S-a asociat regizorul ");
					strcat(text, TEXT_REGIZOR); strcat(text, "\n");
					MessageBox(NULL, text, WindowCaption, MB_OK);

					int nr_asoc = 0;
					S /= 10;
					char* ACT = strtok(TEXT_ACTORI, ",;");
					while (ACT != NULL && S != 0) {
						switch (S % 10) {
						case 1:
							strcpy(text, "A fost adaugat actorul "); strcat(text, ACT); strcat(text, "\n");
							MessageBox(NULL, text, WindowCaption, MB_OK);
							break;
						case 2:
							nr_asoc++;
							break;
						default:
							strcpy(text, "EROARE adaugare actor "); strcat(text, ACT); strcat(text, "\n");
							MessageBeep(MB_ICONERROR);
							MessageBox(NULL, text, WindowCaption, MB_OK);
							break;
						}
						ACT = strtok(NULL, ",;");
						S /= 10;
					}

					if (nr_asoc > 0) {
						char nr[3];
						itoa(nr_asoc, nr, 10);
						strcpy(text, "S-a(u) asociat "); strcat(text, nr); strcat(text, "  actor(i)");
						MessageBox(NULL, text, WindowCaption, MB_OK);
					}
					strcpy(text, "A fost adaugat un film in genul "); strcat(text, TEXT_GEN); strcat(text, ": "); strcat(text, TEXT_PRINCIPAL);
					MessageBeep(MB_ICONINFORMATION);
					MessageBox(NULL, text, WindowCaption, MB_OK);
				}
			}
			break;
		}
		}
		break;

	case WM_CREATE: {
		HFONT hfDefault;
		HWND hEdit;

		label09 = CreateWindow(TEXT("Edit"), TEXT("Gen"),
			WS_VISIBLE | WS_CHILD,
			20, 20, 150, 25,
			FEREASTRA, (HMENU)IDM_CAMP_ADD_FILM_GEN, NULL, NULL);

		label10 = CreateWindow(TEXT("Edit"), TEXT("Titlu"),
			WS_VISIBLE | WS_CHILD,
			190, 20, 600, 25,
			FEREASTRA, (HMENU)IDM_CAMP_ADD_FILM_TITLU, NULL, NULL);

		label11 = CreateWindow(TEXT("Edit"), TEXT("An"),
			WS_VISIBLE | WS_CHILD,
			20, 55, 50, 25,
			FEREASTRA, (HMENU)IDM_CAMP_ADD_FILM_AN, NULL, NULL);

		label12 = CreateWindow(TEXT("Edit"), TEXT("Nr.minute"),
			WS_VISIBLE | WS_CHILD,
			90, 55, 70, 25,
			FEREASTRA, (HMENU)IDM_CAMP_ADD_FILM_MINUTE, NULL, NULL);

		label13 = CreateWindow(TEXT("Edit"), TEXT("Nume Regizor"),
			WS_VISIBLE | WS_CHILD,
			20, 90, 200, 25,
			FEREASTRA, (HMENU)IDM_CAMP_ADD_FILM_REGIZOR, NULL, NULL);

		label14 = CreateWindow(TEXT("Edit"), TEXT("Actor1,Actor2,Actor3..."),
			WS_VISIBLE | WS_CHILD,
			20, 125, 765, 25,
			FEREASTRA, (HMENU)IDM_CAMP_ADD_FILM_ACTORI, NULL, NULL);

		CreateWindow(TEXT("button"), TEXT("ADD FILM"),
			WS_VISIBLE | WS_CHILD,
			300, 160, 200, 25,  //margine stanga, margine sus, lungime, latime
			FEREASTRA, (HMENU)IDM_BUTON_FIN_ADD_FILM, NULL, NULL); //21 ESTE INDICATIVUL

		break;
	}
	case WM_SIZE: {
		HWND hEdit;
		RECT rcClient;
		GetClientRect(FEREASTRA, &rcClient);
		hEdit = GetDlgItem(FEREASTRA, IDC_MAIN_EDIT);
		SetWindowPos(hEdit, NULL, 0, 0, rcClient.right, rcClient.bottom, SWP_NOZORDER);

		break;
	}
	case WM_PAINT: {
		//aici imaginea
		break;
	}
	case WM_DESTROY: {
		if (TEXT_PRINCIPAL != NULL) {
			free(TEXT_PRINCIPAL); TEXT_PRINCIPAL = NULL;
		}
		if (TEXT_GEN != NULL) {
			free(TEXT_GEN); TEXT_GEN = NULL;
		}
		if (TEXT_ACTORI != NULL) {
			free(TEXT_ACTORI); TEXT_ACTORI = NULL;
		}
		if (TEXT_REGIZOR != NULL) {
			free(TEXT_REGIZOR); TEXT_REGIZOR = NULL;
		}
		PostQuitMessage(0);
		break;
	}
	}

	return DefWindowProcW(FEREASTRA, msg, ParamClickStanga, ParamClickDreapta);
}

LRESULT CALLBACK WndProcAddFilmFromFile(HWND FEREASTRA, UINT msg, WPARAM ParamClickStanga, LPARAM ParamClickDreapta)
{
	switch (msg)
	{
	case WM_COMMAND:
		switch (LOWORD(ParamClickStanga))
		{
		case IDM_BUTON_FIN_ADD_FILM_FROM_FILE: {
			int k = 0;
			if (TEXT_PRINCIPAL != NULL)
				free(TEXT_PRINCIPAL);
			TEXT_PRINCIPAL = (char*)malloc(100 * sizeof(char));
			GetWindowText(label15, TEXT_PRINCIPAL, 100);//FEREASTRA_RezCautareFilme se va folosi de textul cautat
			if (!(TEXT_PRINCIPAL[0] >= 'a' && TEXT_PRINCIPAL[0] <= 'z') && !(TEXT_PRINCIPAL[0] >= 'A' && TEXT_PRINCIPAL[0] <= 'Z') && !(TEXT_PRINCIPAL[0] >= '0' && TEXT_PRINCIPAL[0] <= '9')) {
				MessageBox(NULL, "Toate campurile sunt obligatorii", WindowCaption, MB_OK);
				k = 1;
			}

			if (k == 0) {
				int STAT = addFilmFromFile(TEXT_PRINCIPAL);
				char text[100];
				if (STAT == -1) {
					MessageBeep(MB_ICONINFORMATION);
					MessageBox(NULL, "Fisierul specificat este inaccesibil sau nu exista.", WindowCaption, MB_OK);
				}
				else if (STAT == 0) {
					strcpy(text, TEXT_PRINCIPAL); strcat(text, " exista deja in lista de filme a site-ului.");
					MessageBeep(MB_ICONWARNING);
					MessageBox(NULL, text, WindowCaption, MB_OK);
				}
				else {
					int S = 0;
					while (STAT != 0) {
						S = S * 10 + (STAT % 10);
						STAT /= 10;
					}
					if (S % 10 == 1)
						strcpy(text, "A fost adaugat regizorul ");
					else  strcpy(text, "S-a asociat regizorul ");
					strcat(text, TEXT_REGIZOR); strcat(text, "\n");
					MessageBox(NULL, text, WindowCaption, MB_OK);

					int nr_asoc = 0;
					S /= 10;
					char* ACT = strtok(TEXT_ACTORI, ",;");
					while (ACT != NULL && S != 0) {
						switch (S % 10) {
						case 1:
							strcpy(text, "A fost adaugat actorul "); strcat(text, ACT); strcat(text, "\n");
							MessageBox(NULL, text, WindowCaption, MB_OK);
							break;
						case 2:
							nr_asoc++;
							break;
						default:
							strcpy(text, "EROARE adaugare actor "); strcat(text, ACT); strcat(text, "\n");
							MessageBeep(MB_ICONERROR);
							MessageBox(NULL, text, WindowCaption, MB_OK);
							break;
						}
						ACT = strtok(NULL, ",;");
						S /= 10;
					}

					if (nr_asoc > 0) {
						char nr[3];
						itoa(nr_asoc, nr, 10);
						strcpy(text, "S-a(u) asociat "); strcat(text, nr); strcat(text, "actor(i)");
						MessageBox(NULL, text, WindowCaption, MB_OK);
					}
					strcpy(text, "A fost adaugat un film in genul "); strcat(text, TEXT_GEN); strcat(text, ": "); strcat(text, TEXT_PRINCIPAL);
					MessageBeep(MB_ICONINFORMATION);
					MessageBox(NULL, text, WindowCaption, MB_OK);
				}
			}
			break;
		}
		}
		break;

	case WM_CREATE: {
		HFONT hfDefault;
		HWND hEdit;

		label15 = CreateWindow(TEXT("Edit"), TEXT("NumeFisier.extensie"),
			WS_VISIBLE | WS_CHILD,
			20, 20, 300, 25,
			FEREASTRA, (HMENU)IDM_CAMP_ADD_FILM_FROM_FILE, NULL, NULL);

		CreateWindow(TEXT("button"), TEXT("ADD FROM FILE"),
			WS_VISIBLE | WS_CHILD,
			325, 20, 160, 25,  //margine stanga, margine sus, lungime, latime
			FEREASTRA, (HMENU)IDM_BUTON_FIN_ADD_FILM_FROM_FILE, NULL, NULL); //21 ESTE INDICATIVUL

		break;
	}
	case WM_SIZE: {
		HWND hEdit;
		RECT rcClient;
		GetClientRect(FEREASTRA, &rcClient);
		hEdit = GetDlgItem(FEREASTRA, IDC_MAIN_EDIT);
		SetWindowPos(hEdit, NULL, 0, 0, rcClient.right, rcClient.bottom, SWP_NOZORDER);

		break;
	}
	case WM_PAINT: {
		//aici imaginea
		break;
	}
	case WM_DESTROY: {
		if (TEXT_PRINCIPAL != NULL) {
			free(TEXT_PRINCIPAL); TEXT_PRINCIPAL = NULL;
		}
		if (TEXT_GEN != NULL) {
			free(TEXT_GEN); TEXT_GEN = NULL;
		}
		if (TEXT_ACTORI != NULL) {
			free(TEXT_ACTORI); TEXT_ACTORI = NULL;
		}
		if (TEXT_REGIZOR != NULL) {
			free(TEXT_REGIZOR); TEXT_REGIZOR = NULL;
		}
		PostQuitMessage(0);
		break;
	}
	}

	return DefWindowProcW(FEREASTRA, msg, ParamClickStanga, ParamClickDreapta);
}

LRESULT CALLBACK WndProcAddActor(HWND FEREASTRA, UINT msg, WPARAM ParamClickStanga, LPARAM ParamClickDreapta)
{
	switch (msg)
	{
	case WM_COMMAND:
		switch (LOWORD(ParamClickStanga))
		{
		case IDM_BUTON_FIN_ADD_ACTOR: {
			int k = 0;
			if (TEXT_PRINCIPAL != NULL)
				free(TEXT_PRINCIPAL);
			TEXT_PRINCIPAL = (char*)malloc(100 * sizeof(char));
			GetWindowText(label16, TEXT_PRINCIPAL, 100);
			if (!(TEXT_PRINCIPAL[0] >= 'a' && TEXT_PRINCIPAL[0] <= 'z') && !(TEXT_PRINCIPAL[0] >= 'A' && TEXT_PRINCIPAL[0] <= 'Z') && !(TEXT_PRINCIPAL[0] >= '0' && TEXT_PRINCIPAL[0] <= '9')) {
				MessageBox(NULL, "Toate campurile sunt obligatorii", WindowCaption, MB_OK);
				k = 1;
			}

			if (k == 0) {
				int ok = addActor(TEXT_PRINCIPAL);
				char text[100];
				switch (ok) {
				case 0:
					strcpy(text, TEXT_PRINCIPAL); strcat(text, " este asociat ca actor.");
					MessageBox(NULL, text, WindowCaption, MB_OK);
					break;
				case 1:
					strcpy(text, "A fost adaugat actorul "); strcat(text, TEXT_PRINCIPAL);
					MessageBox(NULL, text, WindowCaption, MB_OK);
					break;
				default:
					MessageBeep(MB_ICONERROR);
					MessageBox(NULL, "EROARE.Reincercati!", WindowCaption, MB_OK);
					break;
				}
			}
			break;
		}
		}
		break;

	case WM_CREATE: {
		HFONT hfDefault;
		HWND hEdit;

		label16 = CreateWindow(TEXT("Edit"), TEXT(""),
			WS_VISIBLE | WS_CHILD,
			20, 20, 300, 25,
			FEREASTRA, (HMENU)IDM_CAMP_ADD_ACTOR, NULL, NULL);

		CreateWindow(TEXT("button"), TEXT("ADD ACTOR"),
			WS_VISIBLE | WS_CHILD,
			325, 20, 95, 25,  //margine stanga, margine sus, lungime, latime
			FEREASTRA, (HMENU)IDM_BUTON_FIN_ADD_ACTOR, NULL, NULL); //21 ESTE INDICATIVUL

		break;
	}
	case WM_SIZE: {
		HWND hEdit;
		RECT rcClient;
		GetClientRect(FEREASTRA, &rcClient);
		hEdit = GetDlgItem(FEREASTRA, IDC_MAIN_EDIT);
		SetWindowPos(hEdit, NULL, 0, 0, rcClient.right, rcClient.bottom, SWP_NOZORDER);
		break;
	}
	case WM_PAINT: {
		//aici imaginea
		break;
	}
	case WM_DESTROY: {
		if (TEXT_PRINCIPAL != NULL) {
			free(TEXT_PRINCIPAL); TEXT_PRINCIPAL = NULL;
		}
		PostQuitMessage(0);
		break;
	}
	}

	return DefWindowProcW(FEREASTRA, msg, ParamClickStanga, ParamClickDreapta);
}

LRESULT CALLBACK WndProcAddRegizor(HWND FEREASTRA, UINT msg, WPARAM ParamClickStanga, LPARAM ParamClickDreapta)
{
	switch (msg)
	{
	case WM_COMMAND:
		switch (LOWORD(ParamClickStanga))
		{
		case IDM_BUTON_FIN_ADD_REGIZOR: {
			int k = 0;
			if (TEXT_PRINCIPAL != NULL)
				free(TEXT_PRINCIPAL);
			TEXT_PRINCIPAL = (char*)malloc(100 * sizeof(char));
			GetWindowText(label17, TEXT_PRINCIPAL, 100);
			if (!(TEXT_PRINCIPAL[0] >= 'a' && TEXT_PRINCIPAL[0] <= 'z') && !(TEXT_PRINCIPAL[0] >= 'A' && TEXT_PRINCIPAL[0] <= 'Z') && !(TEXT_PRINCIPAL[0] >= '0' && TEXT_PRINCIPAL[0] <= '9')) {
				MessageBox(NULL, "Toate campurile sunt obligatorii", WindowCaption, MB_OK);
				k = 1;
			}

			if (k == 0) {
				int ok = addRegizor(TEXT_PRINCIPAL);
				char text[100];
				switch (ok) {
				case 0:
					strcpy(text, TEXT_PRINCIPAL); strcat(text, " este asociat ca regizor.");
					MessageBox(NULL, text, WindowCaption, MB_OK);
					break;
				case 1:
					strcpy(text, "A fost adaugat regizorul "); strcat(text, TEXT_PRINCIPAL);
					MessageBox(NULL, text, WindowCaption, MB_OK);
					break;
				default:
					MessageBeep(MB_ICONERROR);
					MessageBox(NULL, "EROARE.Reincercati!", WindowCaption, MB_OK);
					break;

				}
			}
			break;
		}
		}
		break;

	case WM_CREATE: {
		HFONT hfDefault;
		HWND hEdit;

		label17 = CreateWindow(TEXT("Edit"), TEXT(""),
			WS_VISIBLE | WS_CHILD,
			20, 20, 300, 25,
			FEREASTRA, (HMENU)IDM_CAMP_ADD_REGIZOR, NULL, NULL);

		CreateWindow(TEXT("button"), TEXT("ADD REGIZOR"),
			WS_VISIBLE | WS_CHILD,
			325, 20, 110, 25,  //margine stanga, margine sus, lungime, latime
			FEREASTRA, (HMENU)IDM_BUTON_FIN_ADD_REGIZOR, NULL, NULL); //21 ESTE INDICATIVUL

		break;
	}
	case WM_SIZE: {
		HWND hEdit;
		RECT rcClient;
		GetClientRect(FEREASTRA, &rcClient);
		hEdit = GetDlgItem(FEREASTRA, IDC_MAIN_EDIT);
		SetWindowPos(hEdit, NULL, 0, 0, rcClient.right, rcClient.bottom, SWP_NOZORDER);
		break;
	}
	case WM_PAINT: {
		//aici imaginea
		break;
	}
	case WM_DESTROY: {
		if (TEXT_PRINCIPAL != NULL) {
			free(TEXT_PRINCIPAL); TEXT_PRINCIPAL = NULL;
		}
		PostQuitMessage(0);
		break;
	}
	}

	return DefWindowProcW(FEREASTRA, msg, ParamClickStanga, ParamClickDreapta);
}

LRESULT CALLBACK WndProcCautaGen(HWND FEREASTRA, UINT msg, WPARAM ParamClickStanga, LPARAM ParamClickDreapta)
{
	HDC hDC;
	PAINTSTRUCT Ps;
	RECT rc;
	switch (msg)
	{
	case WM_COMMAND:
		switch (LOWORD(ParamClickStanga))
		{
		case IDM_BUTON_FIN_CAUTA_GEN: {
			HINSTANCE* hInstance_r = (HINSTANCE*)malloc(sizeof(HINSTANCE));
			PWSTR lpCmdLine_r = gCMD;
			int nCmdShow_r = 10;

			if (TEXT_PRINCIPAL != NULL)
				free(TEXT_PRINCIPAL);
			TEXT_PRINCIPAL = (char*)malloc(100 * sizeof(char));
			GetWindowText(label18, TEXT_PRINCIPAL, 100);//FEREASTRA_RezCautareFilme se va folosi de textul cautat
			if (!(TEXT_PRINCIPAL[0] >= 'a' && TEXT_PRINCIPAL[0] <= 'z') && !(TEXT_PRINCIPAL[0] >= 'A' && TEXT_PRINCIPAL[0] <= 'Z') && !(TEXT_PRINCIPAL[0] >= '0' && TEXT_PRINCIPAL[0] <= '9'))
				break;

			window_REZ_CAUTARE_GEN(*hInstance_r, NULL, lpCmdLine_r, nCmdShow_r);//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< 

			free(hInstance_r);
			break;
		}

		}
		break;

	case WM_CREATE: {
		HFONT hfDefault;
		HWND hEdit;

		label18 = CreateWindow(TEXT("Edit"), TEXT(""),
			WS_VISIBLE | WS_CHILD,
			20, 170, 300, 25,
			FEREASTRA, (HMENU)IDM_CAMP_CAUTA_GEN, NULL, NULL);

		CreateWindow(TEXT("button"), TEXT("CAUTA GEN"),
			WS_VISIBLE | WS_CHILD,
			325, 170, 95, 25,  //margine stanga, margine sus, lungime, latime
			FEREASTRA, (HMENU)IDM_BUTON_FIN_CAUTA_GEN, NULL, NULL); //21 ESTE INDICATIVUL

		break;
	}
	case WM_SIZE: {
		HWND hEdit;
		RECT rcClient;
		GetClientRect(FEREASTRA, &rcClient);
		hEdit = GetDlgItem(FEREASTRA, IDC_MAIN_EDIT);
		SetWindowPos(hEdit, NULL, 0, 0, rcClient.right, rcClient.bottom, SWP_NOZORDER);
		break;
	}
	case WM_PAINT: {
		hDC = BeginPaint(FEREASTRA, &Ps);
		GetClientRect(FEREASTRA, &rc);
		printListaGenuri(hDC, rc, FEREASTRA);///////////////////////////////
		ReleaseDC(0, hDC);
		break;
	}

	case WM_DESTROY: {
		if (TEXT_PRINCIPAL != NULL) {
			free(TEXT_PRINCIPAL); TEXT_PRINCIPAL = NULL;
		}
		PostQuitMessage(0);
		break;
	}
	}

	return DefWindowProcW(FEREASTRA, msg, ParamClickStanga, ParamClickDreapta);
}

LRESULT CALLBACK WndProcCautaAn(HWND FEREASTRA, UINT msg, WPARAM ParamClickStanga, LPARAM ParamClickDreapta)
{
	switch (msg)
	{
	case WM_COMMAND:
		switch (LOWORD(ParamClickStanga))
		{
		case IDM_BUTON_FIN_CAUTA_AN: {
			HINSTANCE* hInstance_r = (HINSTANCE*)malloc(sizeof(HINSTANCE));
			PWSTR lpCmdLine_r = gCMD;
			int nCmdShow_r = 10;

			int k = 0;
			char an[5];
			GetWindowText(label19, an, 5);
			if (!(an[0] == '1' || an[0] == '2') || strlen(an) != 4) {
				MessageBeep(MB_ICONERROR);
				MessageBox(NULL, "Introduceti un an corect!", WindowCaption, MB_OK);
				k = 1;
			}
			NUMAR_AN = atoi(an);
			if (k == 0)
				window_REZ_CAUTARE_AN(*hInstance_r, NULL, lpCmdLine_r, nCmdShow_r);//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< 

			free(hInstance_r);
			break;
		}

		}
		break;

	case WM_CREATE: {
		HFONT hfDefault;
		HWND hEdit;

		label19 = CreateWindow(TEXT("Edit"), TEXT(""),
			WS_VISIBLE | WS_CHILD,
			20, 20, 300, 25,
			FEREASTRA, (HMENU)IDM_CAMP_CAUTA_AN, NULL, NULL);

		CreateWindow(TEXT("button"), TEXT("CAUTA AN"),
			WS_VISIBLE | WS_CHILD,
			325, 20, 95, 25,  //margine stanga, margine sus, lungime, latime
			FEREASTRA, (HMENU)IDM_BUTON_FIN_CAUTA_AN, NULL, NULL);

		break;
	}
	case WM_SIZE: {
		HWND hEdit;
		RECT rcClient;
		GetClientRect(FEREASTRA, &rcClient);
		hEdit = GetDlgItem(FEREASTRA, IDC_MAIN_EDIT);
		SetWindowPos(hEdit, NULL, 0, 0, rcClient.right, rcClient.bottom, SWP_NOZORDER);
		break;
	}
	case WM_PAINT:
		break;

	case WM_DESTROY: {
		PostQuitMessage(0);
		break;
	}
	}

	return DefWindowProcW(FEREASTRA, msg, ParamClickStanga, ParamClickDreapta);
}

LRESULT CALLBACK WndProcCautaActor(HWND FEREASTRA, UINT msg, WPARAM ParamClickStanga, LPARAM ParamClickDreapta)
{
	HDC hDC;
	PAINTSTRUCT Ps;
	RECT rc;
	switch (msg)
	{
	case WM_COMMAND:
		switch (LOWORD(ParamClickStanga))
		{
		case IDM_BUTON_FIN_CAUTA_ACTOR: {
			HINSTANCE* hInstance_r = (HINSTANCE*)malloc(sizeof(HINSTANCE));
			PWSTR lpCmdLine_r = gCMD;
			int nCmdShow_r = 10;

			if (TEXT_PRINCIPAL != NULL)
				free(TEXT_PRINCIPAL);
			TEXT_PRINCIPAL = (char*)malloc(100 * sizeof(char));
			GetWindowText(label20, TEXT_PRINCIPAL, 100);//FEREASTRA_RezCautareFilme se va folosi de textul cautat
			if (!(TEXT_PRINCIPAL[0] >= 'a' && TEXT_PRINCIPAL[0] <= 'z') && !(TEXT_PRINCIPAL[0] >= 'A' && TEXT_PRINCIPAL[0] <= 'Z') && !(TEXT_PRINCIPAL[0] >= '0' && TEXT_PRINCIPAL[0] <= '9'))
				break;

			window_REZ_CAUTARE_ACTOR(*hInstance_r, NULL, lpCmdLine_r, nCmdShow_r);//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< 

			free(hInstance_r);
			break;
		}

		}
		break;

	case WM_CREATE: {
		HFONT hfDefault;
		HWND hEdit;

		label20 = CreateWindow(TEXT("Edit"), TEXT(""),
			WS_VISIBLE | WS_CHILD,
			20, 320, 430, 25,
			FEREASTRA, (HMENU)IDM_CAMP_CAUTA_ACTOR, NULL, NULL);

		CreateWindow(TEXT("button"), TEXT("CAUTA ACTOR"),
			WS_VISIBLE | WS_CHILD,
			455, 320, 120, 25,  //margine stanga, margine sus, lungime, latime
			FEREASTRA, (HMENU)IDM_BUTON_FIN_CAUTA_ACTOR, NULL, NULL);

		break;
	}
	case WM_SIZE: {
		HWND hEdit;
		RECT rcClient;
		GetClientRect(FEREASTRA, &rcClient);
		hEdit = GetDlgItem(FEREASTRA, IDC_MAIN_EDIT);
		SetWindowPos(hEdit, NULL, 0, 0, rcClient.right, rcClient.bottom, SWP_NOZORDER);
		break;
	}
	case WM_PAINT: {
		hDC = BeginPaint(FEREASTRA, &Ps);
		GetClientRect(FEREASTRA, &rc);
		printListaActori(hDC, rc, FEREASTRA);///////////////////////////////
		ReleaseDC(0, hDC);
		break;
	}

	case WM_DESTROY: {
		if (TEXT_PRINCIPAL != NULL) {
			free(TEXT_PRINCIPAL); TEXT_PRINCIPAL = NULL;
		}
		PostQuitMessage(0);
		break;
	}
	}

	return DefWindowProcW(FEREASTRA, msg, ParamClickStanga, ParamClickDreapta);
}

LRESULT CALLBACK WndProcCautaRegizor(HWND FEREASTRA, UINT msg, WPARAM ParamClickStanga, LPARAM ParamClickDreapta)
{
	HDC hDC;
	PAINTSTRUCT Ps;
	RECT rc;
	switch (msg)
	{
	case WM_COMMAND:
		switch (LOWORD(ParamClickStanga))
		{
		case IDM_BUTON_FIN_CAUTA_REGIZOR: {
			HINSTANCE* hInstance_r = (HINSTANCE*)malloc(sizeof(HINSTANCE));
			PWSTR lpCmdLine_r = gCMD;
			int nCmdShow_r = 10;

			if (TEXT_PRINCIPAL != NULL)
				free(TEXT_PRINCIPAL);
			TEXT_PRINCIPAL = (char*)malloc(100 * sizeof(char));
			GetWindowText(label21, TEXT_PRINCIPAL, 100);//FEREASTRA_RezCautareFilme se va folosi de textul cautat
			if (!(TEXT_PRINCIPAL[0] >= 'a' && TEXT_PRINCIPAL[0] <= 'z') && !(TEXT_PRINCIPAL[0] >= 'A' && TEXT_PRINCIPAL[0] <= 'Z') && !(TEXT_PRINCIPAL[0] >= '0' && TEXT_PRINCIPAL[0] <= '9'))
				break;

			window_REZ_CAUTARE_REGIZOR(*hInstance_r, NULL, lpCmdLine_r, nCmdShow_r);//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< 

			free(hInstance_r);
			break;
		}

		}
		break;

	case WM_CREATE: {
		HFONT hfDefault;
		HWND hEdit;

		label21 = CreateWindow(TEXT("Edit"), TEXT(""),
			WS_VISIBLE | WS_CHILD,
			20, 170, 300, 25,
			FEREASTRA, (HMENU)IDM_CAMP_CAUTA_REGIZOR, NULL, NULL);

		CreateWindow(TEXT("button"), TEXT("CAUTA REGIZOR"),
			WS_VISIBLE | WS_CHILD,
			325, 170, 120, 25,  //margine stanga, margine sus, lungime, latime
			FEREASTRA, (HMENU)IDM_BUTON_FIN_CAUTA_REGIZOR, NULL, NULL);

		break;
	}
	case WM_SIZE: {
		HWND hEdit;
		RECT rcClient;
		GetClientRect(FEREASTRA, &rcClient);
		hEdit = GetDlgItem(FEREASTRA, IDC_MAIN_EDIT);
		SetWindowPos(hEdit, NULL, 0, 0, rcClient.right, rcClient.bottom, SWP_NOZORDER);
		break;
	}
	case WM_PAINT: {
		hDC = BeginPaint(FEREASTRA, &Ps);
		GetClientRect(FEREASTRA, &rc);
		printListaRegizori(hDC, rc, FEREASTRA);///////////////////////////////
		ReleaseDC(0, hDC);
		break;
	}

	case WM_DESTROY: {
		if (TEXT_PRINCIPAL != NULL) {
			free(TEXT_PRINCIPAL); TEXT_PRINCIPAL = NULL;
		}
		PostQuitMessage(0);
		break;
	}
	}

	return DefWindowProcW(FEREASTRA, msg, ParamClickStanga, ParamClickDreapta);
}

LRESULT CALLBACK WndProcRezCautaGen(HWND FEREASTRA, UINT msg, WPARAM ParamClickStanga, LPARAM ParamClickDreapta)
{
	HDC hDC;
	PAINTSTRUCT Ps;
	RECT rc;
	switch (msg)
	{
	case WM_COMMAND:
		break;
	case WM_CREATE:
		break;
	case WM_SIZE: {
		HWND hEdit;
		GetClientRect(FEREASTRA, &rc);
		hEdit = GetDlgItem(FEREASTRA, IDC_MAIN_EDIT);
		SetWindowPos(hEdit, NULL, 0, 0, rc.right, rc.bottom, SWP_NOZORDER);
		break;
	}
	case WM_PAINT: {
		hDC = BeginPaint(FEREASTRA, &Ps);
		GetClientRect(FEREASTRA, &rc);
		printFilmePeGen(hDC, rc, FEREASTRA, TEXT_PRINCIPAL);///////////////////////////////
		ReleaseDC(0, hDC);
		break;
	}
	case WM_DESTROY: {
		EndPaint(FEREASTRA, &Ps);
		PostQuitMessage(0);
		break;
	}
	}

	return DefWindowProcW(FEREASTRA, msg, ParamClickStanga, ParamClickDreapta);
}

LRESULT CALLBACK WndProcRezCautaAn(HWND FEREASTRA, UINT msg, WPARAM ParamClickStanga, LPARAM ParamClickDreapta)
{
	HDC hDC;
	PAINTSTRUCT Ps;
	RECT rc;
	switch (msg)
	{
	case WM_COMMAND:
		break;
	case WM_CREATE:
		break;
	case WM_SIZE: {
		HWND hEdit;
		GetClientRect(FEREASTRA, &rc);
		hEdit = GetDlgItem(FEREASTRA, IDC_MAIN_EDIT);
		SetWindowPos(hEdit, NULL, 0, 0, rc.right, rc.bottom, SWP_NOZORDER);
		break;
	}
	case WM_PAINT: {
		hDC = BeginPaint(FEREASTRA, &Ps);
		GetClientRect(FEREASTRA, &rc);
		printFilmePeAn(hDC, rc, FEREASTRA, NUMAR_AN);///////////////////////////////
		ReleaseDC(0, hDC);
		break;
	}
	case WM_DESTROY: {
		EndPaint(FEREASTRA, &Ps);
		PostQuitMessage(0);
		break;
	}
	}

	return DefWindowProcW(FEREASTRA, msg, ParamClickStanga, ParamClickDreapta);
}

LRESULT CALLBACK WndProcRezCautaActor(HWND FEREASTRA, UINT msg, WPARAM ParamClickStanga, LPARAM ParamClickDreapta)
{
	HDC hDC;
	PAINTSTRUCT Ps;
	RECT rc;
	switch (msg)
	{
	case WM_COMMAND:
		break;
	case WM_CREATE:
		break;
	case WM_SIZE: {
		HWND hEdit;
		GetClientRect(FEREASTRA, &rc);
		hEdit = GetDlgItem(FEREASTRA, IDC_MAIN_EDIT);
		SetWindowPos(hEdit, NULL, 0, 0, rc.right, rc.bottom, SWP_NOZORDER);
		break;
	}
	case WM_PAINT: {
		hDC = BeginPaint(FEREASTRA, &Ps);
		GetClientRect(FEREASTRA, &rc);
		printDetaliiActor(hDC, rc, FEREASTRA, TEXT_PRINCIPAL);///////////////////////////////
		ReleaseDC(0, hDC);
		break;
	}
	case WM_DESTROY: {
		EndPaint(FEREASTRA, &Ps);
		PostQuitMessage(0);
		break;
	}
	}

	return DefWindowProcW(FEREASTRA, msg, ParamClickStanga, ParamClickDreapta);
}

LRESULT CALLBACK WndProcRezCautaRegizor(HWND FEREASTRA, UINT msg, WPARAM ParamClickStanga, LPARAM ParamClickDreapta)
{
	HDC hDC;
	PAINTSTRUCT Ps;
	RECT rc;
	switch (msg)
	{
	case WM_COMMAND:
		break;
	case WM_CREATE:
		break;
	case WM_SIZE: {
		HWND hEdit;
		GetClientRect(FEREASTRA, &rc);
		hEdit = GetDlgItem(FEREASTRA, IDC_MAIN_EDIT);
		SetWindowPos(hEdit, NULL, 0, 0, rc.right, rc.bottom, SWP_NOZORDER);
		break;
	}
	case WM_PAINT: {
		hDC = BeginPaint(FEREASTRA, &Ps);
		GetClientRect(FEREASTRA, &rc);
		printDetaliiRegizor(hDC, rc, FEREASTRA, TEXT_PRINCIPAL);///////////////////////////////
		ReleaseDC(0, hDC);
		break;
	}
	case WM_DESTROY: {
		EndPaint(FEREASTRA, &Ps);
		PostQuitMessage(0);
		break;
	}
	}

	return DefWindowProcW(FEREASTRA, msg, ParamClickStanga, ParamClickDreapta);
}

LRESULT CALLBACK WndProcNoutati(HWND FEREASTRA, UINT msg, WPARAM ParamClickStanga, LPARAM ParamClickDreapta)
{
	HDC hDC;
	PAINTSTRUCT Ps;
	RECT rc;
	switch (msg)
	{
	case WM_COMMAND:
		break;
	case WM_CREATE:
		break;
	case WM_SIZE: {
		HWND hEdit;
		GetClientRect(FEREASTRA, &rc);
		hEdit = GetDlgItem(FEREASTRA, IDC_MAIN_EDIT);
		SetWindowPos(hEdit, NULL, 0, 0, rc.right, rc.bottom, SWP_NOZORDER);
		break;
	}
	case WM_PAINT: {
		hDC = BeginPaint(FEREASTRA, &Ps);
		GetClientRect(FEREASTRA, &rc);
		printNoutati(hDC, rc, FEREASTRA);///////////////////////////////
		ReleaseDC(0, hDC);
		break;
	}
	case WM_DESTROY: {
		EndPaint(FEREASTRA, &Ps);
		PostQuitMessage(0);
		break;
	}
	}

	return DefWindowProcW(FEREASTRA, msg, ParamClickStanga, ParamClickDreapta);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Main///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define IDS_APP_TITLE			103
#define IDR_MAINFRAME			128
#define IDD_WIN32LESSON1_DIALOG	102
#define IDD_ABOUTBOX			103
#define IDM_ABOUT				104
#define IDM_EXIT				105
#define IDI_WIN32LESSON1		107
#define IDI_SMALL				108
#define IDC_WIN32LESSON1		109
#define IDC_MYICON				2

#ifndef IDC_STATIC
#define IDC_STATIC				-1
#endif

#ifdef APSTUDIO_INVOKED
#ifndef APSTUDIO_READONLY_SYMBOLS
#define _APS_NO_MFC					130
#define _APS_NEXT_RESOURCE_VALUE	129
#define _APS_NEXT_COMMAND_VALUE		32771
#define _APS_NEXT_CONTROL_VALUE		1000
#define _APS_NEXT_SYMED_VALUE		110
#endif
#endif

#define MAX_LOADSTRING 100
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

BOOL LoadAndBlitBitmap(LPCWSTR szFileName, HDC hWinDC)
{
	//Load the bitmap image file
	HBITMAP hBitmap = NULL;
	hBitmap = (HBITMAP)LoadImage(NULL, szFileName, IMAGE_BITMAP, 0, 0, NULL);//LoadImage(NULL, szFileName, IMAGE_BITMAP, 300, 300, NULL);

																			 // Verify that the image was loaded
																			 /*if (hBitmap == NULL) {
																			 MessageBox(NULL, __T("LoadImage Failed"), __T("Error"), MB_OK);
																			 return -1;
																			 }*/

																			 // Create a device context that is compatible with the window
	HDC hLocalDC;
	hLocalDC = CreateCompatibleDC(hWinDC);
	// Verify that the device context was created
	/*if (hLocalDC == NULL) {
	MessageBox(NULL, __T("CreateCompatibleDCFailed"), __T("Error"), MB_OK);
	return -1;
	}*/

	//Get the bitmap's parameters and verify the get
	BITMAP qBitmap;
	int iReturn = GetObject((HGDIOBJ)hBitmap, sizeof(BITMAP),
		(LPVOID)&qBitmap);
	/*if (!iReturn) {
	MessageBox(NULL, __T("GetObject Failed"), __T("Error"), MB_OK);
	return -1;
	}*/

	// Select the loaded bitmap into the device context
	HBITMAP hOldBmp = (HBITMAP)SelectObject(hLocalDC, hBitmap);
	/*if (hOldBmp == NULL) {
	MessageBox(NULL, __T("SelectObject Failed"), __T("Error"), MB_OK);
	return -1;
	}*/

	// Blit the dc which holds the bitmap onto the window's dc
	BOOL qRetBlit = BitBlt(hWinDC, 0, 0, qBitmap.bmWidth, qBitmap.bmHeight,
		hLocalDC, 0, 0, SRCCOPY);
	/*if (!qRetBlit) {
	MessageBox(NULL, __T("Blit Failed"), __T("Error"), MB_OK);
	return -1;
	}*/

	// Unitialize and deallocate resources
	SelectObject(hLocalDC, hOldBmp);
	DeleteDC(hLocalDC);
	DeleteObject(hBitmap);
	return 0;
}

INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

//////////////////////////////////////////////////////////////////////////////////////////////
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nCmdShow)
{
	gINST = hInstance;
	gPrevINST = hPrevInstance;
	gCMD = lpCmdLine;
	MSG  msg;
	WNDCLASSW wc = { 0 };

	wc.lpszClassName = TEXT("WWW.FILM.API");
	wc.hInstance = hInstance;
	wc.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
	wc.lpfnWndProc = WndProc01;
	wc.hCursor = LoadCursor(0, IDC_ARROW);



	RegisterClassW(&wc);//fereastra principala
	FEREASTRA01 = CreateWindowW(wc.lpszClassName, L"WWW.FILME.API",
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		200, 200, 450, 350, 0, 0, hInstance, 0);//dimensiuni: margine stanga, sus , lungime, latime

												//ShowWindow(FEREASTRA01, nCmdShow);///////////////////////
												//UpdateWindow(FEREASTRA01);//////////////////////////////
												//HACCEL hAccelTable;/////////////////////////////////////////////////////////////////
												//hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WIN32LESSON1));///////
	while (GetMessage(&msg, NULL, 0, 0)) {
		//if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)){////////////////
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		//}
	}

	BOOL bRet;
	while ((bRet = GetMessage(&msg, NULL, 0, 0)) != 0) {
		if (bRet == -1)
			PostQuitMessage(-1);
		else {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return (int)msg.wParam;
}
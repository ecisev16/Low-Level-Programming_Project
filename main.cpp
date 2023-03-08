#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <iostream>
#include "image_processing.cpp"

using namespace std;

void Dilation(int n, int filter_size, short* resimadres_org);
void Erosion(int n, int filter_size, short* resimadres_org);

int main(void) {
	int M, N, Q, i, j, filter_size;
	bool type;
	int efile;
	char org_resim[100], dil_resim[] = "dilated.pgm", ero_resim[] = "eroded.pgm";
	do {
		printf("Orijinal resmin yolunu (path) giriniz:\n-> ");
		scanf("%s", &org_resim);
		system("CLS");
		efile = readImageHeader(org_resim, N, M, Q, type);
	} while (efile > 1);
	int** resim_org = resimOku(org_resim);

	printf("Orjinal Resim Yolu: \t\t\t%s\n", org_resim);

	short *resimdizi_org = (short*) malloc(N*M * sizeof(short));

	for (i = 0; i < N; i++)
		for (j = 0; j < M; j++)
			resimdizi_org[i*N + j] = (short)resim_org[i][j];

	int menu;
	printf("Yapmak istediginiz islemi giriniz...\n");
	printf("1-) Dilation\n");
	printf("2-) Erosion\n");
	printf("3-) Cikis\n> ");
	scanf("%d", &menu);
	printf("Filtre boyutunu giriniz: ");
	scanf("%d", &filter_size);

	switch (menu){
		case 1:
			Dilation(N*M, filter_size, resimdizi_org);
			resimYaz(dil_resim, resimdizi_org, N, M, Q);
			break;
		case 2:
			Erosion(N*M, filter_size, resimdizi_org);
			resimYaz(ero_resim, resimdizi_org, N, M, Q);
			break;
		case 3:
			system("EXIT");
			break;
		default:
			system("EXIT");
			break;
	}

	system("PAUSE");
	return 0;
}

void Dilation(int n, int filter_size, short* resim_org) {
	
	__asm {
		
		PUSH ECX
		PUSH EDI
		PUSH ESI
		PUSH EAX
		PUSH EBX
		PUSH EDX

		MOV ECX, n
		MOV ESI, resim_org
		P1 : MOVSX EAX, WORD PTR[ESI]
		CMP EAX, 128
		JB SFR
		MOV WORD PTR[ESI], 255
		JMP P2
		SFR : MOV WORD PTR[ESI], 0
		P2 : ADD ESI, 2
		LOOP P1
		
		MOV ESI, resim_org 
		MOV EDI, resim_org

		MOV n, 512
		MOV EAX, n                             // for (i = 0; i < ((n - filter_size + 1)*(n - filter_size + 1)); i++)
		SUB EAX, filter_size
		INC EAX
		MUL EAX
		MOV ECX, EAX
	
		MOV EBX, 1                             // X in baþlangýç deðerinin ayarlanmasý

	i_dongusu: MOV EAX, filter_size            //for (j = 0; j < (filter_size*filter_size); j++);
		MUL EAX
		XOR EDX, EDX                           // j döngüsünden önce j yi sýfýrlama. j, dx de saklanacak
		PUSH ECX                               //j döngüsünün cx ine geçiþ
		MOV ECX, EAX


	j_dongusu: MOVSX EAX, WORD PTR[ESI]
		CMP EAX, 255         
		JNE Label1
		MOV WORD PTR [EDI], 255                

	Label1:	PUSH EDX                               // bölme iþleminden önce dx in deðerlerini koruma
		INC EDX                                //if ((j + 1) % filter_size == 0)           
		MOV EAX, EDX
		XOR EDX,EDX
		DIV filter_size                        // kalan dx de
		CMP EDX, 0
		JNZ Label2
		
		MOV EAX, n                             //n
		SUB EAX, filter_size                   //n - filter_size
		INC EAX                                //(n - filter_size) + 1
		SHL EAX, 1                             //32 bit bellekte hareket ettiði için
		ADD ESI, EAX                           //k = k + (n - filter_size) + 1
		JMP Label3

	Label2:	ADD ESI, 2                             //k=k+1

	Label3:	POP EDX
		INC EDX                                //j+1

		LOOP j_dongusu

		MOV EAX, EBX                           // X þu an ax in içinde
		MUL n
		SUB EAX, EDI
		POP ECX                                // i döngüsünün cx ine tekrar geçiþ

		CMP EAX, filter_size
		JNE Label4
		MOV EAX, filter_size            
		SHL EAX,1
		ADD EDI, EAX                            //konum = konum + filter_size;
		INC EBX                                 //x = x + 1;
		JMP Label5

	Label4:	ADD EDI, 2                              //konum = konum + 1;
	Label5: MOV ESI, EDI                            //k = konum;
		LOOP i_dongusu
		
		POP EDX
		POP EBX
		POP EAX
		POP ESI
		POP EDI
		POP ECX
	}
	printf("\nDilation islemi sonucunda resim \"dilated.pgm\" ismiyle olusturuldu...\n");
}

void Erosion(int n, int filter_size, short* resim_org) {
	__asm {
		PUSH ECX
		PUSH EDI
		PUSH ESI
		PUSH EAX
		PUSH EBX
		PUSH EDX


		MOV ECX, n
		MOV ESI, resim_org
		F1 : MOVSX EAX, WORD PTR[ESI]
		CMP EAX, 128
		JB SFR
		MOV WORD PTR[ESI], 255
		JMP F2
		SFR : MOV WORD PTR[ESI], 0
		F2 : ADD ESI, 2
		LOOP F1
		MOV n, 512

		MOV ESI, resim_org
		MOV EDI, resim_org

		MOV EAX, n                                     // for (i = 0; i < ((n - filter_size + 1)*(n - filter_size + 1)); i++)
		SUB EAX, filter_size
		INC EAX
		MUL EAX
		MOV ECX, EAX
		
		MOV EBX, 1                                     // X in baþlangýç deðerinin ayarlanmasý
		i_dongusu: MOV EAX, filter_size                           //for (j = 0; j < (filter_size*filter_size); j++);
		MUL EAX
		PUSH ECX                                       //j döngüsünün cx ine geçiþ
		MOV ECX, EAX
		XOR EDX, EDX                                   // j döngüsünden önce j yi sýfýrlama. j, dx de saklanacak

		j_dongusu : MOVSX EAX, WORD PTR[ESI]
		CMP EAX, 0                    
		JNE L1
		MOV WORD PTR[EDI], 0                

		L1 : PUSH EDX                                  // bölme iþleminden önce dx in deðerlerini koruma
		INC EDX                                        //if ((j + 1) % filter_size == 0)           
		MOV EAX, EDX
		XOR EDX, EDX
		DIV filter_size                                // kalan dx de
		CMP EDX, 0
		JNZ L2

		MOV EAX, n                                     //n
		SUB EAX, filter_size                           //n - filter_size
		INC EAX                                        //(n - filter_size) + 1
		SHL EAX, 1                                     //32 bit bellekte hareket ettiði için
		ADD ESI, EAX                                   //k = k + (n - filter_size) + 1
		JMP L3

		L2 : ADD ESI, 2                                // k=k+1
		L3 :POP EDX
		INC EDX                                        //j+1
		LOOP j_dongusu

		MOV EAX, EBX                                   // X þu an ax in içinde
		MUL n
		SUB EAX, EDI
		POP ECX                                        // i döngüsünün cx ine tekrar geçiþ
		CMP EAX, filter_size
		JNE L4
		MOV EAX, filter_size
		SHL EAX, 1
		ADD EDI, EAX                                    //konum = konum + filter_size;
		INC EBX                                         //x = x + 1;
		JMP L5
		L4 : ADD EDI, 2                                //konum = konum + 1;
		L5 : MOV ESI, EDI                                   //k = konum;
		LOOP i_dongusu

		POP EDX
		POP EBX
		POP EAX
		POP ESI
		POP EDI
		POP ECX
	}
	printf("\nErosion islemi sonucunda resim \"eroded.pgm\" ismiyle olusturuldu...\n");
}

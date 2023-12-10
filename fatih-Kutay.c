#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <math.h>
#define MAX_CHARACTERS 150
#define MAX_WORDS 3000
#define MAX_SENTENCES 200

void kelimedenParantezKaldir(char* kelime) {
	size_t baslangic = strspn(kelime, "(\"");
	size_t uzunluk = strcspn(kelime + baslangic, "\")");

	memmove(kelime, kelime + baslangic, uzunluk);
	kelime[uzunluk] = '\0';
}

void dosyadanSozluge(FILE *dosya, char **uniqueKelimeler, int *uniqueKelimeSayisi) {
	char satir[MAX_CHARACTERS];

	while (fgets(satir, sizeof(satir), dosya) != NULL) {
		char *kelime = strtok(satir, " \t\n");

		while (kelime != NULL) {
			int unique = 1;
			if (kelime[strlen(kelime) - 1] == '.' || kelime[strlen(kelime) - 1] == ',' || kelime[strlen(kelime) - 1] == ';') {
				kelime[strlen(kelime) - 1] = '\0';
			}

			kelimedenParantezKaldir(kelime);
			int i;
			for (i = 0; i < *uniqueKelimeSayisi; i++) {
				if (strcmp(uniqueKelimeler[i], kelime) == 0) {
					unique = 0;
					break;
				}
			}

			if (unique) {
				strcpy(uniqueKelimeler[*uniqueKelimeSayisi], kelime);
				(*uniqueKelimeSayisi)++;
			}

			kelime = strtok(NULL, " \t\n");
		}
	}
}

void oneHotVectorOlustur(char **uniqueKelimeler, int uniqueKelimeSayisi, char metin[MAX_CHARACTERS], int **oneHotVector, int sentenceIndex) {
	char *kelime = strtok(metin, " \t\n");

	while (kelime != NULL) {
		if (kelime[strlen(kelime) - 1] == '.' || kelime[strlen(kelime) - 1] == ',' || kelime[strlen(kelime) - 1] == ';') {
			kelime[strlen(kelime) - 1] = '\0';
		}
		int i;
		for (i = 0; i < uniqueKelimeSayisi; i++) {
			if (strcmp(uniqueKelimeler[i], kelime) == 0) {
				oneHotVector[sentenceIndex][i] = 1;
				break;
			}
		}
		kelime = strtok(NULL, " \t\n");
	}
}

int isValueInArray(int value, int *array, int size) {
	int i;
	for ( i = 0; i < size; ++i) {
		if (array[i] == value) {
			return 1;
		}
	}
	return 0;
}
double generateRandomRationalNumber() {
	// -2 ile 2 arasında rasyonel sayı üretmek
	double randomValue = ((double)rand() / RAND_MAX) * 4.0 - 2.0;
	return randomValue;
}

void GD(double *W, int **oneHotVector, int *EgitimKumesi, int uniqueKelimeSayisi, FILE *dosyaC, FILE *TimeA) {
    int i, j, iter=1;
    double eps = 0.1;
    double fx, mse,old_mse=0;
    int maxIter = 500;
    double *Y = (double *)malloc(160 * sizeof(double));
    for (i = 0; i < 80; i++) {
        Y[i] = 1;
    }
    for (i = 80; i < 160; i++) {
        Y[i] = -1;
    }
    double HataDegerleri[160];
    double jacobian_t_r; // jacobian^t * r
    double **jacobian = (double **)calloc(160, sizeof(double *));
    for (i = 0; i < 160; i++) {
        jacobian[i] = (double *)calloc(uniqueKelimeSayisi, sizeof(double));
    }
    
    do {
    	clock_t start_time = clock();
    	old_mse=mse;
        mse = 0;
        for (i = 0; i < 160; i++) {
            fx = 0;
            for (j = 0; j < uniqueKelimeSayisi; j++) {
                fx = fx + W[j] * oneHotVector[EgitimKumesi[i]][j];
            }
            
            HataDegerleri[i] = Y[i] - tanh(fx);
            
            for (j = 0; j < uniqueKelimeSayisi; j++) {
                jacobian[i][j] = -oneHotVector[EgitimKumesi[i]][j] * pow(cosh(fx), -2);
            }
            
            mse = mse + pow(Y[i] - tanh(fx), 2);
        }
        
        for (i = 0; i < uniqueKelimeSayisi; i++) {
            jacobian_t_r = 0;
            for (j = 0; j < 160; j++) {
                jacobian_t_r = jacobian_t_r + jacobian[j][i] * HataDegerleri[j];
            }
            W[i] = W[i] - eps * jacobian_t_r ;  
        }
        
        mse = mse / 160;
        fprintf(dosyaC, "%f\n", mse);
        //printf("GD mse: %f | iterasyon: %d\n", mse, (iter + 1));
        iter++;
        clock_t end_time = clock();
        double elapsed_time = elapsed_time + ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
        fprintf(TimeA, "%f\n", elapsed_time);
        
    } while (fabs(mse-old_mse) > 0.0000001 && iter < maxIter);
    for (i = 0; i < 160; i++) {
        free(jacobian[i]);
    }
    free(jacobian);
    free(Y);
}
void SGD(double *W, int **oneHotVector, int *EgitimKumesi, int uniqueKelimeSayisi, FILE *dosyaD,FILE *TimeB) {
    int i, j, iter = 1;
    double eps = 0.1;
    double fx, mse=0, old_mse = 0;
    int maxIter = 2000;
    double *Y = (double *)malloc(160 * sizeof(double));
    int k,l;

    for (i = 0; i < 80; i++) {
        Y[i] = 1;
    }
    for (i = 80; i < 160; i++) {
        Y[i] = -1;
    }

    double HataDegerleri[2];
    double jacobian_t_r; 
    double **jacobian = (double **)calloc(2, sizeof(double *));
    for (i = 0; i < 2; i++) {
        jacobian[i] = (double *)calloc(uniqueKelimeSayisi, sizeof(double));
    }

    int a;

    do {
    	clock_t start_time = clock();

        for (i = 0; i < 2; i++) {
            a = rand() % 160;
            fx = 0;

            for (j = 0; j < uniqueKelimeSayisi; j++) {
                fx = fx + W[j] * oneHotVector[EgitimKumesi[a]][j];
            }

            HataDegerleri[i] = Y[a] - tanh(fx);

            for (j = 0; j < uniqueKelimeSayisi; j++) {
                jacobian[i][j] = -oneHotVector[EgitimKumesi[a]][j] * (1 - pow(tanh(fx), 2));
            }
        }

        for (i = 0; i < uniqueKelimeSayisi; i++) {
            jacobian_t_r = 0;
            for (j = 0; j < 2; j++) {
                jacobian_t_r = jacobian_t_r + jacobian[j][i] * HataDegerleri[j];
            }
            W[i] = W[i] - eps * jacobian_t_r;
        }
        old_mse=mse;
        mse=0;
        for(k=0; k<160; k++) {
        	fx=0;
        	for(l=0; l<uniqueKelimeSayisi; l++) {
        		fx = fx + W[l] * oneHotVector[EgitimKumesi[k]][l];
			}
			mse = mse + pow(Y[k]-tanh(fx),2);
		}
		mse=mse/160;
        fprintf(dosyaD, "%f\n", mse);
        iter++;
        clock_t end_time = clock();
        double elapsed_time = elapsed_time + ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
        fprintf(TimeB, "%f\n", elapsed_time);
    } while (iter < maxIter);
    for (i = 0; i < 2; i++) {
        free(jacobian[i]);
    }
    free(jacobian);
    free(Y);
}




void ADAM(double *ADAM_W, int **oneHotVector, int *EgitimKumesi, int uniqueKelimeSayisi, FILE *dosyaE,FILE *TimeC) {
    int i, j, k, l;
    double beta1 = 0.9;
    double beta2 = 0.99;
    double eps = 0.1;
    int iter = 1;
    int maxIter = 2000;
    double fx = 0;
    double alpha = 0.1;
    double mse = 0, old_mse=0;
    double *Y = (double *)malloc(160 * sizeof(double));
    float mt = 0, vt = 0; 
    double e = 0.000001;
    double mt_hot =0, vt_hot=0;

    for (i = 0; i < 80; i++) {
        Y[i] = 1;
    }
    for (i = 80; i < 160; i++) {
        Y[i] = -1;
    }

    do {
    	clock_t start_time = clock();
        i = rand() % 160;
        fx = 0;
        for (j = 0; j < uniqueKelimeSayisi; j++) {
            fx = fx + ADAM_W[j] * oneHotVector[EgitimKumesi[i]][j];
        }

        for (j = 0; j < uniqueKelimeSayisi; j++) {
            double gradient = (-1) * oneHotVector[EgitimKumesi[i]][j] * pow(cosh(fx), -2) * (Y[i] - tanh(fx));
            mt = beta1 * mt + (1 - beta1) * gradient;
            vt = beta2 * vt + (1 - beta2) * gradient*gradient;
            mt_hot = (1 - pow(beta1, iter));
            vt_hot = (1 - pow(beta2, iter));
            
            ADAM_W[j] = ADAM_W[j] - alpha * (mt / mt_hot) / (sqrt((vt / (vt_hot) + e)));
        }
		old_mse=mse;
        mse = 0;
        for (j = 0; j < 160; j++) {
            fx = 0;
            for (l = 0; l < uniqueKelimeSayisi; l++) {
                fx = fx + ADAM_W[l] * oneHotVector[EgitimKumesi[j]][l];
            }
            mse = mse + pow(Y[j] - tanh(fx), 2);
        }
        mse = mse / 160;

        fprintf(dosyaE, "%f\n", (mse));
        //printf("ADAM mse: %f | iterasyon: %d\n", mse, iter);
        iter++;
        clock_t end_time = clock();
        double elapsed_time = elapsed_time + ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
        fprintf(TimeC, "%f\n", elapsed_time);
    }while (iter < maxIter);
    free(Y);
}

int main() {
	int i, j;
	char **uniqueKelimeler;
	uniqueKelimeler = (char **)calloc(MAX_WORDS,sizeof(char *));
	for (i = 0; i < MAX_WORDS; i++) {
		uniqueKelimeler[i] = (char *)calloc(20,sizeof(char));
	}

	int **oneHotVector;
	int uniqueKelimeSayisi = 0;

	srand((unsigned int)time(NULL));

	const char dosyaAdiA[] = "MetinA.txt";
	const char dosyaAdiB[] = "MetinB.txt";

	FILE *dosyaA = fopen(dosyaAdiA, "r");
	FILE *dosyaB = fopen(dosyaAdiB, "r");

	if (dosyaA == NULL || dosyaB == NULL) {
		perror("Dosya açma hatası");
		return 1;
	}

	dosyadanSozluge(dosyaA, uniqueKelimeler, &uniqueKelimeSayisi);
	dosyadanSozluge(dosyaB, uniqueKelimeler, &uniqueKelimeSayisi);
	for(i=0; i<uniqueKelimeSayisi; i++) {
		printf("i:%d %s\n",i+1,uniqueKelimeler[i]);
	}


	fclose(dosyaA);
	fclose(dosyaB);

	FILE *dosyaMetinA = fopen(dosyaAdiA, "r");
	FILE *dosyaMetinB = fopen(dosyaAdiB, "r");

	if (dosyaMetinA == NULL || dosyaMetinB == NULL) {
		perror("Dosya açma hatası");
		return 1;
	}
	int sentenceIndex = 0;
	char metin[MAX_CHARACTERS];

	oneHotVector = (int **)calloc(MAX_SENTENCES,sizeof(int *));
	for (i = 0; i < MAX_SENTENCES; i++) {
		oneHotVector[i] = (int *)calloc(uniqueKelimeSayisi,sizeof(int));
	}

	while (fgets(metin, sizeof(metin), dosyaMetinA) != NULL && sentenceIndex < MAX_SENTENCES) {
		oneHotVectorOlustur(uniqueKelimeler, uniqueKelimeSayisi, metin, oneHotVector, sentenceIndex);
		sentenceIndex++;
	}

	while (fgets(metin, sizeof(metin), dosyaMetinB) != NULL && sentenceIndex < MAX_SENTENCES) {
		oneHotVectorOlustur(uniqueKelimeler, uniqueKelimeSayisi, metin, oneHotVector, sentenceIndex);
		sentenceIndex++;
	}

	fclose(dosyaMetinA);
	fclose(dosyaMetinB);

	//int TestKumesi[40]; //Test kumemiz icin, kalanlar egitim kumemiz olacak.
	int *TestKumesi;
	TestKumesi = (int *)calloc(40,sizeof(int));
	int count=0;
	int randomnumber;
	while(count <40) {
		if(count<20) {
			randomnumber = (rand() %100) ;
		} else {
			randomnumber = (rand() %100)+100 ;
		}
		if(isValueInArray(randomnumber, TestKumesi,count) == 0) {
			TestKumesi[count]=randomnumber;
			count++;
		}
	}
	/*TestKumesi[0...19] da metinA nin test kumesi.
	TestKumesi[20...39] da metinB nin test kumesi.
	EgitimKumesi[0....79] da metinA nin egitim kumesi.
	EgitimKumesi[80...159] da metinB nin egitim kumesi.*/
	count=0;
	int flag=1;
	int *EgitimKumesi;
	EgitimKumesi= (int *)calloc(160,sizeof(int));
	for(i=0; i<200; i++) {
		for(j=0; j<40; j++) {
			if(i==TestKumesi[j]) {
				flag=0;
				break;
			}
		}
		if(flag==1) {
			EgitimKumesi[count]=i;
			count++;
		} else {
			flag=1;
		}
	}
	double *GD_W;
	double *SGD_W;
	double *ADAM_W;
	GD_W=(double *)malloc(uniqueKelimeSayisi*sizeof(double));
	SGD_W=(double *)malloc(uniqueKelimeSayisi*sizeof(double));
	ADAM_W=(double *)calloc(uniqueKelimeSayisi,sizeof(double));
	for(i=0; i<uniqueKelimeSayisi; i++) {
		GD_W[i] = generateRandomRationalNumber();
		SGD_W[i] = GD_W[i];
		ADAM_W[i] = GD_W[i];
	}
	FILE *dosyaC;
	FILE *dosyaD;
	FILE *dosyaE;
	FILE *TimeA;
	FILE *TimeB;
	FILE *TimeC;
	dosyaC = fopen("GDerror.txt","w");
	dosyaD = fopen("SGDerror.txt","w");
	dosyaE = fopen("ADAMerror.txt","w");
	TimeA = fopen("GDtime.txt","w");
	TimeB = fopen("SGDtime.txt","w");
	TimeC = fopen("ADAMtime.txt","w");
	
	printf("--- GD --- \n");
	GD(GD_W,oneHotVector,EgitimKumesi,uniqueKelimeSayisi,dosyaC,TimeA);

	for(i=0; i<40; i++) {
		double result = 0;
		for(j=0; j<uniqueKelimeSayisi; j++) {
			result += GD_W[j] * oneHotVector[TestKumesi[i]][j];
		}
			printf("Cumle: %d | i:%d | %f\n",TestKumesi[i],i,tanh(result));
	}
	
	printf("--- SGD --- \n");
	SGD(SGD_W,oneHotVector,EgitimKumesi,uniqueKelimeSayisi,dosyaD,TimeB);
	for(i=0; i<40; i++) {
		double result = 0;
		for(j=0; j<uniqueKelimeSayisi; j++) {
			result += SGD_W[j] * oneHotVector[TestKumesi[i]][j];
		}
			printf("Cumle: %d | i:%d | %f\n",TestKumesi[i],i,tanh(result));
	}
	printf("--- ADAM --- \n");
	ADAM(ADAM_W,oneHotVector,EgitimKumesi,uniqueKelimeSayisi,dosyaE,TimeC);
	for(i=0; i<40; i++) {
		double result = 0;
		for(j=0; j<uniqueKelimeSayisi; j++) {
			result += ADAM_W[j] * oneHotVector[TestKumesi[i]][j];
		}
		printf("Cumle: %d | i:%d | %f\n",TestKumesi[i],i,tanh(result));
	}
	
	for (i = 0; i < uniqueKelimeSayisi; i++) {
		free(uniqueKelimeler[i]);
	}

	for (i = 0; i < MAX_SENTENCES; i++) {
		free(oneHotVector[i]);
	}
	free(uniqueKelimeler);
	free(oneHotVector);
	free(GD_W);
	free(SGD_W);
	free(ADAM_W);
	fclose(dosyaC);
	fclose(dosyaD);
	fclose(TimeA);
	fclose(TimeB);
	fclose(TimeC);
	return 0;
}
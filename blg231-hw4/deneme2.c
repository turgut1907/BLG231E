#include <stdio.h>
#include <math.h>

struct person{
	int personId;
	double height;
	double weight;
	double BMI;
};

int main(){
	
	int N=0,M=0;
	double threshold=0.0;
	int i=0,j=0,x,y; //caunter
	
	printf("Enter N: ");
	scanf("%d",&N);
	while(N>1729){
		printf("Enter N: ");
		scanf("%d",&N);
	}
	printf("Enter M: ");
	scanf("%d",&M);
	while(M>N){
		printf("Enter M: ");
		scanf("%d",&M);
	}
	printf("Enter threshold:");
	scanf("%lf",&threshold);
	
	
	struct person P[N];
	struct person temp;
	FILE *ptfile;
	FILE *ptfile2;
	if ((ptfile=fopen("height_weight.txt","r"))==NULL){
		printf("file not found");
		return 0;
	}
	
	fseek(ptfile,33,0); //go to the second line with fseek function
	
	
	while(i<N && !feof(ptfile)){
	fscanf(ptfile,"%d%lf%lf",&P[i].personId,&P[i].height,&P[i].weight);
	P[i].BMI=P[i].weight/((P[i].height/100)*(P[i].height/100));
	i++;
	}
	
	fclose(ptfile);
	
	for(y=0;y<N;y++){ //mutlak degere gore diz
		for(x=0;x<N;x++){
		if((fabs(threshold-P[x].BMI)) < (fabs(threshold-P[x+1].BMI))){ 
			temp=P[x];
			P[x]=P[x+1];
			P[x+1]=temp;
		}
		
		}	
		
	}
	
	for(y=0;y<M;y++){ //bmi ya gore diz
		for(x=0;x<M;x++){
		if((P[x].BMI) < (P[x+1].BMI)){ 
			temp=P[x];
			P[x]=P[x+1];
			P[x+1]=temp;
		}
		
		}	
		
	}
	

	
	printf("Person_id  Height(cm)  Weight(kg)  BMI\n");
	while(j<M){
		printf("%9d %11.0lf %11.0lf %6.2lf",P[j].personId,P[j].height,P[j].weight,P[j].BMI);
		//puts("");
		puts("");
		j++;
	}
	
	if ((ptfile2=fopen("output.txt","w"))==NULL){
		printf("file not found");
		return 0;
	}
	
	j=0;
	fprintf(ptfile2,"Person_id  Height(cm)  Weight(kg)  BMI\n");
	while(j<M){
		fprintf(ptfile2,"%9d %11.0lf %11.0lf %6.2lf\n",P[j].personId,P[j].height,P[j].weight,P[j].BMI);
		j++;
	}
	fclose(ptfile2);
	return 0;
	
}





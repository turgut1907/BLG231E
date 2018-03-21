// TURGUT CAN AYDINALEV 150120021 HW4

#include <stdio.h>
#include <math.h>
#include <stdlib.h>

struct person{
    int personId;
    double height;
    double weight;
    double BMI;
};



int main(int argc, char* argv[])
{
    double temp=0;
    double thresh;
    
    int bayrak=1;
    int counter=1;
    int id;
    char t1[20],t2[20],t3[20];
    
    int N;
    int M;
    
    N = atoi(argv[1]);  // derleyiciden alma kýsmý
    M = atoi(argv[2]);
    thresh = atoi(argv[3]);

    struct person per[N+1];
    
    
    double fark[N+1];
    double topm[M+1];
    int topid[M+1];
    
    FILE *fPtr;
    
    if((fPtr=fopen("height_weight.txt", "r")) == NULL)
    {
        printf("Can not open the file\n");
        return 0;
    }
    
    else
    {
        fscanf(fPtr,"%s%s%s", t1,t2,t3); // ustteki basliklari okuma
        
        while( !feof(fPtr) )
        {
            fscanf(fPtr,"%d%lf%lf", &per[counter].personId, &per[counter].height, &per[counter].weight); //structa dosyadakileri kayýt etme
            
            counter++;
            
            if (counter == (N+1))
                break;  // counterý 1 den baþlattýk id numaralarý karýþmasýn diye dolayýsý ile 1 boyut fazla
        }
       	}
    
    for(counter = 1; (counter <= N); counter++)
    {
        fark[counter] = fabs(thresh - (per[counter].weight / ((per[counter].height/100.00) * (per[counter].height/100.00))));
		per[counter].BMI = (per[counter].weight / ((per[counter].height/100.00) * (per[counter].height/100.00)));	 // fark hesabý
    }
    
    while ( bayrak <= M) // M kadar en buyukleri sýralama
    {
        for (counter=1; counter <=N;counter++)
        {
            if (fark[counter] > temp)
            {
                temp = fark[counter];  
                id = counter;
            }
        }
        
        topid[bayrak] = id;
        topm[bayrak] = temp;
        
        temp = 0;
        fark[id] = 0;
        id = 0;
        
        bayrak++;
    }
    
    counter=1;
    
    if((fPtr=fopen("output.txt", "w")) == NULL)     
    {
        printf("Can not open the file\n");
        return 0;
    }
    
    fprintf(fPtr,"ID\tHeight\tWeight\tBMI\tMMI-TH\n");
    
    while(counter <= M)  //dosyaya yazma
    {
        fprintf(fPtr,"%d\t%.0f\t%.0lf\t%.2lf\t%.2lf\n",topid[counter],per[topid[counter]].height,per[topid[counter]].weight,per[topid[counter]].BMI,topm[counter] );
        counter++;
    }
    
    fclose(fPtr);
    return 0;
}



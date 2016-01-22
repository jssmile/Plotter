#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
  char lineSize[10]; 
  char *token;
  int count=0;

  FILE *fp, *out1, *out2; 
  fp=fopen("input.txt","r"); 
  out1=fopen("output_x.txt","w+t");
  out2=fopen("output_y.txt","w+t");


  if(fp!=NULL){        
        while(fgets(lineSize,10,fp))
        { 
             //fetch the x, y data
             token = strtok(lineSize," "); 
             int x=atoi(token);
             token = strtok(NULL,"\n");
             int y=atoi(token);

             //x-axis
             fprintf(out1, "Moveto(%d);\n", x);
             if(abs(x)<=abs(y))
             {
                printf("Delay(32*%d);\n", (abs(y)-abs(x)));
                fprintf(out1, "Delay(32*%d);\n", (abs(y)-abs(x)));
             }


             
             //y-axis
             fprintf(out2, "Moveto(%d);\n", y);
             if(abs(y)<=abs(x))
             {
                printf("Delay(32*%d);\n", (abs(x)-abs(y)));
                fprintf(out2, "Delay(32*%d);\n", (abs(x)-abs(y)));
             }
 
             count++;  
        } 
   }
  else{
      printf("fail to read file");
  } 
  fclose(fp); 
  fclose(out1);
  fclose(out2);  
}

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
             if(abs(x) > 4 || abs(y) > 4)
             fprintf(out1, "pen_up();");

             fprintf(out1, "Moveto(%d);", x);
             if(abs(x) > 4 || abs(y) > 4)
              fprintf(out1, "pen_down();");


             if(abs(x)<=abs(y))
             {
                //printf("Delay(32*%d);", (abs(y)-abs(x)));
                fprintf(out1, "Delay(32*%d);", (abs(y)-abs(x)));
             }
             fprintf(out1, "\n");

             
             //y-axis
             if(abs(x) > 4 || abs(y) > 4)
             fprintf(out2, "pen_up();");

             fprintf(out2, "Moveto(%d);", y);

             if(abs(x) > 4 || abs(y) > 4)
              fprintf(out2, "pen_down();");

             if(abs(y)<=abs(x))
             {
                //printf("Delay(32*%d);", (abs(x)-abs(y)));
                fprintf(out2, "Delay(32*%d);", (abs(x)-abs(y)));
             }
             fprintf(out2, "\n");
             count++;  
        }
        fprintf(out1, "pen_up();");
        fprintf(out2, "pen_up();"); 
   }
  else{
      printf("fail to read file");
  } 
  fclose(fp); 
  fclose(out1);
  fclose(out2);  
}

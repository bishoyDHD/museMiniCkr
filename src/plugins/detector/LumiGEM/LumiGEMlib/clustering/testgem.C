#include "cGEM.cxx"
#include "cAPV.cxx"

void testgem()
{
  cAPV *myapv =new cAPV();
  cGEM *mygem[4];

   int data[128];

   int n[4];
   int m[4];
   int *x[4];
   int *y[4];
   

  
for(int i=0;i<128;i++)
    for(int j=0;j<128;j++)
   {
     if(myapv->wordid2sample(j)==i)
       data[j]=i;
   };



for(int i=1;i<=3;i++)
{
 
  mygem[i] = new cGEM(4);

  mygem[i]->AddAPVy(4*i-4,1,0); //0,4,8
  mygem[i]->AddAPVy(4*i-3,1,0); //1,5,9
  mygem[i]->AddAPVx(4*i-2,1,0); //2,6,10
  mygem[i]->AddAPVx(4*i-1,1,0); //3,7,11

  mygem[i]->AddAPVData(4*i-4,data);
  mygem[i]->AddAPVData(4*i-3,data);
  mygem[i]->AddAPVData(4*i-2,data);
  mygem[i]->AddAPVData(4*i-1,data);


     n[i]= mygem[i]->PushYdata();
     y[i] = mygem[i]->GetYdata();
     m[i]= mygem[i]->PushXdata();
     x[i] = mygem[i]->GetXdata();
  
}

  
 

 
  /*
  
  mygem1->AddAPVy(0,1,0);
  mygem1->AddAPVy(1,1,0);
  mygem1->AddAPVx(2,1,0);
  mygem1->AddAPVx(3,1,0); 
  

  mygem2->AddAPVy(4,1,0);
  mygem2->AddAPVy(5,1,0);
  mygem2->AddAPVx(6,1,0);
  mygem2->AddAPVx(7,1,0); 

  mygem3->AddAPVy(8,1,0);
  mygem3->AddAPVy(9,1,0);
  mygem3->AddAPVx(10,1,0);
  mygem3->AddAPVx(11,1,0); 
  
   */



  //for(int i=0;i<128;i++)
  // data[i] = i;
    
  
 
  /*
  mygem1->AddAPVData(0,data);
  mygem1->AddAPVData(1,data);
  mygem1->AddAPVData(2,data);
  mygem1->AddAPVData(3,data);


  mygem2->AddAPVData(4,data);
  mygem2->AddAPVData(5,data);
  mygem2->AddAPVData(6,data);
  mygem2->AddAPVData(7,data);


  mygem3->AddAPVData(8,data);
  mygem3->AddAPVData(9,data);
  mygem3->AddAPVData(10,data);
  mygem3->AddAPVData(11,data);

  */

/*
      int n1= mygem[1]->PushYdata();
      int *y1 = mygem[1]->GetYdata();
      int m1= mygem[1]->PushXdata();
      int *x1 = mygem[1]->GetXdata();

      int n2= mygem[2]->PushYdata();
      int *y2 = mygem[2]->GetYdata();
      int m2= mygem[2]->PushXdata();
      int *x2 = mygem[2]->GetXdata();

      int n3= mygem[3]->PushYdata();
      int *y3 = mygem[3]->GetYdata();
      int m3= mygem[3]->PushXdata();
      int *x3 = mygem[3]->GetXdata();
    
*/  

 for(int t=1;t<=3;t++)

   {

printf("\n GEM[%d] APV Samples on Y axis\n", t);
 for (int i=0; i<n[t]; i++)
   {  printf("%d ", y[t][i]); };

 
 printf("\n GEM[%d]  APV Samples on X axis\n",t);
 for (int k=0; k<m[t]; k++)
   {  printf("%d ", x[t][k]); };

   }


/*
 


printf("\n GEM[%d] GEM APV Samples on Y axis\n", i);
 for (int i=0; i<n[1]; i++)
   {  printf("%d ", y[1][i]); };

 
 printf("\n US GEM APV Samples on X axis\n");
 for (int k=0; k<m[1]; k++)
   {  printf("%d ", x[1][k]); };

printf("\n MI GEM APV Samples on Y axis\n");
 for (int i=0; i<n[2]; i++)
   {  printf("%d ", y[2][i]); };

 
 printf("\n MI GEM APV Samples on X axis\n");
 for (int k=0; k<m[2]; k++)
   {  printf("%d ", x[2][k]); };

printf("\n DS GEM APV Samples on Y axis\n");
 for (int i=0; i<n[3]; i++)
   {  printf("%d ", y[3][i]); };

 
 printf("\n DS GEM APV Samples on X axis\n");
 for (int k=0; k<m[3]; k++)
   {  printf("%d ", x[3][k]); };
    
*/
    
 
 //printf("\n n %d values\n", n);
 //printf("\n m %d values\n", m);
 
}

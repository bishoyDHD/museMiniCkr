#include "cGEM.cxx"
#include "cAPV.cxx"

void testgem2()
{
  cGEM *mygem = new cGEM(4);
  cAPV *myapv =new cAPV();
 
 

  mygem->AddAPVy(0,1,0);
  mygem->AddAPVy(1,1,0);
  mygem->AddAPVx(2,1,0);
  mygem->AddAPVx(3,1,0); 


   int data[128];
  //for(int i=0;i<128;i++)
  // data[i] = i;
    
  for(int i=0;i<128;i++)
    for(int j=0;j<128;j++)
   {
     if(myapv->wordid2sample(j)==i)
       data[j]=i;
     
};
 

  mygem->AddAPVData(0,data);
  mygem->AddAPVData(1,data);

  mygem->AddAPVData(2,data);
  mygem->AddAPVData(3,data);


   int n= mygem->PushYdata();
  int *y = mygem->GetYdata();
  
 printf("\n APV Samples on Y axis\n");
   for (int i=0; i<n; i++)
  {  printf("%d ", y[i]); };

   
   

int n= mygem->PushXdata();
  int *x = mygem->GetXdata();
  
 printf("\n APV Samples on X axis\n");
   for (int k=0; k<n; k++)
  {  printf("%d ", x[k]); };

 
 //printf("\n n %d values\n", n);
 //printf("\n m %d values\n", m);
 
}

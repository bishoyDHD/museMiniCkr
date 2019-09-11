#include "cAPV.cxx"

void testapv()
{
  myapv = new cAPV();
  myapv->SkipFirst(2);
  myapv->SkipLast(3);
  myapv->SkipSample(27);
  int a[6]={9,10,11,12,13,14};
  myapv->SkipSamples(a,6);
  for (int i=0; i<128; i++)
    {
      myapv->AddData(i, i, false);
    };

  int result[1000];
  int n = myapv->GetData(result);
  for (int i=0; i<n; i++)
    printf("%d ", result[i]);
  printf("\n%d values\n", n);
}

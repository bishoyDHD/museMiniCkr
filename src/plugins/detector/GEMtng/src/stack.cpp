#include "stack.h"
#include <cmath>

void stack::dH_raw()
{
  for (int i=0;i<2;i++)
    planes[i]->dH_raw();
}

void stack::dH_processed()
{
  for (int i=0;i<2;i++)
    planes[i]->dH_processed();
}

void stack::dH_sum()
{
  for (int i=0;i<2;i++)
    planes[i]->dH_sum();
}

void stack::dH_1dhits()
{
  for (int i=0;i<2;i++)
    planes[i]->dH_1dhits();
}

void stack::dH_2dhits()
{
  h_hitscore2d=plugin->dH2((prefix+"/hitscore2D").c_str(),"Hitscores X/Y",70,5,12,70,5,12);
  h_hitscore1d=plugin->dH1((prefix+"/hitscore1D").c_str(),"Hitscores X-Y",400,-20,20);
  h_striphits2d=plugin->dH2((prefix+"/hits2D").c_str(),"Hit strips X/Y",250,-0.5,249.5,250,-0.5,249.5);
  h_striphits2dbig=plugin->dH2((prefix+"/hits2D_bigsum").c_str(),"Hit strips X/Y",250,-0.5,249.5,250,-0.5,249.5);
}



void stack::analyze(unsigned int * rawdata)
{
  hits.clear();
  for (int i=0;i<2;i++)
    planes[i]->analyze(rawdata);

    for (unsigned int u=0;u<planes[0]->hits.size();u++)
      for (unsigned int v=0;v<planes[1]->hits.size();v++)
	{
	  double hitscorediff=planes[0]->hits[u].hitscore-planes[1]->hits[v].hitscore;
	  double hitscoresum=planes[0]->hits[u].hitscore+planes[1]->hits[v].hitscore;
	  if (h_hitscore2d)
	    {
	      h_hitscore2d->Fill(planes[0]->hits[u].hitscore,planes[1]->hits[v].hitscore);
	      h_hitscore1d->Fill(hitscorediff);
	    }

	  if (fabs(hitscorediff)<1.5)
	    {
	      if (h_hitscore2d)   h_striphits2d->Fill(planes[0]->hits[u].strip,planes[1]->hits[v].strip);
	    if (hitscoresum>15)
	      {
	     	  if (h_hitscore2d) h_striphits2dbig->Fill(planes[0]->hits[u].strip,planes[1]->hits[v].strip);
		  hits.push_back(stackhit(planes[0]->hits[u].strip,planes[1]->hits[v].strip));
		  
	      }
	    }
	}


}




void stack::finalize()
{
  for (int i=0;i<2;i++)
    planes[i]->finalize();
}

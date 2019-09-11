#include "telescope.h"
#include "TH1.h"


void telescope::dH_allRaw()
{
  h_alldata=plugin->dH1((prefix+"/alldata").c_str(),"All data",1560,-0.5,1559.5);
}




void telescope::dH_planeRaw()
{
  for (int i=0;i<3;i++)
    stacks[i]->dH_raw();
}

void telescope::dH_planeProcessed()
{
  for (int i=0;i<3;i++)
    stacks[i]->dH_processed();
}

void telescope::dH_planeSum()
{
  for (int i=0;i<3;i++)
    stacks[i]->dH_sum();
}

void telescope::dH_planeHits()
{
  for (int i=0;i<3;i++)
    stacks[i]->dH_1dhits();
}

void telescope::dH_stackHits()
{
  for (int i=0;i<3;i++)
    stacks[i]->dH_2dhits();
}

void telescope::dH_dxdy()
{
  h_dx=plugin->dH1((prefix+"/dx").c_str(),"dx of track",1000,-500,500);
  h_dy=plugin->dH1((prefix+"/dy").c_str(),"dy of track",1000,-500,500);
  h_dxdy=plugin->dH2((prefix+"/dxdy").c_str(),"dx/dy of track",100,-50,50,100,-50,50);
  h_projected=plugin->dH2((prefix+"/accepted/projected").c_str(),"Projected intersection of track",100,-100,100,100,-100,100);
  h_acceptedU=plugin->dH2((prefix+"/accepted/U").c_str(),"Upstream hits",251,-50.2,50.2,251,-50.2,50.2);
  h_acceptedM=plugin->dH2((prefix+"/accepted/M").c_str(),"Middle hits",251,-50.2,50.2,251,-50.2,50.2);
  h_acceptedD=plugin->dH2((prefix+"/accepted/D").c_str(),"Downstream hits",251,-50.2,50.2,251,-50.2,50.2);
}

void telescope::analyze(unsigned int * rawdata)
{
  raw=rawdata;
  // fill alldata raw histos if defined 
  if (h_alldata)
    for (int s=0;s<12*130;s++)
      h_alldata->SetBinContent(s+1,raw[s]);

  //call all stacks
  for (int i=0;i<3;i++)
    stacks[i]->analyze(rawdata);

  //iterate through all hits of a telescope

  tracks.clear();

  if (stacks[0]->hits.size()*stacks[1]->hits.size()*stacks[2]->hits.size()>1000)
    return;


  for (unsigned int u=0;u<stacks[0]->hits.size();u++)
    {
      double x1= stacks[0]->hits[u].stripx*0.4-50;
      double y1= stacks[0]->hits[u].stripy*0.4-50;
    for (unsigned int v=0;v<stacks[1]->hits.size();v++)
      {
	double x2= stacks[1]->hits[v].stripx*0.4-50;
	double y2= stacks[1]->hits[v].stripy*0.4-50;
      for (unsigned int w=0;w<stacks[2]->hits.size();w++)
	{
	  double x3= stacks[2]->hits[w].stripx*0.4-50;
	  double y3= 50-stacks[2]->hits[w].stripy*0.4;


	  double dx=x2-(dist_MD*x1+dist_UM*x3)/(dist_UM+dist_MD);
	  double dy=y2-(dist_MD*y1+dist_UM*y3)/(dist_UM+dist_MD);

	  if (dx*dx+dy*dy<5)
	    {
	      //these are the tracks we accept. 
	      //calculate track parameters
	      double ds=plane*3+dist_UM*2+dist_MD;
	      double dss=plane*plane +(dist_UM+plane)*(dist_UM+plane)+(dist_UM+dist_MD+plane)*(dist_UM+dist_MD+plane);
	      double dx=x1+x2+x3;
	      double dxs=x1*x1+x2*x2+x3*x3;
	      double dy=y1+y2+y3;
	      double dys=y1*y1+y2*y2+y3*y3;
	      double mdx=plane*x1+(plane+dist_UM)*x2+(plane+dist_UM+dist_MD)*x3;
	      double mdy=plane*y1+(plane+dist_UM)*y2+(plane+dist_UM+dist_MD)*y3;
	      double bx=(mdx-ds*dx/3)/(dss-ds*ds/3);
	      double by=(mdy-ds*dy/3)/(dss-ds*ds/3);
	      double ax=(dx-bx*ds)/3;
	      double ay=(dy-by*ds)/3;
	      if (h_projected)
		{
		  h_projected->Fill(ax,ay);
		  h_acceptedU->Fill(x1,y1);
		  h_acceptedM->Fill(x2,y2);
		  h_acceptedD->Fill(x3,y3);
		}
	      tracks.push_back(gemtrack(ax,ay,bx,by));

	    }

	  if (h_dx)
	    {
	    h_dx->Fill(dx);
	    h_dy->Fill(dy);
	    h_dxdy->Fill(dx,dy);
	    }
	  

	}
    }
}

}

void telescope::finalize()
{
  for (int i=0;i<3;i++)
    stacks[i]->finalize();
}

#include <Det_SlowCtrl.h>

#include<iostream>
#include<cmath>
#include"genericmap.h"

Det_SlowCtrl::Det_SlowCtrl(TTree *in, TTree *out,TFile *inf_, TFile * outf_, TObject *p):Plugin(in,out,inf_,outf_,p)
{
};

Det_SlowCtrl::~Det_SlowCtrl()
{
};


Long_t Det_SlowCtrl::cmdline(char *cmd)
{
  //add cmdline handling here

  return 0; // 0 = all ok
};


Long_t Det_SlowCtrl::init()
{
  index2name=(MRTIndex2Name *) getFileObject ("TableData/SCIndex2Name");
  if (!index2name)
    {
      debug(0,"SLOWCTRL: Could not find index map\n");
      //return -1;
      data = NULL;
      return 1;
    }
  for ( std::map<unsigned short, std::string>::iterator iter=index2name->index2name.begin();iter!=index2name->index2name.end();iter++)
    {
    debug(100,"Index to name %i %s\n",iter->first,iter->second.c_str());
    manager.add(iter->second,iter->first);
    }

  //process initial state
  data=(MRTSlowData *) getFileObject("TableData/SCInitialState");

  dataout=new MRTSlowData();
  makeBranch("SlowData",(TObject **) &dataout);

  getBranchObject("SlowData",(TObject **) &data);


  copyFileObject("TableData/SCIndex2Name");
  //  copyBranch("SlowData");

  setMemoryObject("SlowCtrl Manager",(TObject *) &manager);
  nextevent=0;

  lastEventResult=(int *) getMemoryObject("LastEventResult");

  return 0;
}


void Det_SlowCtrl::internal_process()
{
  for (std::vector<MRTSlowData::slowStruct>::iterator iter=data->slowData.begin();iter!=data->slowData.end();iter++)
    {
      dataout->slowData.push_back(*iter);   //we accumulate

      debug(200,"Updating index %i with value %em\n",iter->index,iter->value);
      slowctrl::datum *up;
      up=manager.getCurrentByID(iter->index);
      up->value=iter->value;
      up->status=iter->status;
      up->timestamp=iter->time;
      manager.updateWatches(up);
      if (iter->status>=0)
	{
	  up=manager.getLastValidByID(iter->index);
	  up->value=iter->value;
	  up->status=iter->status;
	  up->timestamp=iter->time;
	  manager.updateWatches(up);
      	}
    }
}




Long_t Det_SlowCtrl::process()
{
  if (data==NULL) return 1;
  if (*lastEventResult & Plugin::redo) 
    {
      // same as before, so we do nothing except clearing the output
      dataout->slowData.clear();
      return Plugin::ok;
    }

  if( *lastEventResult !=Plugin::skip) 
      dataout->slowData.clear(); // we saved, so we can kill the buffer

  int curevent=  in->GetReadEntry();
  if (nextevent!=  curevent  )  // we skipped some events
    {
      debug(100,"We skipped saving for events %i to %i ... will rewind and reread\n",nextevent,curevent);
      std::vector<MRTSlowData::slowStruct> temp;
      for (int i=nextevent;i<=curevent;i++)
	{
	  
	  in->GetEntry(i);
	  internal_process();
	}
      
    }
  else
    internal_process();
  
  nextevent=in->GetReadEntry()+1;
  
  return Plugin::ok;
}



extern "C"{
Plugin *factory(TTree *in, TTree *out,TFile *inf_, TFile * outf_, TObject *p)
{
  return (Plugin *) new Det_SlowCtrl(in,out,inf_,outf_,p);
}
}


ClassImp(Det_SlowCtrl);


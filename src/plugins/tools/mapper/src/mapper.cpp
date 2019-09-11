#include <mapper.h>

#include <iostream>
#include <cmath>

#include <functional>
typedef  std::multimap<unsigned int,std::function<void(trbhit)>> functrbhitmap;
typedef  std::multimap<unsigned int,std::function<void(double)>> funcdoublemap;
typedef  std::multimap<unsigned int,std::function<void(v1190hit)>> funcv1190hitmap;
typedef  std::multimap<unsigned int,std::function<void(v1290hit)>> funcv1290hitmap;
typedef  std::multimap<unsigned int,std::function<void(short)>> funcshortmap;



mapper::mapper(TTree *in, TTree *out,TFile *inf_, TFile * outf_, TObject *p):Plugin(in,out,inf_,outf_,p)
{


  in_gems=NULL;

  for(int i=0; i<TRB_num; i++)in_TRB[i]=NULL; //loop over all TRB3 board in the system ! TRB_num is defined in "mapper.h"
  for(int i=0; i<TRB_num; i++)in_TBL[i]=NULL; //loop over all TRB3 board in the system ! TRB_num is defined in "mapper.h"
  for(int i=0; i<TRB_num; i++)in_TLA[i]=NULL; //loop over all TRB3 board in the system ! TRB_num is defined in "mapper.h"
  for(int i=0; i<TRB_num; i++)in_TRA[i]=NULL; //loop over all TRB3 board in the system ! TRB_num is defined in "mapper.h"
  for(int i=0; i<TRB_num; i++)in_TSR[i]=NULL; //loop over all TRB3 board in the system ! TRB_num is defined in "mapper.h"
  for(int i=0; i<TRB_num; i++)in_TSL[i]=NULL; //loop over all TRB3 board in the system ! TRB_num is defined in "mapper.h"

  scifi=NULL;
  stt = NULL;
  trbinfo=NULL;
  trbtdcmap=(void *) new functrbhitmap();

  in_v1190=NULL;
  v1190info=NULL;
  v1190map=(void *) new funcv1190hitmap();

  in_v1290=NULL;
  v1290info=NULL;
  v1290map=(void *) new funcv1290hitmap();

  in_v792=NULL;
  v792info=NULL;
  v792map=(void *) new funcshortmap();
  
  in_mqdc=NULL;
  mqdcinfo=NULL;
  mqdcmap=(void *) new funcshortmap();

// 03/15/17 Multiple MQDC readout:
  for(int i=0; i<QDC_num; i++) {
    in_qbl[i]=NULL; 
    qdcinfo[i] = NULL;
    in_qla[i]=NULL; 
    in_qra[i]=NULL; 
    qblmap[i]=(void *) new funcshortmap();
    qlamap[i]=(void *) new funcshortmap();
    qramap[i]=(void *) new funcshortmap();

  }

//  qdcmap=(void *) new funcshortmap();
// ------------------- End -------------------------//


  tbt=NULL;
  BH=NULL;
  SPS=NULL;
  VETO=NULL;
  PbGlass=NULL;
  BM=NULL;
  trb_tdc_debug = NULL;

  SLOWCTRL_names=NULL;
  SLOWCTRL_updates=NULL;
  slowdata=NULL;
};

mapper::~mapper()
{
};


Long_t mapper::startup_slowctrl()
{

  getBranchObject("SCIC",(TObject **) &SLOWCTRL_names); //Id channel mapping
  getBranchObject("SCCC",(TObject **) &SLOWCTRL_updates);

  slowdata=new MRTSlowData();
  makeBranch("SlowData",(TObject **) &slowdata);

    return Plugin::ok;
}

Long_t mapper::startup_TRB3debug()
{

  trbinfo=new TRB3();
  makeBranch("TRB3",(TObject **) &trbinfo);
    
  return Plugin::ok;

}


Long_t mapper::startup_SciFi() //looping over all TRB3 banks starting from TRB1 and un to TRB"n";
{

  for(int i=0; i<TRB_num; i++) { 
    if (!in_TRB[i]) getBranchObject(Form("TRB%i",i),(TObject**) &in_TRB[i]);
  }

  scifi=new SciFi();
  makeBranch("SciFi",(TObject **) &scifi);

  return Plugin::ok;
};

// Ievgen 03/06/2018: Additing TRB_TDC_Debug debuging branch.
Long_t mapper::startup_trb_tdc_debug()
{

  for(int i=0; i<TRB_num; i++) {
    if (!in_TRB[i]) getBranchObject(Form("TRB%i",i),(TObject**) &in_TRB[i]);
  }

  trb_tdc_debug=new TRB_TDC_Board();
  makeBranch("trb_tdc_debug",(TObject **) &trb_tdc_debug);

  return Plugin::ok;
};





Long_t mapper::startup_StrawTube()
{

  // for(int i=0; i<TRB_num; i++) { 
  //   if (!in_TRB[i]) getBranchObject(Form("TRB%i",i),(TObject**) &in_TRB[i]);
  // }


  for(int i=0; i<TRB_num; i++) { 
    if (!in_TSR[i]) getBranchObject(Form("TSR%i",i),(TObject**) &in_TSR[i]);
  }

  for(int i=0; i<TRB_num; i++) { 
    if (!in_TSL[i]) getBranchObject(Form("TSL%i",i),(TObject**) &in_TSL[i]);
  }

  stt=new StrawTube();
  makeBranch("StrawTube",(TObject **) &stt);

  return Plugin::ok;
};


Long_t mapper::startup_GEM()
{
  //We try to get GEM1 and GEM2 branches.

  // GEM0,1 -> GEM0,1

  getBranchObject("GEM1",(TObject**)&in_gems);  
  // getBranchObject("G_US",(TObject**)&in_gems[0]);
  // getBranchObject("G_4t",(TObject**)&in_gems[1]);
  // getBranchObject("G_MI",(TObject**)&in_gems[2]);
  // getBranchObject("G_DS",(TObject**)&in_gems[3]);


  if (in_gems) makeBranch("GEM1", (TObject **)&in_gems);

  // if (in_gems[0]) makeBranch("G_US", (TObject **)&in_gems[0]);
  // if (in_gems[1]) makeBranch("G_4t", (TObject **)&in_gems[1]);
  // if (in_gems[2]) makeBranch("G_MI", (TObject **)&in_gems[2]);
  // if (in_gems[3]) makeBranch("G_DS", (TObject **)&in_gems[3]);
    
  return Plugin::ok;
}


Long_t mapper::startup_V1190debug()
{
  v1190info=new v1190();
  makeBranch("V1190",(TObject **) &v1190info);
  if (!in_v1190)
      getBranchObject("TDC0",(TObject **)&in_v1190);

  if (!in_v1190)
    debug(0,"Warning: Did not find V1190 branch\n");
    
    return Plugin::ok;


}


Long_t mapper::startup_V1290debug()
{
  v1290info=new v1290();
  makeBranch("V1290",(TObject **) &v1290info);
  if (!in_v1290)
      getBranchObject("TDC1",(TObject **)&in_v1290);

  if (!in_v1290)
    debug(0,"Warning: Did not find V1290 branch\n");

    
    return Plugin::ok;

}


Long_t mapper::startup_V792debug()
{
  v792info=new v792();
  makeBranch("V792",(TObject **) &v792info);
  if (!in_v792)
      getBranchObject("ADC0",(TObject **)&in_v792);

  if (!in_v792)
    debug(0,"Warning: Did not find V792 branch\n");

    
    return Plugin::ok;

}


Long_t mapper::startup_MQDCdebug()
{
  mqdcinfo=new mqdc();
  makeBranch("MQDC",(TObject **) &mqdcinfo);
  if (!in_mqdc)
      getBranchObject("MQDC",(TObject **)&in_mqdc);

  if (!in_mqdc)
    debug(0,"Warning: Did not find MQDC branch\n");
    
    return Plugin::ok;

}

// 03/15/17 Multiple MQDC readput:
Long_t mapper::startup_mult_MQDCdebug()
{
  for(int i=0; i<QDC_num; i++){ 
    qdcinfo[i]=new mqdc();
    makeBranch(Form("QDC%i",i),(TObject **) &qdcinfo[i]);

    if (!in_qbl[i])
        getBranchObject(Form("QBL%i",i),(TObject **)&in_qbl[i]);
    if (!in_qbl[i])
       debug(0,"Warning: Did not find QDC%i branch\n", i);
    if (!in_qla[i])
        getBranchObject(Form("QLA%i",i),(TObject **)&in_qla[i]);
    if (!in_qla[i])
       debug(0,"Warning: Did not find QLA%i branch\n", i);
    if (!in_qra[i])
        getBranchObject(Form("QRA%i",i),(TObject **)&in_qra[i]);
    if (!in_qra[i])
       debug(0,"Warning: Did not find QRA%i branch\n", i);

  }
    return Plugin::ok;

}
// ------------------- End -------------------------//




Long_t mapper::startup_mappedchannels()
{
  if (!in_v1190)
      getBranchObject("TDC0",(TObject **)&in_v1190);
  
  if (!in_v1290)
      getBranchObject("TDC1",(TObject **)&in_v1290);

  if (!in_v792)
      getBranchObject("ADC0",(TObject **)&in_v792);
  
  if (!in_mqdc)
      getBranchObject("MQDC",(TObject **)&in_mqdc);

  for(int i=0; i<TRB_num; i++) { 
    if (!in_TRB[i]) getBranchObject(Form("TRB%i",i),(TObject**) &in_TRB[i]);
  }

  for(int i=0; i<TRB_num; i++) {
    if (!in_TBL[i]) getBranchObject(Form("TBL%i",i),(TObject**) &in_TBL[i]);
//    if (!in_TBL[i]) debug(0,"Warning: Did not find TBL%i branch\n", i);
  }
  for(int i=0; i<TRB_num; i++) {
    if (!in_TLA[i]) getBranchObject(Form("TLA%i",i),(TObject**) &in_TLA[i]);
  }



// 03/15/17 Multiple MQDC branches: --------------------//
  for(int i=0; i<QDC_num; i++) {
    if (!in_qbl[i]) getBranchObject(Form("QBL%i",i),(TObject**) &in_qbl[i]);
    if (!in_qra[i]) getBranchObject(Form("QRA%i",i),(TObject**) &in_qra[i]);
    if (!in_qla[i]) getBranchObject(Form("QLA%i",i),(TObject**) &in_qla[i]);

  }
// ----------------------------------------------------//
  
  tbt=new mappedchannels();
  makeBranch("mappedchannels",(TObject **) &tbt);

  return Plugin::ok;
};

Long_t mapper::startup_BH()
{
  if (!in_v1190)
      getBranchObject("TDC0",(TObject **)&in_v1190);
  
  if (!in_v1290)
      getBranchObject("TDC1",(TObject **)&in_v1290);

  if (!in_v792)
      getBranchObject("ADC0",(TObject **)&in_v792);
  
  if (!in_mqdc)
      getBranchObject("MQDC",(TObject **)&in_mqdc);

// 03/15/17 Multiple MQDC branches: --------------------//
  for(int i=0; i<QDC_num; i++) {
    if (!in_qbl[i]) getBranchObject(Form("QBL%i",i),(TObject**) &in_qbl[i]);
  }
// ----------------------------------------------------//

  for(int i=0; i<TRB_num; i++) { 
    if (!in_TBL[i]) getBranchObject(Form("TBL%i",i),(TObject**) &in_TBL[i]);
  }
  
  BH=new BHraw();
  makeBranch("BH",(TObject **) &BH);

  return Plugin::ok;
};

Long_t mapper::startup_VETO()
{

  if (!in_mqdc)
      getBranchObject("MQDC",(TObject **)&in_mqdc);

// 03/15/17 Multiple MQDC branches: --------------------//
  for(int i=0; i<QDC_num; i++) {
    if (!in_qbl[i]) getBranchObject(Form("QBL%i",i),(TObject**) &in_qbl[i]);
  }
// ----------------------------------------------------//

  for(int i=0; i<TRB_num; i++) { 
    if (!in_TBL[i]) getBranchObject(Form("TBL%i",i),(TObject**) &in_TBL[i]);
  }
  
  VETO=new VETOraw();
  makeBranch("VETO",(TObject **) &VETO);

  return Plugin::ok;
}

Long_t mapper::startup_SPS()
{
  if (!in_v1190)
      getBranchObject("TDC0",(TObject **)&in_v1190);
  
  if (!in_v1290)
      getBranchObject("TDC1",(TObject **)&in_v1290);

  if (!in_v792)
      getBranchObject("ADC0",(TObject **)&in_v792);
  
  if (!in_mqdc)
      getBranchObject("MQDC",(TObject **)&in_mqdc);

// 03/15/17 Multiple MQDC branches: --------------------//
  for(int i=0; i<QDC_num; i++) {
    if (!in_qla[i]) getBranchObject(Form("QLA%i",i),(TObject**) &in_qla[i]);
  }
// ----------------------------------------------------//

  // 03/15/17 Multiple MQDC branches: --------------------//
  for(int i=0; i<QDC_num; i++) {
    if (!in_qra[i]) getBranchObject(Form("QRA%i",i),(TObject**) &in_qra[i]);
  }
// ----------------------------------------------------//

  for(int i=0; i<TRB_num; i++) { 
    if (!in_TLA[i]) getBranchObject(Form("TLA%i",i),(TObject**) &in_TLA[i]);
  }

  for(int i=0; i<TRB_num; i++) { 
    if (!in_TRA[i]) getBranchObject(Form("TRA%i",i),(TObject**) &in_TRA[i]);
  }
  
  SPS=new SPSraw();
  makeBranch("SPS",(TObject **) &SPS);

  return Plugin::ok;
};

Long_t mapper::startup_BM()
{
  if (!in_mqdc)
      getBranchObject("MQDC",(TObject **)&in_mqdc);

// 03/15/17 Multiple MQDC branches: --------------------//
  for(int i=0; i<QDC_num; i++) {
    if (!in_qbl[i]) getBranchObject(Form("QBL%i",i),(TObject**) &in_qbl[i]);
  }
// ----------------------------------------------------//

  for(int i=0; i<TRB_num; i++) { 
    if (!in_TBL[i]) getBranchObject(Form("TBL%i",i),(TObject**) &in_TBL[i]);
  }
  
  BM=new BMraw();
  makeBranch("BM",(TObject **) &BM);

  return Plugin::ok;
};

Long_t mapper::startup_PbGlass()
{
  if (!in_v1290)
      getBranchObject("TDC1",(TObject **)&in_v1290);
  
  if (!in_mqdc)
      getBranchObject("MQDC",(TObject **)&in_mqdc);

// 03/15/17 Multiple MQDC branches: --------------------//
  for(int i=0; i<QDC_num; i++) {
    if (!in_qdc[i]) getBranchObject(Form("QDC%i",i),(TObject**) &in_qdc[i]);
  }
// ----------------------------------------------------//

  for(int i=0; i<TRB_num; i++) { 
    if (!in_TRB[i]) getBranchObject(Form("TRB%i",i),(TObject**) &in_TRB[i]);
  }
  
  PbGlass=new PbGlassraw();
  makeBranch("PbGlass",(TObject **) &PbGlass);

  return Plugin::ok;
};


void mapper::processSlowCtrl()
{
  if (SLOWCTRL_names)
    {
      debug(0,"SlowCtrl: Found names database size %i\n", SLOWCTRL_names->size);
      MRTIndex2Name names;
      int size=0;
      char * data=(char *)SLOWCTRL_names->data;
      while(size<SLOWCTRL_names->size)
        {
         int index=*(int *) data;
         size+=4;
         data+=4;
         names.index2name[index]=data;
         size+=strlen(data)+1;
         data+=strlen(data)+1;
         debug(1,"SlowCtrl: mapping %i -> %s\n",index,names.index2name[index].c_str());
        }

      outfile->mkdir("TableData");
      outfile->cd("TableData");
      names.Write("SCIndex2Name");
      outfile->cd();
      SLOWCTRL_names=NULL; //only do it once
    }

  if (SLOWCTRL_updates)
    {
      debug(1,"SlowCtrl: Found updates size %i\n",SLOWCTRL_updates->size);

      int size=0;
      char * data=(char *)SLOWCTRL_updates->data;

      while(size<SLOWCTRL_updates->size)
        {

          MRTSlowData::slowStruct upd;
          upd.index=(unsigned short) (*(int *) data);
          size+=4;
          data+=4;
          upd.time=*(unsigned long long *)data;
          size+=8;
          data+=8;
          upd.value=*(double *)data;
          size+=8;
          data+=8;
          upd.status=*(unsigned short *)data;
          size+=2;
          data+=2;
          debug(2,"SlowCtrl: update id %i to %g %i at time %lli\n",upd.index,upd.value,upd.status,upd.time);
          slowdata->slowData.push_back(upd);
        }



    }

}


Long_t mapper::scanTRB()
{
  processTRB(false);
  for (auto&& it:trbtdchits)
    {
      if (trbtdccalibtab[it.id]==NULL)
	trbtdccalibtab[it.id]=new double[2048](); // () guarantees that it's emptied;

      trbtdccalibtab[it.id][it.fine]+=1;
      
    }

    return Plugin::ok;


}

void mapper::processTRB(bool saveerrors)
{
  trbtdchits.clear();
  for(int trb=0; trb<TRB_num; trb++) { 
//this need to ne written in a nice way:\
// it is just a fast solution:
     if (in_TBL[trb])
      {
        int size=in_TBL[trb]->size/4;
        unsigned int *d=(unsigned int *)in_TBL[trb]->data;
        //      for (int i=0;i<size;i++)
        //     debug(0,"%x\n",d[i]);
        unsigned int *last=d+size-1; //skip end marker
        if (size<5){
            debug(1,"TBL bank too small\n");
            return;
          }
        d+=2;
        int size2=*d;
        d+=2;
 
        d+=2;
        while(d<last)   {
          unsigned int head=*d>>29;
          unsigned int bsize=*d>>16;
          unsigned int bank=*d & 0xffff;
          if (head!=0)
            {
              debug(1,"TBL confused %x %x %x %x\n",head,*d,size,size2/4);
              return;
            }
          d++;

          debug(100,"TBL bank %x size %x\n",bank,bsize);
          unsigned int epoch=0xffffffff;
          unsigned int coarse=0;
          unsigned int edge=0;
          unsigned int error=0;
          unsigned int debugerror=0;

          unsigned int debugmode=0;

          unsigned int fine=0;
          unsigned int channel;
          for (unsigned int i=0;i<bsize;i++,d++) {
            //printf("data word: 0x%x\n",*d);

            switch(*d >>29) {

              case 1: // Additional header
                error = (*d & 0xffff);
                if (error)
                  debug(100,"TBL has header with error %x in bank %x\n",error,bank);
                  break;
              case 2: // Debug word
                debugerror=*d & 0xffffff;

                debugmode=(*d>>24) & 0x1f;
                if (debugerror)
                  {
                    debug(1,"TBL has debug code %x, mode %x in bank %x\n",debugerror, debugmode,bank);
                    if (trbinfo && saveerrors)
                    trbinfo->errors.push_back(debugerror);
                  }
              case 3:   // Epoch
                epoch=*d & 0xffffff;
                break;
              case 4: // time data
                coarse=*d & 0x7ff;
                edge=(*d>> 11) & 0x01;
                fine=(*d>>12) &0x3ff;
                channel=(*d>>22) &0x7f;
                debug(100,"TBL %x, channel %i, epoch %i, coarse %i, fine %i, edge %i\n",bank,channel,epoch,coarse,fine,edge);
                unsigned int id=channel | (bank<<16);
                trbtdchits.emplace_back(id,edge,epoch,coarse,fine);
                break;
            }
          }
        }
      }
/*
  } //end of for loop

    trbtdchits.clear();
    for(int trb=0; trb<TRB_num; trb++) {
//this need to ne written in a nice way:\
// it is just a fast solution
*/
      if (in_TRB[trb])
      {
        int size=in_TRB[trb]->size/4;      
        unsigned int *d=(unsigned int *)in_TRB[trb]->data;
        //      for (int i=0;i<size;i++)
        //     debug(0,"%x\n",d[i]);
        unsigned int *last=d+size-1; //skip end marker
        if (size<5){
	    debug(1,"TRB bank too small\n");
	    return ;
	  }
        d+=2;
        int size2=*d;
        d+=2;

        d+=2;
        while(d<last)	{
	  unsigned int head=*d>>29;
	  unsigned int bsize=*d>>16;
	  unsigned int bank=*d & 0xffff;
	  if (head!=0)
	    {
	      debug(1,"TRB confused %x %x %x %x\n",head,*d,size,size2/4);
	      return;
	    }
	  d++;

          debug(100,"TRB bank %x size %x\n",bank,bsize);
          unsigned int epoch=0xffffffff;
          unsigned int coarse=0;
          unsigned int edge=0;
          unsigned int error=0;
          unsigned int debugerror=0;

	  unsigned int debugmode=0;

          unsigned int fine=0;
          unsigned int channel;
          for (unsigned int i=0;i<bsize;i++,d++) {
            //printf("data word: 0x%x\n",*d);

            switch(*d >>29) {

              case 1: // Additional header
                error = (*d & 0xffff);
                if (error)
                  debug(100,"TRB has header with error %x in bank %x\n",error,bank);
                  break;
              case 2: // Debug word
                debugerror=*d & 0xffffff;

                debugmode=(*d>>24) & 0x1f;
                if (debugerror)
                  {
                    debug(1,"TRB has debug code %x, mode %x in bank %x\n",debugerror, debugmode,bank);
                    if (trbinfo && saveerrors)
                    trbinfo->errors.push_back(debugerror);
                  }
              case 3:   // Epoch
                epoch=*d & 0xffffff;
                break;
              case 4: // time data
                coarse=*d & 0x7ff;
                edge=(*d>> 11) & 0x01;
                fine=(*d>>12) &0x3ff;
                channel=(*d>>22) &0x7f;
                debug(100,"TRB %x, channel %i, epoch %i, coarse %i, fine %i, edge %i\n",bank,channel,epoch,coarse,fine,edge);
                unsigned int id=channel | (bank<<16);
                trbtdchits.emplace_back(id,edge,epoch,coarse,fine);
                break;
            }
          }
        }
    }


      if (in_TLA[trb])
      {
        int size=in_TLA[trb]->size/4;      
        unsigned int *d=(unsigned int *)in_TLA[trb]->data;
        //      for (int i=0;i<size;i++)
        //     debug(0,"%x\n",d[i]);
        unsigned int *last=d+size-1; //skip end marker
        if (size<5){
      debug(1,"TLA bank too small\n");
      return ;
    }
        d+=2;
        int size2=*d;
        d+=2;

        d+=2;
        while(d<last) {
    unsigned int head=*d>>29;
    unsigned int bsize=*d>>16;
    unsigned int bank=*d & 0xffff;
    if (head!=0)
      {
        debug(1,"TLA confused %x %x %x %x\n",head,*d,size,size2/4);
        return;
      }
    d++;

          debug(100,"TLA bank %x size %x\n",bank,bsize);
          unsigned int epoch=0xffffffff;
          unsigned int coarse=0;
          unsigned int edge=0;
          unsigned int error=0;
          unsigned int debugerror=0;

    unsigned int debugmode=0;

          unsigned int fine=0;
          unsigned int channel;
          for (unsigned int i=0;i<bsize;i++,d++) {
            //printf("data word: 0x%x\n",*d);

            switch(*d >>29) {

              case 1: // Additional header
                error = (*d & 0xffff);
                if (error)
                  debug(100,"TRB has header with error %x in bank %x\n",error,bank);
                  break;
              case 2: // Debug word
                debugerror=*d & 0xffffff;

                debugmode=(*d>>24) & 0x1f;
                if (debugerror)
                  {
                    debug(1,"TRB has debug code %x, mode %x in bank %x\n",debugerror, debugmode,bank);
                    if (trbinfo && saveerrors)
                    trbinfo->errors.push_back(debugerror);
                  }
              case 3:   // Epoch
                epoch=*d & 0xffffff;
                break;
              case 4: // time data
                coarse=*d & 0x7ff;
                edge=(*d>> 11) & 0x01;
                fine=(*d>>12) &0x3ff;
                channel=(*d>>22) &0x7f;
                debug(100,"TRB %x, channel %i, epoch %i, coarse %i, fine %i, edge %i\n",bank,channel,epoch,coarse,fine,edge);
                unsigned int id=channel | (bank<<16);
                trbtdchits.emplace_back(id,edge,epoch,coarse,fine);
                break;
            }
          }
        }
    }

 if (in_TSL[trb])
      {
        int size=in_TSL[trb]->size/4;      
        unsigned int *d=(unsigned int *)in_TSL[trb]->data;
        //      for (int i=0;i<size;i++)
        //     debug(0,"%x\n",d[i]);
        unsigned int *last=d+size-1; //skip end marker
        if (size<5){
      debug(1,"TSL bank too small\n");
      return ;
    }
        d+=2;
        int size2=*d;
        d+=2;

        d+=2;
        while(d<last) {
    unsigned int head=*d>>29;
    unsigned int bsize=*d>>16;
    unsigned int bank=*d & 0xffff;
    if (head!=0)
      {
        debug(1,"TSL confused %x %x %x %x\n",head,*d,size,size2/4);
        return;
      }
    d++;

          debug(100,"TSL bank %x size %x\n",bank,bsize);
          unsigned int epoch=0xffffffff;
          unsigned int coarse=0;
          unsigned int edge=0;
          unsigned int error=0;
          unsigned int debugerror=0;

    unsigned int debugmode=0;

          unsigned int fine=0;
          unsigned int channel;
          for (unsigned int i=0;i<bsize;i++,d++) {
            //printf("data word: 0x%x\n",*d);

            switch(*d >>29) {

              case 1: // Additional header
                error = (*d & 0xffff);
                if (error)
                  debug(100,"TSL has header with error %x in bank %x\n",error,bank);
                  break;
              case 2: // Debug word
                debugerror=*d & 0xffffff;

                debugmode=(*d>>24) & 0x1f;
                if (debugerror)
                  {
                    debug(1,"TSL has debug code %x, mode %x in bank %x\n",debugerror, debugmode,bank);
                    if (trbinfo && saveerrors)
                    trbinfo->errors.push_back(debugerror);
                  }
              case 3:   // Epoch
                epoch=*d & 0xffffff;
                break;
              case 4: // time data
                coarse=*d & 0x7ff;
                edge=(*d>> 11) & 0x01;
                fine=(*d>>12) &0x3ff;
                channel=(*d>>22) &0x7f;
                debug(100,"TSL %x, channel %i, epoch %i, coarse %i, fine %i, edge %i\n",bank,channel,epoch,coarse,fine,edge);
                unsigned int id=channel | (bank<<16);
                trbtdchits.emplace_back(id,edge,epoch,coarse,fine);
                break;
            }
          }
        }
    }


 if (in_TSR[trb])
      {
        int size=in_TSR[trb]->size/4;      
        unsigned int *d=(unsigned int *)in_TSR[trb]->data;
        //      for (int i=0;i<size;i++)
        //     debug(0,"%x\n",d[i]);
        unsigned int *last=d+size-1; //skip end marker
        if (size<5){
      debug(1,"TSR bank too small\n");
      return ;
    }
        d+=2;
        int size2=*d;
        d+=2;

        d+=2;
        while(d<last) {
    unsigned int head=*d>>29;
    unsigned int bsize=*d>>16;
    unsigned int bank=*d & 0xffff;
    if (head!=0)
      {
        debug(1,"TSR confused %x %x %x %x\n",head,*d,size,size2/4);
        return;
      }
    d++;

          debug(100,"TSR bank %x size %x\n",bank,bsize);
          unsigned int epoch=0xffffffff;
          unsigned int coarse=0;
          unsigned int edge=0;
          unsigned int error=0;
          unsigned int debugerror=0;

    unsigned int debugmode=0;

          unsigned int fine=0;
          unsigned int channel;
          for (unsigned int i=0;i<bsize;i++,d++) {
            //printf("data word: 0x%x\n",*d);

            switch(*d >>29) {

              case 1: // Additional header
                error = (*d & 0xffff);
                if (error)
                  debug(100,"TSR has header with error %x in bank %x\n",error,bank);
                  break;
              case 2: // Debug word
                debugerror=*d & 0xffffff;

                debugmode=(*d>>24) & 0x1f;
                if (debugerror)
                  {
                    debug(1,"TSR has debug code %x, mode %x in bank %x\n",debugerror, debugmode,bank);
                    if (trbinfo && saveerrors)
                    trbinfo->errors.push_back(debugerror);
                  }
              case 3:   // Epoch
                epoch=*d & 0xffffff;
                break;
              case 4: // time data
                coarse=*d & 0x7ff;
                edge=(*d>> 11) & 0x01;
                fine=(*d>>12) &0x3ff;
                channel=(*d>>22) &0x7f;
                debug(100,"TSR %x, channel %i, epoch %i, coarse %i, fine %i, edge %i\n",bank,channel,epoch,coarse,fine,edge);
                unsigned int id=channel | (bank<<16);
                trbtdchits.emplace_back(id,edge,epoch,coarse,fine);
                break;
            }
          }
        }
    }




  } // end of the for loop

}


Long_t mapper::calibrateTRB()
{

  for (auto && it:trbtdccalibtab)
    {
      // first, find the sum;
      double sum=0;
      for (int i=0;i<2048;i++)
	sum+=it.second[i];
      // now, integrate
      double rsum=0;
      for (int i=0;i<2048;i++)
	{
	  double current=it.second[i];
	  it.second[i]=(rsum+current/2)/sum;
	  rsum+=current;	  
	}
    }

  return Plugin::ok;
}


void mapper::processV1190(const char * name, MRTBinaryBlob *in,v1190 * info, std::vector<v1190hit_w_channel> & hits)
{
  hits.clear();
  if (info)
    info->clear();
  if (!in) return;
  int size=in->size/4;      
  unsigned int *d=(unsigned int *)in->data;
  unsigned int *end=d+size;
  if (size<1)
    return;
       
  //printf("Scanning through %i events in v1190...\n",size);
  //for (int j=0;j<size;j++) {
  //  printf("v1190 Data[%i]: 0x%x\n", j, d[j]);
  //}
  
if ((*d & 0xf8000000) != 0x40000000)
    {
      debug(0,"%s: Global header not found -- not decoding event\n",name);
      return;
    }
  d++;
  unsigned int current_header=0;
  int index_for_trailer=0;
  int index_for_error=0;
  
  while(d<end)
    {
      switch (*d & 0xf8000000)
	{
	case 0x40000000:
	  debug(0,"%s: Second global header found?!?!?\n",name);
	  break;
	case 0x08000000:
	  debug(1,"%s: TDC header\n",name);
	  current_header=(*d) &0x07ffffff;
	  break;
	case 0x18000000:
	  debug(1,"%s: TDC trailer\n",name);
	  //	  for (unsigned int i=index_for_trailer;i<v1190hits.size();i++)
	  //	    v1190hits[i].hit.trailer=(*d) &0x07ffffff;
	  //	  index_for_trailer=v1190hits.size();
	  break;
	case 0x20000000:
	  debug(1,"%s: TDC Error\n",name);
	  for (unsigned int i=index_for_error;i<hits.size();i++)
	    hits[i].hit.error=(*d) &0x07ffffff;
	  index_for_error=hits.size();
	  break;
	case 0x88000000:
	  debug(1,"%s: Extended Trigger Time Tag\n",name);
	  if( info)
	    info->extendedTriggerTimeTag=(*d ) &0x07fffffff;
	  break;
	case 0x80000000:
	  debug(1,"%s: Trailer Word\n",name);
	  if( info)
	    info->status=((*d) &0x07000000)>>24;
	  break;
	case 0x00000000:
	  hits.emplace_back(((*d >>19)&0x007f),std::move(v1190hit(*d)));
	  break;
      	}
      if ((*d &0xff000000)==0xaa000000)
	{
	  debug(1,"%s: Event Counter\n",name);
	  if( info)
	    info->eventCounter=(*d) &0xffffff;      
	}

      if (*d ==0xbad0cafe)
	  debug(1,"%s: extra hit flag\n",name);


      if ((*d &0xffff0000)==0xdcfe0000)
	{
	  debug(1,"%s: extra hit counts\n",name);
	  if( info)
	    info->extraHits=(*d) & 0x7ff;
	}
      
      d++;
    }
   

}


void mapper::processV1290(const char * name, MRTBinaryBlob *in,v1290 * info, std::vector<v1290hit_w_channel> & hits)
{
  hits.clear();
  if (info)
    info->clear();
  if (!in) return;
  int size=in->size/4;      
  unsigned int *d=(unsigned int *)in->data;
  unsigned int *end=d+size;
  if (size<1)
    return;

  if ((*d & 0xf8000000) != 0x40000000)
    {
      debug(0,"%s: Global header not found -- not decoding event\n",name);
      return;
    }
  d++;
  unsigned int current_header=0;
  int index_for_trailer=0;
  int index_for_error=0;
  
  while(d<end)
    {
      switch (*d & 0xf8000000)
        {
        case 0x40000000:
          debug(0,"%s: Second global header found?!?!?\n",name);
          break;
        case 0x08000000:
          debug(1,"%s: TDC header\n",name);
          current_header=(*d) &0x07ffffff;
          break;
        case 0x18000000:
          debug(1,"%s: TDC trailer\n",name);
          break;
        case 0x20000000:
          debug(1,"%s: TDC Error\n",name);
          for (unsigned int i=index_for_error;i<hits.size();i++)
            hits[i].hit.error=(*d) &0x07ffffff;
          index_for_error=hits.size();
          break;
        case 0x88000000:
          debug(1,"%s: Extended Trigger Time Tag\n",name);
          if( info)
            info->extendedTriggerTimeTag=(*d ) &0x07fffffff;
          break;
        case 0x80000000:
          debug(1,"%s: Trailer Word\n",name);
          if( info)
            info->status=((*d) &0x07000000)>>24;
          break;
        case 0x00000000:
          hits.emplace_back(((*d & 0x03e00000) >> 21),std::move(v1290hit(*d)));
          break;
        }
      if ((*d &0xff000000)==0xaa000000)
        {
          debug(1,"%s: Event Counter\n",name);
          if( info)
            info->eventCounter=(*d) &0xffffff;      
        }

      if (*d ==0xbad0cafe)
          debug(1,"%s: extra hit flag\n",name);


      if ((*d &0xffff0000)==0xdcfe0000)
        {
          debug(1,"%s: extra hit counts\n",name);
          if( info)
            info->extraHits=(*d) & 0x7ff;
        }
      
      d++;
    }
   

}

void mapper::processV792(const char * name, MRTBinaryBlob *in,v792 * info, std::vector<v792hit_w_channel> & hits)
{
  hits.clear();
  if (info)
    info->clear();
  if (!in) return;
  int size=in->size/4;      
  unsigned int *d=(unsigned int *)in->data;
  unsigned int *end=d+size;
  //  unsigned int current_header=0;
  
  while(d<end)
    {
      switch (*d & 0x07000000)
	{

	case 0x07000000:
	  debug(1,"%s: ADC header\n",name);
	  break;
	case 0x04000000:
	  debug(1,"%s: ADC trailer\n",name);
          debug(1,"%s: Event Counter\n",name);
          if( info)
            info->eventCounter=( (*d) & 0xffffff ); 
	  break;

	case 0x00000000:
	  hits.emplace_back(((*d & 0x1f0000)>>16),*d & 0xfff);
	  break;
      	}

      if (*d == 0xbad0cafe)
	  debug(1,"%s: extra hit flag\n",name);


      if ((*d & 0xffff0000)==0xdcfe0000)
	{
	  debug(1,"%s: extra hit counts\n",name);
	  if( info)
	    info->extraHits=(*d) & 0x7ff;
	}
      
      d++;
    }
    

}

void mapper::processMQDC(const char * name, MRTBinaryBlob *in,mqdc * info, std::vector<mqdchit_w_channel> & hits)
{
  hits.clear();
  if (info)
    info->clear();
  if (!in) return;
  int size=in->size/4;      
  unsigned int *d=(unsigned int *)in->data;
  unsigned int *end=d+size;
  int gBitResolution = 0xfff; // 12-bit
  //  unsigned int current_header=0;
    
  while(d<end)
    {
      switch (*d & 0xc0000000)
        {

        case 0x40000000:
          debug(1,"%s: ADC header\n",name);
          break;
        case 0xc0000000:
          debug(1,"%s: ADC trailer\n",name);
          debug(1,"%s: Event Counter\n",name);
          if( info)
            info->eventCounter=( (*d) & 0x3fffffff );
          break;

        case 0x00000000:
          hits.emplace_back( (*d>>16 & 0x1f),(*d & gBitResolution) );
          break;
        }

      if (*d == 0xbad0cafe)
          debug(1,"%s: extra hit flag\n",name);


      if ((*d & 0xffff0000)==0xdcfe0000)
        {
          debug(1,"%s: extra hit counts\n",name);
          if( info)
            info->extraHits=(*d) & 0x7ff;
        }
      
      d++;
    }
    

}


Long_t mapper::map()
{
  // do gems
  /*
  for (int i=0;i<2;i++)
    if (in_gems[i])
      {
	debug(1,"GEM%i: size: %i\n",i,in_gems[i]->size);
	for( int k=0;k<std::min(in_gems[i]->size,10);k++)
	  debug(1,"%i ",in_gems[i]->data[k]);
	debug(1,"\n");
      }
  */

  if(trbinfo)
    trbinfo->clear();

  if (scifi)
    scifi->clear();
  
  if (stt)
    stt->clear();

  if (tbt)
    tbt->clear();
  if(BH)
    BH->clear();
  if(SPS)
    SPS->clear();
  if(PbGlass)
    PbGlass->clear();
  if(BM)
    BM->clear();
  if(VETO)
    VETO->clear();

  if(trb_tdc_debug)
    trb_tdc_debug->clear();

  if (slowdata)
    slowdata->slowData.clear();

  for(int i =0; i < QDC_num; i++)
  {
    qblhits[i].clear();
    qrahits[i].clear();
    qlahits[i].clear();   
  }

  processSlowCtrl();



  // do trb
  processTRB(true);

  trbzeros.clear();

  for (auto && it:trbtdchits)
    if ((it.id & 0xffff)==0) // calibration channel
      {
	double time=CLOCK_CYCLE_LENGTH*(it.epoch*pow(2.0,11) + it.coarse - trbtdccalibtab[it.id][it.fine]);
       	trbzeros[it.id>>16]=time;
      }

  
  for (auto && it:trbtdchits)
    {
      //find mapping

      //      std::pair <std::multimap<unsigned int, trbmapinfo>::iterator, std::multimap<unsigned int, trbmapinfo>::iterator>
      double time=CLOCK_CYCLE_LENGTH*(it.epoch*pow(2.0,11) + it.coarse - trbtdccalibtab[it.id][it.fine]);

      auto map=((functrbhitmap *)trbtdcmap)->equal_range(it.id);
      trbhit temptrbhit;

      if (map.first!=map.second)
        for (auto m=map.first;m!=map.second;++m) {	  
	  //	  debug(0,"For bank %x channel %i, subtraction %g\n",it.id>>16,it.id &0xffff,trbzeros[it.id>>16]);
          temptrbhit.time = time-trbzeros[it.id>>16];
          temptrbhit.rising = it.edge;
          m->second(temptrbhit);
        }
 
      else
        if(trbinfo)
        {
          temptrbhit.time = time-trbzeros[it.id>>16];
          temptrbhit.rising = it.edge;
          trbinfo->unmapped_channels.emplace(it.id,temptrbhit); // dump to collect-all
          debug(1,"Unmapped TRB bank %x channel %i, time: %g calib: %g diff: %g\n",it.id>>16, it.id &0xffff,time, trbzeros[it.id>>16],temptrbhit.time);
        }
      
    }

  processV1190("V1190 (TDC0)", in_v1190,v1190info,v1190hits);
  for (auto && it:v1190hits)
    {
      auto map=((funcv1190hitmap *)v1190map)->equal_range(it.channel);

      if (map.first!=map.second)
	for (auto m=map.first;m!=map.second;++m)
	  m->second(it.hit);
      else
	if (v1190info)
	  {
	    v1190info->unmapped_channels.emplace(it.channel,it.hit);
	    debug(1,"Unmapped v1190 hit, channel %i\n",it.channel);
	  }
    }


    processV1290("V1290 (TDC1)", in_v1290,v1290info,v1290hits);
    for (auto && it:v1290hits)
      {
	auto map=((funcv1290hitmap *)v1290map)->equal_range(it.channel);
        	
	if (map.first!=map.second)
	  for (auto m=map.first;m!=map.second;++m)
	    m->second(it.hit);
	else
	  if (v1290info)
	    {
	      v1290info->unmapped_channels.emplace(it.channel,it.hit);
	      debug(1,"Unmapped v1290 hit, channel %i\n",it.channel);
	    }
      }

    processV792("V792 (ADC0)", in_v792,v792info,v792hits);
    for (auto && it:v792hits)
      {
	auto map=((funcshortmap *)v792map)->equal_range(it.channel);
	
	if (map.first!=map.second)
	  for (auto m=map.first;m!=map.second;++m)
	    m->second(it.value);
	else
	  if (v792info)
	    {
	      v792info->unmapped_channels.emplace(it.channel,it.value);
	      debug(1,"Unmapped v792 hit, channel %i\n",it.channel);
	    }
      }

    processMQDC("MQDC (MQDC)", in_mqdc,mqdcinfo,mqdchits);
    for (auto && it:mqdchits)
      {
        auto map=((funcshortmap *)mqdcmap)->equal_range(it.channel);
        
        if (map.first!=map.second)
          for (auto m=map.first;m!=map.second;++m)
            m->second(it.value);
        else
          if (mqdcinfo)
            {
              mqdcinfo->unmapped_channels.emplace(it.channel,it.value);
              debug(1,"Unmapped MQDC hit, channel %i\n",it.channel);
            }
      }
  
// 03/15/17 Multiple MQDCs readout/mapping: ------------------------//
   for (int i=0; i<QDC_num; i++){
     processMQDC(Form("QBL%i (QBL%i)",i,i), in_qbl[i],qdcinfo[i],qblhits[i]);
     for (auto && it:qblhits[i]){
         auto map=((funcshortmap *)qblmap[i])->equal_range(it.channel);
         if (map.first!=map.second)
           for (auto m=map.first;m!=map.second;++m)
             m->second(it.value);
         else
           if (qdcinfo[i]){
              qdcinfo[i]->unmapped_channels.emplace(it.channel,it.value);
              debug(1,"Unmapped QDC%i  hit, channel %i\n",i, it.channel);
            }
      }
   }
// ---------------- End ---------------------------------------------//

   // 12/08/18 Multiple MQDCs readout/mapping: ------------------------//
   for (int i=0; i<QDC_num; i++){
     processMQDC(Form("QLA%i (QLA%i)",i,i), in_qla[i],qdcinfo[i],qlahits[i]);
     for (auto && it:qlahits[i]){
         auto map=((funcshortmap *)qlamap[i])->equal_range(it.channel);
         if (map.first!=map.second)
           for (auto m=map.first;m!=map.second;++m)
             m->second(it.value);
         else
           if (qdcinfo[i]){
              qdcinfo[i]->unmapped_channels.emplace(it.channel,it.value);
              debug(1,"Unmapped QDC%i  hit, channel %i\n",i, it.channel);
            }
      }
   }
// ---------------- End ---------------------------------------------//

   // 12/08/18 Multiple MQDCs readout/mapping: ------------------------//
   for (int i=0; i<QDC_num; i++){
     processMQDC(Form("QRA%i (QRA%i)",i,i), in_qra[i],qdcinfo[i],qrahits[i]);
     for (auto && it:qrahits[i]){
         auto map=((funcshortmap *)qramap[i])->equal_range(it.channel);
         if (map.first!=map.second)
           for (auto m=map.first;m!=map.second;++m)
             m->second(it.value);
         else
           if (qdcinfo[i]){
              qdcinfo[i]->unmapped_channels.emplace(it.channel,it.value);
              debug(1,"Unmapped QDC%i  hit, channel %i\n",i, it.channel);
            }
      }
   }
// ---------------- End ---------------------------------------------//
  return Plugin::ok;
};


Long_t mapper::addSciFiChannel(int id,int bank,int from,int plane, int side, int to)
 {
   if (bank<0) return Plugin::ok;
   debug(1,"Adding SciFi mapping with id %i, bank 0x%x from ch %i to plane %i side %i to %i\n",id,bank, from, plane, side, to);
     ((functrbhitmap *) trbtdcmap)->emplace((bank<<16)|from,[plane,side,to,this](trbhit hit){scifi->hits[plane][side].emplace(to,std::move(hit));});
   //	trbtdcmap.emplace(from,[it,this](double time){scifi->reference_time.emplace_back(time);});

   return Plugin::ok;
 }

Long_t mapper::addSciFiReferenceClock(int id,int bank,int from)
{
  if (bank<0) 
    {
      debug(1,"ScFi unmapping refclock id %i\n",id);
      return Plugin::ok;
    }
  debug(0,"SciFi mapping refclock: TRB bank 0x%x, ch %i to id %i \n",bank,from,id);
  ((functrbhitmap *) trbtdcmap)->emplace((bank<<16)|from,[this](trbhit hit){scifi->reference_time.emplace_back(std::move(hit));});
  
   return Plugin::ok;
}



//Ievgen 03/06/2018: Added for TRB_TDC_Debuging brach
Long_t mapper::addTRB_TDC_DebugChannel(int tdc_num, int bank)
{
  if (bank<0) {
    debug(1,"TRB_TDC unmapping TRB with bank %x\n",bank);
    return Plugin::ok;

  }

  for(int i=0; i<48; i++){ //loop over all TDC channels in trb3
     debug(0,"TRB_TDC_Debug:  bank 0x%x (%d) is mapped to tdc: %d (%d) \n",bank, i+1, tdc_num, i);

     int to = TRB_TDC_logic_to_internal(tdc_num, i);
    ((functrbhitmap *) trbtdcmap)->emplace((bank<<16)|i+1,[to,this,tdc_num,i](trbhit hit){trb_tdc_debug ->TDC[tdc_num][i].trb_tdc.emplace(to,std::move(hit));});

  }

   return Plugin::ok;
}



Long_t mapper::addStrawChannel(int side,int plane, int straw_in_plane,int bank,int from)
 {
   if (bank<0) 
     {
       debug(1,"STT unmapping %i %i %i\n",side, plane,straw_in_plane);
       return Plugin::ok;
     }
   int to = STT_logic_to_internal(side,plane,straw_in_plane);
   
   debug(1,"STT mapping bank 0x%x  ch %i to id %i (Side %i  plane %i  straw in plane %i)\n",bank,from,to,side,plane,straw_in_plane);
   // if( (plane<0) || (plane>19))
   //   {
   //     debug(0,"Can't map to plane %i \n",plane);
   //     return -1;
   //   }
   ((functrbhitmap *) trbtdcmap)->emplace((bank<<16)|from,[to,this](trbhit hit){stt->straw.emplace(to,std::move(hit));});

   return Plugin::ok;
 }


Long_t mapper::addStrawReferenceClock(int bank,int from)
{
  if (bank<0) 
    {
      debug(1,"STT unmapping refclock\n");
      return Plugin::ok;
    }
  debug(0,"STT mapping refclock bank 0x%x, ch %i\n",bank,from);
   ((functrbhitmap *) trbtdcmap)->emplace((bank<<16)|from,[this](trbhit hit){stt->reference_time.emplace_back(hit);});
  
   return Plugin::ok;
}

Long_t mapper::addTestBeamTDC(char *tgt, int TDCtype, int channel)
{
  if (TDCtype<0)
    return Plugin::ok;

  debug(1,"Adding testbeam TDC mapping with type %i, ch %i to %s\n",TDCtype,channel,tgt);

  
  switch (TDCtype)
    {
    case 0:
      ((funcv1190hitmap *) v1190map)->emplace(channel,[this,tgt](v1190hit hit){tbt->tdc_v1190.emplace(tgt,std::move(hit));});
      break;
    case 1:
      ((funcv1290hitmap *) v1290map)->emplace(channel,[this,tgt](v1290hit hit){tbt->tdc_v1290.emplace(tgt,std::move(hit));});
      break;
    }

  
  return Plugin::ok;
}



Long_t mapper::addTestBeamTRB(char *tgt, int bank, int channel)
{
  if (bank<0) return Plugin::ok;

  debug(1,"Adding testbeam TRB mapping with bank %x, ch %i to %s\n",bank,channel,tgt);

  ((functrbhitmap *) trbtdcmap)->emplace((bank<<16)|channel,[this,tgt](trbhit hit){tbt->tdc_trb.emplace(tgt,std::move(hit));});

  //printf("hit with time %d placed at %x:\n",time,(bank<<16)|channel);
  
  return Plugin::ok;
}


Long_t mapper::addTestBeamADC(char *tgt, int ADCtype, int channel)
{
  if (ADCtype<0) return Plugin::ok;
  debug(1,"Adding testbeam ADC mapping with type %i, ch %i to %s\n",ADCtype,channel,tgt);  
  switch (ADCtype)
    {
    case 0:
      ((funcshortmap *) v792map)->emplace(channel,[this,tgt](short value){tbt->adc_v792[tgt]=value;});
      break;
    case 1:
      ((funcshortmap *) mqdcmap)->emplace(channel,[this,tgt](short value){tbt->adc_mqdc[tgt]=value;});
      break;
    }
  
  return Plugin::ok;
}

Long_t mapper::addBHTDC(int plane, int bar, int side, int TDCtype, int channel)//string name
{
  if (TDCtype<0 || plane<0 || side<0)
    {
      debug(1,"BH unmapping V1%i90 TDC ch %i \n", TDCtype+1,channel); 
      return Plugin::ok;
    }
  debug(0,"BH mapping TDC with type %i, ch %i, plane %i, side %i\n",TDCtype,channel,plane,side);
  switch (TDCtype)
    {
    case 0:
      debug(0,"Don't use the V1190 with BHs!!");
      break;
    case 1:
      int to = BH_logic_to_internal(plane,bar,side);
      ((funcv1290hitmap *) v1290map)->emplace(channel,[to,this,plane,bar,side](v1290hit hit){BH->plane[plane][bar].tdc_v1290[side].emplace(to,std::move(hit));});
      break;
    }
  return Plugin::ok;
}

Long_t mapper::addVETOReferenceClock(int id, int TDCtype,int channel)//If you are using a TRB for the ref clock replace TDCtype with the bank
{
  if (TDCtype<0)
    {
      debug(1,"VETO unmapping refclock with TDCtype %x, channel %i\n",TDCtype,channel);
     return Plugin::ok;
    }
  debug(0,"VETO mapping refclock with TDCtype %x, channel %i\n",TDCtype,channel);
  ((functrbhitmap *) trbtdcmap)->emplace((TDCtype<<16)|channel,[this](trbhit hit){VETO->trb_reftime.emplace_back(hit);});
  return Plugin::ok;
}

Long_t mapper::addVETOTrigger(int id,int bank,int channel)
{
  if (bank<0)
    {
      debug(1,"VETO unmapping trigger from bank %x, channel %i\n",bank,channel);
     return Plugin::ok;
    }
  debug(0,"VETO mapping trigger to bank %x, channel %i\n",bank,channel);

  ((functrbhitmap *) trbtdcmap)->emplace((bank<<16)|channel,[this](trbhit hit){VETO->trig.emplace_back(hit);});
  return Plugin::ok;
}

Long_t mapper::addVETO_MQDC(int bar, int QDC_bank, int channel)
{
  if (QDC_bank<0 || bar<0 ){
    debug(1,"VETO unmapping MQDC:  QDC%i, ch %i, bar %i\n",QDC_bank,channel,bar);
    return Plugin::ok;
  }
    debug(0,"VETO mapping MQDC:  QDC%i, ch %i, bar %i\n",QDC_bank,channel,bar);

 ((funcshortmap *) qblmap[QDC_bank])->emplace(channel,[this,bar](short value){VETO->plane[bar].adc_mqdc.emplace_back(value);});
 
  return Plugin::ok;
}

Long_t mapper::addVETOTRB(int bar, int bank, int channel)
{
  if (bank<0 ||bar<0){
    debug(1,"VETO unmapping TRB with bank %x, ch %i, bar %i\n",bank,channel,bar);   
    return Plugin::ok;
  }
  debug(0,"VETO mapping TRB with bank %x, ch %i, bar %i\n",bank,channel,bar);
  int to = bar;
  ((functrbhitmap *) trbtdcmap)->emplace((bank<<16)|channel,[to,this,bar](trbhit hit){VETO->plane[bar].tdc_trb.emplace(to,std::move(hit));});
  return Plugin::ok;
}

Long_t mapper::addBHReferenceClock(int id,int TDCtype,int channel)//If you are using a TRB for the ref clock replace TDCtype with the bank
{
  if (TDCtype<0)
    {
      debug(1,"BH unmapping refclock with TDCtype %x, channel %i\n",TDCtype,channel);
     return Plugin::ok;
    }
  debug(0,"BH mapping refclock with TDCtype %x, channel %i\n",TDCtype,channel);
  switch (TDCtype)
    {
    case 0:
      debug(0,"Don't use the V1190 with BHs!!");
      break;
    case 1:
      debug(0,"V1290 Ref");
      ((funcv1290hitmap *) v1290map)->emplace(channel,[this](v1290hit hit){BH->reftime.emplace_back(hit);});
      break;
    default:
      debug(0,"TRB ref");
      ((functrbhitmap *) trbtdcmap)->emplace((TDCtype<<16)|channel,[this](trbhit hit){BH->trb_reftime.emplace_back(hit);});
      break;
    }
  return Plugin::ok;
}

Long_t mapper::addBHTrigger(int id,int bank,int channel)
{
  if (bank<0)
    {
      debug(1,"BH unmapping trigger from bank %x, channel %i\n",bank,channel);
     return Plugin::ok;
    }
  debug(0,"BH mapping trigger to bank %x, channel %i\n",bank,channel);

  ((functrbhitmap *) trbtdcmap)->emplace((bank<<16)|channel,[this](trbhit hit){BH->trig.emplace_back(hit);});
  return Plugin::ok;
}

Long_t mapper::addBHADC(int plane, int bar, int side, int ADCtype, int channel)
{
  if (ADCtype<0 || plane<0 || side<0){
    debug(1,"BH unmapping ADC with type %i, ch %i, plane %i, side %i\n",ADCtype,channel,plane,side);
    return Plugin::ok;
  }
  debug(0,"BH mapping ADC with type %i, ch %i, plane %i, side %i\n",ADCtype,channel,plane,side);


  switch (ADCtype)
    {
    case 0:
      debug(0,"DON'T USE THE v792 WITH THE SIPM");
        //((funcshortmap *) v792map)->emplace(channel,[this,plane,side](short value){BH->plane[plane].adc_v792[side]=value;});
      break;
    case 1:
      ((funcshortmap *) mqdcmap)->emplace(channel,[this,plane,bar,side](short value){BH->plane[plane][bar].adc_mqdc[side].emplace_back(value);});
      break;
    }
  return Plugin::ok;
}

// 03/15/17 BH mapper for multiple MQDCs:
Long_t mapper::addBH_MQDC(int plane, int bar, int side, int QDC_bank, int channel)
{
  if (QDC_bank<0 || plane<0 || side<0){
    debug(1,"BH unmapping MQDC:  QDC%i, ch %i, plane %i, side %i\n",QDC_bank,channel,plane,side);
    return Plugin::ok;
  }
    debug(0,"BH mapping MQDC:  QDC%i, ch %i, plane %i, side %i\n",QDC_bank,channel,plane,side);

 ((funcshortmap *) qblmap[QDC_bank])->emplace(channel,[this,plane,bar,side](short value){BH->plane[plane][bar].adc_mqdc[side].emplace_back(value);});
 
  return Plugin::ok;
}


Long_t mapper::addBHTRB(int plane, int bar, int side, int bank, int channel)
{
  if (bank<0 || plane<0 || side<0){
    debug(1,"BH unmapping TRB with bank %x, ch %i, plane %i, side %i\n",bank,channel,plane,side);   
    return Plugin::ok;
  }
  debug(0,"BH mapping TRB with bank %x, ch %i, plane %i, side %i\n",bank,channel,plane,side);
  int to = BH_logic_to_internal(plane,bar,side);
  ((functrbhitmap *) trbtdcmap)->emplace((bank<<16)|channel,[to,this,plane,bar,side](trbhit hit){BH->plane[plane][bar].tdc_trb[side].emplace(to,std::move(hit));});
  return Plugin::ok;
}

Long_t mapper::addSPSTDC(int wall, int bar, int up, int TDCtype, int channel)
{

  if (TDCtype<0 || channel<0 )
    {
      debug(1,"SPS unmapping TDC with type %i, ch %i to wall/bar/up %i %i %i\n",TDCtype,channel,wall, bar, up);
      return Plugin::ok;
    }

  debug(0,"SPS mapping TDC with type %i, ch %i to wall/bar/up %i %i %i\n",TDCtype,channel,wall, bar, up);
  switch (TDCtype)
    {
    case 0:
      debug(0,"Don't use the V1190 with SPSs!!");
      break;
    case 1:
      int to = SPS_logic_to_internal(wall,bar,up);
      ((funcv1290hitmap *) v1290map)->emplace(channel,[to,this,wall,bar,up](v1290hit hit){SPS->wall[wall][bar].tdc_v1290[up].emplace(to,std::move(hit));});
      break;
    }
  return Plugin::ok;
}

Long_t mapper::addSPSReferenceClock(int id,int TDCtype,int channel)
{
  if (TDCtype<0) {
    debug(1,"SPS unmapping refclock with TDCtype %x, channel %i\n",TDCtype,channel);
    return Plugin::ok;
  }
  debug(0,"SPS mapping refclock with TDCtype %x, channel %i\n",TDCtype,channel);
  switch (TDCtype)
    {
    case 0:
      debug(0,"Don't use the V1190 with SPSs!!");
      break;
    case 1:
      ((funcv1290hitmap *) v1290map)->emplace(channel,[this](v1290hit hit){SPS->reftime.emplace_back(hit);});
      break;
    default:
      ((functrbhitmap *) trbtdcmap)->emplace((TDCtype<<16)|channel,[this](trbhit hit){SPS->trb_reftime.emplace_back(hit);});
      break;
    }
   return Plugin::ok;
}

Long_t mapper::addSPSTrigger(int id,int bank,int channel)
{
  if (bank<0)
    {
      debug(1,"SPS unmapping trigger from bank %x, channel %i\n",bank,channel);
     return Plugin::ok;
    }
  debug(0,"SPS mapping trigger to bank %x, channel %i\n",bank,channel);

  ((functrbhitmap *) trbtdcmap)->emplace((bank<<16)|channel,[this](trbhit hit){SPS->trig.emplace_back(hit);});
  return Plugin::ok;
}



Long_t mapper::addSPSADC(int wall, int bar, int up, int ADCtype, int channel)
{
  if (ADCtype<0 || wall <0 || bar<0 || up<0) 
    {
      debug(1,"SPS unmapping ADC with type %i, ch %i to wall/bar/up %i %i %i\n",ADCtype,channel,wall, bar, up);
      return Plugin::ok;
    }
  debug(0,"SPS mapping ADC with type %i, ch %i to wall/bar/up %i %i %i\n",ADCtype,channel,wall, bar, up);
  switch (ADCtype)
    {
    case 0:
        ((funcshortmap *) v792map)->emplace(channel,[this,wall,bar,up](short value){SPS->wall[wall][bar].adc_v792[up]=value;});
      break;
    case 1:
      ((funcshortmap *) mqdcmap)->emplace(channel,[this,wall,bar,up](short value){SPS->wall[wall][bar].adc_mqdc[up].emplace_back(value);});
      break;
    }
  return Plugin::ok;
}


// 03/15/17 SPS mapper for multiple MQDCs:
Long_t mapper::addSPS_MQDC(int wall, int bar, int up, int QDC_bank, int channel)
{
  if (QDC_bank<0 || wall <0 || bar<0 || up<0)
    {
      debug(1,"SPS unmapping MQDC: QDC%i, ch %i to wall/bar/up %i %i %i\n",QDC_bank,channel,wall, bar, up);
      return Plugin::ok;
    }
  debug(0,"SPS mapping MQDC: QDC%i, ch %i to wall/bar/up %i %i %i\n",QDC_bank,channel,wall, bar, up);

  ((funcshortmap *) qlamap[QDC_bank])->emplace(channel,[this,wall,bar,up](short value){SPS->wall[wall][bar].adc_mqdc[up].emplace_back(value);});
 

   return Plugin::ok;
}



Long_t mapper::addSPSTRB(int wall, int bar, int up, int bank, int channel)
{
  if (bank<0 || wall <0 || bar<0 || up<0){
    debug(1,"SPS unmapping TRB with bank %x, ch %i to wall %i bar %i up/down %i\n",bank,channel,wall, bar,up);
    return Plugin::ok;
  }
  debug(0,"SPS mapping TRB with bank %x, ch %i to wall %i bar %i up/down %i\n",bank,channel,wall, bar,up);
  int to = SPS_logic_to_internal(wall,bar,up);
  ((functrbhitmap *) trbtdcmap)->emplace((bank<<16)|channel,[to,this,wall,bar,up](trbhit hit){SPS->wall[wall][bar].tdc_trb[up].emplace(to,std::move(hit));});
  return Plugin::ok;
}

Long_t mapper::addPbGlassTDC(int x, int y, int TDCtype, int channel)
{

  if (TDCtype<0 || channel<0 )
    {
      debug(1,"PbGlass unmapping TDC with type %i, ch %i to x,y %i %i\n",TDCtype,channel,x,y);
      return Plugin::ok;
    }

  debug(0,"PbGlass mapping TDC with type %i, ch %i to x,y %i %i\n",TDCtype,channel,x,y);
  switch (TDCtype)
    {
    case 0:
      debug(0,"Don't use the V1190 with SPSs!!");
      break;
    case 1:
      ((funcv1290hitmap *) v1290map)->emplace(channel,[this,x,y](v1290hit hit){PbGlass->bar[x][y].tdc_v1290.emplace_back(hit);});
      break;
    }
  return Plugin::ok;
}

Long_t mapper::addPbGlassReferenceClock(int id,int TDCtype,int channel)
{
  if (TDCtype<0) {
    debug(1,"PbGlass unmapping refclock with TDCtype %x, channel %i\n",TDCtype,channel);
    return Plugin::ok;
  }
  debug(0,"PbGlass mapping refclock with TDCtype %x, channel %i\n",TDCtype,channel);
  switch (TDCtype)
    {
    case 0:
      ((functrbhitmap *) trbtdcmap)->emplace((TDCtype<<16)|channel,[this](trbhit hit){PbGlass->trb_reftime.emplace_back(hit);});
      break;
    case 1:
      ((funcv1290hitmap *) v1290map)->emplace(channel,[this](v1290hit hit){PbGlass->reftime.emplace_back(hit);});
      break;
    default:
      ((functrbhitmap *) trbtdcmap)->emplace((TDCtype<<16)|channel,[this](trbhit hit){PbGlass->trb_reftime.emplace_back(hit);});
      break;
    }
   return Plugin::ok;
}

Long_t mapper::addPbGlassTrigger(int id,int bank,int channel)
{
  if (bank<0)
    {
      debug(1,"PbGlass unmapping trigger from bank %x, channel %i\n",bank,channel);
     return Plugin::ok;
    }
  debug(0,"PbGlass mapping trigger to bank %x, channel %i\n",bank,channel);

  ((functrbhitmap *) trbtdcmap)->emplace((bank<<16)|channel,[this](trbhit hit){PbGlass->trig.emplace_back(hit);});
  return Plugin::ok;
}

Long_t mapper::addPbGlassADC(int x, int y, int ADCtype, int channel)
{
  if (ADCtype<0 || x <0 || y<0 ) 
    {
      debug(1,"PbGlass unmapping ADC with type %i, ch %i to x/y %i %i\n",ADCtype,channel,x/y);
      return Plugin::ok;
    }
  debug(0,"PbGlass mapping ADC with type %i, ch %i to x/y %i %i\n",ADCtype,channel,x/y);
  switch (ADCtype)
    {
    case 0:
      debug(0,"Don't use the v792 with the PbGlass detector!!");
      break;
    case 1:
      ((funcshortmap *) mqdcmap)->emplace(channel,[this,x,y](short value){PbGlass->bar[x][y].adc_mqdc.emplace_back(value);});
      break;
    }
  return Plugin::ok;
}

// 03/15/17 SPS mapper for multiple MQDCs:
Long_t mapper::addPbGlass_MQDC(int x, int y, int QDC_bank, int channel)
{
  if (QDC_bank<0 || x<0 || y<0)
    {
      debug(1,"PbGlass unmapping MQDC: QDC%i, ch %i to x/y %i %i\n",QDC_bank,channel,x, y);
      return Plugin::ok;
    }
  debug(0,"PbGlass mapping MQDC: QDC%i, ch %i to x/y %i %i\n",QDC_bank,channel,x, y);

  ((funcshortmap *) qdcmap[QDC_bank])->emplace(channel,[this,x,y](short value){PbGlass->bar[x][y].adc_mqdc.emplace_back(value);});
 

   return Plugin::ok;
}

Long_t mapper::addPbGlassTRB(int x, int y, int bank, int channel)
{
  if (bank<0 || x <0 || y<0){
    debug(1,"SPS unmapping TRB with bank %x, ch %i to x %i y %i \n",bank,channel,x,y);
    return Plugin::ok;
  }
  debug(0,"SPS mapping TRB with bank %x, ch %i to x %i y %i\n",bank,channel,x,y);

  ((functrbhitmap *) trbtdcmap)->emplace((bank<<16)|channel,[this,x,y](trbhit hit){PbGlass->bar[x][y].tdc_trb.emplace_back(hit);});
  return Plugin::ok;
}







Long_t mapper::addBMReferenceClock(int id,int TDCtype,int channel)
{
  if (TDCtype<0) {
    debug(1,"BM unmapping refclock with TDCtype %x, channel %i\n",TDCtype,channel);
    return Plugin::ok;
  }
  debug(0,"BM mapping refclock with TDCtype %x, channel %i\n",TDCtype,channel);
  switch (TDCtype)
    {
    default:
      ((functrbhitmap *) trbtdcmap)->emplace((TDCtype<<16)|channel,[this](trbhit hit){BM->trb_reftime.emplace_back(hit);});
      break;
    }
   return Plugin::ok;
}

Long_t mapper::addBMTrigger(int id,int bank,int channel)
{
  if (bank<0)
    {
      debug(1,"BM unmapping trigger from bank %x, channel %i\n",bank,channel);
     return Plugin::ok;
    }
  debug(0,"BM mapping trigger to bank %x, channel %i\n",bank,channel);

  ((functrbhitmap *) trbtdcmap)->emplace((bank<<16)|channel,[this](trbhit hit){BM->trig.emplace_back(hit);});
  return Plugin::ok;
}



// 03/15/17 SPS mapper for multiple MQDCs:
Long_t mapper::addBM_MQDC(int plane, int bar, int up, int QDC_bank, int channel)
{
  if (QDC_bank<0 || bar<0 || up<0)
    {
      debug(1,"BM unmapping MQDC: QDC%i, ch %i to x/y %i %i\n",QDC_bank,channel,bar, up);
      return Plugin::ok;
    }
  debug(0,"BM mapping MQDC: QDC%i, ch %i to plane %i bar %i up/down %i\n",QDC_bank,channel,plane,bar, up);

  ((funcshortmap *) qblmap[QDC_bank])->emplace(channel,[this,plane,bar,up](short value){BM->plane[plane][bar].adc_mqdc[up].emplace_back(value);});
 

   return Plugin::ok;
}

Long_t mapper::addBMTRB(int plane, int bar, int up, int bank, int channel)
{
  if (bank<0 || bar <0 || up<0){
    debug(1,"BM unmapping TRB with bank %x, ch %i to x %i y %i \n",bank,channel,bar,up);
    return Plugin::ok;
  }
  //debug(0,"BM mapping TRB with bank %x, ch %i to x %i y %i\n",bank,channel,bar,up);
  // ((functrbhitmap *) trbtdcmap)->emplace((bank<<16)|channel,[this,tgt](trbhit hit){tbt->tdc_trb.emplace(tgt,std::move(hit));});


  // int to = SPS_logic_to_internal(wall,bar,up);
  // ((functrbhitmap *) trbtdcmap)->emplace((bank<<16)|channel,[to,this,wall,bar,up](trbhit hit){SPS->wall[wall][bar].tdc_trb[up].emplace(to,std::move(hit));});
  // return Plugin::ok;
    debug(0,"BM mapping TRB with bank %x, ch %i to plane %i bar %i up/down %i\n",bank,channel,plane, bar,up);
  int to = BM_logic_to_internal(plane, bar,up);
  ((functrbhitmap *) trbtdcmap)->emplace((bank<<16)|channel,[to,this,plane,bar,up](trbhit hit){BM->plane[plane][bar].tdc_trb[up].emplace(to,std::move(hit));});
  return Plugin::ok;
}

Long_t mapper::finalize()
{
    return Plugin::ok;
};


Long_t mapper::cmdline(char *cmd)
{
  //add cmdline hanling here

  return 0; // 0 = all ok
};


extern "C"{
Plugin *factory(TTree *in, TTree *out,TFile *inf_, TFile * outf_, TObject *p)
{
  return (Plugin *) new mapper(in,out,inf_,outf_,p);
}
}


ClassImp(mapper);



#include "RecipeReader.h"
#include <xqilla/xqilla-simple.hpp>
#include <iostream>
#include <boost/algorithm/string.hpp>


class_method::class_method (std::string c, std::string m)
{
  classname=c;
  method=m;
  boost::trim(classname);
  boost::trim(method);
}

RecipeReader::RecipeReader(std::string filename)
{

  XQilla xqilla;

  try{ 
 
   AutoDelete<XQQuery> qinit(xqilla.parse(X("data(/muse/init)")));
   AutoDelete<DynamicContext> context (qinit->createDynamicContext());
    Sequence seq = context->resolveDocument(X(filename.c_str()));
    if(!seq.isEmpty() && seq.first()->isNode()) {
      context->setContextItem(seq.first());
      context->setContextPosition(1);
      context->setContextSize(1);
    }
 
    Result rinit=qinit->execute(context);

    InitXML=UTF8(rinit->next(context)->asString(context));

    AutoDelete<XQQuery> qsrctree(xqilla.parse(X("data(/muse/source)")));
    Result rsrctree=qsrctree->execute(context);
    srctree=UTF8(rsrctree->next(context)->asString(context));

    AutoDelete<XQQuery> qdsttree(xqilla.parse(X("data(/muse/destination)")));
    Result rdsttree=qdsttree->execute(context);
    dsttree=UTF8(rdsttree->next(context)->asString(context));

    AutoDelete<XQQuery> qplugs(xqilla.parse(X("/muse/plugins/plugin")));
    Result rplugs=qplugs->execute(context);

    AutoDelete<XQQuery> qplname(xqilla.parse(X("data(./name)")));
    AutoDelete<XQQuery> qplfile(xqilla.parse(X("data(./file)")));
    AutoDelete<DynamicContext> context2 (qplname->createDynamicContext());
    
    while(Node::Ptr item=rplugs->next(context))
      {

	context2->setContextItem(item);
    	context2->setContextPosition(1);
    	context2->setContextSize(1);
	
    	Result rname=qplname->execute(context2);
    	Result rfile=qplfile->execute(context2);
	plugins[UTF8(rname->next(context2)->asString(context2))]=UTF8(rfile->next(context2)->asString(context2));
       }

    AutoDelete<XQQuery> qdefineHistograms(xqilla.parse(X("/muse/defineHistograms/*")));
    Result rdefineHistograms=qdefineHistograms->execute(context);
    while(Node::Ptr item=rdefineHistograms->next(context))
      {
	defineHistograms.push_back(class_method(UTF8(item->dmNodeName(context)->getName()), UTF8(item->dmStringValue(context))));
      }

    AutoDelete<XQQuery> qstartup(xqilla.parse(X("/muse/startup/*")));
    Result rstartup=qstartup->execute(context);
    while(Node::Ptr item=rstartup->next(context))
      {
	startup.push_back(class_method(UTF8(item->dmNodeName(context)->getName()), UTF8(item->dmStringValue(context))));
      }

    AutoDelete<XQQuery> qexecute(xqilla.parse(X("/muse/execute/*")));
    Result rexecute=qexecute->execute(context);
    while(Node::Ptr item=rexecute->next(context))
      {
	commands.push_back(class_method(UTF8(item->dmNodeName(context)->getName()), UTF8(item->dmStringValue(context))));
      }
    AutoDelete<XQQuery> qfinalize(xqilla.parse(X("/muse/finalize/*")));
    Result rfinalize=qfinalize->execute(context);
    while(Node::Ptr item=rfinalize->next(context))
      {
	finalize.push_back(class_method(UTF8(item->dmNodeName(context)->getName()), UTF8(item->dmStringValue(context))));
      }

    AutoDelete<XQQuery> qexecute2(xqilla.parse(X("/muse/execute2/*")));
    Result rexecute2=qexecute2->execute(context);
    while(Node::Ptr item=rexecute2->next(context))
      {
	commands2.push_back(class_method(UTF8(item->dmNodeName(context)->getName()), UTF8(item->dmStringValue(context))));
      }
    AutoDelete<XQQuery> qpostprocess(xqilla.parse(X("/muse/postprocess/*")));
    Result rpostprocess=qpostprocess->execute(context);
    while(Node::Ptr item=rpostprocess->next(context))
      {
	postprocess.push_back(class_method(UTF8(item->dmNodeName(context)->getName()), UTF8(item->dmStringValue(context))));
      }

  }
  catch(   XQException E)
    {
      std::cerr<<"Parsing of recipe XML failed, error type:"<<UTF8(E.getType())<<" Error:"<<UTF8(E.getError())<<std::endl;
    } 
  
  
}


RecipeReader::~RecipeReader()
{
}

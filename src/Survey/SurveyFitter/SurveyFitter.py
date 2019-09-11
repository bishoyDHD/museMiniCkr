import scipy.optimize
import sys
from numpy import *;
from math import pi;



    

    

def loadASCII(filename):
    ngroups=0
    groups=dict()
    nnames=0;
    names=dict()
    data=[]
    for l in open(filename):
        try:
            group,net,point,azi,ele,distance, offset1,offset2, date1,date2 ,active=l.split();
        except:
            continue
        group=group.upper()
        
        if not (group in groups):
            groups[group]=ngroups
            ngroups+=1
        name=(point).upper()
        if not (name  in names):
            names[name]=nnames
            nnames+=1;
            
        azi=float(azi)*pi/180;
        ele=float(ele)*pi/180;
        dist=float(distance)#+float(offset1)#+float(offset2)
        D=array([dist*cos(azi)*sin(ele),dist*sin(azi)*sin(ele),dist*cos(ele)],float)
        data.append([groups[group],names[name], D]);
    return ngroups,groups,nnames,names,data



a,b,c,d,data=loadASCII(sys.argv[1]);


nparams=a*6+c*3
gparams=array([6129.63612549 ,3001.03070599 ,922.204358895,0.0,0.0,90.0*pi/180]*a+[0.0]*c*3);
offset=a*6;
i=0
for l in open("cache.dat"):
    ok=True
    try:
        name,da,db,dc,dd,de,df=l.split()
        print name
    except:
        ok=False
    if ok:
        try:
            if b[name]>=0:
                gparams[(b[name])*6:(b[name])*6+6]=[float (i) for i in [da,db,dc,dd,de,df]]
        except:
            pass
        continue
    ok=True
    try:
        name,da,db,dc=l.split()
    except:
        ok=False
    if ok:
        try:
            if d[name]>=0:
                gparams[offset+(d[name])*3:offset+(d[name])*3+3]=[float (i) for i in [da,db,dc]]
        except:
            pass


print gparams[0:12]

rotcache=[[0,0]]*a;


def makerot(gid,a):
    hash=str(a[0])+str(a[1])+str(a[2])
    if rotcache[gid][1]!=hash:
        s0,s1,s2=sin(a)
        c0,c1,c2=cos(a)
        BAC=array([c1*c2-s0*s1*s2,-c2*s0*s1-c1*s2,c0*s1,   c0*s2,c0*c2,s0, -c2*s1-c1*s0*s2,-c1*c2*s0+s1*s2,c0*c1]).reshape(3,3)
        rotcache[gid]=[BAC,hash]

def rot(gid,D):
    BAC=rotcache[gid][0]
    return BAC.dot(D)


def calc(d,params):
    gid,pid,D=d
    pos=params[offset+pid*3:offset+pid*3+3]
    if gid>=0:
        soff=params[gid*6:gid*6+3]
        #        srot=params[gid*6+3:gid*6+6]
    else:
        soff=array([4784.3638,1610.7441,298.6962]);
        #srot=array([0.2448*pi/180,0.7381*pi/180,85.6672*pi/180]);

    verr=(rot(gid,D)+soff-pos)
    return verr

def pos(d,params):
    gid,pid,D=d
    return calc(d,params)+params[offset+pid*3:offset+pid*3+3]




ndata=len(data);
iter=0;
#makerot(0,array([0.2138*pi/180,0.4447*pi/180,170.1279*pi/180]))

ncons=6+2+2

tid=[d[n]*3+offset for n in ["T1","T2","T3","T4","T5","T6","T8","T10" ]]
    
    
#@profile
def chifunc(p):
    global iter
    ret=zeros(ndata*3+ncons)
    for gid in range(a):
        srot=p[gid*6+3:gid*6+6]
        makerot(gid,srot)

        
    e=0
    ind=0
    ogid=-2
    soff=zeros(3)
    pos=zeros(3)
    doe=iter %50==0
    for i in data:
        gid,pid,D=i
        o=offset+pid*3
        pos=p[o:o+3]
        if gid!=ogid:
            soff=p[gid*6:gid*6+3]
            trans=rotcache[gid][0]
            ogid=gid
        ret[ind:ind+3]=(trans.dot(D)+soff-pos)
        ind+=3

    # target constrains
    #beamline y=0
    for i in range (6):
        ret[ind]=p[tid[i]+1]
        ind+=1
    # window points
    #   ret[ind]=p[tid[6]]-p[tid[7]]
    #    ind+=1
    #ret[ind]=p[tid[6]+1]-p[tid[7]+1]
    #ind+=1
    # z around flange
    ret[ind]=p[tid[0]+2]+p[tid[1]+2]
    ind+=1
    ret[ind]=p[tid[4]+2]+p[tid[5]+2]
    ind+=1
    # pos in x
    ret[ind]=p[tid[2]]+599.2
    ind+=1
    ret[ind]=p[tid[3]]-515.102
    ind+=1
    
    if doe: print iter,p[0:3],p[3:6]*180/pi, p[tid[0]+1],ret.dot(ret)
    iter+=1;
    return ret



erg=scipy.optimize.leastsq(chifunc,gparams,full_output=True);
#erg=[gparams]

f=open("cache.dat","write")
for i in b:
    print >>f,i,
    if b[i]>=0:
        for k in range (6):
            print >>f,erg[0][(b[i])*6+k],
    print >>f
print >>f
for i in d:
    print >>f,i,
    for k in range (3):
        print >>f,erg[0][offset+(d[i])*3+k],
    print >>f
    
f.close()

print erg
for gid in range(a):
    srot=erg[0][gid*6+3:gid*6+6]
    makerot(gid,srot)
   

print "***********"
print b
for g in b:
    gid= b[g]
    print g
    print erg[0][gid*6:gid*6+3],erg[0][gid*6+3:gid*6+6]*180/pi

print "=============="
for p in sorted(d):
    print p
    for i in data:
        if i[1]==d[p]:
            v=calc(i,erg[0])
            print "\t",i[0],pos(i,erg[0]),v.dot(v)
        
# f=open("test.txt","write")
# for p in ["FDG.14","FDG.18"]:
#     i=d[p]
#     print >>f,erg[0][i*3+offset],erg[0][1+i*3+offset],erg[0][2+i*3+offset],p
    

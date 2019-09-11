par=["0x0650",
     "0x0651",
     "",
     "",
    ]

par1=["0",
      "1",
      "2",
      "3",
    ]


#Beam left SPS
print '<!--Beam left SPS -->'
for j in range(18):
  print '<SPStrb id="1,',j,',1" >',par[0],',',j+1,' </SPStrb>'
  print '<SPSmqdc id="1,',j,',1" >',par1[0],',',j,' </SPSmqdc>'
for j in range(28):
  print '<SPStrb id="3,',j,',1" >',par[0],',',j+1+18,' </SPStrb>'
  print '<SPSmqdc id="3,',j,',1" >',par1[0],',',j,' </SPSmqdc>'
for j in range(18):
  print '<SPStrb id="1,',j,',0" >',par[1],',',j+1,' </SPStrb>'
  print '<SPSmqdc id="1,',j,',0" >',par1[1],',',j,' </SPSmqdc>'
for j in range(28):  
  print '<SPStrb id="3,',j,',0" >',par[1],',',j+1+18,' </SPStrb>'
  print '<SPSmqdc id="3,',j,',1" >',par1[1],',',j,' </SPSmqdc>'
#Beam right SPS
print '<!--Beam right SPS -->'
for j in range(18):
  print '<SPStrb id="0,',j,',1" >',par[0],',',j+1,' </SPStrb>'
  print '<SPSmqdc id="0,',j,',1" >',par1[2],',',j,' </SPSmqdc>'
for j in range(28):
  print '<SPStrb id="2,',j,',1" >',par[0],',',j+1+18,' </SPStrb>'
  print '<SPSmqdc id="2,',j,',1" >',par1[2],',',j,' </SPSmqdc>'
for j in range(18):
  print '<SPStrb id="0,',j,',0" >',par[1],',',j+1,' </SPStrb>'
  print '<SPSmqdc id="0,',j,',0" >',par1[3],',',j,' </SPSmqdc>'
for j in range(28):  
  print '<SPStrb id="2,',j,',0" >',par[1],',',j+1+18,' </SPStrb>'
  print '<SPSmqdc id="2,',j,',1" >',par1[3],',',j,' </SPSmqdc>'
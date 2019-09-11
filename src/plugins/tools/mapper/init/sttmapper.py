par=["",
     "",
     "",
     "",
    ]
side = input("side ")
plane = input("plane ")
trb = input("trb ")
card = input("card ")
conn = input("connector ")

plane = plane -1

for i in range (16):
  if side == 0:
    if plane < 5:
      if ((15-i)+(4-card)*16)>54:
        continue
      print '<stt id="',side,',',plane,',',(15-i)+(4-card)*16,'">0x'+str(trb-1)+',',i+(conn-1)*16+1,'</stt>'
    if plane > 4:
      if (i+(card-1)*16)>59:
        break     
      print '<stt id="',side,',',plane,',',i+(card-1)*16,'">0x'+str(trb-1)+',',i+(conn-1)*16+1,'</stt>'
  if side == 1:
    if plane < 5:
      if (i+(4-card)*16)>54:
        break
      print '<stt id="',side,',',plane,',',i+(4-card)*16,'">0x'+str(trb-1)+',',i+(conn-1)*16+1,'</stt>'
    if plane > 4:
      if ((15-i)+(card-1)*16)>59:
        continue     
      print '<stt id="',side,',',plane,',',(15-i)+(card-1)*16,'">0x'+str(trb-1)+',',i+(conn-1)*16+1,'</stt>'
  if side == 2:
    if plane < 5:
      if ((15-i)+(6-card)*16) > 89:
        continue
      print '<stt id="',side,',',plane,',',(15-i)+(6-card)*16,'">0x'+str(trb-1)+',',i+(conn-1)*16+1,'</stt>'
    if plane > 4:
      if (i+(card-1)*16) > 89:
        break
      print '<stt id="',side,',',plane,',',i+(card-1)*16,'">0x'+str(trb-1)+',',i+(conn-1)*16+1,'</stt>'
  if side == 3:
    if plane < 5:
      if (i+(6-card)*16) > 89:
        continue
      print '<stt id="',side,',',plane,',',i+(6-card)*16,'">0x'+str(trb-1)+',',i+(conn-1)*16+1,'</stt>'
    if plane > 4:
      if ((15-i)+(card-1)*16) > 89:
        continue
      print '<stt id="',side,',',plane,',',(15-i)+(card-1)*16,'">0x'+str(trb-1)+',',i+(conn-1)*16+1,'</stt>'


# #Beam left STT
# print '<!--Beam left STT -->'
# for i in range (10):
#   for j in range(50):
     #print '<STTtrb id="1,',i,',',j,'" >',par[0],',',j+1,' </STTtrb>'
# for i in range (10):
#   for j in range(89):
#     print '<STTtrb id="3,',i,',',j,'" >',par[1],',',j+1,' </STTtrb>'

# #Beam right STT
# print '<!--Beam right STT -->'
# for i in range (10):
#   for j in range(50):
#     print '<STTtrb id="0,',i,',',j,'" >',par[0],',',j+1,' </STTtrb>'
# for i in range (10):
#   for j in range(89):
#     print '<STTtrb id="2,',i,',',j,'" >',par[1],',',j+1,' </STTtrb>'

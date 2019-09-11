set term canvas
set out "NearOnlineAuto.html"
plot "ROOTfiles/blub.dat" u 1:2 w l
plot "ROOTfiles/blub.dat" u 1:2 w lp
replot
pause 10
reread
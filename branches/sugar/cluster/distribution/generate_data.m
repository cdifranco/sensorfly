load 'real_rang_dist.mat';

x=900;
y=900;
base = [0,0;
x,0;
x,y;
0,y;
x/3,0;
x,y/2;
2*x/3,y;
0,y/2;
2*x/3,0;
x,3*y/4;
x/3,y;
0,y/4;
x,y/4;
0,3*y/4];  
data = [];  

for i=0:10:x
    for j = 0:10:y
        for k=1:20
            data = [data; i j 0 3600 convert(i, j, 14, base, coefficient)];
        end
    end
end  

save('hallway3.mat','data');
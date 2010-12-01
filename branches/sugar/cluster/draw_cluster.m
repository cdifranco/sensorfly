function a = draw_cluster(b,center_size,reading)
% b is the position of clustering record
colors = randperm(center_size);
%scatter(reading(:,3),reading(:,4),10,colors(reading(:,b)),'filled');

%Show Results
%{
X = reading(:,3);
Y = reading(:,4);
Z = reading(:,b);
xtemp=linspace(min(X),max(X),100);
ytemp=linspace(min(Y),max(Y),100);
[x,y]=meshgrid(xtemp,ytemp);
z=griddata(X,Y,Z,x,y);
for i = 1:100
    for j = 1:100
        if x(i, j) < 1 && y(i,j) > 4
            z(i, j) = -1;
        end
    end
end
surfc(x,y,z,'EdgeColor','none');
hold on;

for i = 1:100
    for j = 1:100
        z(i, j) = 0;
    end
end

%surf(x,y,z);
hold off;
%}

rawX = reading(:,3);
rawY = reading(:,4);
rawZ = reading(:,b);

for c = 1:center_size
    X = [];
    Y = [];
    Z = [];
    for i = 1:size(reading(:,1),1)
        if rawZ(i) == c
            X = [X rawX(i)];
            Y = [Y rawY(i)];
            Z = [Z rawZ(i)];
        end
    end
    xtemp=linspace(min(X),max(X),100);
    ytemp=linspace(min(Y),max(Y),100);
    [x,y]=meshgrid(xtemp,ytemp);
    z=griddata(X,Y,Z,x,y);
    surf(x,y,z,'EdgeColor','none');
    colormap hsv
    alpha(.4)
    hold on;
end


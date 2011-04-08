figure;

colors = randperm(size(sigxy.ctrs,1));

rawX = sigxy.x;
rawY = sigxy.y;
rawZ = sigxy.cluster_id;

for i = 1:size(sigxy.ctrs,1)
    X = [];
    Y = [];
    Z = [];
    for ii = 1:size(rawX,2)
        if rawZ(ii) == i %&& ~remove(rawX, rawY, rawZ, ii, 0.05, 15)
            X = [X rawX(ii)];
            Y = [Y rawY(ii)];
            Z = [Z rawZ(ii)];
        end
    end
    if length(Z) > 2
        xtemp=linspace(min(X),max(X),100);
        ytemp=linspace(min(Y),max(Y),100);
        [x,y]=meshgrid(xtemp,ytemp);
        z=griddata(X,Y,Z,x,y)*10;
        surf(x,y,z,'EdgeColor','none');
        colormap hsv
        alpha(.4)
        hold on;
    end
end

for i = 1:size(sigxy.ctrs,1)
   centerxy.x(i) = mean(sigxy.x(sigxy.cluster_id == i));
   centerxy.y(i) = mean(sigxy.y(sigxy.cluster_id == i));
end

for i = 1:size(sigxy.ctrs,1)
     text(centerxy.x(i), centerxy.y(i), num2str(i));
end

axis tight
axis equal

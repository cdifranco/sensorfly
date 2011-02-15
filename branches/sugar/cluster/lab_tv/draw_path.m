hold on;
for i = 1:(size(coordRoute,1)-1)
    line([coordRoute(i, 1) coordRoute(i+1, 1)], [coordRoute(i, 2) coordRoute(i+1, 2)], 'Color','k');
end
hold off;

clear i;
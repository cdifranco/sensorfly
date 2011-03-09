clear all;
close all;
load 'processed_data.mat';

for i = 1 : size(std_sig, 1)
    location = std_sig(i, 1:2);
    angle = 360 - std_sig(i, 4)/10;
    north_x = location(1) + sin(angle/180*pi);
    south_x = location(1) - sin(angle/180*pi);
    north_y = location(2) + cos(angle/180*pi);
    south_y = location(2) - cos(angle/180*pi);
    plot([location(1),north_x],[location(2),north_y], 'r','LineWidth',2);
    hold on;
    plot([location(1),south_x],[location(2),south_y], 'b','LineWidth',2);
    hold on;
end
hold off;
axis tight;
axis equal;
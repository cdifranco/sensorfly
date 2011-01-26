%{
x_rec = zeros(1, 4);
y_rec = zeros(1, 4);
grid = cell(27, 1);
for i = 1 : area_number
    grid{i}.x = x_rec;
    grid{i}.y = y_rec;
end
grid{1}.x = [20 25 25 20];
grid{2}.x = [15 20 20 15];
grid{3}.x = [10 15 15 10];
grid{4}.x = [5 10 10 5];
grid{5}.x = [5 10 10 5];
grid{6}.x = [10 15 15 10];
grid{7}.x = [20 25 25 20];
grid{8}.x = [20 25 25 20];
grid{9}.x = [15 20 20 15];
grid{10}.x = [10 15 15 10];
grid{11}.x = [5 10 10 5];
grid{12}.x = [0 5 5 0];
grid{13}.x = [0 5 5 0];
grid{14}.x = [5 10 10 5];
grid{15}.x = [10 15 15 10];
grid{16}.x = [10 15 15 10];
grid{17}.x = [5 10 10 5];
grid{18}.x = [0 5 5 0];
grid{19}.x = [5 10 10 5];
grid{20}.x = [10 15 15 10];
grid{21}.x = [15 20 20 15];
grid{22}.x = [20 25 25 20];
grid{23}.x = [20 25 25 20];
grid{24}.x = [15 20 20 15];
grid{25}.x = [10 15 15 10];
grid{26}.x = [5 10 10 5];
grid{27}.x = [5 10 10 5];
grid{1}.y = [0 0 5 5];
grid{2}.y = [0 0 5 5];
grid{3}.y = [0 0 5 5];
grid{4}.y = [0 0 5 5];
grid{5}.y = [5 5 10 10];
grid{6}.y = [5 5 10 10];
grid{7}.y = [5 5 10 10];
grid{8}.y = [10 10 15 15];
grid{9}.y = [10 10 15 15];
grid{10}.y = [10 10 15 15];
grid{11}.y = [10 10 15 15];
grid{12}.y = [10 10 15 15];
grid{13}.y = [15 15 20 20];
grid{14}.y = [15 15 20 20];
grid{15}.y = [15 15 20 20];
grid{16}.y = [20 20 25 25];
grid{17}.y = [20 20 25 25];
grid{18}.y = [20 20 25 25];
grid{19}.y = [25 25 30 30];
grid{20}.y = [25 25 30 30];
grid{21}.y = [25 25 30 30];
grid{22}.y = [25 25 30 30];
grid{23}.y = [30 30 35 35];
grid{24}.y = [30 30 35 35];
grid{25}.y = [30 30 35 35];
grid{26}.y = [30 30 35 35];
grid{27}.y = [35 35 40 40];

save 'grid_in_lab.mat';

%}
%% draw the main frame
load 'grid.mat';
Z = zeros(4);
figure;
for i =  1 : area_number
    surf(grid{i}.x, grid{i}.y, ones(4)*0, ones(4)*0);
    colormap([1 1 0; 0 1 1])
    %colorbar
    hold on;
end
axis([-inf,inf,-inf,inf,0,10]);
%% input grid number and draw
%{
while 1
    cont = input('continue?');
    if cont == 0
        break;
    end
    grid_id = input('grid id: ');
    for j =  1 : 5
        surf(grid{grid_id}.x, grid{grid_id}.y, ones(4)*0, ones(4)*-1);
        colormap([1 1 0; 0 1 1])
        pause(1);
        surf(grid{grid_id}.x, grid{grid_id}.y, ones(4)*0, ones(4)*0);
        colormap([1 1 0; 0 1 1])
        pause(1);
    end
    %colorbar
    hold on;
end
%}
%% draw cluster
while 1
    cont = input('continue?');
    if cont == 0 
        break;
    end
    for i =  1 : area_number
        surf(grid{i}.x, grid{i}.y, ones(4)*0, ones(4)*0);
        colormap([1 1 0; 0 1 1])
        %colorbar
        hold on;
    end
    center_id = input('center id:');
    for i = 1 : area_number
        if area_cluster_relation{i} == -1 
            continue;
        elseif sum(area_cluster_relation{i} == center_id) > 10 %~isempty(find(ismember(area_cluster_relation{i}, center_id), 1))
            surf(grid{i}.x, grid{i}.y, ones(4)*0, ones(4)*-1);
        end
    end
    
end
%%
hold off;
close all;

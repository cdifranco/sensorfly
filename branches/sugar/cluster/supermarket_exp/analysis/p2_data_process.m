% this part is for raw data process, get average signatures 
%% Initialization
clear all;
load ('2feet_grid.mat');
iteration = 20;
base_number = 30;
raw_reading_count = size(data, 1);
reading_count = raw_reading_count/iteration;
std_sig = zeros(reading_count, size(data, 2) + base_number);
%% Get average data for each point
for i = 1 : reading_count
    std_sig(i, 1:3) = mean(data(20*(i-1)+1:20*i,1:3));
    std_sig(i, 4) = mean(data(20*(i-1) + find(data(20*(i-1)+1:20*i,4) ~= inf), 4));
    for j = 1 : base_number
        temp = data(20*(i-1) + find(data(20*(i-1)+1:20*i,4+j) ~= inf), 4+j);
        if ~isempty(temp)
            std_sig(i, 5+(j-1)*2) = mean(data(20*(i-1) + find(data(20*(i-1)+1:20*i,4+j) ~= inf), 4+j));
            std_sig(i, 6+(j-1)*2) = std(data(20*(i-1) + find(data(20*(i-1)+1:20*i,4+j) ~= inf), 4+j));
        else
            std_sig(i, 5+(j-1)*2) = inf;
            std_sig(i, 6+(j-1)*2) = inf;
        end
    end
end



%% Clear up
clear raw_reading_count;
clear reading_count;
clear i;
clear j;

%% Save results
save 'processed_data.mat';

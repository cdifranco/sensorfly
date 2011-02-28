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
    std_sig(i, 1:4) = mean(data(20*(i-1)+1:20*i,1:4));
    for j = 1 : base_number
        std_sig(i, 5+(j-1)*2) = mean(data(20*(i-1)+1:20*i,4+j));
        std_sig(i, 6+(j-1)*2) =  std(data(20*(i-1)+1:20*i,4+j));
    end
end

%% Clear up
clear raw_reading_count;
clear reading_count;
clear i;
clear j;

%% Save results
save 'processed_data.mat';


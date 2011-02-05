% this part is for raw data process, get average signatures 
%% Initialization
clear all;
load ('raw_data.mat');
raw_reading_count = size(reading, 1);
reading_count = raw_reading_count/iteration;
%signatures = zeros(reading_count, size(reading, 2));
std_sig = zeros(reading_count, size(reading, 2) + base_number);
%% Get average reading for each point
for i = 1 : reading_count
    %signatures(i, :) = mean(reading(20*(i-1)+1:20*i,:));
    std_sig(i, 1:4) = mean(reading(20*(i-1)+1:20*i,1:4));
    for j = 1 : base_number
        std_sig(i, 5+(j-1)*2) = mean(reading(20*(i-1)+1:20*i,4+j));
        std_sig(i, 6+(j-1)*2) =  std(reading(20*(i-1)+1:20*i,4+j));
    end
end

%% Clear up
clear raw_reading_count;
clear reading;
clear reading_count;
clear i;
clear j;

%% Save results
save 'processed_data.mat';


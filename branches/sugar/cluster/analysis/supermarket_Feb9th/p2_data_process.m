% this part is for raw data process, get average signatures 
%% Initialization
clear all;
load ('supermarket_reading_2_9.mat');
iteration = 20;
base_number = 15;
raw_reading_count = size(readings, 1);
reading_count = raw_reading_count/iteration;
std_sig = zeros(reading_count, size(readings, 2) + base_number);
%% Get average reading for each point
for i = 1 : reading_count
    std_sig(i, 1:4) = mean(readings(20*(i-1)+1:20*i,1:4));
    for j = 1 : base_number
        if isempty(readings(readings(20*(i-1)+1:20*i,4+j)~=inf,4+j))
            std_sig(i, 5+(j-1)*2) = inf;
            std_sig(i, 6+(j-1)*2) = inf;
        else
            std_sig(i, 5+(j-1)*2) = mean(readings([false(20*(i-1), 1); readings(20*(i-1)+1:20*i,4+j)~=inf],4+j));
            std_sig(i, 6+(j-1)*2) =  std(readings([false(20*(i-1), 1); readings(20*(i-1)+1:20*i,4+j)~=inf],4+j));
        end
    end
end

ave_std = zeros(1, base_number);
for i = 3 : base_number+2
    ave_std(i-2) = mean(std_sig(std_sig(:, i*2)~=inf, i*2));
end
reading_fail = zeros(1, base_number);
for i = 3 : base_number+2
    reading_fail(i-2) = sum(readings(:, i+2) == inf);
end

%% Clear up
clear raw_reading_count;
clear readings;
clear reading_count;
clear i;
clear j;

%% Save results
save 'processed_data_supermarket.mat';


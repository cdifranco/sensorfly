% this part is for raw data process, get average signatures 
%% Initialization
clear all;
load ('raw_data_supermarket.mat');
raw_reading_count = size(reading, 1);
reading_count = raw_reading_count/iteration
std_sig = zeros(reading_count, size(reading, 2) + base_number);
%% Get average reading for each point
for i = 1 : reading_count
    %signatures(i, :) = mean(reading(20*(i-1)+1:20*i,:));
    std_sig(i, 1:4) = mean(reading(20*(i-1)+1:20*i,1:4));
    for j = 1 : base_number
        if isempty(reading(reading(20*(i-1)+1:20*i,4+j)~=inf,4+j))
            std_sig(i, 5+(j-1)*2) = inf;
            std_sig(i, 6+(j-1)*2) = inf;
        else
            std_sig(i, 5+(j-1)*2) = mean(reading([false(20*(i-1), 1); reading(20*(i-1)+1:20*i,4+j)~=inf],4+j));
            std_sig(i, 6+(j-1)*2) =  std(reading([false(20*(i-1), 1); reading(20*(i-1)+1:20*i,4+j)~=inf],4+j));
        end
    end
end

%upper_6_std = mean(std_sig(std_sig(:, 12)~=inf, 12));
%lower_8_std = mean(std_sig(std_sig(:, 16)~=inf, 16));
ave_std = zeros(1, base_number);
for i = 3 : base_number+2
    ave_std(i-2) = mean(std_sig(std_sig(:, i*2)~=inf, i*2));
end
reading_fail = zeros(1, base_number);
for i = 3 : base_number+2
    reading_fail(i-2) = sum(reading(:, i+2) == inf);
end
std_sig_6 = std_sig;
std_sig_6(:, 11:12) = inf;
std_sig_7 = std_sig;
std_sig_7(:, 11:12) = inf;

%% Clear up
clear raw_reading_count;
clear reading;
clear reading_count;
clear i;
clear j;

%% Save results
save 'processed_data_supermarket.mat';


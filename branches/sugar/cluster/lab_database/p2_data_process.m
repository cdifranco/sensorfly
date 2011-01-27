% this part is for raw data process, get average signatures 
%% Initialization
load ('raw_data.mat');
raw_reading_count = size(reading, 1);
reading_count = raw_reading_count/iteration;
signatures = zeros(reading_count, size(reading, 2));

%% Get average reading for each point
for i = 1 : reading_count
    signatures(i, :) = mean(reading(20*(i-1)+1:20*i,:));
end

%% Clear up
clear raw_reading_count;
clear reading;
clear reading_count;

%% Save results
save 'processed_data.mat';


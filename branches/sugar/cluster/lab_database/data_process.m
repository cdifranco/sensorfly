%% Initialization
load 'raw_data.mat';
iteration = 20;
base_number = 10;
raw_reading_count = size(reading, 1);
reading_count = raw_reading_count/iteration;
signatures = zeros(reading_count, size(reading, 2));

%% Get average reading for each point
for i = 1 : reading_count
    signatures(i, :) = mean(reading(20*(i-1)+1:20*i,:));
end

%% Save results
save 'processed_data.mat';


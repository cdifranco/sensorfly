%% Select 3000 Readings for Clustering
random_start_point = unidrnd(size(signatures, 1));
start_point = [signatures(random_start_point, 1) , signatures(random_start_point, 2)];

%% 
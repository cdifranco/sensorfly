% Main

load 'cluster.mat';
% data(cluster_id, x, y, compass, sig)

dist_error = [];
ave_step = [];

for i = 4 : 10
    kmeans;
    generate_training_path;
    testing;
end

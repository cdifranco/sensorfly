% Main

dist_error_collection = [];
step_collection = [];
save ('final_result.mat','dist_error_collection','dist_error_collection');
clear all;

for anchor_number = 10 : 10 % anchor number
    for cluster_number = 5:5:30 % max cluster number
        save ('parameters.mat', 'cluster_number', 'anchor_number');
        kmeans_clustering;
        training;
        testing;
    end
end

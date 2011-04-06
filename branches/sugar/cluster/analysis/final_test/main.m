% Main
clear all;
close all;
dist_error_collection_controlled = [];
step_collection_controlled = [];
error_count_collection_controlled =[];
dist_error_collection_random = [];
step_collection_random = [];
error_count_collection_random =[];
save ('final_result_controlled.mat','dist_error_collection_controlled','step_collection_controlled','error_count_collection_controlled');
save ('final_result_random.mat','dist_error_collection_random','step_collection_random','error_count_collection_random');
clear all;


for cluster_number = 5:5:20 % max cluster number
    save ('parameters.mat', 'cluster_number');
    kmeans_clustering;
    %training;
    %testing;
    %random_testing;
end

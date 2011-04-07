% Main
clear all;
close all;
dist_error_collection_controlled = [];
step_collection_controlled = [];
error_count_collection_controlled = [];

dist_error_collection_random = [];
step_collection_random = [];
error_count_collection_random =[];

training_round_record = [];
cluster_number_record = [];

save ('final_result_controlled.mat','dist_error_collection_controlled','step_collection_controlled','error_count_collection_controlled');
save ('final_result_random.mat','dist_error_collection_random','step_collection_random','error_count_collection_random');
save ('testing_parameters_record.mat', 'training_round_record', 'cluster_number_record');
clear all;


for cluster_number = 12:12 % max cluster number
   save ('parameters.mat', 'cluster_number');
        for reading_amount = 1000 : 5000 : 21000
            save ('training_amount.mat','reading_amount');
            load 'testing_parameters_record.mat';
            load 'parameters.mat';
            training_round_record = [training_round_record, reading_amount];
            cluster_number_record = [cluster_number_record, cluster_number];
            save ('testing_parameters_record.mat', 'training_round_record', 'cluster_number_record');
            kmeans_clustering;
            training;
            testing;
            random_testing;
        end
   
end

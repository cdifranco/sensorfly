clear 
close all
load 'final_result_controlled.mat';
load 'final_result_random.mat';
load 'testing_parameters_record.mat';
p1 = [1,2,3,4,5];

errorbar(cluster_number_record(p1), dist_error_collection_controlled(p1,1), dist_error_collection_controlled(p1,2),'b');
% hold on;
% errorbar(cluster_number_record(p1), dist_error_collection_random(p1,1), dist_error_collection_random(p1,2),'r');
% hold off;
figure;
errorbar(cluster_number_record(p1), step_collection_controlled(p1,1), step_collection_controlled(p1,2),'b');
hold on;
errorbar(cluster_number_record(p1), step_collection_random(p1,1), step_collection_random(p1,2),'r');
hold off;
figure;
plot(cluster_number_record(p1), step_collection_controlled(p1,1), 'b');
hold on;
plot(cluster_number_record(p1), step_collection_random(p1,1), 'r');
hold off;
%{
figure;

errorbar(training_round_record(p2), dist_error_collection_controlled(p2,1), dist_error_collection_controlled(p2,2),'b');
hold on;
errorbar(training_round_record(p2), dist_error_collection_random(p2,1), dist_error_collection_random(p2,2),'r');
hold off;
figure;
errorbar(training_round_record(p2), step_collection_controlled(p2,1), step_collection_controlled(p2,2),'b');
hold on;
errorbar(training_round_record(p2), step_collection_random(p2,1), step_collection_random(p2,2),'r');
hold off;
figure;
plot(training_round_record(p2), step_collection_controlled(p2,1), 'b');
hold on;
plot(training_round_record(p2), step_collection_random(p2,1), 'r');
hold off;
%}

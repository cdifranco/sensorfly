clear 
close all
load 'final_result_controlled.mat';
load 'final_result_random.mat';
load 'testing_parameters_record.mat';
%p1 = [1,2,5,8,9];
p2 = [1,2,3,4];
%{
errorbar(cluster_number_record(p1), dist_error_collection_controlled(p1,1), dist_error_collection_controlled(p1,2),'b');
hold on;
errorbar(cluster_number_record(p1), dist_error_collection_random(p1,1), dist_error_collection_random(p1,2),'r');
hold off;
figure;
errorbar(cluster_number_record(p1), step_collection_controlled(p1,1), step_collection_controlled(p1,2),'b');
hold on;
errorbar(cluster_number_record(p1), step_collection_random(p1,1), step_collection_random(p1,2),'r');
hold off;
figure;
plot(cluster_number_record(p1), step_collection_controlled(p1,1),'b');
hold on;
plot(cluster_number_record(p1), step_collection_random(p1,1), 'r');
hold off;

figure;
%}
errorbar(4:2:10, dist_error_collection_controlled(p2,1), dist_error_collection_controlled(p2,2),'b');
hold on;
errorbar(4:2:10, dist_error_collection_random(p2,1), dist_error_collection_random(p2,2),'r');
hold off;
figure;
errorbar(4:2:10, step_collection_controlled(p2,1), step_collection_controlled(p2,2),'b');
hold on;
errorbar(4:2:10, step_collection_random(p2,1), step_collection_random(p2,2),'r');
hold off;
figure;
plot(4:2:10, step_collection_controlled(p2,1), 'b');
hold on;
plot(4:2:10, step_collection_random(p2,1), 'r');
hold on;
plot([4,10], [1/0.45, 1/0.45], 'g');
hold off;
axis ([4, 10, 0, max(step_collection_random(p2,1))]);


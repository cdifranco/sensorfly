clear 
close all
load 'final_result_controlled.mat';
load 'final_result_random.mat';
errorbar(5:5:5+(size(dist_error_collection_controlled, 1)-1)*5, dist_error_collection_controlled(:,1), dist_error_collection_controlled(:,2),'b');
hold on;
errorbar(5:5:5+(size(dist_error_collection_random, 1)-1)*5, dist_error_collection_random(:,1), dist_error_collection_random(:,2),'r');
hold off;
figure;
errorbar(5:5:5+(size(step_collection_controlled, 1)-1)*5, step_collection_controlled(:,1), step_collection_controlled(:,2)/100,'b');
hold on;
errorbar(5:5:5+(size(step_collection_random, 1)-1)*5, step_collection_random(:,1), step_collection_random(:,2)/100,'r');
hold off;
%figure;%
%plot(5:5:5+(size(error_count_collection_controlled, 1)-1)*5, error_count_collection_controlled(:,1), 'b');
%hold on;
%plot(5:5:5+(size(error_count_collection_random, 1)-1)*5, error_count_collection_random(:,1), 'r');
%hold off;

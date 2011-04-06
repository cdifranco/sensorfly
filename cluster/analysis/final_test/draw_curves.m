clear 
load 'final_result_controlled_cn_5to20.mat';
load 'final_result_random_cn_5to20.mat';
%plot(5:5:5+(size(dist_error_collection_controlled, 1)-1)*5, dist_error_collection_controlled(:,1),'b');
errorbar(5:5:5+(size(dist_error_collection_controlled, 1)-1)*5, dist_error_collection_controlled(:,1), dist_error_collection_controlled(:,2),'b');
hold on;
%plot(5:5:5+(size(dist_error_collection_random, 1)-1)*5, dist_error_collection_random(:,1),'r');
errorbar(5:5:5+(size(dist_error_collection_random, 1)-1)*5, dist_error_collection_random(:,1), dist_error_collection_random(:,2),'r');
hold off;

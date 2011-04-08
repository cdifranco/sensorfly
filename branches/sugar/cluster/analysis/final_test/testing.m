%% Initialization
clear all;
close all;
load 'trained_data.mat';
load 'kmeans_cluster.mat';
RandStream.setDefaultStream(RandStream('mt19937ar','seed',sum(100*clock)));
testing_round = 1000;
success = []; % measurement: success rate
step = []; % measurement: steps
dist_error = []; % measurement: distance error
trans_init_number = 1000;
direction_number = 4;
path = [];
%% Generate Paths
for i = 1:testing_round
    start_index = unidrnd(size(sigxy.sigs,1));
    end_index = unidrnd(size(sigxy.sigs,1));
    while end_index == start_index
        end_index = unidrnd(size(sigxy.sigs,1));
    end
    path = [path; sigxy.x(start_index), sigxy.y(start_index), sigxy.x(end_index), sigxy.y(end_index)];
end

%% Testing loop
len = 0;
error = 0;
error_count = 0;

for j = 1:testing_round
    fprintf('testing round: %d\n', j);
    startX = path(j,1);
    startY = path(j,2);
    destX = path(j,3);
    destY = path(j,4);
    %create matrix
    noOfNodes = size(trans_history,1);
    matrix = zeros(noOfNodes, noOfNodes);
    for l = 1:noOfNodes
        for m = 1:noOfNodes
            sumIJ = sum(trans_history(l,:,m));
            if sumIJ == trans_init_number*direction_number
                matrix(l, m) = inf;
            else
                matrix(l, m) = sum(trans_history(l,:,m))/sum(sum(trans_history(l,:,:)));
                matrix(l, m) = 1/matrix(l, m);
            end;
        end;
    end;
    [succ cluster_route last_coord] = navigate([startX, startY], [destX, destY], trans_history, matrix, sigxy);
    if succ == 1
        step = [step, length(cluster_route)*100/((sum(([startX, startY]-[destX, destY]).^2)).^.5)];
        dist_error = [dist_error, sum((last_coord-[destX destY]).^2).^.5];
    else
        error_count = error_count + 1
    end
end
dist_error_mean = mean(dist_error);
dist_error_std = std(dist_error);
step_ave = mean(step);
step_std = std(step);
load 'final_result_controlled.mat';
dist_error_collection_controlled = [dist_error_collection_controlled; dist_error_mean, dist_error_std];
step_collection_controlled = [step_collection_controlled; step_ave, step_std];
error_count_collection_controlled = [error_count_collection_controlled, error_count];
save ('final_result_controlled.mat','dist_error_collection_controlled','step_collection_controlled','error_count_collection_controlled');


%% Initialization
clear all;
close all;
load 'trained_data.mat';
load 'kmeans_cluster.mat';
RandStream.setDefaultStream(RandStream('mt19937ar','seed',sum(100*clock)));
testing_round = 10000;
step_len = 50; % 50 cm
success = []; % measurement: success rate
step = []; % measurement: steps
dist_error = []; % measurement: distance error
trans_init_number = 1;
direction_number = 4;
path = [];
error_count = 0;
%% Generate Paths
for i = 1:testing_round
    start_index = unidrnd(size(sigxy.sigs,1));
    end_index = unidrnd(size(sigxy.sigs,1));
    while end_index == start_index
        end_index = unidrnd(size(sigxy.sigs,1));
    end
    path = [path; sigxy.x(start_index), sigxy.y(start_index), sigxy.x(end_index), sigxy.y(end_index)];
end

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
    [succ cluster_route last_coord] = random_navigate([startX, startY], [destX, destY], trans_history, matrix, sigxy);
    if succ == 1
        step = [step, length(cluster_route)*100/((sum((last_coord-[destX destY]).^2)).^.5)];
        dist_error = [dist_error, sum((last_coord-[destX destY]).^2).^.5];
    else
        error_count = error_count + 1
    end
end
dist_error_mean = mean(dist_error);
dist_error_std = std(dist_error);
step_ave = mean(step);
step_std = std(step);
load 'final_result_random.mat';
dist_error_collection_random = [dist_error_collection; dist_error_mean, dist_error_std];
step_collection_random = [step_collection; step_ave, step_std];
error_count_collection_random = [error_count_collection, error_count];
save ('final_result_random.mat','dist_error_collection_random','step_collection_random','error_count_collection_random');


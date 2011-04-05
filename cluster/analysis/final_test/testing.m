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
%% Generate Paths
for i = 1:testing_round
    start_index = unidrnd(size(sigxy.sigs,1));
    end_index = unidrnd(size(sigxy.sigs,1));
    while end_index == start_index
        end_index = unidrnd(size(sigxy.sigs,1));
    end
    path = [path; sigxy.x(start_index), sigxy.y(end_index), sigxy.y(start_index), sigxy.y(end_index)];
end

%% Testing loop
len = 0;
error = 0;
e = 0;

for j = 1:testing_round
    fprintf('testing round: %d\n', j);
    startX = path(j,1)
    startY = path(j,2)
    destX = path(j,3)
    destY = path(j,4)
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
    fprintf('start to navigate\n');
    [succ cluster_route] = navigate([startX, startY], [destX, destY], trans_history, matrix, sigxy);
    if succ == 1
        len = len + length(cluster_route)/((sum(([startX, startY]-[destX destY]).^2)).^.5);
        error = error + sum(([startX, startY]-[destX destY]).^2).^.5;
    else
        e = e + 1;
    end
end
error_rate = error / (testing_round-e)
step_ave = len / (testing_round-e)
error_count = e


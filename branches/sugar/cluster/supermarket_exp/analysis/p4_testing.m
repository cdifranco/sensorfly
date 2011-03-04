%% Initialization
clear all;
close all;
load 'clustering_0p9_1000.mat';
testing_round = 1000;
step_len = 3;
success = [];
observed_step = [];
step = [];
RandStream.setDefaultStream(RandStream('mt19937ar','seed',sum(100*clock)));
path = [];
%% Generate Paths
for i = 1:testing_round
    start_index = unidrnd(size(points,1));
    end_index = unidrnd(size(points,1));
    while end_index == start_index
        end_index = unidrnd(size(points,1));
    end
    path = [path; points(start_index , :), points(start_index , :)];
end

%% Testing loop
len = 0;
error = 0;
e = 0;

for j = 1:testing_round
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
    [succ sigRoute clusterRoute coordRoute startClus destClus] = navigate([startX, startY], [destX, destY], step_len, base_number, trans_history, center_sig, points, matrix, std_sig, distribution_table, distribution_table_id);
    if succ == 1
        len = len + length(clusterRoute)/((sum(([startX, startY]-[destX destY]).^2)).^.5);
        error = error + sum((coordRoute(end,:)-[destX destY]).^2).^.5;
    else 
        e = e + 1;
    end
end
error_rate = error / (testing_round-e);
step_ave = len / (testing_round-e);
error_count = e;

%measurements
time = [time step_ave]
accuracy = [accuracy error_rate]
center_count = [center_count size(center, 1)]
error_time = [error_time e]

% Select 3000 Readings for Clustering
%% Initialization
clear all;
close all;
load 'kmeans_cluster.mat';
direction_number = 4; % how many direction can each sensorfly take
trans_init_number = 1; % initiate number of transition histroy
trans_history = []; % transition history     
center_filter = 0.7; % to filter clusters that contains too little readings
base_number = 10;
reading_count = 1; % used when generate readings
reading_amount = 10000; % readings size
reading = zeros(reading_amount, size(std_sig, 2)+1);% reading(cluster_id, real_x, real_y, dir, sig)
current_point_index = unidrnd(size(sigxy.sigs, 1));
anchor_number = size(sigxy.sigs,2)/2;

for mainloop = 1 : reading_amount   
    fprintf('round %d\n',mainloop);
    [next_point_index direction] = get_next_point(current_point_index, sigxy);
    % cluster cluster_id, real_x, real_y, dir, sig
    reading(reading_count, 1) = sigxy.cluster_id(current_point_index);
    reading(reading_count, 2) = sigxy.x(current_point_index);
    reading(reading_count, 3) = sigxy.y(current_point_index);
    reading(reading_count, 4) = direction;
    reading(reading_count, 4 + anchor_number) = sigxy.sigs(current_point_index,1:anchor_number);
    if mainloop ~= 1
        trans_history(reading(reading_count-1,1), reading(reading_count-1,4), reading(reading_count,1)) = trans_history(reading(reading_count-1,1), reading(reading_count-1,4), reading(reading_count,1))+1;
    end
    current_point_index = next_point_index;
    reading_count = reading_count + 1;
end
hold off;

save('', '');
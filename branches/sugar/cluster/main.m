close all
clear all
load ('distribution')

base_number = 4;%change
main_loop_count = 10;%how many sensorflies going in
path_reading_count = 20;% how many readings each sensorfly will take
step_len = 0.7;%how long will each step take
direction_number = 4;%how many direction can each sensorfly take
trans_init_number = 1;
bel_threshold = 0.0;

%measurements
time = [];
accuracy = [];
center_count = [];
error_time = [];

for base_number = 4:4
    fprintf('-----start: base station number %d clustering-----\n', base_number);
    basic_cluster;
    fprintf('-----start: base station number %d testing1-----\n', base_number);
    testing;
    % measurements
    time = [time step_ave]
    accuracy = [accuracy error_rate]
    center_count = [center_count size(center, 1)]
    error_time = [error_time e]

    fprintf('-----start: base station number %d reclustering-----\n', base_number);
    recluster_kmeans;
    fprintf('-----start: base station number %d testing2-----\n', base_number);
    testing;
    % measurements
    time = [time step_ave]
    accuracy = [accuracy error_rate]
    center_count = [center_count size(center, 1)]
    error_time = [error_time e]

end
clear_up;